/*
* Copyright (c) 2015, Lasse Farnung Laursen - http://www.lasselaursen.com
*
* This file is a part of SimplePose, and subject to the new and revised BSD license.
* Please see the LICENSE file for more information.
* All other rights reserved.
*/

#ifndef SIMPLEPOSE_TAGBITPATTERN
#define SIMPLEPOSE_TAGBITPATTERN

#define BIT_PATTERN_MAX_SIZE 36

#include "Tag.h"
#include "cinder/Noncopyable.h"
#include <vector>
#include <bitset>

struct TagBitPattern : public Tag
{
	TagBitPattern() :
		Tag(),
		kmBitPatternDimSize( 0 )
	{

	}

	TagBitPattern( unsigned short const &bitPatternDimSize, unsigned long long const &id ) :
		Tag( id ),
		kmBitPatternDimSize( bitPatternDimSize )
	{
		mBitPattern = std::bitset<BIT_PATTERN_MAX_SIZE>( id );

		//mBitPattern = std::unique_ptr< bool[] >( new bool[bitPatternSize*bitPatternSize] );
		// Turn ID into bit pattern

		auto test = std::bitset< BIT_PATTERN_MAX_SIZE >( id );

		//mBitPattern2 = std::bitset< 36 >( 22 );

		auto test2 = std::bitset< BIT_PATTERN_MAX_SIZE >( 0 );

		test2[8] = 1;

		//mBitPattern = new bool[bitPatternSize][bitPatternSize];
	}

	// Move Operators!
	TagBitPattern( TagBitPattern&& other ) :
		Tag( std::move( static_cast<Tag&>( other ) ) ),
		kmBitPatternDimSize( other.kmBitPatternDimSize )
	{
	}
	TagBitPattern& operator=( TagBitPattern&& ) { return *this; }

	ci::gl::Texture2dRef getTagAsTexture() override
	{
		ci::Surface8uRef surface = ci::Surface8u::create( 300, 300, false );
		auto surfaceIter = surface->getIter();

		// Draw border
		while ( surfaceIter.line() )
		{
			while ( surfaceIter.pixel() )
			{
				if ( surfaceIter.x() < 50 || surfaceIter.x() >= 250 || surfaceIter.y() < 50 || surfaceIter.y() >= 250 )
				{
					surfaceIter.r() = 0;
					surfaceIter.g() = 0;
					surfaceIter.b() = 255;
				}
			}
		}

		//// Draw bit-pattern
		for ( short bitRow = 0; bitRow < kmBitPatternDimSize; ++bitRow )
		{
			for ( short bitColumn = 0; bitColumn < kmBitPatternDimSize; ++bitColumn )
			{
				if ( getBit( bitColumn, bitRow ) )
				{
					drawSquareOnSurface( surface, bitColumn, bitRow );
				}
			}
		}

		return ci::gl::Texture2d::create( *surface.get() );
	}

	std::bitset<BIT_PATTERN_MAX_SIZE> getRotatedBitPattern( )
	{
		std::bitset<BIT_PATTERN_MAX_SIZE> patternRotated;

		for ( int i = 0; i < kmBitPatternDimSize; ++i ) {
			for ( int j = 0; j < kmBitPatternDimSize; ++j ) {
				patternRotated[( i * kmBitPatternDimSize ) + j] = mBitPattern[((kmBitPatternDimSize - j - 1) * kmBitPatternDimSize) + i];
			}
		}

		return std::move( patternRotated );
	}



	bool getBit( unsigned short const &x, unsigned short const &y )
	{
		return mBitPattern[( y * kmBitPatternDimSize ) + x];
	}

	// Does rotating the tag 90Deg ever result in exactly the same pattern?
	bool isSelfSymmetric() override
	{
		// XXX: fix
		return false;
	}

	std::vector<unsigned long long> getInvalidatedTagIDs() override
	{
		std::vector<unsigned long long> invalidatedTagIDs;

		auto rotatedPattern90 = getRotatedBitPattern();

		auto tag90 = TagBitPattern( kmBitPatternDimSize, rotatedPattern90.to_ulong() );
		auto rotatedPattern180 = tag90.getRotatedBitPattern();

		auto tag180 = TagBitPattern( kmBitPatternDimSize, rotatedPattern180.to_ulong() );
		auto rotatedPattern270 = tag180.getRotatedBitPattern();

		invalidatedTagIDs.emplace_back( rotatedPattern90.to_ulong() );
		invalidatedTagIDs.emplace_back( rotatedPattern180.to_ulong() );
		invalidatedTagIDs.emplace_back( rotatedPattern270.to_ulong() );

		return std::move( invalidatedTagIDs );
	}

	std::bitset<BIT_PATTERN_MAX_SIZE> mBitPattern;

private:

	void drawSquareOnSurface( ci::Surface8uRef surface, short const & bitPosX, short const & bitPosY )
	{
		// A 'bit' ugly, pardon the pun...
		short bitSize = 200 / kmBitPatternDimSize;
		short borderSize = 50;

		short startPixelX, startPixelY;
		startPixelX = borderSize + (bitPosX * bitSize);
		startPixelY = borderSize + (bitPosY * bitSize);

		for ( int y = startPixelY; y < startPixelY + bitSize; ++y )
		{
			for ( int x = startPixelX; x < startPixelX + bitSize; ++x )
			{
				surface->setPixel( ci::ivec2( x, y ), ci::Color8u( 0, 0, 255 ) );
			}
		}
	}

	unsigned short const kmBitPatternDimSize;
	unsigned short const kmBitPatternSizeMax = 6;
};

#endif /* SIMPLEPOSE_TAGBITPATTERN */