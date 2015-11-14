/*
* Copyright (c) 2015, Lasse Farnung Laursen - http://www.lasselaursen.com
*
* This file is a part of SimplePose, and subject to the new and revised BSD license.
* Please see the LICENSE file for more information.
* All other rights reserved.
*/

/*
	Data structure to contain the integer-based contour map. The class is also responsible
	for providing neighborhood traversing iterators around pixel areas. Currently only 8-way
	pixel iteration is supporte. No love fof 4-way for now I'm afraid.
*/

#ifndef CONTOURMAP
#define CONTOURMAP

#include <memory>
#include <array>
#include "cinder/Vector.h"
#include "cinder/Channel.h"

class ContourMap {

public:

	ContourMap( unsigned int const & width, unsigned int const & height );
	// Create from existing channel/surface
	ContourMap( ci::Channel8uRef binaryImg );

	~ContourMap();

	void update( ci::Channel8uRef binaryImg );

	void createTestCase();

	void printAsASCII();

private:

	void createOuterFrame();

	const unsigned int kmWidth, kmHeight;

	// Using ci::Channel seems ideal, but we need signed integers to differentiate between outer/inner borders :/
	std::unique_ptr<int> mData;

	// Left, Up-Left, Up, Up-Right, Right, Down-Right, Down, Down-Left
	std::array<ci::ivec2, 8> const mNB8CW = std::array<ci::ivec2, 8>{ { ci::vec2( -1, 0 ), ci::vec2( -1, -1 ), ci::vec2( 0, -1 ), ci::vec2( 1, -1 ), ci::vec2( 1, 0 ), ci::vec2( 1, 1 ), ci::vec2( -1, 1 ) } };

	// Left, Down-Left, Down, Down-Right, Right, Up-Right, Up, Up-Left
	std::array<ci::ivec2, 8> const mNB8CCW = std::array<ci::ivec2, 8>{ { ci::vec2( -1, 0 ), ci::vec2( -1, 1 ), ci::vec2( 1, 1 ), ci::vec2( 1, 0 ), ci::vec2( 1, -1 ), ci::vec2( 0, -1 ), ci::vec2( -1, -1 ) } };

	/* Iterators */
public:

	// Iterator for the inner portion of the contour map
	class InnerMapIter {
	public:
	};

	// Iterator for a pixels surrounding neighbors.
	class NBIter {
	public:
		IterNeighborhood( std::unique_ptr<int> & contourMapData, ci::ivec2 const & pos )
		{
			//ContourDetector::mNB8CW[];
		};
	};

	//class ConstIterNeighborhood {
	//public:
	//	ConstIterNeighborhood( std::unique_ptr<int> const & contourMapData, ci::ivec2 const & pos )
	//	{};
	//};

	//! Returns an Iter which iterates the surrounding pixels of pos, in a clockwise order.
	IterNeighborhood		getCWNBIter( ci::ivec2 const &pos ) { return IterNeighborhood( mData, pos ); }
	//! Returns a ConstIterwhich iterates the surrounding pixels of pos, in a clockwise order.
	//ConstIterNeighborhood	getCWNBIter( ci::ivec2 const &pos ) const { return ConstIterNeighborhood( mData, pos ); }

	//! Returns an Iter which iterates the surrounding pixels of pos, in a counter-clockwise order.
	IterNeighborhood		getCCWNBIter( ci::ivec2 const &pos ) { return IterNeighborhood( mData, pos ); }
	//! Returns a ConstIterwhich iterates the surrounding pixels of pos, in a counter-clockwise order.
	//ConstIterNeighborhood	getCCWNBIter( ci::ivec2 const &pos ) const { return ConstIterNeighborhood( mData, pos ); }
};

#endif /* CONTOURMAP */