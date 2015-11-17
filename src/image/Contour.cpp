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

void Contour::draw()
{
	ci::gl::color( mColor );
	ci::gl::draw( mPolyLine );
}