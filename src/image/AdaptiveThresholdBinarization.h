/*
* Copyright (c) 2015, Lasse Farnung Laursen - http://www.lasselaursen.com
*
* This file is a part of SimplePose, and subject to the new and revised BSD license.
* Please see the LICENSE file for more information.
* All other rights reserved.
*/

#ifndef SIMPLEPOSE_ADAPTIVETHRESHOLDBINARIZATION_H_INCLUDED
#define SIMPLEPOSE_ADAPTIVETHRESHOLDBINARIZATION_H_INCLUDED

#include <memory>
#include "cinder/Surface.h"

namespace SimplePose {

class AdaptiveThresholdBinarization {

public:

	AdaptiveThresholdBinarization( unsigned int const & incomingImagesWidth, unsigned int const & incomingImagesHeight );
	~AdaptiveThresholdBinarization();

	ci::Channel8uRef process( ci::Channel8uRef surface );

private:

	inline void processPixel( ci::Channel8u::Iter & iter );

	// Adaptive binarization variables
	unsigned short const		kmBinaPixelWindow;
	double const				kmBinaThresholdRelaxation;
	std::unique_ptr<double>		mBinaPrevRowThresholdEstimate;

	// Could be unsigned?
	int mBinaPixelWindowEstimate;
	int mBinaThreshold;


	ci::Channel8uRef mImageProcessed;
	unsigned int const kmIncomingImgsWidth, kmIncomingImgsHeight;

};

};

#endif /* SIMPLEPOSE_ADAPTIVETHRESHOLDBINARIZATION_H_INCLUDED */