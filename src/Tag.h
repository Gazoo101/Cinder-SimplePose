/*
* Copyright (c) 2015, Lasse Farnung Laursen - http://www.lasselaursen.com
*
* This file is a part of SimplePose, and subject to the new and revised BSD license.
* Please see the LICENSE file for more information.
* All other rights reserved.
*/

#ifndef TAG
#define TAG

#include <vector>

struct Tag
{
	Tag( unsigned char const &bitPatternSize, unsigned short const &id ) :
		kmBitPatternSize( bitPatternSize ),
		kmId( id ),
		mBitPattern( new bool[bitPatternSize*bitPatternSize] )
	{
		// Create bit layout
		auto array = new bool[3][3]();

		//mBitPattern = new bool[bitPatternSize][bitPatternSize];
	}

	bool getBit( unsigned char const &x, unsigned char const &y)
	{
		return mBitPattern.get() + ( y * kmBitPatternSize ) + x;
	}

	// Does rotating the tag 90Deg ever result in exactly the same pattern?
	bool isSelfSymmetric()
	{

	}

	std::vector<unsigned short> getInvalidatedTagIDs()
	{

	}

	//bool mBitPattern;
	std::unique_ptr<bool[]> mBitPattern;

	unsigned short const kmId;
	unsigned char const kmBitPatternSize;
};

#endif /* TAG */