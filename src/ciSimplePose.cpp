/*
* Copyright (c) 2015, Lasse Farnung Laursen - http://www.lasselaursen.com
*
* This file is a part of SimplePose, and subject to the new and revised BSD license.
* Please see the LICENSE file for more information.
* All other rights reserved.
*/

#include "ciSimplePose.h"

CiSimplePose::CiSimplePose(unsigned int const & incomingImagesWidth, unsigned int const & incomingImagesHeight) :
	kmIncomingImgsWidth( incomingImagesWidth ),
	kmIncomingImgsHeight( incomingImagesHeight ),
	kmBinaPixelWindow( incomingImagesWidth / 4 ),	// C++ automatically floors integer division
	kmBinaThresholdRelaxation( 1.0f )
{
	mBinaPrevRowThresholdEstimate = std::unique_ptr<double>( new double[incomingImagesWidth] );

	// Raii principles
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

void CiSimplePose::detectTags( ci::Surface8uRef surface )
{
	// Convert to grayscale
	processIncomingToGrayscale( surface );
	mTexGrayscale->update( *mIncomingGrayscale.get() );

	// Apply adaptive thresholding
	processGrayscaleToBinary( mIncomingGrayscale );
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

void CiSimplePose::processGrayscaleToBinaryPixel( ci::Surface8u::Iter & surfaceIter )
{
	mBinaPixelWindowEstimate = mBinaPixelWindowEstimate - ( mBinaPixelWindowEstimate / kmBinaPixelWindow ) + ( surfaceIter.r() );	// could be r,g, or b

	// Improved threshold
	mBinaThreshold = ( ( ( mBinaPrevRowThresholdEstimate.get()[surfaceIter.x()] + mBinaPixelWindowEstimate ) / 2.0f ) / kmBinaPixelWindow )*kmBinaThresholdRelaxation;

	int estimationResult = 255 * ( ( surfaceIter.r() ) >= mBinaThreshold );
	//*pBin++ = estimationResult;
	surfaceIter.r() = estimationResult;
	surfaceIter.g() = estimationResult;
	surfaceIter.b() = estimationResult;

	mBinaPrevRowThresholdEstimate.get()[surfaceIter.x()] = mBinaPixelWindowEstimate;
}

void CiSimplePose::processGrayscaleToBinary( ci::Surface8uRef surface )
{
	/* 
	(Brief) How this algorithm works:
	
	Based on: Pierre D. Wellner. Adaptive thresholding for the digitaldesk. Technical Report EPC-1993-110, Rank Xerox Research Centre, Cambridge Laboratory, 61 Regent Street, Cambridge CB2 1AB, 1993.
	
	The threshold estimate starts at 0. At every pixel, the threshold is re-evaluated with the current pixel
	added as value, and an estimate of the dropped pixel subtracted (to save memory). Each evaluation is then an
	average of "all the pixel values in the window" (+ the values of the last row averaged together)
	divided by the pixel window, and then "lessened by the threshold". This value then helps determine if the next pixel
	should be considered foreground or background.
	*/

	mBinaPixelWindowEstimate = 0;
	mBinaThreshold = 0;
	std::memset( mBinaPrevRowThresholdEstimate.get(), 0, kmIncomingImgsWidth ); // Not super necessary

	auto surfaceIter = surface->getIter();

	// Draw border
	while ( surfaceIter.line() )
	{
		if ( surfaceIter.y() % 2 )
		{
			// Even Rows
			while ( surfaceIter.pixel() )
			{
				processGrayscaleToBinaryPixel( surfaceIter );
			}
		}
		else {
			// Un-even rows

			// Cinder doesn't support reverse iteration so we'll have to work a little internal-pointer majik here.
			// Set interal pixel pointer to last pixel on row
			surfaceIter.pixel();
			surfaceIter.mPtr += ( surfaceIter.mWidth - 1 ) * surfaceIter.mInc;
			surfaceIter.mX += surfaceIter.mEndX - 1;

			while ( surfaceIter.mX >= surfaceIter.mStartX )
			{
				processGrayscaleToBinaryPixel( surfaceIter );

				surfaceIter.mPtr -= surfaceIter.mInc;
				--surfaceIter.mX;
			}
		}
	}

	mIncomingBinarized = surface;
}

void CiSimplePose::detectSquaresInBinary( ci::Surface8uRef surface )
{

}

void CiSimplePose::detectTagsInSquares( ci::Surface8uRef surface )
{

}
