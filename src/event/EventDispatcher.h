/*
* Copyright (c) 2015-2016, Lasse Farnung Laursen - http://www.lasselaursen.com
*
* This file is a part of SimplePose, and subject to the new and revised BSD license.
* Please see the LICENSE file for more information.
* All other rights reserved.
*/

#ifndef SIMPLEPOSE_EVENTDISPATCHER_H_INCLUDED
#define SIMPLEPOSE_EVENTDISPATCHER_H_INCLUDED

#include <functional>
#include <unordered_map>
#include <typeindex>
#include <memory>

#include "event/EventListeners.h"

namespace SimplePose {

	//class EventListeners;
	class Tag;
	class TagEvent;
	class EventListeners;

	//! This class links a single type of Tag (Bit-pattern, image-tag, etc.) to an EventListeners class.
	//! EventListeners will link a particlar tag type to all known ID's of that type.
	//!
	//! EventDispatcher - Map of EventListeners -> EventListeners - Map of EventListener -> Vector of Callbacks
	class EventDispatcher {
	public:

		EventDispatcher();

		void add( std::function<bool( TagEvent const& )> callbackFn );
		void remove( );

		//template <typename T>
		void sendEvent( TagEvent const& event );

		void sendIndividualEvents( std::vector<std::shared_ptr<Tag>> const& detectedTags );

	private:

		//! TagType to EventListeners
		std::unordered_map<std::type_index, EventListeners> mListenersToTagType;
	};

};

#endif // SIMPLEPOSE_EVENTDISPATCHER_H_INCLUDED