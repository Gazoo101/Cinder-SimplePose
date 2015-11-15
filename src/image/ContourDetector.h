/*
* Copyright (c) 2015, Lasse Farnung Laursen - http://www.lasselaursen.com
*
* This file is a part of SimplePose, and subject to the new and revised BSD license.
* Please see the LICENSE file for more information.
* All other rights reserved.
*/

#ifndef CONTOURDETECTOR
#define CONTOURDETECTOR

#include "cinder/Surface.h"
#include "Contour.h"	// Required due to enum
#include "ContourMap.h"	// Required due to enum

class ContourDetector {

public:

	ContourDetector( unsigned int const & incomingImagesWidth, unsigned int const & incomingImagesHeight );
	~ContourDetector();

	ci::Surface8uRef process( ci::Channel8uRef surface );

	ci::Channel8uRef getDebugImg() { return mImageDebug; };

	void testProcess();

private:

	Contour annotateContour( ci::ivec2 const &pos, Contour::TYPE const & borderType, ContourMap::NeighborDirectionCW const & startingNeighbor );

	void processBordersToContours();

	inline unsigned int const posToIndex( ci::ivec2 const &pos ) { return ( pos.y * kmImgBorderedWidth ) + pos.x; };
	inline unsigned int const posToIndex( unsigned int const & x, unsigned int const & y ) { return ( y * kmImgBorderedWidth ) + x; };

	unsigned int mContourCounter;
	unsigned int mLatestBorderEncountered;
	//unsigned int mLastPixelValue;

	ci::Surface8uRef mImageProcessed;
	ci::Surface8uRef mImageProcessedBordered;

	ci::Channel8uRef mImageDebug;

	std::unique_ptr<ContourMap> mContourMap;
	std::vector<Contour> mContours;

	//std::vector<ci::ivec2> mNB8CW, mNB8CCW, mNB4CW, mNB4CCW;

	unsigned int const kmIncomingImgsWidth, kmIncomingImgsHeight;
	unsigned int const kmImgBorderedWidth, kmImgBorderedHeight;

};

#endif /* CONTOURDETECTOR */