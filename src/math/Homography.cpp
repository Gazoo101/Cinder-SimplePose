/*
* Copyright (c) 2015, Lasse Farnung Laursen - http://www.lasselaursen.com
*					  Paul Houx
*
* This file is a part of SimplePose, and subject to the new and revised BSD license.
* Please see the LICENSE file for more information.
* All other rights reserved.
*
* Partial source contained in this file used with permission from the following original source:
* https://github.com/paulhoux/Cinder-Warping/blob/master/src/WarpPerspective.cpp#L238-L307
*/

#include "Homography.h"

namespace SimplePose {

	ci::mat4 Homography::getPerspectiveTransform( const ci::vec2 src[4], const ci::vec2 dst[4] ) const
	{
		float p[8][9] = {
			{ -src[0][0], -src[0][1], -1, 0, 0, 0, src[0][0] * dst[0][0], src[0][1] * dst[0][0], -dst[0][0] }, // h11
			{ 0, 0, 0, -src[0][0], -src[0][1], -1, src[0][0] * dst[0][1], src[0][1] * dst[0][1], -dst[0][1] }, // h12
			{ -src[1][0], -src[1][1], -1, 0, 0, 0, src[1][0] * dst[1][0], src[1][1] * dst[1][0], -dst[1][0] }, // h13
			{ 0, 0, 0, -src[1][0], -src[1][1], -1, src[1][0] * dst[1][1], src[1][1] * dst[1][1], -dst[1][1] }, // h21
			{ -src[2][0], -src[2][1], -1, 0, 0, 0, src[2][0] * dst[2][0], src[2][1] * dst[2][0], -dst[2][0] }, // h22
			{ 0, 0, 0, -src[2][0], -src[2][1], -1, src[2][0] * dst[2][1], src[2][1] * dst[2][1], -dst[2][1] }, // h23
			{ -src[3][0], -src[3][1], -1, 0, 0, 0, src[3][0] * dst[3][0], src[3][1] * dst[3][0], -dst[3][0] }, // h31
			{ 0, 0, 0, -src[3][0], -src[3][1], -1, src[3][0] * dst[3][1], src[3][1] * dst[3][1], -dst[3][1] }, // h32
		};

		gaussianElimination( &p[0][0], 9 );

		ci::mat4 result = ci::mat4( p[0][8], p[3][8], 0, p[6][8],
			p[1][8], p[4][8], 0, p[7][8],
			0, 0, 1, 0,
			p[2][8], p[5][8], 0, 1 );

		return result;
	}

	ci::mat3 Homography::getHomography( const ci::vec2 src[4], const ci::vec2 dst[4] ) const
	{
		float p[8][9] = {
			{ -src[0][0], -src[0][1], -1, 0, 0, 0, src[0][0] * dst[0][0], src[0][1] * dst[0][0], -dst[0][0] }, // h11
			{ 0, 0, 0, -src[0][0], -src[0][1], -1, src[0][0] * dst[0][1], src[0][1] * dst[0][1], -dst[0][1] }, // h12
			{ -src[1][0], -src[1][1], -1, 0, 0, 0, src[1][0] * dst[1][0], src[1][1] * dst[1][0], -dst[1][0] }, // h13
			{ 0, 0, 0, -src[1][0], -src[1][1], -1, src[1][0] * dst[1][1], src[1][1] * dst[1][1], -dst[1][1] }, // h21
			{ -src[2][0], -src[2][1], -1, 0, 0, 0, src[2][0] * dst[2][0], src[2][1] * dst[2][0], -dst[2][0] }, // h22
			{ 0, 0, 0, -src[2][0], -src[2][1], -1, src[2][0] * dst[2][1], src[2][1] * dst[2][1], -dst[2][1] }, // h23
			{ -src[3][0], -src[3][1], -1, 0, 0, 0, src[3][0] * dst[3][0], src[3][1] * dst[3][0], -dst[3][0] }, // h31
			{ 0, 0, 0, -src[3][0], -src[3][1], -1, src[3][0] * dst[3][1], src[3][1] * dst[3][1], -dst[3][1] }, // h32
		};

		gaussianElimination( &p[0][0], 9 );

		ci::mat3 result = ci::mat3(
			p[0][8], p[3][8], p[6][8],
			p[1][8], p[4][8], p[7][8],
			p[2][8], p[5][8], 1 );

		return result;
	}

	void Homography::gaussianElimination( float *a, int n ) const
	{
		int i = 0;
		int j = 0;
		int m = n - 1;

		while ( i < m && j < n ) {
			int maxi = i;
			for ( int k = i + 1; k<m; ++k ) {
				if ( fabs( a[k*n + j] ) > fabs( a[maxi*n + j] ) ) {
					maxi = k;
				}
			}

			if ( a[maxi*n + j] != 0 ) {
				if ( i != maxi )
					for ( int k = 0; k < n; k++ ) {
						float aux = a[i*n + k];
						a[i*n + k] = a[maxi*n + k];
						a[maxi*n + k] = aux;
					}

				float a_ij = a[i*n + j];
				for ( int k = 0; k < n; k++ ) {
					a[i*n + k] /= a_ij;
				}

				for ( int u = i + 1; u < m; u++ ) {
					float a_uj = a[u*n + j];
					for ( int k = 0; k < n; k++ ) {
						a[u*n + k] -= a_uj*a[i*n + k];
					}
				}

				++i;
			}
			++j;
		}

		for ( int i = m - 2; i >= 0; --i ) {
			for ( int j = i + 1; j < n - 1; j++ ) {
				a[i*n + m] -= a[i*n + j] * a[j*n + m];
			}
		}
	}

};