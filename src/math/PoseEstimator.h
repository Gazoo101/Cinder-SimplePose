/*
* Copyright (c) 2015, Lasse Farnung Laursen - http://www.lasselaursen.com
*
* This file is a part of SimplePose, and subject to the new and revised BSD license.
* Please see the LICENSE file for more information.
* All other rights reserved.
*/

#ifndef SIMPLEPOSE_POSEESTIMATOR_H_INCLUDED
#define SIMPLEPOSE_POSEESTIMATOR_H_INCLUDED

#include "cinder/Matrix.h"
#include <array>

// Forward declarations
class Homography;

class PoseEstimator {

public:

	PoseEstimator( ci::mat3 const & intrinsicCameraParameters );
	~PoseEstimator();

	void estimatePose( ci::vec2 const tagCornerScreenCoords[4] );

	ci::mat4 estimateViewMatrix( ci::vec2 const tagCornerScreenCoords[4] );

protected:
	void extractPositionAndOrientationFromViewMatrix(
		ci::mat4 const & matViewMatrixVirtualCamera,
		ci::mat4 const & matViewMatrixEstimatedFromTag );

	std::array<ci::vec2, 4> mTagVirtualCoords;

	ci::mat3 mMatRRTUnscaled;
	ci::mat3 mMatRRT;

	ci::mat3 mMatScale;

	ci::mat3 mMatIntrinsicCameraParameters;
	ci::mat3 mMatIntrinsicCameraParametersInverse;

	std::unique_ptr<Homography> mHomographyCalculator;
};

#endif /* SIMPLEPOSE_POSEESTIMATOR_H_INCLUDED */