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
	// Neighborhood searchers

	// 8-Neighborhood Search
	mNB8CW.reserve( 8 );
	mNB8CCW.reserve( 8 );
	// Left, Up-Left, Up, Up-Right, Right, Down-Right, Down, Down-Left
	mNB8CW = { ci::vec2( -1, 0 ), ci::vec2( -1, -1 ), ci::vec2( 0, -1 ), ci::vec2( 1, -1 ), ci::vec2( 1, 0 ), ci::vec2( 1, 1 ), ci::vec2( -1, 1 ) };
	// Left, Down-Left, Down, Down-Right, Right, Up-Right, Up, Up-Left,
	mNB8CCW = { ci::vec2( -1, 0 ), ci::vec2( -1, 1 ), ci::vec2( 1, 1 ), ci::vec2( 1, 0 ), ci::vec2( 1, -1 ), ci::vec2( 0, -1 ), ci::vec2( -1, -1 ), };

	// 4-Neighborhood Search
	mNB4CW.reserve( 4 );
	mNB4CCW.reserve( 4 );

	// Left, Up, Right, Down
	mNB4CW = { ci::vec2( -1, 0 ), ci::vec2( 0, -1 ), ci::vec2( 1, 0 ), ci::vec2( 0, 1 ) };
	// Left, Down, Right, Up
	mNB4CCW = { ci::vec2( -1, 0 ), ci::vec2( 0, 1 ), ci::vec2( 1, 0 ), ci::vec2( 0, -1 ) };


	std::vector<double> cpp11vector2 = { 32.0, 6.003, -0.1 };
	std::vector<ci::vec2> test = { ci::vec2( 0, 0 ), ci::vec2( 3, 4 ) };


	mImageProcessed = ci::Surface8u::create( kmIncomingImgsWidth, kmIncomingImgsHeight, false );
	mImageProcessedBordered = ci::Surface8u::create( kmImgBorderedWidth, kmImgBorderedHeight, false );
	mImageDebug = ci::Channel8u::create( kmImgBorderedWidth, kmImgBorderedHeight );

	//auto chanIter = mImageDebug->getIter();

	//while ( chanIter.line() )
	//{
	//	while ( chanIter.pixel() )
	//	{
	//		chanIter.v() = 0;// ci::randInt( 0, 255 );
	//	}
	//}

	mContourMap = std::unique_ptr<int>( new int[kmImgBorderedWidth * kmImgBorderedHeight] );
}

ContourDetector::~ContourDetector()
{

}

void ContourDetector::processBinaryImageIntoContourBaseMap( ci::Channel8uRef surface, std::unique_ptr<int> &contourMap )
{
	// Top / Bottom
	for ( unsigned int i = 0; i < kmImgBorderedWidth; ++i )
	{
		contourMap.get()[i] = 1;															// Top
		contourMap.get()[( kmImgBorderedWidth * ( kmImgBorderedHeight - 1 ) ) + i] = 1;		// Bottom
	}

	// Left / Right
	for ( unsigned int i = 1; i < kmImgBorderedHeight - 1; ++i )
	{
		contourMap.get()[( i * kmImgBorderedWidth ) + 0] = 1;	// Left Column
		contourMap.get()[( i * kmImgBorderedWidth ) + kmImgBorderedWidth - 1] = 1;	// Right Column
	}

	// Copy channel on to center of contour map
	auto surfaceIter = surface->getIter();

	while ( surfaceIter.line() )
	{
		while ( surfaceIter.pixel() )
		{
			contourMap.get()[ (kmImgBorderedWidth * (surfaceIter.y() + 1)) + 1 + surfaceIter.x() ] = surfaceIter.v() / 255;
		}
	}

	int test = contourMap.get()[ (kmImgBorderedWidth * kmImgBorderedHeight) - 1];


	// Convert into temp surface for show...
	auto iter = mImageDebug->getIter();

	auto index = 0;
	while ( iter.line() )
	{
		while ( iter.pixel() )
		{
			iter.v() = mContourMap.get()[index] * 255;

			++index;
		}
	}
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
	//mLastPixelValue = mImageProcessedBordered->getPixel( ci::ivec2(0,0) ).r;

	/* At the start of the algorithm, a frame surrounds the entire image with value 1. Create an
	iterator to only iterate over this inner portion of the image. */
	//auto partSurfaceIter = ci::Channel8u::Iter( *surface.get(), ci::Area( 1, 1, kmIncomingImgsWidth, kmIncomingImgsHeight ) );

	//auto surfaceIter = mImageProcessedBordered->getIter();


	mLastPixelValue = mContourMap.get()[0];

	// There's no need to check the image frame, thus go from 1 to imgDim -1
	for ( unsigned int y = 1; y < kmImgBorderedHeight - 1; ++y )
	{
		mLatestBorderEncountered = 1;

		for ( unsigned int x = 1; x < kmImgBorderedWidth - 1; ++x )
		{
			int pixelValue = mContourMap.get()[ posToIndex( x, y ) ];

			if ( pixelValue != 0 )
			{
				if ( ( pixelValue == 1 ) && ( mLastPixelValue == 0 ) )
				{
					annotateContour( ci::ivec2( x, y ), Contour::TYPE::OUTER );

				}
				else if ( ( pixelValue >= 1 ) && ( mContourMap.get()[posToIndex( x+1, y )] == 0) )
				{
					annotateContour( ci::ivec2( x, y ), Contour::TYPE::HOLE );
				}
				else if ( pixelValue != 1 )
				{
					mLatestBorderEncountered = std::abs( pixelValue );
				}
				else {
					// We should NEVER reach this
					assert( 0 );
				}

			}

			//posToIndex( x, y );

			mLastPixelValue = pixelValue;
		}
	}

}

void ContourDetector::annotateContour( ci::ivec2 const &pos, Contour::TYPE borderType )
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