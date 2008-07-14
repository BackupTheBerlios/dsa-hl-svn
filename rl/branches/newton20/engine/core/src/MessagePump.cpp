/* This source file is part of Rastullahs Lockenpracht.
 * Copyright (C) 2003-2008 Team Pantheon. http://www.team-pantheon.de
 * 
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the Clarified Artistic License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  Clarified Artistic License for more details.
 *
 *  You should have received a copy of the Clarified Artistic License
 *  along with this program; if not you can get it here
 *  http://www.jpaulmorrison.com/fbp/artistic2.htm.
 */
#include "stdinc.h"

#include "MessagePump.h"

template<> rl::MessagePump* Ogre::Singleton<rl::MessagePump>::ms_Singleton = 0;

namespace rl
{
    MessagePump::~MessagePump()
    {

        while(!mMessageQueue.empty())
        {
        
            delete mMessageQueue.front();
            mMessageQueue.pop();    
        }

        MessageHandlerMap::iterator it = mMessageHandlerMap.begin();
        for( ; it != mMessageHandlerMap.end(); it++ )
            if( it->second != NULL )
                delete it->second;
    }

    void MessagePump::run(Ogre::Real elapsedTime)
    {
        static bool sending = false;
        if(!sending)
        {
            sending = true;
            sendPending();
            sending = false;  
        }
    }

    const Ogre::String& MessagePump::getName() const
    {
        static Ogre::String name = "MessagePump";
        return name;
    }

}
