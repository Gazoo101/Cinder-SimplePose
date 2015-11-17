# Cinder-SimplePose
Simple homography based pose-estimation CinderBlock

This pose-estimator is a modern C++11 re-write of my masters thesis code, with a few more portions implemented by me. The pose-estimation is homography-based and relies on the standard AR-Tag being within full view.

## Version

0.2 - Still Alpha
- Working adaptive binary thresholding algorithm
- Working contour detection algorithm (Probably needs optimizing)
- Included 'psimpl - generic n-dimensional polyline simplification', licensed under the MPL 1.1

0.1 - Alpha
- Still being implemented, so check back later.

## Installation

Clone and put this directory in the 'blocks' sub-directory of your cinder installation.

## Use

To write...

## License

SimplePose is licensed under the new BSD license.

psimpl - generic n-dimensional polyline simplification is licensed under MPL 1.1.