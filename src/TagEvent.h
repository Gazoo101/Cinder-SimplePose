/*
* Copyright (c) 2015, Lasse Farnung Laursen - http://www.lasselaursen.com
*
* This file is a part of SimplePose, and subject to the new and revised BSD license.
* Please see the LICENSE file for more information.
* All other rights reserved.
*/

#include "cinder/app/Event.h"
#include "cinder/Signals.h"

namespace SimplePose {

	class TagEvent : public ci::app::Event {
	public:

		TagEvent();
		//TagEvent( Gesture &aGesture );

		//virtual void trigger( ph::nodes::Node2DRootRef nodeRootRef );

		// Fix
		//Gesture &mGesture;

	private:

	};

}