/*
* Copyright (c) 2015, Lasse Farnung Laursen - http://www.lasselaursen.com
*
* This file is a part of SimplePose, and subject to the new and revised BSD license.
* Please see the LICENSE file for more information.
* All other rights reserved.
*/

#ifndef SIMPLEPOSE_POLYGONAPPROXIMATOR
#define SIMPLEPOSE_POLYGONAPPROXIMATOR

#include <vector>
#include "cinder/PolyLine.h"
struct Contour;

class PolygonApproximator {

public:

	PolygonApproximator();
	~PolygonApproximator();

	void process( std::vector<Contour> const & contours );

	void testSimplification();

	void drawTestPolys();

private:

	ci::PolyLine2f mTestPoly1, mTestPoly1Reduced;
	ci::PolyLine2f mTestPoly2, mTestPoly2Reduced;
};

#endif /* SIMPLEPOSE_POLYGONAPPROXIMATOR */