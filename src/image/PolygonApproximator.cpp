/*
* Copyright (c) 2015, Lasse Farnung Laursen - http://www.lasselaursen.com
*
* This file is a part of SimplePose, and subject to the new and revised BSD license.
* Please see the LICENSE file for more information.
* All other rights reserved.
*/

#include "PolygonApproximator.h"
#include "Contour.h"
#include "Polygon.h"

#include "psimpl_v7_src/psimpl.h"

#include "cinder/Log.h"

// For debug purposes
#include "cinder/gl/draw.h"
#include "cinder/gl/wrapper.h"

PolygonApproximator::PolygonApproximator()
{

}

PolygonApproximator::~PolygonApproximator()
{

}

void PolygonApproximator::process( std::vector<Contour> const & contours )
{
	mPolygons.clear();

	auto polygons = approximatePolygonsFromContours( contours );
	mPolygons = filterPolygons( polygons );
}

std::vector<Polygon> PolygonApproximator::approximatePolygonsFromContours( std::vector<Contour> const & contours )
{
	std::vector<int> contourCoords;
	std::vector<int> approximatedPolygonCoords;

	contourCoords.reserve(100);
	approximatedPolygonCoords.reserve(40);

	std::vector<Polygon> approximatedPolygons;

	for ( auto & contour : contours )
	{
		contourCoords.clear();
		approximatedPolygonCoords.clear();

		for ( auto & pos : contour.mCoords )
		{
			contourCoords.push_back( pos.x );
			contourCoords.push_back( pos.y );
		}

		std::vector<int>::const_iterator begin = contourCoords.begin();
		std::vector<int>::const_iterator end = contourCoords.begin();

		// In original implementation, Epsilon was:
		// cvContourPerimeter(mInnerContours)*0.02
		// Essentially the length of the contour * 0.02
		psimpl::simplify_douglas_peucker<2>( begin, end, 2, std::back_inserter( approximatedPolygonCoords ) );

		approximatedPolygons.emplace_back( Polygon( approximatedPolygonCoords ));
	}

	return std::move( approximatedPolygons );
}

std::vector<Polygon> PolygonApproximator::filterPolygons( std::vector<Polygon> const & polygons )
{
	std::vector<Polygon> tagLikePolygons;

	return std::move( tagLikePolygons );
}

void PolygonApproximator::testSimplification()
{
	mTestPoly1.push_back( ci::vec2( 20, 20 ) );
	mTestPoly1.push_back( ci::vec2( 30, 40 ) );
	mTestPoly1.push_back( ci::vec2( 70, 140 ) );
	mTestPoly1.push_back( ci::vec2( 10, 99 ) );

	for ( float x = 30; x < 500; ++x )
	{
		mTestPoly1.push_back( ci::vec2( x, 200 + 200 * sin( x / 100 ) ) );
		CI_LOG_V( "pos " << ci::vec2( x, 200 + 200 * sin( x / 100 ) ) );
	}

	std::list<float> aList;

	for ( auto const & temp : mTestPoly1 )
	{
		aList.push_back( temp.x );
		aList.push_back( temp.y );
		CI_LOG_V( "this a coord?" << temp );
	}

	auto begin = aList.begin();
	auto end = aList.end();

	std::vector<float> aVec;

	psimpl::simplify_douglas_peucker<2>( begin, end, 100, std::back_inserter( aVec ) );

	for ( int vecIndex = 0; vecIndex < aVec.size(); )
	{
		mTestPoly1Reduced.push_back( ci::vec2( aVec[vecIndex], aVec[vecIndex + 1] ) );

		vecIndex += 2;
	}


	mTestPoly2.push_back( ci::vec2( 20, 20 ) );
	mTestPoly2.push_back( ci::vec2( 25, 15 ) );
	mTestPoly2.push_back( ci::vec2( 30, 10 ) );

	mTestPoly2.push_back( ci::vec2( 300, 10 ) );
	mTestPoly2.push_back( ci::vec2( 305, 15 ) );
	mTestPoly2.push_back( ci::vec2( 310, 20 ) );

	mTestPoly2.push_back( ci::vec2( 310, 320 ) );
	mTestPoly2.push_back( ci::vec2( 305, 325 ) );
	mTestPoly2.push_back( ci::vec2( 300, 330 ) );

	mTestPoly2.push_back( ci::vec2( 30, 330 ) );
	mTestPoly2.push_back( ci::vec2( 25, 325 ) );
	mTestPoly2.push_back( ci::vec2( 20, 320 ) );

	mTestPoly2.push_back( ci::vec2( 20, 20 ) );


	std::list<float> aList2;

	for ( auto const & temp : mTestPoly2 )
	{
		aList2.push_back( temp.x );
		aList2.push_back( temp.y );
	}

	auto begin2 = aList2.begin();
	auto end2 = aList2.end();

	std::vector<float> aVec2;

	psimpl::simplify_douglas_peucker<2>( begin2, end2, 50, std::back_inserter( aVec2 ) );

	for ( int vecIndex = 0; vecIndex < aVec2.size(); )
	{
		mTestPoly2Reduced.push_back( ci::vec2( aVec2[vecIndex], aVec2[vecIndex + 1] ) );

		vecIndex += 2;
	}

}

void PolygonApproximator::drawAllPolygons() const
{
	for ( auto const & polygon : mPolygons )
	{
		polygon.draw();
	}
}

void PolygonApproximator::drawTestPolys()
{
	ci::gl::color( ci::Color( "green" ) );
	ci::gl::draw( mTestPoly1 );

	ci::gl::color( ci::Color( "red" ) );
	ci::gl::draw( mTestPoly1Reduced );

	ci::gl::color( ci::Color( "yellow" ) );
	ci::gl::draw( mTestPoly2 );

	ci::gl::color( ci::Color( "purple" ) );
	ci::gl::draw( mTestPoly2Reduced );

}