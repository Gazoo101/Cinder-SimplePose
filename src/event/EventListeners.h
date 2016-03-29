/*
* Copyright (c) 2015-2016, Lasse Farnung Laursen - http://www.lasselaursen.com
*
* This file is a part of SimplePose, and subject to the new and revised BSD license.
* Please see the LICENSE file for more information.
* All other rights reserved.
*/

#ifndef EVENTLISTENERS_H_INCLUDED
#define EVENTLISTENERS_H_INCLUDED

#include <typeindex>
#include <memory>
#include <vector>
#include <unordered_map>
#include "EventListener.h"

namespace SimplePose {

	//! This class is basically a wrapper around a vector of callbacks containers
	//! A single EventListener encapsulates a Node and an associated function call
	class EventListeners {
	public:

		EventListeners() {};

		void add( std::function<bool( TagEvent const& )> callbackFn );
		void remove();

		//inline void invalidate() { mIsValid = false; };

		//template <typename T>
		void sendEvent( TagEvent const& event );

	private:
		
		//! Match Id from (specific type of) AR Tag to EventListner (vector of callbacks)
		std::unordered_map<unsigned long long, EventListener> mListenersToTagId;
	};

};

#endif // EVENTLISTENERS_H_INCLUDED