/*
* Copyright (c) 2015-2016, Lasse Farnung Laursen - http://www.lasselaursen.com
*
* This file is a part of SimplePose, and subject to the new and revised BSD license.
* Please see the LICENSE file for more information.
* All other rights reserved.
*/

#include "Polygon.h"
#include "cinder/gl/draw.h"
#include "cinder/gl/wrapper.h"
#include "cinder/Rand.h"

template <typename T>
inline bool isEven( T value ) {
	return ( value % 2 ) == 0;
}

// Does not work with infinite numbers
inline bool sameSign( float a, float b ) {
	return a*b >= 0.0f;
}

namespace SimplePose {

Polygon::Polygon()
{
	mColor = ci::Color( ci::CM_HSV, ci::vec3( ci::Rand::randFloat(), 1, 1 ) );
}

Polygon::Polygon( std::vector<int> & coords )
{
	assert( isEven( coords.size() ) );

	mColor = ci::Color( ci::CM_HSV, ci::vec3( ci::Rand::randFloat(), 1, 1 ) );

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

bool Polygon::isConvex() const
{
	// Our Contour constructed Polygons are guaranteed to form the boundary. So calculating convexity can be done quite easily:
	// http://stackoverflow.com/questions/471962/how-do-determine-if-a-polygon-is-complex-convex-nonconvex

	// Convexity is guaranteed if we only have three points.
	if ( mCoords.size() > 3 )
	{
		// Init pre-ending iterator positions as we're always checking 3 vertices
		auto posPrevIter = --mCoords.end();
		auto posPrevPrevIter = --posPrevIter;
		float lastZCrossProduct = 0.0f;
		float zCrossProduct = 0.0f;

		for ( auto posIter = mCoords.begin(); posIter != mCoords.end(); ++posIter )
		{
			float dx1, dx2, dy1, dy2;

			dx1 = posPrevIter->x - posPrevPrevIter->x;
			dy1 = posPrevIter->y - posPrevPrevIter->y;

			dx2 = posIter->x - posPrevIter->x;
			dy2 = posIter->y - posPrevIter->y;

			zCrossProduct = dx1*dy2 - dy1*dx2;

			if ( !sameSign( lastZCrossProduct, zCrossProduct ) )
			{
				return false;
			}

			lastZCrossProduct = zCrossProduct;

			// Move all iterators along
			posPrevPrevIter = posPrevIter;
			posPrevIter = posIter;
		}

	}

	return true;
}

float Polygon::getContainedArea() const
{
	float area = 0.0f;

	//if ( mCoords.size() > 1 )
	//{
	//	auto posPrevIter = mCoords.begin();
	//	for ( auto posIter = ++posPrevIter; posIter != mCoords.end(); ++posIter )
	//	{

	//	//	float dx, dy;

	//	//	dx = posIter->x - posPrevIter->x;
	//	//	dy = posIter->y - posPrevIter->y;

	//	//	perimeter += std::sqrtf( dx * dx + dy * dy );

	//	//	posPrevIter = posIter;
	//	}
	//}

	return area;
}

};