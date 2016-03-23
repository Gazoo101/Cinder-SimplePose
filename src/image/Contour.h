/*
* Copyright (c) 2015, Lasse Farnung Laursen - http://www.lasselaursen.com
*
* This file is a part of SimplePose, and subject to the new and revised BSD license.
* Please see the LICENSE file for more information.
* All other rights reserved.
*/

#ifndef SIMPLEPOSE_CONTOUR_H_INCLUDED
#define SIMPLEPOSE_CONTOUR_H_INCLUDED

#include <list>
#include "cinder/Color.h"
#include "cinder/Rand.h"
#include "cinder/PolyLine.h"

struct Contour {

	enum class TYPE : unsigned char {
		OUTER,
		HOLE,
		UNDEFINED
	};

	Contour() :
		mId(0),
		mParentContourId( 0 ),
		mType( TYPE::UNDEFINED )
	{
		// Each contour should ideally have its own unique color
		mColor = ci::Color( ci::CM_HSV, ci::vec3( ci::Rand::randFloat(), 1, 1) );
	};

	Contour( int const & id, int const & parentId, TYPE const & type ) :
		mId( id ),
		mParentContourId( parentId ),
		mType( type )
	{
		// Each contour should ideally have its own unique color
		mColor = ci::Color( ci::CM_HSV, ci::vec3( ci::Rand::randFloat(), 1, 1 ) );
	};

	void addPoint( ci::ivec2 const & pos )
	{
		mCoords.push_back( pos );
		mPolyLine.push_back( pos );
		//mCoordsComplex.push_back( pos.x );
		//mCoordsComplex.push_back( pos.y );
	};

	float const calcPerimeter() const;

	void draw() const;

	int const mId;
	int const mParentContourId;

	TYPE const mType;

	std::vector<ci::vec2> mCoords;

	//std::list<int> mCoordsComplex;
	//std::vector<int> mCoordsSimple;

	// For rendering purposes
	ci::Color mColor;
	ci::PolyLine2 mPolyLine;

};

#endif /* SIMPLEPOSE_CONTOUR_H_INCLUDED */