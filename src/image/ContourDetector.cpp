/*
* Copyright (c) 2015, Lasse Farnung Laursen - http://www.lasselaursen.com
*
* This file is a part of SimplePose, and subject to the new and revised BSD license.
* Please see the LICENSE file for more information.
* All other rights reserved.
*/

#include "ContourDetector.h"
#include "Contour.h"

#include "psimpl_v7_src/psimpl.h"

ContourDetector::ContourDetector(unsigned int const & incomingImagesWidth, unsigned int const & incomingImagesHeight) :
	kmIncomingImgsWidth( incomingImagesWidth ),
	kmIncomingImgsHeight( incomingImagesHeight ),
	kmImgBorderedWidth( incomingImagesWidth + 2),
	kmImgBorderedHeight( incomingImagesHeight + 2 )
{
	mImageProcessed = ci::Surface8u::create( kmIncomingImgsWidth, kmIncomingImgsHeight, false );
	mImageProcessedBordered = ci::Surface8u::create( kmImgBorderedWidth, kmImgBorderedHeight, false );

	mContourMap = std::unique_ptr<int>( new int[kmImgBorderedWidth * kmImgBorderedHeight] );
}

ContourDetector::~ContourDetector()
{

}

void ContourDetector::processBinaryImageIntoContourBaseMap( ci::Channel8uRef surface, std::unique_ptr<int> &contourMap )
{
	// Set image frame of 1's
	std::memset( contourMap.get(), 1, kmImgBorderedWidth );		// Top Row
	std::memset( contourMap.get() + ( ( kmImgBorderedHeight - 1 ) * kmImgBorderedWidth ), 1, kmImgBorderedWidth );	// Bottom Row

	// Left/right frame
	for ( unsigned int i = 0; i < kmIncomingImgsHeight; ++i )
	{
		contourMap.get()[( i * kmImgBorderedWidth ) + 0] = 1;	// Left Column
		contourMap.get()[( i * kmImgBorderedWidth ) + kmImgBorderedWidth - 1] = 1;	// Right Column
	}

	// Convert into temp surface for show...
	auto chan = ci::Channel8u::create( kmImgBorderedWidth, kmImgBorderedHeight );
	auto iter = chan->getIter();

	auto index = 0;
	while ( iter.line() )
	{
		while ( iter.pixel() )
		{
			iter.v() = mContourMap.get()[index] * 255;

			++index;
		}
	}

	//auto channel = ci::Channel8u::create( kmIncomingImgsWidth + 2, kmIncomingImgsWidth + 2, sizeof( int ) * ( kmIncomingImgsWidth + 2 ), 1, reinterpret_cast<uint8_t*>( *contourMap.get() ) ); //contourMap.get()

	mImageProcessedBordered = ci::Surface8u::create( *chan.get() );

	// Convert into a surface for now

	//ci::Surface8u::create()

}

ci::Surface8uRef ContourDetector::process( ci::Channel8uRef surface )
{
	processBinaryImageIntoContourBaseMap( surface, mContourMap );

	return mImageProcessedBordered;

	// Copy into enlarged surface
	//mImageProcessedBordered->copyFrom( *surface.get(), ci::Area( 0, 0, kmIncomingImgsWidth, kmIncomingImgsHeight ), ci::vec2(1,1) );

	// Zero the contourMap
	std::memset( mContourMap.get(), 0, mImageProcessedBordered->getWidth() * mImageProcessedBordered->getHeight() );

	// find them contours!
	mContourCounter = 1;
	mLatestBorderEncountered = 1;
	mLastPixelValue = mImageProcessedBordered->getPixel( ci::ivec2(0,0) ).r;

	/* At the start of the algorithm, a frame surrounds the entire image with value 1. Create an
	iterator to only iterate over this inner portion of the image. */
	auto partSurfaceIter = ci::Channel8u::Iter( *surface.get(), ci::Area( 1, 1, kmIncomingImgsWidth, kmIncomingImgsHeight ) );

	//auto surfaceIter = mImageProcessedBordered->getIter();

	while ( partSurfaceIter.line() )
	{
		mLatestBorderEncountered = 1;

		while ( partSurfaceIter.pixel() )
		{
			auto x = partSurfaceIter.x();
			auto y = partSurfaceIter.y();



			// if curPixel = 1, and last pixel = 0
			// then curPixel is part of new border
			// increment mContourCounter and start border follow procedure
			// pass last pixel pos to newContourDetected();
			
			//followBorder();

			// if ...



		}
	}

}

void ContourDetector::followBorder( ci::ivec2 const &pos )
{

}

void ContourDetector::processBordersToContours()
{
	for ( auto & contour : mContours )
	{
		std::list<int>::const_iterator begin = contour.mCoordsComplex.begin();
		std::list<int>::const_iterator end = contour.mCoordsComplex.end();

		// In original implementation, Epsilon was:
		// cvContourPerimeter(mInnerContours)*0.02
		// Essentially the length of the contour * 0.02

		std::vector<int> temp;

		// 2 dimentions
		psimpl::simplify_douglas_peucker<2>( begin, end, 3, std::back_inserter( contour.mCoordsSimple ) );
	}
}