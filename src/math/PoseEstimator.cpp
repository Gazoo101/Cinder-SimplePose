/*
* Copyright (c) 2015, Lasse Farnung Laursen - http://www.lasselaursen.com
*
* This file is a part of SimplePose, and subject to the new and revised BSD license.
* Please see the LICENSE file for more information.
* All other rights reserved.
*/

#include "PoseEstimator.h"
#include "Homography.h"

#include "cinder/Log.h"

namespace SimplePose {

	PoseEstimator::PoseEstimator( ci::mat3 const & intrinsicCameraParameters ) :
		mMatIntrinsicCameraParameters( intrinsicCameraParameters )
	{
		mHomographyCalculator = std::make_unique<Homography>();

		CI_LOG_I( "mMatIntrinsicCameraParameters" );
		CI_LOG_I( mMatIntrinsicCameraParameters );

		mMatIntrinsicCameraParametersInverse = glm::inverse( mMatIntrinsicCameraParameters );

		CI_LOG_I( "mMatIntrinsicCameraParametersInverse" );
		CI_LOG_I( mMatIntrinsicCameraParametersInverse );

		// Set scale to identity matrix
		mMatScale = ci::mat3( 1.0f );

		auto tagMarkerWidth = 1.0f;
		auto tagMarkerHeight = 1.0f;

		// Long long ago, Daniel Wagner suggested using the following virtual coordinates
		mTagVirtualCoords[0].x = tagMarkerWidth / 2;
		mTagVirtualCoords[0].y = tagMarkerHeight / 2;
		mTagVirtualCoords[1].x = tagMarkerWidth / 2;
		mTagVirtualCoords[1].y = -tagMarkerHeight / 2;
		mTagVirtualCoords[2].x = -tagMarkerWidth / 2;
		mTagVirtualCoords[2].y = -tagMarkerHeight / 2;
		mTagVirtualCoords[3].x = -tagMarkerWidth / 2;
		mTagVirtualCoords[3].y = tagMarkerHeight / 2;
	}

	PoseEstimator::~PoseEstimator()
	{

	}

	void PoseEstimator::estimatePose( ci::vec2 const tagCornerScreenCoords[4] )
	{
		auto estimatedViewMatrix = estimateViewMatrix( tagCornerScreenCoords );

		//extractPositionAndOrientationFromViewMatrix( estimatedViewMatrix );
	}

	ci::mat4 PoseEstimator::estimateViewMatrix( ci::vec2 const tagCornerScreenCoords[4] )
	{
		CI_LOG_I( "----------------------------- SimplePose ---------------------------------" );

		// homography = mat3x3
		auto homography = mHomographyCalculator->getHomography( mTagVirtualCoords.data(), tagCornerScreenCoords );

		CI_LOG_I( "Our Estimation" );
		CI_LOG_I( homography );

		CI_LOG_I( "mMatIntrinsicCameraParameters" );
		CI_LOG_I( mMatIntrinsicCameraParameters );

		CI_LOG_I( "mMatIntrinsicCameraParametersInverse" );
		CI_LOG_I( mMatIntrinsicCameraParametersInverse );

		// Can we get from virtual to tagCornerScreenCoords

		// Verify: does matrix multiplication work the way we expect it to?
		mMatRRTUnscaled = mMatIntrinsicCameraParametersInverse * homography;

		// Scale is more or less a NOOP for now
		mMatRRT = mMatRRTUnscaled / mMatScale;

		// Extract rotation and translation vectors
		ci::vec3 rotVec1 = glm::column( mMatRRT, 0 );
		ci::vec3 rotVec2 = glm::column( mMatRRT, 1 );
		ci::vec3 transVec = glm::column( mMatRRT, 2 );

		// Debug prints
		CI_LOG_I( "mMatRRT" );
		CI_LOG_I( mMatRRT );

		CI_LOG_I( "rotVec1" );
		CI_LOG_I( rotVec1 );

		CI_LOG_I( "rotVec2" );
		CI_LOG_I( rotVec2 );

		CI_LOG_I( "transVec" );
		CI_LOG_I( transVec );

		// Long long ago, Daniel Wagner Suggested the following operations to achieve the proper transform!
		auto r1length = ci::length( rotVec1 );
		auto r2length = ci::length( rotVec2 );
		double transAvrg = ( r1length + r2length ) / 2;

		// Calculate the remaining rotation vector
		ci::vec3 rotVec3 = glm::cross( rotVec1, rotVec2 );

		// Ensure orthogonality - Is this needed? - On first look, yes due to difference in vector lengths...?
		rotVec1 = glm::cross( rotVec2, rotVec3 );

		transVec.x /= transAvrg;
		transVec.y /= transAvrg;
		transVec.z /= transAvrg;

		// Make rotation Matrix ortho-normal
		rotVec1 = glm::normalize( rotVec1 );
		rotVec2 = glm::normalize( rotVec2 );
		rotVec3 = glm::normalize( rotVec3 );

		auto viewMatrix = ci::mat4(
			rotVec1.x, rotVec2.x, rotVec3.x, transVec.x,
			rotVec1.y, rotVec2.y, rotVec3.y, transVec.y,
			rotVec1.z, rotVec2.z, rotVec3.z, transVec.z,
			0, 0, 0, 1
			);

		// Transpose the matrix.
		// Required as the matrix was entered in ROW major fashion, to a constructor expecting COLUMN major ordering.
		viewMatrix = glm::transpose( viewMatrix );

		ci::mat4 leftToRightHandedCoordSystem = {
			1, 0, 0, 0,
			0, -1, 0, 0,
			0, 0, -1, 0,
			0, 0, 0, 1
		};

		//CI_LOG_I( "viewMatrix" );
		//CI_LOG_I( viewMatrix );

		viewMatrix = leftToRightHandedCoordSystem * viewMatrix;

		//CI_LOG_I( "viewMatrix" );
		//CI_LOG_I( viewMatrix );

		return viewMatrix;
	}




	void PoseEstimator::extractPositionAndOrientationFromViewMatrix(
		ci::mat4 const & matViewMatrixVirtualCamera,
		ci::mat4 const & matViewMatrixEstimatedFromTag )
	{
		/*
			To determine the virtual world position and orientation of one or several tags, we use a fixed
			(virtual) camera to anchor them all to. We do this, because we want to mimic the real world
			where a single camera observes all the individual tags.

			This may sound a little odd, but the reason for this is that pose estimation currently yields
			a view matrix from which we can extract object location/orientation.

			*/





		// The (estimated) ViewMatrix contains calculations that translate an observed object
		// from world-coordinates to view-coordinates. Remember that we've defined the objects
		// world coordinates as being centered around the point of origin (Mr. Wagner claims this
		// leads to the most accurate estimation).

		// Thus, the ViewMatrix implicitly tells us where our virtual camera is located in world
		// coordinates. Thus, we can 


		ci::mat4 heiy;

		glm::toQuat( heiy );


		// The calculated viewMatrix (which describes where the camera is located in relation
		// to the observed tag) contains the information we need. We are interested in the
		// inverse viewMatrix, as we prefer a world where the camera is static, and the observed
		// tag/object moves.


	}

};