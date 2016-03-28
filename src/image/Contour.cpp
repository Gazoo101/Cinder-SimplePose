/*
* Copyright (c) 2015, Lasse Farnung Laursen - http://www.lasselaursen.com
*
* This file is a part of SimplePose, and subject to the new and revised BSD license.
* Please see the LICENSE file for more information.
* All other rights reserved.
*/

#include "Contour.h"
#include "cinder/gl/draw.h"
#include "cinder/gl/wrapper.h"

namespace SimplePose {

void Contour::draw() const
{
	ci::gl::color( mColor );
	ci::gl::draw( mPolyLine );
}

float const Contour::calcPerimeter() const
{
	float perimeter = 0;

	if ( mCoords.size() > 1 )
	{
		auto posPrevIter = mCoords.begin();
		for ( auto posIter = ++posPrevIter; posIter != mCoords.end(); ++posIter )
		{

			float dx, dy;

			dx = posIter->x - posPrevIter->x;
			dy = posIter->y - posPrevIter->y;

			perimeter += std::sqrtf( dx * dx + dy * dy );

			posPrevIter = posIter;
		}
	}

	return perimeter;
}

};