/*
* Copyright (c) 2015, Lasse Farnung Laursen - http://www.lasselaursen.com
*
* This file is a part of SimplePose, and subject to the new and revised BSD license.
* Please see the LICENSE file for more information.
* All other rights reserved.
*/

#ifndef CISIMPLEPOSE
#define CISIMPLEPOSE

#include "cinder/Surface.h"
#include "cinder/gl/Texture.h"

// Forward declarations
class TagRecognizer;
class AdaptiveThresholdBinarization;
class ContourDetector;

class CiSimplePose {
	
public:

	CiSimplePose( unsigned int const & incomingImagesWidth, unsigned int const & incomingImagesHeight );
	~CiSimplePose();

	//getCameraMatrix();	- Todo. Use OpenCV for now.

	void detectTags( ci::Surface8uRef surface );

	inline ci::Surface8uRef getTagTex( unsigned int const &numTags );
	
	ci::gl::Texture2dRef getTextureGrayscale() { return mTexGrayscale; };
	ci::gl::Texture2dRef getTextureBinary() { return mTexBinary; };
	ci::gl::Texture2dRef getTextureSquares() { return mTexSquares; };
	ci::gl::Texture2dRef getTextureTags() { return mTexTags; };

private:

	void processIncomingToGrayscale( ci::Surface8uRef surface );
	void processGrayscaleToBinary( ci::Surface8uRef surface );
	void detectSquaresInBinary( ci::Surface8uRef surface );
	void detectTagsInSquares( ci::Surface8uRef surface );

	ci::Surface8uRef mIncomingGrayscale, mIncomingBinarized, mIncomingSquaresDetected, mIncomingTagsDetected;
	ci::gl::Texture2dRef mTexGrayscale, mTexBinary, mTexSquares, mTexTags;

	unsigned int const kmIncomingImgsWidth, kmIncomingImgsHeight;

	std::unique_ptr<TagRecognizer> mTagRecognizer;

	// Image Processing Objects
	std::unique_ptr<AdaptiveThresholdBinarization> mBinarizer;
	std::unique_ptr<ContourDetector> mSquareFinder;
};

#endif /* CISIMPLEPOSE */