/*
* Copyright (c) 2015, Lasse Farnung Laursen - http://www.lasselaursen.com
*
* This file is a part of SimplePose, and subject to the new and revised BSD license.
* Please see the LICENSE file for more information.
* All other rights reserved.
*/

#include "ContourDetector.h"
#include "ContourMap.h"
#include "Contour.h"

//#include "cinder/Log.h"	// Temp
//#include <sstream>

#include "psimpl_v7_src/psimpl.h"

ContourDetector::ContourDetector(unsigned int const & incomingImagesWidth, unsigned int const & incomingImagesHeight) :
	kmIncomingImgsWidth( incomingImagesWidth ),
	kmIncomingImgsHeight( incomingImagesHeight ),
	kmImgBorderedWidth( incomingImagesWidth + 2),
	kmImgBorderedHeight( incomingImagesHeight + 2 )
{
	mContourMap = std::unique_ptr<ContourMap>( new ContourMap( kmImgBorderedWidth, kmImgBorderedHeight) );

	mImageProcessed = ci::Surface8u::create( kmIncomingImgsWidth, kmIncomingImgsHeight, false );
	mImageProcessedBordered = ci::Surface8u::create( kmImgBorderedWidth, kmImgBorderedHeight, false );
	mImageDebug = ci::Channel8u::create( kmImgBorderedWidth, kmImgBorderedHeight );
}

ContourDetector::~ContourDetector()
{

}


void ContourDetector::testProcess()
{
	// Make ContourMap nice and small.
	mContourMap.reset();

	mContourMap = std::unique_ptr<ContourMap>( new ContourMap( 13, 8 ) );
	mContourMap->createTestCase();

	mContourMap->printAsASCII();

	// Random UnitTests!
	//auto innerIter = mContourMap->getInnerMapIter();

	//while ( innerIter.line() )
	//{
	//	mLatestBorderEncountered = 1;

	//	auto apos2 = innerIter.getPos();

	//	while ( innerIter.pixel() )
	//	{

	//		auto apos3 = innerIter.getPos();

	//		if ( innerIter.v() == 1 )
	//		{
	//			auto apos = innerIter.getPos();

	//			auto nbIter = mContourMap->getCCWNBIter( innerIter.getPos(), ContourMap::NeighborDirectionCCW::LEFT );

	//			int temp = 0;
	//			while ( nbIter.neighbor() )
	//			{
	//				// Make em 3s!
	//				nbIter.v() = ++temp;
	//			}

	//			mContourMap->printAsASCII();
	//		}


	//		innerIter.v() = 2;
	//	}
	//}

	//mContourMap->printAsASCII();
}

ci::Surface8uRef ContourDetector::process( ci::Channel8uRef surface )
{
	mContours.clear();

	// Set binary-based 255 channel as new contourmap
	//mContourMap->update( surface ); // cancelled for testProcess

	// find them contours!
	mContourCounter = 1;
	mLatestBorderEncountered = 1;

	int lastPixelValue, pixelValue, nextPixelValue;

	auto innerIter = mContourMap->getInnerMapIter();

	while ( innerIter.line() )
	{
		mLatestBorderEncountered = 1;

		while ( innerIter.pixel() )
		{
			lastPixelValue = innerIter.lastV();
			pixelValue = innerIter.v();
			nextPixelValue = innerIter.nextV();

			if ( pixelValue != 0 )
			{
				if ( ( pixelValue == 1 ) && ( lastPixelValue == 0 ) )
				{
					mContours.emplace_back( annotateContour( innerIter.getPos(), Contour::TYPE::OUTER, ContourMap::NeighborDirectionCW::LEFT ) );
				}
				else if ( ( pixelValue >= 1 ) && ( nextPixelValue == 0 ) )
				{
					if ( pixelValue > 1 )
					{
						mLatestBorderEncountered = pixelValue;
					}

					mContours.emplace_back( annotateContour( innerIter.getPos(), Contour::TYPE::HOLE, ContourMap::NeighborDirectionCW::RIGHT ) );
				}
			
				// We used the iter here, as annotateContour may have changed the pixel value
				if ( innerIter.v() != 1 )
				{
					mLatestBorderEncountered = std::abs( innerIter.v() );
				}
				//else {
				//	// We should NEVER reach this
				//	assert( 0 );
				//}

			}
		}
	}

	mContourMap->printAsASCII();


	return mImageProcessedBordered;
}

Contour ContourDetector::annotateContour( ci::ivec2 const &pos, Contour::TYPE const & borderType, ContourMap::NeighborDirectionCW const & startingNeighbor )
{
	// We've discovered a new contour!
	++mContourCounter;

	auto nbIter = mContourMap->getCWNBIter( ci::ivec2( 0, 0 ), startingNeighbor );

	while ( nbIter.neighbor() )
	{
		if ( nbIter.v() != 0 )
		{
			// Do some stuff
		}
	}

	// We did not find a non-0 pixel
	mContourMap->setValue( pos, -mContourCounter );



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