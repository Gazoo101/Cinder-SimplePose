/*
* Copyright (c) 2015, Lasse Farnung Laursen - http://www.lasselaursen.com
*
* This file is a part of SimplePose, and subject to the new and revised BSD license.
* Please see the LICENSE file for more information.
* All other rights reserved.
*/

#include "TagRecognizer.h"
#include "TagBitPattern.h"
#include "cinder/Log.h"

TagRecognizer::TagRecognizer( unsigned char const &bitPatternSize ) :
kmBitPatternSize( bitPatternSize )
{
	if ( kmBitPatternSize > kmMaxBitPattern )
	{
		CI_LOG_W( "Selected bit Pattern size (" << kmBitPatternSize << ") larger than max (" << kmMaxBitPattern << "), detection disabled." );
		return;
	}
	else if ( kmBitPatternSize < kmMinBitPattern )
	{
		CI_LOG_W( "Selected bit Pattern size (" << kmBitPatternSize << ") smaller than min (" << kmMinBitPattern << "), detection disabled." );
		return;
	}

	generateTags();
}

TagRecognizer::~TagRecognizer()
{

}

void TagRecognizer::generateTags()
{
	// For now generates MAX tags
	unsigned int tagMaxId = 2^( kmBitPatternSize*kmBitPatternSize );

	for ( unsigned int tagId = 0; tagId < tagMaxId; ++tagId )
	{
		auto tag = new TagBitPattern( kmBitPatternSize, tagId );

		// Only add if Tag is NOT self-symmetric
		if ( !tag->isSelfSymmetric() )
		{
			auto rotatedDupesIds = tag->getInvalidatedTagIDs();

			for ( auto &dupeId : rotatedDupesIds )
			{
				mInvalidTags.push_back( dupeId );
			}

			mRecognizedTags.emplace_back( std::move( tag ) );
		}
	}
}

ci::Surface8uRef TagRecognizer::getTagTex( unsigned int const &numTags )
{
	return ci::Surface::create( 32, 32, true );
}