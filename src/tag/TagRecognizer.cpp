/*
* Copyright (c) 2015-2016, Lasse Farnung Laursen - http://www.lasselaursen.com
*
* This file is a part of SimplePose, and subject to the new and revised BSD license.
* Please see the LICENSE file for more information.
* All other rights reserved.
*/

#include "TagRecognizer.h"
#include "cinder/Log.h"

#include "tag/Tag.h"
#include "image/Polygon.h"

namespace SimplePose {

	TagRecognizer::TagRecognizer( )
	{

		//generateTags();
	}

	TagRecognizer::~TagRecognizer()
	{

	}

	void TagRecognizer::addTagType( std::unique_ptr<Tag> tagDetector )
	{
		mRecognizedTagsTypes.emplace_back( std::move( tagDetector ) );
	}

	void TagRecognizer::generateTags()
	{
		//// For now generates MAX tags
		//unsigned int tagMaxId = 2^( kmBitPatternSize*kmBitPatternSize );

		//for ( unsigned int tagId = 0; tagId < tagMaxId; ++tagId )
		//{
		//	auto tag = new TagBitPattern( kmBitPatternSize, tagId );

		//	// Only add if Tag is NOT self-symmetric
		//	if ( !tag->isSelfSymmetric() )
		//	{
		//		auto rotatedDupesIds = tag->getInvalidatedTagIDs();

		//		for ( auto &dupeId : rotatedDupesIds )
		//		{
		//			mInvalidTags.push_back( dupeId );
		//		}

		//		//mRecognizedTags.emplace_back( std::move( tag ) );
		//	}
		//}
	}

	ci::Surface8uRef TagRecognizer::getTagTex( unsigned int const &numTags )
	{
		return ci::Surface::create( 32, 32, true );
	}

	std::vector<std::shared_ptr<Tag>> TagRecognizer::process( ci::Channel8uRef binaryImg, std::vector<Polygon> const & squares )
	{
		std::vector<std::shared_ptr<Tag>> detectedTags;

		for ( auto const & knownTagType : mRecognizedTagsTypes )
		{
			for ( auto const & potentialTag : squares )
			{
				auto detectedTag = knownTagType->cloneIfDetected( binaryImg, potentialTag );

				if ( detectedTag )
				{
					detectedTags.emplace_back( std::shared_ptr<Tag>( detectedTag ) );
				}
			}
		}

		// Copy Elision?
		return detectedTags;
	}

};