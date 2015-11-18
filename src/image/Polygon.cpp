/*
* Copyright (c) 2015, Lasse Farnung Laursen - http://www.lasselaursen.com
*
* This file is a part of SimplePose, and subject to the new and revised BSD license.
* Please see the LICENSE file for more information.
* All other rights reserved.
*/

#include "Polygon.h"
#include "cinder/gl/draw.h"
#include "cinder/gl/wrapper.h"

template <typename T>
inline bool isEven( T value ) {
	return ( value % 2 ) == 0;
}

Polygon::Polygon()
{

}

Polygon::Polygon( std::vector<int> & coords )
{
	assert( isEven( coords.size() ) );

	for ( int vecIndex = 0; vecIndex < coords.size(); )
	{
		auto vec2 = ci::vec2( coords[vecIndex], coords[vecIndex + 1] );
		mCoords.push_back( vec2 );
		mPolyLine.push_back( vec2 );
		vecIndex += 2;
	}

}

Polygon::~Polygon()
{

}

void Polygon::draw() const
{
	ci::gl::color( mColor );
	ci::gl::draw( mPolyLine );
}

bool Polygon::isConvex()
{

	return true;
}

float Polygon::getContainedArea()
{
	return 0.0f;
}