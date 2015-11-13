/*
* Copyright (c) 2015, Lasse Farnung Laursen - http://www.lasselaursen.com
*
* This file is a part of SimplePose, and subject to the new and revised BSD license.
* Please see the LICENSE file for more information.
* All other rights reserved.
*/

#include "ciSimplePose.h"

#include "TagRecognizer.h"

#include "cinder/Log.h"
#include "cinder/Rand.h"

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

	auto chanIter = mImgBinary->getIter();

	while ( chanIter.line() )
	{
		while ( chanIter.pixel() )
		{
			chanIter.v() = ci::randInt( 0, 255 );
		}
	}
}

CiSimplePose::~CiSimplePose()
{


}

ci::Surface8uRef CiSimplePose::getTagTex( unsigned int const &numTags ) { return mTagRecognizer->getTagTex( numTags ); };

void CiSimplePose::detectTags( ci::Surface8uRef surface )
{
	// Convert to grayscale
	auto imgGrayScale = ci::Channel8u::create( *surface );
	mTexGrayscale->update( *imgGrayScale );

	// Apply adaptive thresholding
	mImgBinary = mBinarizer->process( imgGrayScale );
	mTexBinary->update( *mImgBinary );

	// Test Contour'r
	mContourFinder->testProcess();
	return;
	// Find Contours
	mImgContours = mContourFinder->process( mImgBinary );

	// Temp Debugging
	mTexDebug->update( *mContourFinder->getDebugImg() );

	//mImgBinary = mContourFinder->getDebugImg();

	//auto chanIter = mImgBinary->getIter();

	//while ( chanIter.line() )
	//{
	//	while ( chanIter.pixel() )
	//	{
	//		CI_LOG_V("insanity pixel value " << chanIter.v() );
	//	}
	//}
	

	

	// Debugs
	//mImgBinary = mContourFinder->getDebugImg();
	

	mTexContours->update( *mImgContours );
	
	
	
	
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
