/*
* Copyright (c) 2015, Lasse Farnung Laursen - http://www.lasselaursen.com
*
* This file is a part of SimplePose, and subject to the new and revised BSD license.
* Please see the LICENSE file for more information.
* All other rights reserved.
*/

#include "ciSimplePose.h"

#include "TagRecognizer.h"

// Image Processing
#include "image/AdaptiveThresholdBinarization.h"
#include "image/ContourDetector.h"

CiSimplePose::CiSimplePose( unsigned int const & incomingImagesWidth, unsigned int const & incomingImagesHeight ) :
	kmIncomingImgsWidth( incomingImagesWidth ),
	kmIncomingImgsHeight( incomingImagesHeight )
{
	// Raii principles
	mBinarizer = std::unique_ptr<AdaptiveThresholdBinarization>( new AdaptiveThresholdBinarization( incomingImagesWidth, incomingImagesHeight ) );
	mContourFinder = std::unique_ptr<ContourDetector>( new ContourDetector( incomingImagesWidth, incomingImagesHeight ) );

	mTagRecognizer = std::unique_ptr<TagRecognizer>( new TagRecognizer(4) );

	// Setup Detection Surfaces
	//mIncomingGrayscale = ci::Surface8u::create( kmIncomingImgsWidth, kmIncomingImgsHeight, false );
	//mIncomingBinarized = ci::Surface8u::create( kmIncomingImgsWidth, kmIncomingImgsHeight, false );
	//mIncomingSquaresDetected = ci::Surface8u::create( kmIncomingImgsWidth, kmIncomingImgsHeight, false );
	//mIncomingTagsDetected = ci::Surface8u::create( kmIncomingImgsWidth, kmIncomingImgsHeight, false );

	mImgGrayScale = ci::Channel8u::create( kmIncomingImgsWidth, kmIncomingImgsHeight );
	mImgBinary = ci::Channel8u::create( kmIncomingImgsWidth, kmIncomingImgsHeight );
	mImgContours = ci::Surface8u::create( kmIncomingImgsWidth, kmIncomingImgsHeight, false );
	mImgSquares = ci::Surface8u::create( kmIncomingImgsWidth, kmIncomingImgsHeight, false );
	mImgTags = ci::Surface8u::create( kmIncomingImgsWidth, kmIncomingImgsHeight, false );

	//ci::Surface8u::create()

	// Associated debug textures
	mTexGrayscale = ci::gl::Texture2d::create( kmIncomingImgsWidth, kmIncomingImgsHeight );
	mTexBinary = ci::gl::Texture2d::create( kmIncomingImgsWidth, kmIncomingImgsHeight );
	mTexContours = ci::gl::Texture2d::create( kmIncomingImgsWidth, kmIncomingImgsHeight );
	mTexSquares = ci::gl::Texture2d::create( kmIncomingImgsWidth, kmIncomingImgsHeight );
	mTexTags = ci::gl::Texture2d::create( kmIncomingImgsWidth, kmIncomingImgsHeight );


}

CiSimplePose::~CiSimplePose()
{


}

ci::Surface8uRef CiSimplePose::getTagTex( unsigned int const &numTags ) { return mTagRecognizer->getTagTex( numTags ); };

void CiSimplePose::detectTags( ci::Surface8uRef surface )
{
	// Convert to grayscale
	auto imgGrayScale = ci::Channel8u::create( *surface.get() );
	auto graySurf = ci::Surface8u::create( *imgGrayScale.get() );		// Work-around for Cinder bug...
	mTexGrayscale->update( *graySurf.get() );

	//// Apply adaptive thresholding
	mImgBinary = mBinarizer->process( imgGrayScale );
	auto binSurf = ci::Surface8u::create( *mImgBinary.get() );
	mTexBinary->update( *binSurf.get() );

	//// Find Contours
	mImgContours = mContourFinder->process( mImgBinary );
	auto conSurf = ci::Surface8u::create( *mImgContours.get() );
	mTexContours->update( *conSurf.get() );
	//mContourFinder->process( mImgBinary );

	// Detect squares
	//detectSquaresInBinary( mIncomingBinarized );
	//mTexSquares->update( *mIncomingSquaresDetected.get() );

	// Detect tags within squares
	//detectTagsInSquares( mIncomingSquaresDetected );
	//mTexTags->update( *mIncomingTagsDetected.get() );

	// Do serious math to determine its position in relation to camera

	// Put the location or something in vector ready to return
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
