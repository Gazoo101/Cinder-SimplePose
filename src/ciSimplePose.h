/*
* Copyright (c) 2015, Lasse Farnung Laursen - http://www.lasselaursen.com
*
* This file is a part of SimplePose, and subject to the new and revised BSD license.
* Please see the LICENSE file for more information.
* All other rights reserved.
*/

#ifndef CISIMPLEPOSE
#define CISIMPLEPOSE

#include "TagRecognizer.h"
#include "cinder/Surface.h"
#include "cinder/gl/Texture.h"

class CiSimplePose {
	
public:

	CiSimplePose( unsigned int const & incomingImagesWidth, unsigned int const & incomingImagesHeight );
	~CiSimplePose();



	//getCameraMatrix();	- Todo. Use OpenCV for now.

	void detectTags( ci::Surface8uRef surface );

	ci::Surface8uRef getTagTex( unsigned int const &numTags ) { return mTagRecognizer->getTagTex( numTags ); };
	
	ci::gl::Texture2dRef getTextureGrayscale() { return mTexGrayscale; };
	ci::gl::Texture2dRef getTextureBinary() { return mTexBinary; };
	ci::gl::Texture2dRef getTextureSquares() { return mTexSquares; };
	ci::gl::Texture2dRef getTextureTags() { return mTexTags; };

private:

	void processIncomingToGrayscale( ci::Surface8uRef surface );
	void processGrayscaleToBinary( ci::Surface8uRef surface );
	void detectSquaresInBinary( ci::Surface8uRef surface );
	void detectTagsInSquares( ci::Surface8uRef surface );

	inline void processGrayscaleToBinaryPixel( ci::Surface8u::Iter & iter );

	// Adaptive binarization variables
	unsigned short const		kmBinaPixelWindow;
	double const				kmBinaThresholdRelaxation;
	std::unique_ptr<double>		mBinaPrevRowThresholdEstimate;

	// Could be unsigned?
	int mBinaPixelWindowEstimate;
	int mBinaThreshold;

	ci::Surface8uRef mIncomingGrayscale, mIncomingBinarized, mIncomingSquaresDetected, mIncomingTagsDetected;
	ci::gl::Texture2dRef mTexGrayscale, mTexBinary, mTexSquares, mTexTags;

	unsigned int const kmIncomingImgsWidth, kmIncomingImgsHeight;

	std::unique_ptr<TagRecognizer> mTagRecognizer;
};

#endif /* CISIMPLEPOSE */