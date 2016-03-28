/*
* Copyright (c) 2015, Lasse Farnung Laursen - http://www.lasselaursen.com
*
* This file is a part of SimplePose, and subject to the new and revised BSD license.
* Please see the LICENSE file for more information.
* All other rights reserved.
*/

#include "AdaptiveThresholdBinarization.h"

namespace SimplePose {

AdaptiveThresholdBinarization::AdaptiveThresholdBinarization(unsigned int const & incomingImagesWidth, unsigned int const & incomingImagesHeight) :
	kmIncomingImgsWidth( incomingImagesWidth ),
	kmIncomingImgsHeight( incomingImagesHeight ),
	kmBinaPixelWindow( incomingImagesWidth / 4 ),	// C++ automatically floors integer division
	kmBinaThresholdRelaxation( 1.0f )
{
	mBinaPrevRowThresholdEstimate = std::unique_ptr<double>( new double[incomingImagesWidth] );

	mImageProcessed = ci::Channel8u::create( kmIncomingImgsWidth, kmIncomingImgsHeight );

}

AdaptiveThresholdBinarization::~AdaptiveThresholdBinarization()
{

}

void AdaptiveThresholdBinarization::processPixel( ci::Channel8u::Iter & surfaceIter )
{
	mBinaPixelWindowEstimate = mBinaPixelWindowEstimate - ( mBinaPixelWindowEstimate / kmBinaPixelWindow ) + ( surfaceIter.v() );

	// Improved threshold
	mBinaThreshold = ( ( ( mBinaPrevRowThresholdEstimate.get()[surfaceIter.x()] + mBinaPixelWindowEstimate ) / 2.0f ) / kmBinaPixelWindow )*kmBinaThresholdRelaxation;

	int estimationResult = 255 * ( ( surfaceIter.v() ) >= mBinaThreshold );
	surfaceIter.v() = estimationResult;

	mBinaPrevRowThresholdEstimate.get()[surfaceIter.x()] = mBinaPixelWindowEstimate;
}

ci::Channel8uRef AdaptiveThresholdBinarization::process( ci::Channel8uRef surface )
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

	mImageProcessed = surface;
	auto surfaceIter = mImageProcessed->getIter();

	// Draw border
	while ( surfaceIter.line() )
	{
		if ( surfaceIter.y() % 2 )
		{
			// Even Rows
			while ( surfaceIter.pixel() )
			{
				processPixel( surfaceIter );
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
				processPixel( surfaceIter );

				surfaceIter.mPtr -= surfaceIter.mInc;
				--surfaceIter.mX;
			}
		}
	}

	return mImageProcessed;
}

};