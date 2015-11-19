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

#include "image/Polygon.h"

CiSimplePose::CiSimplePose( unsigned int const & incomingImagesWidth, unsigned int const & incomingImagesHeight ) :
	kmIncomingImgsWidth( incomingImagesWidth ),
	kmIncomingImgsHeight( incomingImagesHeight )
{
	// Raii principles
	mBinarizer = std::unique_ptr<AdaptiveThresholdBinarization>( new AdaptiveThresholdBinarization( incomingImagesWidth, incomingImagesHeight ) );
	mContourFinder = std::unique_ptr<ContourDetector>( new ContourDetector( incomingImagesWidth, incomingImagesHeight ) );
	mPolygonApproximator = std::unique_ptr<PolygonApproximator>( new PolygonApproximator() );

	mTagRecognizer = std::unique_ptr<TagRecognizer>( new TagRecognizer(4) );

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

	// Filter contours that aren't big enough
	auto contours = mContourFinder->getContours();

	std::copy_if( contours.begin(), contours.end(), std::back_inserter( mContoursTagSized ),
		[]( Contour const & contour ) { return contour.calcPerimeter() > 100.0f; } );

	// Approximate Polygons from Contours and filter polygons to retain only possible tags
	mPolygonApproximator->process( mContoursTagSized );

	// Filter polygons that are not convex
	auto polygons = mPolygonApproximator->getPolygons();

	std::copy_if( polygons.begin(), polygons.end(), std::back_inserter( mPolygonsConvex ),
		[]( Polygon const & polygon ) { return polygon.isConvex(); } );


	// Fix
	std::copy_if( mPolygonsConvex.begin(), mPolygonsConvex.end(), std::back_inserter( mPolygonsSquare ),
		[]( Polygon const & polygon ) { return polygon.isSquare(); } );





	// Todo:

	// Detect squares
	//detectSquaresInBinary( mIncomingBinarized );
	//mTexSquares->update( *mIncomingSquaresDetected.get() );

	// Detect tags within squares
	//detectTagsInSquares( mIncomingSquaresDetected );
	//mTexTags->update( *mIncomingTagsDetected.get() );

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

ci::Channel8uRef CiSimplePose::processIncomingToGrayscale( ci::Surface8uRef surface )
{
	//mImgGrayScale
	auto grayscale = ci::Channel8u::create( *surface.get() );





	return grayscale;
}

void CiSimplePose::detectSquaresInBinary( ci::Surface8uRef surface )
{
	/*
	How the algorithm works:

	Based on: Satoshi Suzuki and Keiichi Abe, Topological Structural Analysis of Digitized Binary Images by Border Following
	
	
	*/

}

void CiSimplePose::detectTagsInSquares( ci::Surface8uRef surface )
{

}


void CiSimplePose::unitTest()
{
	mContourFinder->testContourCalculation();

	mPolygonApproximator->testSimplification();
}