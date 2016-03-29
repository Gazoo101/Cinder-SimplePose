/*
* Copyright (c) 2015-2016, Lasse Farnung Laursen - http://www.lasselaursen.com
*
* This file is a part of SimplePose, and subject to the new and revised BSD license.
* Please see the LICENSE file for more information.
* All other rights reserved.
*/

#ifndef SIMPLEPOSE_TAGBITPATTERN
#define SIMPLEPOSE_TAGBITPATTERN

#include "cinder/Log.h"
#include "Tag.h"
//#include "cinder/Noncopyable.h"
#include <vector>
#include <bitset>

#define BIT_PATTERN_MAX_SIZE 36

namespace SimplePose {

	class TagBitPattern : public Tag
	{
	public:

		// Fake remove soon
		TagBitPattern( ) :
			Tag( ),
			mBitPatternValue( 0 ),
			kmBitPatternDimSize( 4 )
		{

		}

		TagBitPattern( unsigned short const &bitPatternDimSize ) :
			TagBitPattern( bitPatternDimSize, 0 ) {}

		TagBitPattern( unsigned short const &bitPatternDimSize, unsigned long long const &id ) :
			Tag( std::type_index( typeid( TagBitPattern ) ), id ),
			mBitPatternValue( 0 ),
			kmBitPatternDimSize( bitPatternDimSize )
		{
			if ( kmBitPatternDimSize > kmBitPatternSizeMax )
			{
				CI_LOG_W( "Selected bit Pattern size (" << kmBitPatternDimSize << ") larger than max (" << kmBitPatternSizeMax << "), detection disabled." );
				return;
			}
			else if ( kmBitPatternDimSize < kmBitPatternSizeMax )
			{
				CI_LOG_W( "Selected bit Pattern size (" << kmBitPatternDimSize << ") smaller than min (" << kmBitPatternSizeMax << "), detection disabled." );
				return;
			}

			mBitPattern = std::bitset<BIT_PATTERN_MAX_SIZE>( id );
		}

		virtual void draw() const override;

		virtual Tag * clone() override { return new TagBitPattern( *this ); };
		virtual bool detect( ci::Channel8uRef const& binaryImg, Polygon const & potentialTagOutline ) override;

		virtual Tag * cloneIfDetected( ci::Channel8uRef const& binaryImg, Polygon const & potentialTagOutline );

		//// Move Operators!
		//TagBitPattern( TagBitPattern&& other ) :
		//	Tag( std::move( static_cast<Tag&>( other ) ) ),
		//	kmBitPatternDimSize( other.kmBitPatternDimSize )
		//{
		//}
		//TagBitPattern& operator=( TagBitPattern&& ) { return *this; }

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

		std::bitset<BIT_PATTERN_MAX_SIZE> getRotatedBitPattern( std::bitset<BIT_PATTERN_MAX_SIZE> const & bitPattern )
		{
			std::bitset<BIT_PATTERN_MAX_SIZE> patternRotated;

			for ( int i = 0; i < kmBitPatternDimSize; ++i ) {
				for ( int j = 0; j < kmBitPatternDimSize; ++j ) {
					patternRotated[( i * kmBitPatternDimSize ) + j] = bitPattern[( ( kmBitPatternDimSize - j - 1 ) * kmBitPatternDimSize ) + i];
				}
			}

			return std::move( patternRotated );
		}



		bool getBit( unsigned short const &x, unsigned short const &y )
		{
			return mBitPattern[( y * kmBitPatternDimSize ) + x];
		}

		//// Does rotating the tag 90Deg ever result in exactly the same pattern?
		//bool isSelfSymmetric() override
		//{
		//	// XXX: fix
		//	return false;
		//}

		//std::vector<unsigned long long> getInvalidatedTagIDs() override
		//{
		//	std::vector<unsigned long long> invalidatedTagIDs;

		//	auto rotatedPattern90 = getRotatedBitPattern();

		//	auto tag90 = TagBitPattern( kmBitPatternDimSize, rotatedPattern90.to_ulong() );
		//	auto rotatedPattern180 = tag90.getRotatedBitPattern();

		//	auto tag180 = TagBitPattern( kmBitPatternDimSize, rotatedPattern180.to_ulong() );
		//	auto rotatedPattern270 = tag180.getRotatedBitPattern();

		//	invalidatedTagIDs.emplace_back( rotatedPattern90.to_ulong() );
		//	invalidatedTagIDs.emplace_back( rotatedPattern180.to_ulong() );
		//	invalidatedTagIDs.emplace_back( rotatedPattern270.to_ulong() );

		//	return std::move( invalidatedTagIDs );
		//}

	private:

		unsigned long long mBitPatternValue;
		std::bitset<BIT_PATTERN_MAX_SIZE> mBitPattern;

		void drawSquareOnSurface( ci::Surface8uRef surface, short const & bitPosX, short const & bitPosY )
		{
			// A 'bit' ugly, pardon the pun...
			short bitSize = 200 / kmBitPatternDimSize;
			short borderSize = 50;

			short startPixelX, startPixelY;
			startPixelX = borderSize + ( bitPosX * bitSize );
			startPixelY = borderSize + ( bitPosY * bitSize );

			for ( int y = startPixelY; y < startPixelY + bitSize; ++y )
			{
				for ( int x = startPixelX; x < startPixelX + bitSize; ++x )
				{
					surface->setPixel( ci::ivec2( x, y ), ci::Color8u( 0, 0, 255 ) );
				}
			}
		}


		ci::vec2 vecA1Start, vecA1Dir, vecA2Start, vecA2Dir;// , vecADir, vecBStart, vecBDir;

		std::vector<std::pair<ci::vec2, ci::vec2>> bitVecsA, bitVecsB;

		std::vector<ci::vec2> mBitLocations;

		unsigned short const kmBitPatternDimSize;

		static unsigned short const kmBitPatternSizeMax = 6;
		static unsigned short const kmBitPatternSizeMin = 2;
	};

};

#endif /* SIMPLEPOSE_TAGBITPATTERN */