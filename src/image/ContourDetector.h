/*
* Copyright (c) 2015, Lasse Farnung Laursen - http://www.lasselaursen.com
*
* This file is a part of SimplePose, and subject to the new and revised BSD license.
* Please see the LICENSE file for more information.
* All other rights reserved.
*/

#ifndef CONTOURDETECTOR
#define CONTOURDETECTOR

#include "cinder/Surface.h"

class ContourDetector {

public:

	ContourDetector( unsigned int const & incomingImagesWidth, unsigned int const & incomingImagesHeight );
	~ContourDetector();

	ci::Surface8uRef process( ci::Surface8uRef surface );

private:


	unsigned int const kmIncomingImgsWidth, kmIncomingImgsHeight;

};

#endif /* CONTOURDETECTOR */