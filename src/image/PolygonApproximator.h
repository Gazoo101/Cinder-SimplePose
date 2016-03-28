/*
* Copyright (c) 2015, Lasse Farnung Laursen - http://www.lasselaursen.com
*
* This file is a part of SimplePose, and subject to the new and revised BSD license.
* Please see the LICENSE file for more information.
* All other rights reserved.
*/

#ifndef SIMPLEPOSE_POLYGONAPPROXIMATOR_H_INCLUDED
#define SIMPLEPOSE_POLYGONAPPROXIMATOR_H_INCLUDED

#include <vector>
#include "cinder/PolyLine.h"

namespace SimplePose {

// Forward Declarations
struct Contour;
struct Polygon;

class PolygonApproximator {

public:

	PolygonApproximator();
	~PolygonApproximator();

	void process( std::vector<Contour> const & contours );

	void testSimplification();

	void drawAllPolygons() const;

	void drawTestPolys();

	std::vector<Polygon> const & getPolygons() const { return mPolygons; }

private:

	std::vector<Polygon> approximatePolygonsFromContours( std::vector<Contour> const & contours );
	std::vector<Polygon> filterPolygons( std::vector<Polygon> const & polygons );

	std::vector<Polygon> mPolygons;

	ci::PolyLine2f mTestPoly1, mTestPoly1Reduced;
	ci::PolyLine2f mTestPoly2, mTestPoly2Reduced;
};

};

#endif /* SIMPLEPOSE_POLYGONAPPROXIMATOR_H_INCLUDED */