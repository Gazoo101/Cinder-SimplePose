/*
* Copyright (c) 2015, Lasse Farnung Laursen - http://www.lasselaursen.com
*
* This file is a part of SimplePose, and subject to the new and revised BSD license.
* Please see the LICENSE file for more information.
* All other rights reserved.
*/

#define CI_MIN_LOG_LEVEL 4

#include "ContourDetector.h"
#include "ContourMap.h"
#include "Contour.h"

#include "cinder/Log.h"

#include "cinder/gl/draw.h"	// temp
#include "cinder/gl/wrapper.h"

ContourDetector::ContourDetector(unsigned int const & incomingImagesWidth, unsigned int const & incomingImagesHeight) :
	kmIncomingImgsWidth( incomingImagesWidth ),
	kmIncomingImgsHeight( incomingImagesHeight ),
	kmImgBorderedWidth( incomingImagesWidth + 2 ),
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

void ContourDetector::process( ci::Channel8uRef surface )
{
	// Set binary-based 255 channel as new contourmap
	mContourMap->update( surface );

	detectContours();
}

void ContourDetector::detectContours()
{
	mContours.clear();
	CI_LOG_V( "Contour detection: [start]" );

	// find them contours!
	mContourCounter = 1;
	mLatestBorderEncountered = 1;
	CI_LOG_V( "mContourCounter=" << mContourCounter << " | mLatestBorderEncountered=" << mLatestBorderEncountered );

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
					CI_LOG_V( "Contour Detected - Pos: " << innerIter.getPos() << " Type: Outer" );
					mContours.emplace_back( annotateContour( innerIter.getPos(), Contour::TYPE::OUTER, ContourMap::NeighborDirectionCW::LEFT ) );
					//CI_LOG_D( "[Contour added ID = " << mContourCounter << ", outer, containing " << mContours.back().mCoords.size() << " pixels]" );
				}
				else if ( ( pixelValue >= 1 ) && ( nextPixelValue == 0 ) )
				{
					if ( pixelValue > 1 )
					{
						mLatestBorderEncountered = pixelValue;
					}

					CI_LOG_V( "Contour Detected - Pos: " << innerIter.getPos() << " Type: Hole" );
					mContours.emplace_back( annotateContour( innerIter.getPos(), Contour::TYPE::HOLE, ContourMap::NeighborDirectionCW::RIGHT ) );
					//CI_LOG_D( "[Contour added ID = " << mContourCounter << ", hole, containing " << mContours.back().mCoords.size() << " pixels]" );
				}

				// We used the iter here, as annotateContour may have changed the pixel value
				if ( innerIter.v() != 1 )
				{
					mLatestBorderEncountered = std::abs( innerIter.v() );
					CI_LOG_V( "Updated mLatestBorderEncountered=" << mLatestBorderEncountered );
				}

			}
		}
	}

	//mContourMap->printAsASCII();

	CI_LOG_V( "Contour detection: [end]" );
	//return mImageProcessedBordered;
}

Contour ContourDetector::annotateContour( ci::ivec2 const &pos, Contour::TYPE const & borderType, ContourMap::NeighborDirectionCW const & startingDirCW )
{
	// We've discovered a new contour!
	++mContourCounter;
	Contour contour( mContourCounter, 0, borderType );

	// Clock-wise Neighborhood Iterator
	auto cwNBIter = mContourMap->getCWNBIter( pos, startingDirCW );
	CI_LOG_V( "Clock-wise checking pixels around pos " << pos << " starting in direction " << mContourMap->getCWDirString( startingDirCW ) );

	// Visit neighboring pixels in a clock-wise order
	while ( cwNBIter.neighbor() )
	{
		if ( cwNBIter.v() != 0 )
		{
			CI_LOG_V( "Neighboring NON-0 pixel found during iteration at pos " << cwNBIter.getPos() );

			// Remember starting point
			ci::ivec2 const contourStartPos = pos;
			ci::ivec2 const contourStartFirstNonZeroNBPos = cwNBIter.getPos();

			ci::ivec2 ccwContourCrawlCenterPos = contourStartPos;

			// Counter clockwise Neighborhood Iterator
			//contour.addPoint( centerPos );
			auto index = cwNBIter.index();
			auto direction = mContourMap->getCCWMinusOnefromCWIndex( index );
			auto ccwNBIter = mContourMap->getCCWNBIter( ccwContourCrawlCenterPos, direction );
			CI_LOG_V( "Counter clock-wise checking pixels around pos " << ccwContourCrawlCenterPos << " starting in direction " << mContourMap->getCCWDirString( direction ) );

			while ( ccwNBIter.neighbor() )
			{
				if ( ccwNBIter.v() != 0 )
				{
					CI_LOG_V( "Neighboring NON-0 pixel found during iteration at pos " << ccwNBIter.getPos() );
					contour.addPoint( ccwContourCrawlCenterPos );

					// If right neighbor examined and it happened to be a 0-pixel...
					if ( ccwNBIter.visited( static_cast<unsigned char>( ContourMap::NeighborDirectionCCW::RIGHT ) ) && mContourMap->getValue( ccwContourCrawlCenterPos + ci::ivec2( 1, 0 ) ) == 0 )
					{
						CI_LOG_V( "Set value at pos " << ccwContourCrawlCenterPos << " to " << -( mContourCounter ) );
						mContourMap->setValue( ccwContourCrawlCenterPos, -( mContourCounter ) );

					} else if ( mContourMap->getValue( ccwContourCrawlCenterPos ) == 1 )
					{
						CI_LOG_V( "Set value at pos " << ccwContourCrawlCenterPos << " to " << -( mContourCounter ) );
						mContourMap->setValue( ccwContourCrawlCenterPos, mContourCounter );
					}

					bool backAtBeginning = ( contourStartPos == ccwNBIter.getPos() ) && ( contourStartFirstNonZeroNBPos == ccwContourCrawlCenterPos );

					if ( backAtBeginning )
					{
						CI_LOG_V( "[Contour Complete ID = " << mContourCounter << "]" );
						// End

						// Add a point that closes the loop on the Contour
						contour.addPoint( contourStartPos );

						//mContourMap->printAsASCII();
						return std::move( contour );
					}
					
					// If we're not back at the beginning, swap places and restart the search!
					ccwContourCrawlCenterPos = ccwNBIter.getPos();
					auto direction = ContourMap::NeighborDirectionCCW(( ccwNBIter.index() + 5 ) % 8);	// +5 because we need to start our search 1 step past where we just were.

					// Update centerPos and nbPos
					//contour.addPoint( centerPos );
					ccwNBIter = mContourMap->getCCWNBIter( ccwContourCrawlCenterPos, direction );
					CI_LOG_V( "Counter clock-wise checking pixels around pos " << ccwContourCrawlCenterPos << " starting in direction " << mContourMap->getCCWDirString( direction ) );			
				}
			}
			// We should never be able to reach this place. Our initial clockwise scan of a pixel revealed at
			// least ONE non zero neighbor, so we should encounter it no matter what.
			assert( 0 );
		}
	}

	// Single point contour
	contour.addPoint( pos );

	// We did not find a non-0 pixel
	mContourMap->setValue( pos, -(mContourCounter) );
	CI_LOG_V( "[Contour Complete ID = " << mContourCounter << "]" );

	//mContourMap->printAsASCII();
	return std::move( contour );
}

void ContourDetector::drawAllContours()
{
	for ( auto const & contour : mContours )
	{
		contour.draw();
	}
}

void ContourDetector::testContourCalculation()
{
	// Make ContourMap nice and small.
	mContourMap.reset();

	mContourMap = std::unique_ptr<ContourMap>( new ContourMap( 13, 8 ) );
	//mContourMap->createTestCase();
	mContourMap->createTestCase2();
	//mContourMap->createTestCase3();

	detectContours();

	mContourMap->printAsASCII();

	// Reset so test fits intended original
	mContourMap.reset();
	mContourMap = std::unique_ptr<ContourMap>( new ContourMap( kmImgBorderedWidth, kmImgBorderedHeight ) );
}