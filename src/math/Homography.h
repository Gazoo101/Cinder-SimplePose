/*
* Copyright (c) 2015, Lasse Farnung Laursen - http://www.lasselaursen.com
*
* This file is a part of SimplePose, and subject to the new and revised BSD license.
* Please see the LICENSE file for more information.
* All other rights reserved.
*/

#include "cinder/Vector.h"
#include "cinder/Matrix.h"

class Homography {

public:

	ci::mat4 getPerspectiveTransform( const ci::vec2 src[4], const ci::vec2 dst[4] ) const;
	ci::mat3 getHomography( const ci::vec2 src[4], const ci::vec2 dst[4] ) const;
	void gaussianElimination( float *a, int n ) const;
};