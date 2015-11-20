/*
* Copyright (c) 2015, Lasse Farnung Laursen - http://www.lasselaursen.com
*
* This file is a part of SimplePose, and subject to the new and revised BSD license.
* Please see the LICENSE file for more information.
* All other rights reserved.
*/

#include "TagBitPattern.h"
#include "image/Polygon.h"

#include "cinder/gl/wrapper.h"
#include "cinder/gl/draw.h"

void TagBitPattern::draw() const
{
	// XXX: We use cinders draw functions to achieve our intended points. This is ok for now,
	// but ideally we should be utilizing a custom shader which, provided with all the coordinates,
	// draws the points on screen.

	ci::gl::color( ci::Color("green") );
	
	for ( auto const & bitLocation : mBitLocations )
	{
		ci::gl::drawSolidCircle( bitLocation, 3.0f );
	}

	ci::gl::color( ci::Color("purple") );
	ci::gl::drawSolidCircle( vecA1Start, 4.0f );

	ci::gl::drawSolidCircle( vecA2Start, 4.0f );

	ci::gl::color( ci::Color("Red"));

	ci::gl::drawLine( vecA1Start, vecA1Start + vecA1Dir );

	ci::gl::drawLine( vecA2Start, vecA2Start + vecA2Dir );

	ci::gl::color( ci::Color( "Yellow" ) );

	for ( auto const & bitVec : bitVecsA )
	{
		ci::gl::drawLine( bitVec.first, bitVec.first + bitVec.second );
	}

	for ( auto const & bitVec : bitVecsB )
	{
		ci::gl::drawLine( bitVec.first, bitVec.first + bitVec.second );
	}

}

ci::vec2 infLineCollision( ci::vec2 lineA1, ci::vec2 lineA2, ci::vec2 lineB1, ci::vec2 lineB2 )
{
	// Inspiration: http://devmag.org.za/2009/04/17/basic-collision-detection-in-2d-part-2/

	float denom = ( ( lineB2.y - lineB1.y ) * ( lineA2.x - lineA1.x ) ) - ( ( lineB2.x - lineB1.x ) * ( lineA2.y - lineA1.y ) );

	if ( denom == 0 )
	{
		return ci::vec2( 0 );	// No collision
	}

	float ua = ( ( ( lineB2.x - lineB1.x ) * ( lineA1.y - lineB1.y ) ) - ( ( lineB2.y - lineB1.y ) *( lineA1.x - lineB1.x ) ) ) / denom;

	//// Next lines only required if lines are of finite length
	//float ub = ( ( line1pos2.x - line1pos1.x ) * ( line1pos1.y - line2pos1.y ) - ( ( line1pos2.y - line1pos1.y ) *( line1pos1.x - line2pos1.x ) ) ) / denom;
	//if ( ua < 0 || ua > 1 || ub < 0 || ub > 1 )
	//{
	//	return ci::vec2( 0 );	// No collision
	//}

	return lineA1 + ua * ( lineA2 - lineA1 );
}

bool TagBitPattern::detect( ci::Channel8uRef binaryImg, Polygon const & potentialTagOutline )
{
	/*
	At this stage, each given polygon is known/expected to have exactly 5 positions/vertices.

	We use standard linear interpolation (based on the complexity of the bit pattern, to determine
	the places in the (binary-)image where we should read a positive or negative bit-value.
	*/

	// XXX: Possibly turn into members, as they are constantly in use?
	ci::vec2 pos1, pos2, pos3, pos4;
	std::vector<ci::vec2>::const_iterator posIter;

	ci::vec2 dirVecA1, dirVecA2, dirVecB1, dirVecB2;

	posIter = potentialTagOutline.mCoords.begin();

	pos1 = *( posIter++ );
	pos2 = *( posIter++ );
	pos3 = *( posIter++ );
	pos4 = *( posIter++ );

	/* The four corner points are in a linear counter-clockwise fashion. We use this knowledge
	to create 4 vectors that move on opposing lines in the same direction. These vectors will
	allow us to easily map the location of the individual bits. */

	dirVecA1 = pos2 - pos1;
	dirVecA2 = pos3 - pos4;

	dirVecB1 = pos3 - pos2;
	dirVecB2 = pos4 - pos1;

	/* For now, we assume that the border surrounding the tag is as thick as the bit squares
	themselves and vice-versa. */

	dirVecA1 /= kmBitPatternDimSize + 2;
	dirVecA2 /= kmBitPatternDimSize + 2;

	dirVecB1 /= kmBitPatternDimSize + 2;
	dirVecB2 /= kmBitPatternDimSize + 2;

	// Determining bit placement is a simple two step process. First, find lines that cross each
	// bit row/column. Then determine intersection points of all lines

	bitVecsA.clear();
	bitVecsB.clear();

	ci::vec2 bitLineAStart = pos1 + (dirVecA1 * 1.5f);
	ci::vec2 bitLineAEnd = pos4 + (dirVecA2 * 1.5f);

	// Debug vissing
	vecA1Start = pos1;
	vecA1Dir = dirVecA1;
	vecA2Start = pos4;
	vecA2Dir = dirVecA2;

	ci::vec2 bitLineBStart = pos2 + (dirVecB1 * 1.5f);
	ci::vec2 bitLineBEnd = pos1 + (dirVecB2 * 1.5f);

	for ( unsigned char bitLineIndex = 0; bitLineIndex < kmBitPatternDimSize; ++bitLineIndex )
	{
		auto bitVecAPair = std::pair<ci::vec2, ci::vec2>( bitLineAStart, bitLineAEnd - bitLineAStart);
		bitVecsA.push_back( bitVecAPair );

		auto bitVecBPair = std::pair<ci::vec2, ci::vec2>( bitLineBStart, bitLineBEnd - bitLineBStart );
		bitVecsB.push_back( bitVecBPair );

		bitLineAStart += dirVecA1;
		bitLineAEnd += dirVecA2;

		bitLineBStart += dirVecB1;
		bitLineBEnd += dirVecB2;
	}


	mBitLocations.clear();

	for ( auto const & vecAPair : bitVecsA )
	{
		for ( auto const & vecBPair : bitVecsB )
		{
			mBitLocations.push_back( infLineCollision( vecAPair.first, vecAPair.first + vecAPair.second, vecBPair.first, vecBPair.first + vecBPair.second ) );
		}
	}

	return true;
}