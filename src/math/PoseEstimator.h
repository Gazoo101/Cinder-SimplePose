/*
* Copyright (c) 2015, Lasse Farnung Laursen - http://www.lasselaursen.com
*
* This file is a part of SimplePose, and subject to the new and revised BSD license.
* Please see the LICENSE file for more information.
* All other rights reserved.
*/

#include "cinder/Matrix.h"
#include <array>

// Forward declarations
class Homography;

class PoseEstimator {

public:

	PoseEstimator( ci::mat3 const & intrinsicCameraParameters );
	~PoseEstimator();

	void estimatePose( ci::vec2 const tagCornerScreenCoords[4] );

private:

	std::array<ci::vec2, 4> mTagVirtualCoords;

	ci::mat3 mRRTUnscaled;

	ci::mat3 mScale;

	ci::mat3 mIntrinsicCameraParameters;
	ci::mat3 mIntrinsicCameraParametersInverse;


	std::unique_ptr<Homography> mHomographyCalculator;

};