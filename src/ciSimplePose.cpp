/*
* Copyright (c) 2015, Lasse Farnung Laursen - http://www.lasselaursen.com
*
* This file is a part of SimplePose, and subject to the new and revised BSD license.
* Please see the LICENSE file for more information.
* All other rights reserved.
*/

#include "ciSimplePose.h"

#include "tag/TagRecognizer.h"
#include "event/EventDispatcher.h"

#include "cinder/Log.h"
#include "cinder/Rand.h"

// Image Processing
#include "image/AdaptiveThresholdBinarization.h"
#include "image/ContourDetector.h"
#include "image/PolygonApproximator.h"

#include "math/PoseEstimator.h"

#include "image/Polygon.h"

#include "tag/TagBitPattern.h"

namespace SimplePose {

	CiSimplePose::CiSimplePose(
		unsigned int const & incomingImagesWidth,
		unsigned int const & incomingImagesHeight,
		ci::mat3 const & matIntrinsicCameraParameters ) :
		kmIncomingImgsWidth( incomingImagesWidth ),
		kmIncomingImgsHeight( incomingImagesHeight ),
		mMatIntrinsicCameraParameters( matIntrinsicCameraParameters )
	{
		// Raii principles
		mBinarizer = std::make_unique<AdaptiveThresholdBinarization>( incomingImagesWidth, incomingImagesHeight );
		mContourFinder = std::make_unique<ContourDetector>( incomingImagesWidth, incomingImagesHeight );
		mPolygonApproximator = std::make_unique<PolygonApproximator>();

		mTagRecognizer = std::make_unique<TagRecognizer>( );
		mPoseEstimator = std::make_unique<PoseEstimator>( mMatIntrinsicCameraParameters );

		mIndividualTagEventDispatcher = std::make_unique<EventDispatcher>();

		// Setup Detection Surfaces
		mImgGrayScale = ci::Channel8u::create( kmIncomingImgsWidth, kmIncomingImgsHeight );
		mImgBinary = ci::Channel8u::create( kmIncomingImgsWidth, kmIncomingImgsHeight );
		mImgContours = ci::Surface8u::create( kmIncomingImgsWidth, kmIncomingImgsHeight, false );
		mImgSquares = ci::Surface8u::create( kmIncomingImgsWidth, kmIncomingImgsHeight, false );
		mImgTags = ci::Surface8u::create( kmIncomingImgsWidth, kmIncomingImgsHeight, false );

		auto temp = ci::Channel8u::create( kmIncomingImgsWidth + 2, kmIncomingImgsHeight + 2 );

		// Associated debug textures
		mTexGrayscale = ci::gl::Texture2d::create( *mImgGrayScale );
		mTexBinary = ci::gl::Texture2d::create( *mImgBinary );
		mTexContours = ci::gl::Texture2d::create( *mImgContours );
		mTexSquares = ci::gl::Texture2d::create( *mImgSquares );
		mTexTags = ci::gl::Texture2d::create( *mImgTags );

		mTexDebug = ci::gl::Texture2d::create( *temp );

	}

	CiSimplePose::~CiSimplePose()
	{


	}

	void CiSimplePose::addTagType( std::unique_ptr<Tag> tagDetector )
	{
		mTagRecognizer->addTagType( std::move( tagDetector ) );
	}

	void CiSimplePose::update( ci::Surface8uRef const& surface )
	{
		// No image to process
		if ( !surface )
			return;

		// Process image and get detected tags
		auto detectedTags = detectTags( surface );

		if ( detectedTags.size() )
		{
			TagEvent event;
			mSignalTagDiscovered.emit( event );
		}

		// Turn into shared_ptr's and then send off?
		// XXX: Attend to this next.

		// Updated?
		//mSignalTagUpdated.emit( event );

		// Individual update emit!
		//mIndividualTagEventDispatcher->sendEvent( event );
	}

	ci::Surface8uRef CiSimplePose::getTagTex( unsigned int const &numTags ) { return mTagRecognizer->getTagTex( numTags ); };

	ci::mat4 CiSimplePose::getEstimatedViewMatrixFromTag( ci::Surface8uRef surface, unsigned char const & debugTagNr )
	{
		std::vector<std::unique_ptr<Tag>> tempVec;

		tempVec.emplace_back( std::move( std::unique_ptr<Tag>( new TagBitPattern( true, debugTagNr ) ) ) );

		auto posCornersArray = tempVec.begin()->get()->getPosCornersScreencoords();
		auto ptr = reinterpret_cast<ci::vec2 const *>( &tempVec.begin()->get()->getPosCornersScreencoords() );
		return mPoseEstimator->estimateViewMatrix( ptr );

		//auto detectedTags = detectTags( surface );

		//if ( !detectedTags.empty() )
		//{
		//	auto firstDetectedTag = detectedTags.begin();

		//	auto posCornersArray = firstDetectedTag->get()->getPosCornersScreencoords();

		//	auto ptr = reinterpret_cast<ci::vec2 const *>( &firstDetectedTag->get()->getPosCornersScreencoords() );

		//	return mPoseEstimator->estimateViewMatrix( ptr );
		//}
		//else {
		//	return ci::mat4( 0 );
		//}
	}

	ci::mat4 CiSimplePose::getEstimatedViewMatrixFromTagPoints( ci::vec2 const& pt1, ci::vec2 const& pt2, ci::vec2 const& pt3, ci::vec2 const& pt4 )
	{
		// Recast to std::array...
		std::array<ci::vec2, 4> ptsArray;
		ptsArray[0] = pt1;
		ptsArray[1] = pt2;
		ptsArray[2] = pt3;
		ptsArray[3] = pt4;

		auto ptr = reinterpret_cast<ci::vec2 const *>( &ptsArray );
		return mPoseEstimator->estimateViewMatrix( ptr );
	}

	void CiSimplePose::getTagPoses( ci::Surface8uRef surface, ci::mat4 viewMatrix )
	{
		detectTags( surface );
	}

	// Fix to not use unique_ptr. Probably not worth it.
	std::vector<std::unique_ptr<Tag>> CiSimplePose::detectTags( ci::Surface8uRef const& surface )
	{
		mContoursTagSized.clear();
		mPolygonsConvex.clear();
		mPolygonsSquare.clear();

		// Convert to grayscale
		auto imgGrayScale = ci::Channel8u::create( *surface );
		mTexGrayscale->update( *imgGrayScale );

		// Apply adaptive thresholding
		mImgBinary = mBinarizer->process( imgGrayScale );
		mTexBinary->update( *mImgBinary );

		// End early to find load...
		return std::vector<std::unique_ptr<Tag>>();

		// Detect Contours
		mContourFinder->process( mImgBinary );

		// Filter contours that aren't big enough and aren't outer contours
		auto contours = mContourFinder->getContours();

		std::copy_if( contours.begin(), contours.end(), std::back_inserter( mContoursTagSized ),
			[]( Contour const & contour ) { return ( contour.calcPerimeter() > 100.0f ); } ); // && contour.mType == Contour::TYPE::OUTER;

		// Approximate Polygons from Contours and filter polygons to retain only possible tags
		mPolygonApproximator->process( mContoursTagSized );

		/***
		* Filter Polygons
		*/
		auto polygons = mPolygonApproximator->getPolygons();

		// Throw out non-convex poly's
		std::copy_if( polygons.begin(), polygons.end(), std::back_inserter( mPolygonsConvex ),
			[]( Polygon const & polygon ) { return polygon.isConvex(); } );


		// Throw out non-square poly's
		std::copy_if( mPolygonsConvex.begin(), mPolygonsConvex.end(), std::back_inserter( mPolygonsSquare ),
			[]( Polygon const & polygon ) { return polygon.isSquare(); } );


		// Copy Elision?
		return mTagRecognizer->process( mImgBinary, mPolygonsSquare );

		//auto detectedTags = mTagRecognizer->process( mImgBinary, mPolygonsSquare );

		//return std::move( detectedTags );
	}

	void CiSimplePose::drawAllContours() const
	{
		mContourFinder->drawAllContours();

		//mPolygonApproximator->drawTestPolys();
	}

	void CiSimplePose::drawTagSizedContours() const
	{
		for ( auto const & contour : mContoursTagSized )
		{
			contour.draw();
		}
	}

	void CiSimplePose::drawPolygonsAll() const
	{
		mPolygonApproximator->drawAllPolygons();
	}

	void CiSimplePose::drawPolygonsConvexOnly() const
	{
		for ( auto const & polygon : mPolygonsConvex )
		{
			polygon.draw();
		}
	}

	void CiSimplePose::drawPolygonsSquaresOnly() const
	{
		for ( auto const & polygon : mPolygonsSquare )
		{
			polygon.draw();
		}
	}

	void CiSimplePose::drawTagsAll() const
	{
		mTagRecognizer->drawDetectedTags();
	}

	ci::Channel8uRef CiSimplePose::processIncomingToGrayscale( ci::Surface8uRef surface )
	{
		auto grayscale = ci::Channel8u::create( *surface.get() );
		return grayscale;
	}



	void CiSimplePose::unitTest()
	{
		mContourFinder->testContourCalculation();

		mPolygonApproximator->testSimplification();
	}

	void CiSimplePose::matchVirtualCamToRealCamParameters( ci::CameraPersp camPersp )
	{
		// XXX: setPerspective currently does not allow us to set varying focal point values for x/y
		// For now pick either/or. They should more or less be identical (otherwise you have some really odd camera stuff going on)
		auto fov = 0;
		auto aspectRatio = 0;
		auto nearPlane = mMatIntrinsicCameraParameters[0]; // (nearClip)
		// or mMatIntrinsicCameraParameters[4]
		auto farClip = 0;


		//camPersp.setPerspective();
	}

	ci::mat4 CiSimplePose::convertIntrinsicCameraParametersToOpenGLProjectionMatrix(
		ci::mat3 const & intrinsicCameraParameters,
		float const &imgFeedWidth,
		float const &imgFeedHeight )
	{
		//float a = intrinsicCameraParameters[0][0];
		//float b = intrinsicCameraParameters[0][1];
		//float c = intrinsicCameraParameters[0][2];

		//float hello = intrinsicCameraParameters[1][1];

		//float hello2 = intrinsicCameraParameters[2][1];

		// Apparently access is [row][column], i.e. [y][x]
		auto mat = ci::mat4(
			2.0f * intrinsicCameraParameters[0][0] / imgFeedWidth, 0.0f, 2.0f * ( intrinsicCameraParameters[0][2] / imgFeedWidth ) - 1.0f, 0.0f,
			0.0f, 2.0f * intrinsicCameraParameters[1][1] / imgFeedHeight, 2.0f * ( intrinsicCameraParameters[1][2] / imgFeedHeight ) - 1.0f, 0.0f,
			0.0f, 0.0f, -1.0f, -0.02f,
			0.0f, 0.0f, -1.0f, 0.0f
			);

		return glm::transpose( mat );
	}

};