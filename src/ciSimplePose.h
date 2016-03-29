/*
* Copyright (c) 2015-2016, Lasse Farnung Laursen - http://www.lasselaursen.com
*
* This file is a part of SimplePose, and subject to the new and revised BSD license.
* Please see the LICENSE file for more information.
* All other rights reserved.
*/

#ifndef SIMPLEPOSE_CISIMPLEPOSE_H_INCLUDED
#define SIMPLEPOSE_CISIMPLEPOSE_H_INCLUDED

#include "cinder/Surface.h"
#include "cinder/gl/Texture.h"
#include "cinder/Camera.h"

#include <unordered_map>
#include <typeindex>

#include "cinder/Timer.h"	// For performance measurements/improvements only

#include "tag/Tag.h"
#include "event/TagEvent.h"

namespace SimplePose {

	// Forward declarations
	class TagRecognizer;
	class AdaptiveThresholdBinarization;
	class ContourDetector;
	class PolygonApproximator;
	class PoseEstimator;
	class EventDispatcher;
	struct Contour;
	struct Polygon;

	class CiSimplePose {

	public:

		CiSimplePose(
			unsigned int const & incomingImagesWidth,
			unsigned int const & incomingImagesHeight,
			ci::mat3 const & matIntrinsicCameraParameters );
		~CiSimplePose();

		//getCameraMatrix();	- Todo. Use OpenCV for now.

		void update( ci::Surface8uRef const& surface );

		//! Add a detectable tag type (inheriting from the Tag class)
		//! SimplePose will submit all detected squares to each detector in order of addition
		//! Thus, adding the detector with the highest 'filter' first is advisable
		void addTagType( std::unique_ptr<Tag> tagDetector );


		ci::mat4 getEstimatedViewMatrixFromTag( ci::Surface8uRef surface, unsigned char const & debugTagNr );
		ci::mat4 getEstimatedViewMatrixFromTagOpenCVOld( ci::Surface8uRef surface, unsigned char const & debugTagNr );
		ci::mat4 getEstimatedViewMatrixFromTagOpenCVNew( ci::Surface8uRef surface, unsigned char const & debugTagNr );

		ci::mat4 getEstimatedViewMatrixFromTagPoints( ci::vec2 const& pt1, ci::vec2 const& pt2, ci::vec2 const& pt3, ci::vec2 const& pt4 );
		ci::mat4 getEstimatedViewMatrixFromTagPointsOpenCVOld( ci::vec2 const& pt1, ci::vec2 const& pt2, ci::vec2 const& pt3, ci::vec2 const& pt4 );
		ci::mat4 getEstimatedViewMatrixFromTagPointsOpenCVNew( ci::vec2 const& pt1, ci::vec2 const& pt2, ci::vec2 const& pt3, ci::vec2 const& pt4 );

		void getTagPoses( ci::Surface8uRef surface, ci::mat4 viewMatrix );


		inline ci::Surface8uRef getTagTex( unsigned int const &numTags );

		ci::gl::Texture2dRef getTextureGrayscale() { return mTexGrayscale; };
		ci::gl::Texture2dRef getTextureBinary() { return mTexBinary; };
		ci::gl::Texture2dRef getTextureContours() { return mTexContours; };
		ci::gl::Texture2dRef getTextureSquares() { return mTexSquares; };
		ci::gl::Texture2dRef getTextureTags() { return mTexTags; };
		ci::gl::Texture2dRef getTextureDebug() { return mTexDebug; };

		void drawAllContours() const;
		void drawTagSizedContours() const;

		void drawPolygonsAll() const;
		void drawPolygonsConvexOnly() const;
		void drawPolygonsSquaresOnly() const;

		void drawTagsAll() const;

		void unitTest();

		ci::mat4 convertIntrinsicCameraParametersToOpenGLProjectionMatrix( ci::mat3 const & intrinsicCameraParameters, float const &imgFeedWidth, float const &imgFeedHeight );

		void matchVirtualCamToRealCamParameters( ci::CameraPersp camPersp );

		EventTag&		getSignalTagDiscovered()		{ return mSignalTagDiscovered; }
		EventTag&		getSignalTagUpdated()			{ return mSignalTagUpdated; }

	protected:

		std::vector<std::shared_ptr<Tag>> detectTags( ci::Surface8uRef const& surface );

		ci::Channel8uRef processIncomingToGrayscale( ci::Surface8uRef surface );
		void processGrayscaleToBinary( ci::Surface8uRef surface );
		void detectSquaresInBinary( ci::Surface8uRef surface );
		void detectTagsInSquares( ci::Surface8uRef surface );

		bool updateTags( std::vector<std::shared_ptr<Tag>> const& detectedTags );

		ci::Channel8uRef mImgGrayScale, mImgBinary;						// These images only require a single color channel
		ci::Surface8uRef mImgContours, mImgSquares, mImgTags;			// These images 'need' color to better differentiate detected elements
		ci::gl::Texture2dRef mTexGrayscale, mTexBinary, mTexContours, mTexSquares, mTexTags, mTexDebug;

		ci::mat3 mMatIntrinsicCameraParameters;

		unsigned int const kmIncomingImgsWidth, kmIncomingImgsHeight;

		std::unique_ptr<TagRecognizer> mTagRecognizer;

		std::vector<Contour> mContoursTagSized;

		std::vector<Polygon> mPolygonsConvex;
		std::vector<Polygon> mPolygonsSquare;




		// Image Processing Objects
		std::unique_ptr<AdaptiveThresholdBinarization> mBinarizer;
		std::unique_ptr<ContourDetector> mContourFinder;
		std::unique_ptr<PolygonApproximator> mPolygonApproximator;

		std::unique_ptr<PoseEstimator> mPoseEstimator;

		std::unordered_map<TagId, std::shared_ptr<Tag>> mKnownTags;

		// AR Callback
		EventTag mSignalTagDiscovered;
		EventTag mSignalTagUpdated;

		//! A class that handles dispatching detected AR Tags
		std::unique_ptr<EventDispatcher> mIndividualTagEventDispatcher;

	};

};

#endif /* SIMPLEPOSE_CISIMPLEPOSE_H_INCLUDED */