/* SoundPlayEvent.h - Diese Klassse repr�sentiert ein Event das beim Abspielen auftritt.
 * (C) 2003-2005. Team Pantheon. www.team-pantheon.de
 * 
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the Perl Artistic License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  Perl Artistic License for more details.
 *
 *  You should have received a copy of the Perl Artistic License
 *  along with this program; if not you can get it here
 *  http://www.perldoc.com/perl5.6/Artistic.html.
 */
 
#ifndef SOUNDPLAYEVENT_H
#define SOUNDPLAYEVENT_H

#include "SoundPrerequisites.h"
#include "SoundEvent.h"

namespace rl {
 
class SoundPlayEvent : public SoundEvent {
public:

	SoundPlayEvent(EventSource *source);
	virtual ~SoundPlayEvent();
    
    static const unsigned int STARTEVENT = 10;
    static const unsigned int STOPEVENT = 11;
};

}

#endif // SOUNDPLAYEVENT_H
