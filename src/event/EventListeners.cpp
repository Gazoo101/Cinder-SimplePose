/*
* Copyright (c) 2015-2016, Lasse Farnung Laursen - http://www.lasselaursen.com
*
* This file is a part of SimplePose, and subject to the new and revised BSD license.
* Please see the LICENSE file for more information.
* All other rights reserved.
*/

#include "EventListeners.h"
#include "TagEvent.h"

#include <algorithm>

namespace SimplePose {

	void EventListeners::add( std::function<bool( TagEvent const& )> callbackFn )
	{
		//auto aSize = mListeners.size();

		//auto test = node->getParent();

		//mListeners.emplace_back( std::make_shared<EventListener>( node, callbackFn ) );
	}

	void EventListeners::remove( )
	{
		//// This function shouldn't really be needed, as weak_ptr's expire...
		//auto new_end = std::remove_if( mListeners.begin(), mListeners.end(),
		//	[node]( std::shared_ptr<EventListener> const& listener )
		//{ return listener->isRelatedToNode( node ); } );

		//mListeners.erase( new_end, mListeners.end() );
	}

	//template <typename T>
	void EventListeners::sendEvent( TagEvent const& event )
	{
		auto idAttachedListeners = mListenersToTagId.find( event.getTagId().kmId );

		if ( idAttachedListeners != mListenersToTagId.end() )
		{
			idAttachedListeners->second.sendEvent( event );
		}
	}

};