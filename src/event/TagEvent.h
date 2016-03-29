/*
* Copyright (c) 2015-2016, Lasse Farnung Laursen - http://www.lasselaursen.com
*
* This file is a part of SimplePose, and subject to the new and revised BSD license.
* Please see the LICENSE file for more information.
* All other rights reserved.
*/

#ifndef SIMPLEPOSE_TAGEVENT_H_INCLUDED
#define SIMPLEPOSE_TAGEVENT_H_INCLUDED

#include "cinder/app/Event.h"
#include "cinder/Signals.h"
#include "tag/Tag.h"

namespace SimplePose {

	class TagEvent : public ci::app::Event {
	public:

		TagEvent();
		TagEvent( TagId const& tagId ) :
			mUpdatedTagId( tagId ) {};

		TagId const getTagId() const { return mUpdatedTagId; }

	private:

		TagId const mUpdatedTagId;

	};

	typedef cinder::signals::Signal<bool( TagEvent& ), cinder::signals::CollectorWhile0<bool> >		EventTag;

}

#endif // SIMPLEPOSE_TAGEVENT_H_INCLUDED