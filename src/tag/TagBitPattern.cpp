/*
* Copyright (c) 2015, Lasse Farnung Laursen - http://www.lasselaursen.com
*
* This file is a part of SimplePose, and subject to the new and revised BSD license.
* Please see the LICENSE file for more information.
* All other rights reserved.
*/

#include "TagBitPattern.h"
#include "image/Polygon.h"

long long TagBitPattern::detect( ci::Channel8uRef binaryImg, Polygon const & potentialTagOutline )
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
	dirVecB1 = pos3 - pos2;

	dirVecA2 = pos3 - pos4;
	dirVecB2 = pos4 - pos1;

	/* For now, we assume that the border surrounding the tag is as thick as the bit squares
	themselves and vice-versa. */

	dirVecA1 /= kmBitPatternDimSize + 2;
	dirVecA2 /= kmBitPatternDimSize + 2;

	dirVecB1 /= kmBitPatternDimSize + 2;
	dirVecB2 /= kmBitPatternDimSize + 2;






	return 0;
}