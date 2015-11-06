/*
* Copyright (c) 2015, Lasse Farnung Laursen - http://www.lasselaursen.com
*
* This file is a part of SimplePose, and subject to the new and revised BSD license.
* Please see the LICENSE file for more information.
* All other rights reserved.
*/

#include "TagRecognizer.h"
#include "Tag.h"

TagRecognizer::TagRecognizer( unsigned char const &bitPatternSize ) :
kmBitPatternSize( bitPatternSize )
{

}

TagRecognizer::~TagRecognizer()
{

}

ci::Surface8uRef getTagTex( unsigned int const &numTags )
{
	
}