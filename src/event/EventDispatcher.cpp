/*
* Copyright (c) 2015-2016, Lasse Farnung Laursen - http://www.lasselaursen.com
*
* This file is a part of SimplePose, and subject to the new and revised BSD license.
* Please see the LICENSE file for more information.
* All other rights reserved.
*/

#include "EventDispatcher.h"
#include "tag/Tag.h"
#include "event/TagEvent.h"
#include "event/EventListeners.h"

namespace SimplePose {

EventDispatcher::EventDispatcher()
{

}

void EventDispatcher::sendEvent( TagEvent const& event )
{

}

void EventDispatcher::sendIndividualEvents( std::vector<std::shared_ptr<Tag>> const& detectedTags )
{
	for ( auto & detectedTag : detectedTags )
	{
		auto typeAttachedListeners = mListenersToTagType.find( detectedTag->getType() );

		if ( typeAttachedListeners != mListenersToTagType.end() )
		{
			TagEvent event;
			typeAttachedListeners->second.sendEvent( event );
		}
	}
}

};