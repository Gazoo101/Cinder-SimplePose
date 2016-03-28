/*
* Copyright (c) 2015, Lasse Farnung Laursen - http://www.lasselaursen.com
*
* This file is a part of SimplePose, and subject to the new and revised BSD license.
* Please see the LICENSE file for more information.
* All other rights reserved.
*/

#ifndef SIMPLEPOSE_TAG
#define SIMPLEPOSE_TAG

//#include "cinder/Noncopyable.h"
#include <vector>
#include <bitset>

#include "cinder/gl/Texture.h"

namespace SimplePose {

	// Forward declarations
	struct Polygon;

	class Tag
	{

	public:

		Tag() :
			kmId( 0 )
		{

		}

		Tag( unsigned long long const &id ) :
			kmId( id )
		{

		}

		Tag( bool fakery, int debugInt ) :
			kmId( 0 )
		{
			// Currently in clock-wise order... Why?
			//mPosCorners[0] = ci::vec2( 213, 151 );
			//mPosCorners[1] = ci::vec2( 354, 175 );
			//mPosCorners[2] = ci::vec2( 335, 300 );
			//mPosCorners[3] = ci::vec2( 171, 270 );

			// Change to Counter clock-wise - should be same order as the virtual points we've designated
			switch ( debugInt )
			{
			case 0:

				mPosCorners[0] = ci::vec2( 300, 300 );
				mPosCorners[1] = ci::vec2( 300, 100 );
				mPosCorners[2] = ci::vec2( 100, 100 );
				mPosCorners[3] = ci::vec2( 100, 300 );

				break;
			case 1:

				mPosCorners[0] = ci::vec2( 400, 400 );
				mPosCorners[1] = ci::vec2( 400, 200 );
				mPosCorners[2] = ci::vec2( 200, 200 );
				mPosCorners[3] = ci::vec2( 200, 400 );

				break;
			case 2:

				//mPosCorners[0] = ci::vec2(213, 151);
				//mPosCorners[1] = ci::vec2(171, 270);
				//mPosCorners[2] = ci::vec2(335, 300);
				//mPosCorners[3] = ci::vec2(354, 175);

				mPosCorners[0] = ci::vec2( 335, 300 );
				mPosCorners[1] = ci::vec2( 354, 175 );
				mPosCorners[2] = ci::vec2( 213, 151 );
				mPosCorners[3] = ci::vec2( 171, 270 );

				break;
			}


		}

		enum class STATE : unsigned char
		{
			NOT_RECOGNIZED,
			RECOGNIZED
		};

		virtual void draw() const = 0;

		virtual Tag * clone() = 0;
		virtual bool detect( ci::Channel8uRef const& binaryImg, Polygon const & potentialTagOutline ) = 0;

		virtual Tag * cloneIfDetected( ci::Channel8uRef const& binaryImg, Polygon const & potentialTagOutline ) = 0;

		std::array<ci::vec2, 4> const getPosCornersScreencoords() const { return mPosCorners; };

		//// Move Operators!
		//Tag( Tag&& other ) :
		//	kmId( other.kmId )
		//{
		//}
		//Tag& operator=( Tag&& ) { return *this; }

	protected:

		virtual ci::gl::Texture2dRef getTagAsTexture() = 0;

		std::array<ci::vec2, 4> mPosCorners;

		// Does rotating the tag 90Deg ever result in exactly the same pattern?
		//virtual bool isSelfSymmetric() = 0;
		//virtual std::vector<unsigned long long> getInvalidatedTagIDs() = 0;

		unsigned long long const kmId;
	};

};

#endif /* SIMPLEPOSE_TAG */