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
	void createTestCase2();
	void createTestCase3();

	void printAsASCII();

	inline unsigned int const getWidth() { return kmWidth; };
	inline unsigned int const getHeight() { return kmHeight; };

	enum class NeighborDirectionCW : unsigned char {
		LEFT = 0, UP_LEFT, UP, UP_RIGHT, RIGHT,	DOWN_RIGHT,	DOWN, DOWN_LEFT
	};

	enum class NeighborDirectionCCW : unsigned char {
		LEFT = 0, DOWN_LEFT, DOWN, DOWN_RIGHT, RIGHT, UP_RIGHT, UP,	UP_LEFT
	};

	char const * getCWDirString( NeighborDirectionCW const & cwDir ) { return mNB8CWStr[ static_cast<unsigned char>( cwDir ) ]; }
	char const * getCCWDirString( NeighborDirectionCCW const & ccwDir ) { return mNB8CCWStr[static_cast<unsigned char>( ccwDir )]; }

	/*! Returns a reference to the value of the pixel that the Iter currently points to */
	int& getValue( ci::ivec2 const &pos ) { return mData.get()[posToIndex( pos )]; }
	void setValue( ci::ivec2 const &pos, int const & value ) { mData.get()[posToIndex( pos )] = value; }

	NeighborDirectionCCW getCCWfromCW( NeighborDirectionCW const & cwDir ) { return NeighborDirectionCCW( mNBCWToCCWIndex[static_cast<unsigned char>( cwDir )] ); };

	NeighborDirectionCCW getCCWMinusOnefromCW( NeighborDirectionCW const & cwDir ) { return NeighborDirectionCCW( mNBCWToCCWIndex[( (static_cast<unsigned char>( cwDir ) - 1) % 8 )] ); };

	NeighborDirectionCCW getCCWMinusOnefromCWIndex( char const & cwDirIndex ) { return NeighborDirectionCCW( mNBCWToCCWIndex[( ( cwDirIndex - 1 ) % 8 )] ); };

private:

	void createOuterFrame();

	inline unsigned int const posToIndex( ci::ivec2 const &pos ) { return ( pos.y * kmWidth ) + pos.x; };
	inline unsigned int const posToIndex( unsigned int const & x, unsigned int const & y ) { return ( y * kmWidth ) + x; };

	

	const unsigned int kmWidth, kmHeight;

	// Using ci::Channel seems ideal, but we need signed integers to differentiate between outer/inner borders :/
	std::unique_ptr<int> mData;

	// Left, Up-Left, Up, Up-Right, Right, Down-Right, Down, Down-Left
	std::array<ci::ivec2, 8> const mNB8CW = std::array<ci::ivec2, 8>{ { ci::vec2( -1, 0 ), ci::vec2( -1, -1 ), ci::vec2( 0, -1 ), ci::vec2( 1, -1 ), ci::vec2( 1, 0 ), ci::vec2( 1, 1 ), ci::vec2( 0, 1 ), ci::vec2( -1, 1 ) } };
	std::array<const char *, 8> const mNB8CWStr = std::array<const char *, 8>{{"Left", "Up Left", "Up", "Up Right", "Right", "Down Right", "Down", "Down Left"}};

	// Helper array to go to the CCW direction, from a CW direction!
	std::array<unsigned char, 8> const mNBCWToCCWIndex = std::array<unsigned char, 8>{ { 0, 7, 6, 5, 4, 3, 2, 1 }};
	//std::array<unsigned char, 8> const mNBToCCWIndex = std::array<unsigned char, 8>{ { 4, 3, 2, 1, 0, 7, 6, 5 }};

	// Left, Down-Left, Down, Down-Right, Right, Up-Right, Up, Up-Left
	std::array<ci::ivec2, 8> const mNB8CCW = std::array<ci::ivec2, 8>{ { ci::vec2( -1, 0 ), ci::vec2( -1, 1 ), ci::vec2( 0, 1 ), ci::vec2( 1, 1 ), ci::vec2( 1, 0 ), ci::vec2( 1, -1 ), ci::vec2( 0, -1 ), ci::vec2( -1, -1 ) } };
	std::array<const char *, 8> const mNB8CCWStr = std::array<const char *, 8>{{"Left", "Down Left", "Down", "Down Right", "Right", "Up Right", "Up", "Up Left"}};

	/* Iterators */
public:

	// Iterator for the inner portion of the contour map
	class InnerMapIter {
	public:

		InnerMapIter( ContourMap & contourMap ) :
			mInc( 1 ),
			mRowInc( contourMap.getWidth() )
		{
			mLinePtr = contourMap.mData.get() + contourMap.posToIndex( 1, 1 );
			mPtr = mLinePtr;
			mStartX = mX = 1;
			mStartY = mY = 1;
			
			mEndX = contourMap.getWidth() - 1;
			mEndY = contourMap.getHeight() - 1;

			// in order to be at the right place after an initial call to line(), we need to back up one line
			mY = 1 - 1;
			mLinePtr -= mRowInc;
		}

		/*! Returns a reference to the value of the pixel that the Iter currently points to */
		int& v() const { return mPtr[0]; }

		/*! Returns a reference to the value of the pixel ahead of the one that the Iter currently points to */
		int& nextV() const { return mPtr[1]; }

		/*! Returns a reference to the value of the pixel prior to the one that the Iter currently points to */
		int& lastV() const { return mPtr[-1]; }

		//! Returns the coordinate of the pixel the Iter currently points to		
		ci::ivec2 getPos() const { return ci::ivec2( mX, mY ); }

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
			mPtr = mLinePtr;

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
		NBIter( ContourMap *contourMap, ci::ivec2 const & pos, std::array<ci::ivec2, 8> const & NBs, unsigned char const & startingIndex ) :
			mContourMap( contourMap ),
			mPosCenter( pos ),
			kmNBs( NBs ),
			kmNumNeighbors( NBs.size() ),
			mNumNeighborsRemaining( NBs.size() ),
			mNBVisited( std::unique_ptr<bool>( new bool[NBs.size()] ) ),
			mIndex( startingIndex )
		{
			std::memset( mNBVisited.get(), false, kmNumNeighbors );

			mPtr = &mContourMap->mData.get()[mContourMap->posToIndex( mPosCenter + kmNBs[startingIndex] )];
			--mIndex;					// Back up index a single step as neighbor() will be called once upon prior to accessing values.
			mNumNeighborsRemaining++;	// Ditto for remaining neighbors

		};

		NBIter& operator = ( const NBIter &rhs )
		{
			// Check for self assignment
			if ( this != &rhs )
			{
				mContourMap = rhs.mContourMap;
				kmNumNeighbors = rhs.kmNumNeighbors;
				mNumNeighborsRemaining = rhs.mNumNeighborsRemaining;
				mIndex = rhs.mIndex;
				mPtr = rhs.mPtr;
				mNBVisited = std::unique_ptr<bool>( new bool[kmNumNeighbors] );
				std::memset( mNBVisited.get(), false, kmNumNeighbors );

				mPosCenter = rhs.mPosCenter;
				kmNBs = rhs.kmNBs;
			}
				
			return *this;
		}

		/*! Returns a reference to the value of the pixel that the Iter currently points to */
		int& v() const {
			mNBVisited.get()[mIndex] = true;
			return mPtr[0];
		}

		char const & index() const { return mIndex; }

		bool const & visited( unsigned char const & index ) { return mNBVisited.get()[index]; }

		//! Returns the coordinate of the pixel the Iter currently points to		
		ci::ivec2 getPos() const { return mPosCenter + kmNBs[mIndex]; }

		bool neighbor()
		{
			mIndex = ++mIndex % kmNumNeighbors;
			--mNumNeighborsRemaining;
			mPtr = &mContourMap->mData.get()[mContourMap->posToIndex( mPosCenter + kmNBs[mIndex] )];
			
			return mNumNeighborsRemaining > 0;
		}

		ContourMap			*mContourMap;
		unsigned char		kmNumNeighbors;
		unsigned char		mNumNeighborsRemaining;
		char				mIndex;
		int					*mPtr;
		std::unique_ptr<bool> mNBVisited;


		ci::ivec2			mPosCenter;		// The pixel/pos the surrounding neighborhoods check
		std::array<ci::ivec2, 8> kmNBs;		// This should ideally be CONST, but then making an assignement operator is... Well bleh..
	};



	//! Returns an Iter which iterates the surrounding pixels of pos, in a clockwise order.
	InnerMapIter	getInnerMapIter() { return InnerMapIter( *this ); }

	//! Returns an Iter which iterates the surrounding pixels of pos, in a clockwise order.
	NBIter			getCWNBIter( ci::ivec2 const &pos, NeighborDirectionCW startingDirection ) { return NBIter( this, pos, mNB8CW, static_cast<unsigned char>( startingDirection ) ); }
	//! Returns a ConstIterwhich iterates the surrounding pixels of pos, in a clockwise order.
	//ConstIterNeighborhood	getCWNBIter( ci::ivec2 const &pos ) const { return ConstIterNeighborhood( mData, pos ); }

	//! Returns an Iter which iterates the surrounding pixels of pos, in a counter-clockwise order.
	NBIter			getCCWNBIter( ci::ivec2 const &pos, NeighborDirectionCCW startingDirection ) { return NBIter( this, pos, mNB8CCW, static_cast<unsigned char>( startingDirection ) ); }
	//! Returns a ConstIterwhich iterates the surrounding pixels of pos, in a counter-clockwise order.
	//ConstIterNeighborhood	getCCWNBIter( ci::ivec2 const &pos ) const { return ConstIterNeighborhood( mData, pos ); }

	friend class NBIter;
};

#endif /* CONTOURMAP */