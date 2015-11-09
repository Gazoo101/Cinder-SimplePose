/*
* Copyright (c) 2015, Lasse Farnung Laursen - http://www.lasselaursen.com
*
* This file is a part of SimplePose, and subject to the new and revised BSD license.
* Please see the LICENSE file for more information.
* All other rights reserved.
*/

#ifndef TAGBITPATTERN
#define TAGBITPATTERN

#include "Tag.h"
#include "cinder/Noncopyable.h"
#include <vector>
#include <bitset>

struct TagBitPattern : public Tag
{
	TagBitPattern() :
		Tag(),
		kmBitPatternSize( 0 )
	{

	}

	TagBitPattern( unsigned char const &bitPatternSize, unsigned short const &id ) :
		Tag( id ),
		kmBitPatternSize( bitPatternSize )
	{
		//mBitPattern = std::unique_ptr< bool[] >( new bool[bitPatternSize*bitPatternSize] );
		// Turn ID into bit pattern

		auto test = std::bitset< 32 >( id );

		//mBitPattern2 = std::bitset< 36 >( 22 );

		auto test2 = std::bitset< 32 >( 0 );

		test2[8] = 1;

		//mBitPattern = new bool[bitPatternSize][bitPatternSize];
	}

	// Move Operators!
	TagBitPattern( TagBitPattern&& other ) :
		Tag(std::move( static_cast<Tag&>( other ) )),
		kmBitPatternSize( other.kmBitPatternSize )
	{
	}
	TagBitPattern& operator=( TagBitPattern&& ) { return *this; }

	void getRotatedBitPattern()
	{
		//mBitPattern2[22][2];

		//// Rotating the bitpattern / matrix

		//int[, ] ret = new int[n, n];

		//for ( int i = 0; i < n; ++i ) {
		//	for ( int j = 0; j < n; ++j ) {
		//		ret[i, j] = matrix[n - j - 1, i];
		//	}
		//}

		//return ret;
	}



	bool getBit( unsigned char const &x, unsigned char const &y )
	{
		//return mBitPattern.get()[( y * kmBitPatternSize ) + x];
	}

	// Does rotating the tag 90Deg ever result in exactly the same pattern?
	bool isSelfSymmetric() override
	{
		return true;
	}

	std::vector<unsigned short> getInvalidatedTagIDs() override
	{
		std::vector<unsigned short> temp;
		return temp;
	}

	std::bitset<36> mBitPattern;

	//bool mBitPattern;
	//std::unique_ptr<bool[]> mBitPattern;

	unsigned char const kmBitPatternSize;
	unsigned char const kmBitPatternSizeMax = 6;
};

#endif /* TAGBITPATTERN */