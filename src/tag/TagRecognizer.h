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

#ifndef SIMPLEPOSE_TAG_RECOGNIZER
#define SIMPLEPOSE_TAG_RECOGNIZER

#include "cinder/Surface.h"

namespace SimplePose {

	class Tag;
	struct Polygon;

	class TagRecognizer {

	public:

		TagRecognizer( );
		~TagRecognizer();

		std::vector<std::shared_ptr<Tag>> process( ci::Channel8uRef binaryImg, std::vector<Polygon> const & squares );

		void addTagType( std::unique_ptr<Tag> tagDetector );

		ci::Surface8uRef getTagTex( unsigned int const &numTags );

		void drawDetectedTags();

	private:

		void generateTags();

		std::vector<std::unique_ptr<Tag>> mRecognizedTagsTypes;
		//std::vector<std::unique_ptr<Tag>> mDetectedTags;

		//std::vector<Tag> mRecognizedTags;
		std::vector<unsigned long long> mInvalidTags;

	};

};

#endif /* SIMPLEPOSE_TAG_RECOGNIZER */