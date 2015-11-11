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
	mSquareFinder = std::unique_ptr<ContourDetector>( new ContourDetector( incomingImagesWidth, incomingImagesHeight ) );

	mTagRecognizer = std::unique_ptr<TagRecognizer>( new TagRecognizer(4) );

	// Setup Detection Surfaces
	mIncomingGrayscale = ci::Surface8u::create( kmIncomingImgsWidth, kmIncomingImgsHeight, false );
	mIncomingBinarized = ci::Surface8u::create( kmIncomingImgsWidth, kmIncomingImgsHeight, false );
	mIncomingSquaresDetected = ci::Surface8u::create( kmIncomingImgsWidth, kmIncomingImgsHeight, false );
	mIncomingTagsDetected = ci::Surface8u::create( kmIncomingImgsWidth, kmIncomingImgsHeight, false );

	//ci::Surface8u::create()

	// Associated debug textures
	mTexGrayscale = ci::gl::Texture2d::create( kmIncomingImgsWidth, kmIncomingImgsHeight );
	mTexBinary = ci::gl::Texture2d::create( kmIncomingImgsWidth, kmIncomingImgsHeight );
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
	processIncomingToGrayscale( surface );
	mTexGrayscale->update( *mIncomingGrayscale.get() );

	// Apply adaptive thresholding
	mIncomingBinarized = mBinarizer->process( mIncomingGrayscale );
	//processGrayscaleToBinary( mIncomingGrayscale );
	mTexBinary->update( *mIncomingBinarized.get() );

	// Detect squares
	detectSquaresInBinary( mIncomingBinarized );
	mTexSquares->update( *mIncomingSquaresDetected.get() );

	// Detect tags within squares
	detectTagsInSquares( mIncomingSquaresDetected );
	mTexTags->update( *mIncomingTagsDetected.get() );

	// Do serious math to determine its position in relation to camera

	// Put the location or something in vector ready to return
}

void CiSimplePose::processIncomingToGrayscale( ci::Surface8uRef surface )
{
	auto grayscale = ci::Channel8u( *surface.get() );

	// XXX: Fix/Optimize
	mIncomingGrayscale = ci::Surface8u::create( grayscale );
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
