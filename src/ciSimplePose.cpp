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

CiSimplePose::CiSimplePose( 
	unsigned int const & incomingImagesWidth,
	unsigned int const & incomingImagesHeight,
	ci::mat3 const & intrinsicCameraParameters ) :
	kmIncomingImgsWidth( incomingImagesWidth ),
	kmIncomingImgsHeight( incomingImagesHeight )
{
	// Raii principles
	mBinarizer = std::unique_ptr<AdaptiveThresholdBinarization>( new AdaptiveThresholdBinarization( incomingImagesWidth, incomingImagesHeight ) );
	mContourFinder = std::unique_ptr<ContourDetector>( new ContourDetector( incomingImagesWidth, incomingImagesHeight ) );
	mPolygonApproximator = std::unique_ptr<PolygonApproximator>( new PolygonApproximator() );

	mTagRecognizer = std::unique_ptr<TagRecognizer>( new TagRecognizer(3) );

	mPoseEstimator = std::unique_ptr<PoseEstimator>( new PoseEstimator( intrinsicCameraParameters ) );

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

void CiSimplePose::detectTags( ci::Surface8uRef surface )
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

	mTagRecognizer->process( mImgBinary, mPolygonsSquare );



	// Todo:

	// Do serious math to determine its position in relation to camera

	// Put the location or something in vector ready to return
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