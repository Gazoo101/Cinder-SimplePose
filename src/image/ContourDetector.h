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

struct Contour;

class ContourDetector {

public:

	ContourDetector( unsigned int const & incomingImagesWidth, unsigned int const & incomingImagesHeight );
	~ContourDetector();

	ci::Surface8uRef process( ci::Channel8uRef surface );

private:

	void processBinaryImageIntoContourBaseMap( ci::Channel8uRef channel, std::unique_ptr<int> &contourMap );

	void followBorder( ci::ivec2 const &pos );

	void processBordersToContours();

	inline unsigned int posToIndex( ci::ivec2 const &pos ) { return ( pos.y * kmImgBorderedWidth ) + pos.x; };

	unsigned int mContourCounter;
	unsigned int mLatestBorderEncountered;
	unsigned int mLastPixelValue;

	ci::Surface8uRef mImageProcessed;
	ci::Surface8uRef mImageProcessedBordered;

	// Using ci::Channel seems ideal, but we need signed integers to differentiate between outer/inner borders :/
	std::unique_ptr<int> mContourMap;
	std::vector<Contour> mContours;

	unsigned int const kmIncomingImgsWidth, kmIncomingImgsHeight;
	unsigned int const kmImgBorderedWidth, kmImgBorderedHeight;

};

#endif /* CONTOURDETECTOR */