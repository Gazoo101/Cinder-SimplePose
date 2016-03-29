/*
* Copyright (c) 2015-2016, Lasse Farnung Laursen - http://www.lasselaursen.com
*
* This file is a part of SimplePose, and subject to the new and revised BSD license.
* Please see the LICENSE file for more information.
* All other rights reserved.
*/

#ifndef SIMPLEPOSE_EVENTLISTENER_H_INCLUDED
#define SIMPLEPOSE_EVENTLISTENER_H_INCLUDED

#include <vector>
#include <functional>

namespace SimplePose {

	class TagEvent;

	//! This class is - in its base version - just a pair with the callback and node contained
	//! But since we'll probably want to compare EventListeners and do other stuff, it's more
	//! proper to turn it into a fully fledged class!
	class EventListener {
	public:

		EventListener() : mIsValid( true ) {};

		//template <typename T>
		bool sendEvent( TagEvent const& event );

		void add( std::function<bool( TagEvent const& )> callbackFn );
		void remove();

	private:

		void sortDirtyListeners();

		bool mIsValid;

		// shared_ptr (or any type of ptr) reqd. to prevent 'object slicing'
		std::vector<std::function<bool( TagEvent const& )>> mCallbacks;

		friend class EventListeners;
	};

};

#endif // SIMPLEPOSE_EVENTLISTENER_H_INCLUDED