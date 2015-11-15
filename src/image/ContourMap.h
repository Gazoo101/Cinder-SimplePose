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

	inline unsigned int const getWidth() { return kmWidth; };
	inline unsigned int const getHeight() { return kmHeight; };

private:

	void createOuterFrame();

	inline unsigned int const posToIndex( ci::ivec2 const &pos ) { return ( pos.y * kmImgBorderedWidth ) + pos.x; };
	inline unsigned int const posToIndex( unsigned int const & x, unsigned int const & y ) { return ( y * kmImgBorderedWidth ) + x; };

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

		InnerMapIter( ContourMap & contourMap ) :
			mInc( 1 ),
			mRowInc( contourMap.getWidth() )
		{
			mLinePtr = contourMap.mData.get() + mRowInc;
			mPtr = contourMap.mData.get() + mRowInc;
			mStartX = mX = 0;
			mStartY = mY = 0;

			mEndX = contourMap.getWidth() - 1;
			mEndY = contourMap.getHeight() - 1;
			// in order to be at the right place after an initial call to line(), we need to back up one line
			mLinePtr -= mRowInc;

		}

		/*! Returns a reference to the value of the pixel that the Iter currently points to */
		int& v() const { return mPtr[0]; }

		//! Increments which pixel of the current row the Iter points to, and returns \c false when no pixels remain in the current row.
		bool pixel() {
			++mX;
			mPtr += mInc;
			return mX < mEndX;
		}

		//! Increments which row the Iter points to, and returns \c false when no rows remain in the Surface.
		bool line() {
			++mY;
			mLinePtr += mRowInc;
			mPtr = reinterpret_cast<int*>( mLinePtr );
			// in order to be at the right place after an initial call to pixel(), we need to back up one pixel
			mPtr -= mInc;
			mX = mStartX - 1;
			return mY < mEndY;
		}

	private:

		// Optimize later
		unsigned char		mInc;
		int					*mLinePtr;
		int					*mPtr;
		unsigned int		mRowInc, mWidth, mHeight;
		int32_t				mX, mY, mStartX, mStartY, mEndX, mEndY;
	};

	// Iterator for a pixels surrounding neighbors.
	class NBIter {
	public:
		NBIter( ContourMap & contourMap, ci::ivec2 const & pos, std::array<ci::ivec2, 8> const & NBs ) :
			mPosCenter( pos ),
			kmNBs( NBs )
		{
			mPtr = contourMap.mData.get() + mRowInc;
			//ContourDetector::mNB8CW[];
		};

		int					*mPtr;
		ci::ivec2 const mPosCenter;	// The pixel/pos the surrounding neighborhoods check
		std::array<ci::ivec2, 8> const kmNBs;
	};



	//! Returns an Iter which iterates the surrounding pixels of pos, in a clockwise order.
	InnerMapIter	getInnerMapIter() { return InnerMapIter( *this ); }

	//! Returns an Iter which iterates the surrounding pixels of pos, in a clockwise order.
	NBIter			getCWNBIter( ci::ivec2 const &pos ) { return NBIter( mData, pos ); }
	//! Returns a ConstIterwhich iterates the surrounding pixels of pos, in a clockwise order.
	//ConstIterNeighborhood	getCWNBIter( ci::ivec2 const &pos ) const { return ConstIterNeighborhood( mData, pos ); }

	//! Returns an Iter which iterates the surrounding pixels of pos, in a counter-clockwise order.
	//NBIter			getCCWNBIter( ci::ivec2 const &pos ) { return IterNeighborhood( mData, pos ); }
	//! Returns a ConstIterwhich iterates the surrounding pixels of pos, in a counter-clockwise order.
	//ConstIterNeighborhood	getCCWNBIter( ci::ivec2 const &pos ) const { return ConstIterNeighborhood( mData, pos ); }

	friend class NBIter;
};

#endif /* CONTOURMAP */