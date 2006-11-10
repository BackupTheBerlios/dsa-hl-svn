/* This source file is part of Rastullahs Lockenpracht.
 * Copyright (C) 2003-2005 Team Pantheon. http://www.team-pantheon.de
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

#include "QuestListener.h"

#include "Exception.h"

namespace rl {

    QuestListener::~QuestListener()
    {
    }

    bool QuestListener::eventRaised(QuestEvent* evt)
    {
        try
        {
            if( evt->getReason() == QuestEvent::QUEST_STATE )
                questStateChanged(evt);
            else if( evt->getReason() == QuestEvent::QUEST_PARTSDONE )
                questPartsDoneChanged(evt);
            else if( evt->getReason() == QuestEvent::QUEST_KNOWN )
                questKnownChanged(evt);
            else if( evt->getReason() == QuestEvent::QUEST_SUBQUEST )
                questSubquestAdded(evt);
        }
        catch( ScriptInvocationFailedException& sife )
        {
            LOG_ERROR(Logger::RULES, sife.toString() );
        }

        // consumed or not
        return false;
    }

    bool QuestListener::eventRaised(JournalEvent* evt)
    {
        try
        {
            if (evt->getReason() == JournalEvent::JOURNAL_ENTRY_ADDED)
            {
                journalEntryAdded(evt);
            }
        }
        catch (ScriptInvocationFailedException& sife)
        {
            LOG_ERROR(Logger::RULES, sife.toString());
        }

        // consumed or not
        return false;
    }
}
