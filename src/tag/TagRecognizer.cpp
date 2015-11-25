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

#include "image/Polygon.h"

TagRecognizer::TagRecognizer( unsigned short const &bitPatternSize ) :
kmBitPatternSize( bitPatternSize )
{
	/* Each recognizable Tag Type is added here. */
	mRecognizedTagsTypes.emplace_back( std::unique_ptr<TagBitPattern>( new TagBitPattern( bitPatternSize ) ) );

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

std::vector<std::unique_ptr<Tag>> TagRecognizer::process( ci::Channel8uRef binaryImg, std::vector<Polygon> const & squares )
{
	mDetectedTags.clear();

	for ( auto const & knownTagType : mRecognizedTagsTypes )
	{
		// Clone the detection blueprint
		auto tagDetectionBlueprint = knownTagType->clone();

		for ( auto const & potentialTag : squares )
		{
			auto tagDetected = tagDetectionBlueprint->detect( binaryImg, potentialTag );

			if ( tagDetected )
			{
				// detect() will have ordered and saved the corner locations of the tag upon detection!

				// Blueprint is now a valid tag
				mDetectedTags.push_back( std::unique_ptr<Tag>( tagDetectionBlueprint ) );

				// Make another blueprint available
				tagDetectionBlueprint = knownTagType->clone();
			}
		}

	}

	return std::move( mDetectedTags );
}

void TagRecognizer::drawDetectedTags()
{
	for ( auto const & detectedTag : mDetectedTags )
	{
		detectedTag->draw();
	}
}