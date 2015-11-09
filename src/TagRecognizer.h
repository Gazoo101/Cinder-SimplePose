/*
* Copyright (c) 2015, Lasse Farnung Laursen - http://www.lasselaursen.com
*
* This file is a part of SimplePose, and subject to the new and revised BSD license.
* Please see the LICENSE file for more information.
* All other rights reserved.
*/

/*

TagRecognizer takes care of knowing what sized tags we can recognize (bit pattern),

*/

#ifndef TAG_RECOGNIZER
#define TAG_RECOGNIZER

#include "cinder/Surface.h"

struct Tag;

class TagRecognizer {

public:

	TagRecognizer( unsigned char const &bitPatternSize );
	~TagRecognizer();

	ci::Surface8uRef getTagTex( unsigned int const &numTags );

private:

	void generateTags();

	std::vector<std::unique_ptr<Tag>> mRecognizedTags;
	//std::vector<Tag> mRecognizedTags;
	std::vector<unsigned int> mInvalidTags;

	unsigned char const kmBitPatternSize;

	static unsigned char const kmMaxBitPattern = 6;
	static unsigned char const kmMinBitPattern = 2;

};

#endif /* TAG_RECOGNIZER */