/*
* Copyright (c) 2015, Lasse Farnung Laursen - http://www.lasselaursen.com
*
* This file is a part of SimplePose, and subject to the new and revised BSD license.
* Please see the LICENSE file for more information.
* All other rights reserved.
*/

#ifndef TAG
#define TAG

#include "cinder/Noncopyable.h"
#include <vector>
#include <bitset>

struct Tag : private cinder::Noncopyable
{
	Tag() :
		kmId( 0 )
	{

	}

	Tag( unsigned short const &id ) :
		kmId( id )
	{

	}

	// Move Operators!
	Tag( Tag&& other ) :
		kmId( other.kmId )
	{
	}
	Tag& operator=( Tag&& ) { return *this; }



	// Does rotating the tag 90Deg ever result in exactly the same pattern?
	virtual bool isSelfSymmetric() = 0;
	virtual std::vector<unsigned short> getInvalidatedTagIDs() = 0;

	unsigned short const kmId;
};

#endif /* TAG */