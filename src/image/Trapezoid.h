/*
* Copyright (c) 2015-2016, Lasse Farnung Laursen - http://www.lasselaursen.com
*
* This file is a part of SimplePose, and subject to the new and revised BSD license.
* Please see the LICENSE file for more information.
* All other rights reserved.
*/

#ifndef SIMPLEPOSE_TRAPEZOID_H_INCLUDED
#define SIMPLEPOSE_TRAPEZOID_H_INCLUDED

#include "cinder/Vector.h"

//#include "cinder/Color.h"
//#include "cinder/PolyLine.h"

namespace SimplePose {

	struct Polygon;

	struct Trapezoid {

		Trapezoid( Polygon const& polygon );
		//Polygon( std::vector<int> & coords );
		//Polygon( std::vector<ci::vec2> & coords );
		~Trapezoid();

		void draw() const;

		//bool isConvex() const;

		//inline bool isSquare() const { return mCoords.size() == 5; };

		//float getContainedArea() const;

		//std::vector<ci::vec2> mCoords;

		//// For rendering purposes
		//ci::Color mColor;
		//ci::PolyLine2 mPolyLine;

		ci::vec2 p1, p2, p3, p4;
	};

};

#endif /* SIMPLEPOSE_TRAPEZOID_H_INCLUDED */