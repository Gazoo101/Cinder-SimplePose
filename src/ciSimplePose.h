/*
* Copyright (c) 2015, Lasse Farnung Laursen - http://www.lasselaursen.com
*
* This file is a part of SimplePose, and subject to the new and revised BSD license.
* Please see the LICENSE file for more information.
* All other rights reserved.
*/

#ifndef CISIMPLEPOSE
#define CISIMPLEPOSE

#include "TagRecognizer.h"
#include "cinder/Surface.h"

class CiSimplePose {
	
public:

	CiSimplePose();
	~CiSimplePose();

	//getCameraMatrix();	- Todo. Use OpenCV for now.

	ci::Surface8uRef getTagTex( unsigned int const &numTags ) { return mTagRecognizer->getTagTex( numTags ); };
	
private:


	std::unique_ptr<TagRecognizer> mTagRecognizer;
};

#endif /* CISIMPLEPOSE */