/*
* Copyright (c) 2015, Lasse Farnung Laursen - http://www.lasselaursen.com
*
* This file is a part of SimplePose, and subject to the new and revised BSD license.
* Please see the LICENSE file for more information.
* All other rights reserved.
*/

#include "ContourDetector.h"
#include "Contour.h"

#include "cinder/Log.h"	// Temp
#include <sstream>

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

	CI_LOG_I("Crash city");
}

ContourDetector::~ContourDetector()
{

}

void ContourDetector::createContourMapFrame( std::unique_ptr<int> &contourMap, unsigned int const & width, unsigned int const & height )
{
	// Top / Bottom
	for ( unsigned int i = 0; i < width; ++i )
	{
		contourMap.get()[i] = 1;									// Top
		contourMap.get()[( width * ( height - 1 ) ) + i] = 1;		// Bottom
	}

	// Left / Right
	for ( unsigned int i = 1; i < height - 1; ++i )
	{
		contourMap.get()[( i * width ) + 0] = 1;			// Left Column
		contourMap.get()[( i * width ) + width - 1] = 1;	// Right Column
	}
}

void ContourDetector::processBinaryImageIntoContourBaseMap( ci::Channel8uRef surface, std::unique_ptr<int> &contourMap )
{
	createContourMapFrame( contourMap, kmImgBorderedWidth, kmImgBorderedHeight );

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

void ContourDetector::testProcess()
{
	// Make ContourMap nice and small.
	mContourMap.reset();
	
	unsigned int smallWidth, smallHeight;
	unsigned int x, y;
	int index;
	smallWidth = 13;
	smallHeight = 8;

	// 12 x 12
	mContourMap = std::unique_ptr<int>( new int[smallWidth * smallHeight] );
	std::memset( mContourMap.get(), 0, smallWidth * smallHeight * sizeof( int ) );

	createContourMapFrame( mContourMap, smallWidth, smallHeight );

	// Draw a little whole contents thingy
	//
	// 1 1 1 1 1 1 1
	// 1     1     1   1
	// 1     1     1
	// 1 1 1 1 1 1 1

	// Draw start scenario!
	x = 2;
	y = 2;

	index = ( smallWidth * y ) + x;
	for ( unsigned int i = 0; i < 7; ++i )
	{
		mContourMap.get()[index + i] = 1;
	}

	x = 2;
	y = 5;

	index = ( smallWidth * y ) + x;
	for ( unsigned int i = 0; i < 7; ++i )
	{
		mContourMap.get()[index + i] = 1;
	}

	// Individual 1's
	mContourMap.get()[( smallWidth * 3 ) + 2] = 1;
	mContourMap.get()[( smallWidth * 3 ) + 5] = 1;
	mContourMap.get()[( smallWidth * 3 ) + 8] = 1;
	mContourMap.get()[( smallWidth * 3 ) + 10] = 1;

	mContourMap.get()[( smallWidth * 4 ) + 2] = 1;
	mContourMap.get()[( smallWidth * 4 ) + 5] = 1;
	mContourMap.get()[( smallWidth * 4 ) + 8] = 1;

	printContourMapAscii( mContourMap, smallWidth, smallHeight );
}

void ContourDetector::printContourMapAscii( std::unique_ptr<int> &contourMap, unsigned int const & width, unsigned int const & height )
{
	CI_LOG_V( "--------------------------" );
	std::stringstream ss;

	for ( unsigned int y = 0; y < height; ++y )
	{
		for ( unsigned int x = 0; x < width; ++x )
		{
			int value = contourMap.get()[(width * y) + x];

			if ( value >= 0 )
			{
				ss << " " << value;
			}
			else {
				ss << value;
			}
		}
		// Print line
		CI_LOG_V( ss.str() );
		ss.str( "" );
	}
}

ci::Surface8uRef ContourDetector::process( ci::Channel8uRef surface )
{
	mContours.clear();

	// Fully overwrites
	processBinaryImageIntoContourBaseMap( surface, mContourMap );

	// find them contours!
	mContourCounter = 1;
	mLatestBorderEncountered = 1;
	//mLastPixelValue = mImageProcessedBordered->getPixel( ci::ivec2(0,0) ).r;

	/* At the start of the algorithm, a frame surrounds the entire image with value 1. Create an
	iterator to only iterate over this inner portion of the image. */
	//auto partSurfaceIter = ci::Channel8u::Iter( *surface.get(), ci::Area( 1, 1, kmIncomingImgsWidth, kmIncomingImgsHeight ) );

	//auto surfaceIter = mImageProcessedBordered->getIter();


	int lastPixelValue, pixelValue, nextPixelValue;

	// There's no need to check the image frame, thus go from 1 to imgDim -1
	for ( unsigned int y = 1; y < kmImgBorderedHeight - 1; ++y )
	{
		mLatestBorderEncountered = 1;

		lastPixelValue = mContourMap.get()[ posToIndex( 0, y ) ];
		pixelValue = mContourMap.get()[ posToIndex( 1, y ) ];
		nextPixelValue = mContourMap.get()[ posToIndex( 2, y ) ];

		for ( unsigned int x = 1; x < kmImgBorderedWidth - 1; ++x )
		{

			if ( pixelValue != 0 )
			{
				if ( ( pixelValue == 1 ) && ( lastPixelValue == 0 ) )
				{
					mContours.emplace_back( annotateContour( ci::ivec2( x, y ), Contour::TYPE::OUTER ) );
				}
				else if ( ( pixelValue >= 1 ) && ( nextPixelValue == 0 ) )
				{
					if ( pixelValue > 1 )
					{
						mLatestBorderEncountered = pixelValue;
					}

					mContours.emplace_back( annotateContour( ci::ivec2( x, y ), Contour::TYPE::HOLE ) );
				}
				
				if ( pixelValue != 1 )
				{
					mLatestBorderEncountered = std::abs( pixelValue );
				}
				else {
					// We should NEVER reach this
					assert( 0 );
				}

			}

			// Normally, we should check that we don't go out of bounds here
			// but we'll never execute on the last line in the image, so we'll reference
			// existing data, and I'll bet (for now) that the if statement is potentially
			// more costly than the look-up function.
			lastPixelValue = pixelValue;
			pixelValue = nextPixelValue;
			nextPixelValue = mContourMap.get()[posToIndex( x + 2, y )];
		}
	}

	return mImageProcessedBordered;
}

Contour ContourDetector::annotateContour( ci::ivec2 const &pos, Contour::TYPE borderType )
{
	// We've discovered a new contour!
	++mContourCounter;

	Contour contour( 0, 0 );






	return std::move( contour );
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