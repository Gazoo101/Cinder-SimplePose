/*
* Copyright (c) 2015-2016, Lasse Farnung Laursen - http://www.lasselaursen.com
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
#include <memory>
#include <typeindex>

#include "cinder/gl/Texture.h"

namespace SimplePose {

	// Forward declarations
	struct Polygon;

	struct TagId
	{
		TagId() :
			kmType( std::type_index( typeid( nullptr ) ) ),
			kmId( 0 ) {};

		TagId( std::type_index type, unsigned long long id ) :
			kmType( type ),
			kmId( id ) {};

		unsigned long long const kmId;
		std::type_index const kmType;

		bool operator==( TagId const& other ) const
		{
			return ( kmType == other.kmType &&
				kmId == other.kmId );
		}

	};

	class Tag : public std::enable_shared_from_this<Tag>
	{

	public:

		Tag() :
			mTagId( )
		{

		}

		Tag( std::type_index type, unsigned long long const &id ) :
			mTagId( type, id )
		{

		}

		enum class STATE : unsigned char
		{
			NOT_RECOGNIZED,
			RECOGNIZED
		};

		unsigned long long getId() const { return mTagId.kmId; };
		std::type_index getType() const { return mTagId.kmType; };
		TagId getTagId() const { return mTagId; };

		virtual void draw() const = 0;

		virtual Tag * clone() = 0;
		virtual bool detect( ci::Channel8uRef const& binaryImg, Polygon const & potentialTagOutline ) = 0;

		virtual Tag * cloneIfDetected( ci::Channel8uRef const& binaryImg, Polygon const & potentialTagOutline ) = 0;

		std::array<ci::vec2, 4> const getPosCornersScreencoords() const { return mPosCorners; };

		virtual void update( std::shared_ptr<Tag> const& detectedTag )
		{
			// Fix
			mPosCorners = detectedTag->mPosCorners;
		}

		//// Move Operators!
		//Tag( Tag&& other ) :
		//	kmId( other.kmId )
		//{
		//}
		//Tag& operator=( Tag&& ) { return *this; }

	protected:

		virtual ci::gl::Texture2dRef getTagAsTexture() = 0;

		std::array<ci::vec2, 4> mPosCorners;

		TagId mTagId;

		// Does rotating the tag 90Deg ever result in exactly the same pattern?
		//virtual bool isSelfSymmetric() = 0;
		//virtual std::vector<unsigned long long> getInvalidatedTagIDs() = 0;

		//unsigned long long const kmId;
	};

};

// Define Hash function for TagId
namespace std {

	template <>
	struct hash<SimplePose::TagId>
	{
		std::size_t operator()( SimplePose::TagId const& k ) const
		{
			using std::size_t;
			using std::hash;
			using std::string;

			// Compute individual hash values for first,
			// second and third and combine them using XOR
			// and bit shifting:

			return ( hash<std::type_index>()( k.kmType )
				^ ( hash<unsigned long long>()( k.kmId ) ) << 1 );
		}
	};

}

#endif /* SIMPLEPOSE_TAG */