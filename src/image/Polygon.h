/*
* Copyright (c) 2015, Lasse Farnung Laursen - http://www.lasselaursen.com
*
* This file is a part of SimplePose, and subject to the new and revised BSD license.
* Please see the LICENSE file for more information.
* All other rights reserved.
*/

#ifndef SIMPLEPOSE_POLYGON
#define SIMPLEPOSE_POLYGON

#include <vector>

#include "cinder/Color.h"
#include "cinder/Rand.h"
#include "cinder/PolyLine.h"

struct Polygon {

	Polygon();
	Polygon( std::vector<int> & coords );
	//Polygon( std::vector<ci::vec2> & coords );
	~Polygon();

	void draw() const;

	bool isConvex();

	float getContainedArea();

	std::vector<ci::vec2> mCoords;

	//std::list<int> mCoordsComplex;
	//std::vector<int> mCoordsSimple;

	// For rendering purposes
	ci::Color mColor;
	ci::PolyLine2 mPolyLine;
};

#endif /* SIMPLEPOSE_POLYGON */