/*
* Copyright (c) 2015, Lasse Farnung Laursen - http://www.lasselaursen.com
*
* This file is a part of SimplePose, and subject to the new and revised BSD license.
* Please see the LICENSE file for more information.
* All other rights reserved.
*/

#include "PoseEstimator.h"
#include "Homography.h"

PoseEstimator::PoseEstimator( ci::mat3 const & intrinsicCameraParameters ) :
	mIntrinsicCameraParameters( intrinsicCameraParameters )
{
	mHomographyCalculator = std::unique_ptr<Homography>( new Homography() );

	mIntrinsicCameraParametersInverse = glm::inverse( mIntrinsicCameraParameters );

	// Set scale to identity matrix
	mScale = ci::mat3( 1.0f );

	auto tagMarkerWidth = 1.0f;
	auto tagMarkerHeight = 1.0f;
	
	// Long long ago, Daniel Wagner suggested using the following virtual coordinates
	mTagVirtualCoords[0].x = tagMarkerWidth / 2; mTagVirtualCoords[0].y = tagMarkerHeight / 2;
	mTagVirtualCoords[1].x = tagMarkerWidth / 2; mTagVirtualCoords[1].y = - tagMarkerHeight / 2;
	mTagVirtualCoords[2].x = - tagMarkerWidth / 2; mTagVirtualCoords[2].y = - tagMarkerHeight / 2;
	mTagVirtualCoords[3].x = - tagMarkerWidth / 2; mTagVirtualCoords[3].y = tagMarkerHeight / 2;
	

}

PoseEstimator::~PoseEstimator()
{

}

void PoseEstimator::estimatePose( ci::vec2 const tagCornerScreenCoords[4] )
{
	// homography = mat3x3
	auto homography = mHomographyCalculator->getHomography( mTagVirtualCoords.data(), tagCornerScreenCoords );


	// Verify: does matrix multiplication work the way we expect it to?
	mRRTUnscaled = mIntrinsicCameraParametersInverse * homography;


}