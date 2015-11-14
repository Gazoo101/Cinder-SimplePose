/*
* Copyright (c) 2015, Lasse Farnung Laursen - http://www.lasselaursen.com
*
* This file is a part of SimplePose, and subject to the new and revised BSD license.
* Please see the LICENSE file for more information.
* All other rights reserved.
*/

#include "ContourMap.h"

#include "cinder/Log.h"
#include <sstream>

ContourMap::ContourMap( unsigned int const & width, unsigned int const & height ) :
	kmWidth( width ),
	kmHeight( height ),
	mData( std::unique_ptr<int>( new int[width * height] ) )
{
	// Clear ContourMap Data
	std::memset( mData.get(), 0, kmWidth * kmHeight * sizeof( int ) );
	createOuterFrame();

};

// Create from existing channel/surface
ContourMap::ContourMap( ci::Channel8uRef binaryImg ) :
	kmWidth( binaryImg->getWidth() + 2 ),
	kmHeight( binaryImg->getHeight() + 2 ),
	mData( std::unique_ptr<int>( new int[( binaryImg->getWidth() + 2 ) * ( binaryImg->getHeight() + 2 )] ) )
{
	// Clear ContourMap Data
	std::memset( mData.get(), 0, kmWidth * kmHeight * sizeof( int ) );
	createOuterFrame();

	update( binaryImg );

};

ContourMap::~ContourMap()
{

}

void ContourMap::update( ci::Channel8uRef binaryImg )
{
	// Copy channel on to center of contour map
	auto iter = binaryImg->getIter();

	while ( iter.line() )
	{
		while ( iter.pixel() )
		{
			mData.get()[( kmWidth * ( iter.y() + 1 ) ) + 1 + iter.x()] = iter.v() / 255;
		}
	}
}

void ContourMap::createTestCase()
{
	// 1 1 1 1 1 1 1
	// 1     1     1   1
	// 1     1     1
	// 1 1 1 1 1 1 1

	unsigned int x, y, index;

	// Draw start scenario!
	x = 2;
	y = 2;

	index = ( kmWidth * y ) + x;
	for ( unsigned int i = 0; i < 7; ++i )
	{
		mData.get()[index + i] = 1;
	}

	x = 2;
	y = 5;

	index = ( kmWidth * y ) + x;
	for ( unsigned int i = 0; i < 7; ++i )
	{
		mData.get()[index + i] = 1;
	}

	// Individual 1's
	mData.get()[( kmWidth * 3 ) + 2] = 1;
	mData.get()[( kmWidth * 3 ) + 5] = 1;
	mData.get()[( kmWidth * 3 ) + 8] = 1;
	mData.get()[( kmWidth * 3 ) + 10] = 1;

	mData.get()[( kmWidth * 4 ) + 2] = 1;
	mData.get()[( kmWidth * 4 ) + 5] = 1;
	mData.get()[( kmWidth * 4 ) + 8] = 1;
}

void ContourMap::printAsASCII()
{
	CI_LOG_V( "--------------------------" );
	std::stringstream ss;

	for ( unsigned int y = 0; y < kmHeight; ++y )
	{
		for ( unsigned int x = 0; x < kmWidth; ++x )
		{
			int value = mData.get()[( kmWidth * y ) + x];

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

void ContourMap::createOuterFrame()
{
	// Top / Bottom
	for ( unsigned int i = 0; i < kmWidth; ++i )
	{
		mData.get()[i] = 1;										// Top
		mData.get()[( kmWidth * ( kmHeight - 1 ) ) + i] = 1;	// Bottom
	}

	// Left / Right
	for ( unsigned int i = 1; i < kmHeight - 1; ++i )
	{
		mData.get()[( i * kmWidth ) + 0] = 1;			// Left Column
		mData.get()[( i * kmWidth ) + kmWidth - 1] = 1;	// Right Column
	}
}