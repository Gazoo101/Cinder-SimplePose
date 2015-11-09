/*
* Copyright (c) 2015, Lasse Farnung Laursen - http://www.lasselaursen.com
*
* This file is a part of SimplePose, and subject to the new and revised BSD license.
* Please see the LICENSE file for more information.
* All other rights reserved.
*/

#include "ciSimplePose.h"

CiSimplePose::CiSimplePose()
{
	// RiAA principles

	mTagRecognizer = std::unique_ptr<TagRecognizer>( new TagRecognizer(4) );


}

CiSimplePose::~CiSimplePose()
{


}

