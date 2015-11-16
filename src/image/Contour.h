/*
* Copyright (c) 2015, Lasse Farnung Laursen - http://www.lasselaursen.com
*
* This file is a part of SimplePose, and subject to the new and revised BSD license.
* Please see the LICENSE file for more information.
* All other rights reserved.
*/

#ifndef CONTOUR
#define CONTOUR

#include <list>
#include "cinder/Color.h"
#include "cinder/Rand.h"

struct Contour {

	Contour() :
		mId(0),
		mParentContourId( 0 )
	{
		// Each contour should ideally have its own unique color
		mColor = ci::Color( ci::CM_HSV, ci::vec3( ci::Rand::randFloat(), 1, 1) );
	};

	Contour( int const & id, int const & parentId ) :
		mId( id ),
		mParentContourId( parentId )
	{
		// Each contour should ideally have its own unique color
		mColor = ci::Color( ci::CM_HSV, ci::vec3( ci::Rand::randFloat(), 1, 1 ) );
	};

	~Contour()
	{
	};

	enum TYPE {
		OUTER,
		HOLE
	};

	void addPoint( ci::ivec2 const & pos ) { mCoords.push_back( pos ); };

	int const mId;
	int const mParentContourId;

	std::vector<ci::ivec2> mCoords;

	std::list<int> mCoordsComplex;
	std::vector<int> mCoordsSimple;

	ci::Color mColor;

};

#endif /* CONTOUR */