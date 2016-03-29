/*
* Copyright (c) 2015-2016, Lasse Farnung Laursen - http://www.lasselaursen.com
*
* This file is a part of SimplePose, and subject to the new and revised BSD license.
* Please see the LICENSE file for more information.
* All other rights reserved.
*/

#include "EventListener.h"

namespace SimplePose {

bool EventListener::sendEvent( TagEvent const& event )
{
	if ( !mIsValid )
		sortDirtyListeners();

	for ( auto & callback : mCallbacks )
	{
		callback( event );
	}

	return false;
}

void EventListener::sortDirtyListeners()
{
	// To implement later.
}

};