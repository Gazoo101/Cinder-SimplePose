/*
* Copyright (c) 2015, Lasse Farnung Laursen - http://www.lasselaursen.com
*
* This file is a part of SimplePose, and subject to the new and revised BSD license.
* Please see the LICENSE file for more information.
* All other rights reserved.
*/

#ifndef SIMPLEPOSE_TAG
#define SIMPLEPOSE_TAG

#include "cinder/Noncopyable.h"
#include <vector>
#include <bitset>

#include "cinder/gl/Texture.h"

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

	enum class STATE : unsigned char
	{
		NOT_RECOGNIZED,
		RECOGNIZED
	};

	virtual void draw() const = 0;

	virtual Tag * clone() = 0;
	virtual bool detect( ci::Channel8uRef binaryImg, Polygon const & potentialTagOutline ) = 0;

	//// Move Operators!
	//Tag( Tag&& other ) :
	//	kmId( other.kmId )
	//{
	//}
	//Tag& operator=( Tag&& ) { return *this; }

private:

	virtual ci::gl::Texture2dRef getTagAsTexture() = 0;

	// Does rotating the tag 90Deg ever result in exactly the same pattern?
	virtual bool isSelfSymmetric() = 0;
	virtual std::vector<unsigned long long> getInvalidatedTagIDs() = 0;

	unsigned long long const kmId;
};

#endif /* SIMPLEPOSE_TAG */