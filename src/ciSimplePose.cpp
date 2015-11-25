/*
* Copyright (c) 2015, Lasse Farnung Laursen - http://www.lasselaursen.com
*
* This file is a part of SimplePose, and subject to the new and revised BSD license.
* Please see the LICENSE file for more information.
* All other rights reserved.
*/

#include "ciSimplePose.h"

#include "tag/TagRecognizer.h"

#include "cinder/Log.h"
#include "cinder/Rand.h"

// Image Processing
#include "image/AdaptiveThresholdBinarization.h"
#include "image/ContourDetector.h"
#include "image/PolygonApproximator.h"

#include "math/PoseEstimator.h"

#include "image/Polygon.h"

#include "cinder/Camera.h"

CiSimplePose::CiSimplePose( 
	unsigned int const & incomingImagesWidth,
	unsigned int const & incomingImagesHeight,
	ci::mat3 const & matIntrinsicCameraParameters ) :
	kmIncomingImgsWidth( incomingImagesWidth ),
	kmIncomingImgsHeight( incomingImagesHeight ),
	mMatIntrinsicCameraParameters( matIntrinsicCameraParameters )
{
	// Raii principles
	mBinarizer = std::unique_ptr<AdaptiveThresholdBinarization>( new AdaptiveThresholdBinarization( incomingImagesWidth, incomingImagesHeight ) );
	mContourFinder = std::unique_ptr<ContourDetector>( new ContourDetector( incomingImagesWidth, incomingImagesHeight ) );
	mPolygonApproximator = std::unique_ptr<PolygonApproximator>( new PolygonApproximator() );

	mTagRecognizer = std::unique_ptr<TagRecognizer>( new TagRecognizer(3) );

	mPoseEstimator = std::unique_ptr<PoseEstimator>( new PoseEstimator( matIntrinsicCameraParameters ) );

	// Setup Detection Surfaces
	mImgGrayScale = ci::Channel8u::create( kmIncomingImgsWidth, kmIncomingImgsHeight );
	mImgBinary = ci::Channel8u::create( kmIncomingImgsWidth, kmIncomingImgsHeight );
	mImgContours = ci::Surface8u::create( kmIncomingImgsWidth, kmIncomingImgsHeight, false );
	mImgSquares = ci::Surface8u::create( kmIncomingImgsWidth, kmIncomingImgsHeight, false );
	mImgTags = ci::Surface8u::create( kmIncomingImgsWidth, kmIncomingImgsHeight, false );

	auto temp = ci::Channel8u::create( kmIncomingImgsWidth + 2, kmIncomingImgsHeight + 2);

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

ci::Surface8uRef CiSimplePose::getTagTex( unsigned int const &numTags ) { return mTagRecognizer->getTagTex( numTags ); };

ci::mat4 CiSimplePose::getEstimatedViewMatrixFromTag( ci::Surface8uRef surface )
{
	auto detectedTags = detectTags( surface );

	auto firstDetectedTag = detectedTags.begin();

	auto posCornersArray = firstDetectedTag->get()->getPosCornersScreencoords();

	auto ptr = reinterpret_cast<ci::vec2 const *>( &firstDetectedTag->get()->getPosCornersScreencoords() );

	return mPoseEstimator->estimateViewMatrix( ptr );

	//mPoseEstimator->estimateViewMatrix( ptr );

	//return ci::mat4( 0 );
}

void CiSimplePose::getTagPoses( ci::Surface8uRef surface, ci::mat4 viewMatrix )
{
	detectTags( surface );
}

std::vector<std::unique_ptr<Tag>> CiSimplePose::detectTags( ci::Surface8uRef surface )
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

	// Detect Contours
	mContourFinder->process( mImgBinary );

	// Filter contours that aren't big enough and aren't outer contours
	auto contours = mContourFinder->getContours();

	std::copy_if( contours.begin(), contours.end(), std::back_inserter( mContoursTagSized ),
		[]( Contour const & contour ) { return (contour.calcPerimeter() > 100.0f) && contour.mType == Contour::TYPE::OUTER; } );

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

	auto detectedTags = mTagRecognizer->process( mImgBinary, mPolygonsSquare );

	return std::move( detectedTags );
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

//void AR2::calcOpenGLCameraProjMatrix() {
//	// The implementation as explained for the POSIT in OpenCV
//	double *intrinsicData = mIntrinsicCameraMat->data.db;
//
//	// Row 0
//	mCameraProjMatrix[0][0] = 2.0 * intrinsicData[0] / mImgFeedWidth;
//	mCameraProjMatrix[0][1] = 0.0;
//	mCameraProjMatrix[0][2] = 2.0 * ( intrinsicData[2] / mImgFeedWidth ) - 1.0;
//	mCameraProjMatrix[0][3] = 0.0;
//
//	// Row 1
//	mCameraProjMatrix[1][0] = 0.0;
//	mCameraProjMatrix[1][1] = 2.0 * intrinsicData[4] / mImgFeedHeight;
//	mCameraProjMatrix[1][2] = 2.0 * ( intrinsicData[5] / mImgFeedHeight ) - 1.0;
//	mCameraProjMatrix[1][3] = 0.0;
//
//	// Row 2 (Clipping Plane)
//	mCameraProjMatrix[2][0] = 0.0;
//	mCameraProjMatrix[2][1] = 0.0;
//	mCameraProjMatrix[2][2] = -1.0;
//	mCameraProjMatrix[2][3] = -0.02;
//
//	// Row 3 | Per-definition as follows:
//	mCameraProjMatrix[3][0] = 0.0;
//	mCameraProjMatrix[3][1] = 0.0;
//	mCameraProjMatrix[3][2] = -1.0;
//	mCameraProjMatrix[3][3] = 0.0;
//}

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