/* This source file is part of Rastullahs Lockenpracht.
 * Copyright (C) 2003-2007 Team Pantheon. http://www.team-pantheon.de
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
#include "stdinc.h" //precompiled header

#include "Combat.h"

namespace rl
{
    Combat::Combat(Creature* character) : mCharacter(character)
    {
    }

    Combat::~Combat()
    {
    }

    void Combat::addOpponent(Creature* creature)
    {
        mOpponents.insert(creature);
    }

    void Combat::removeOpponent(Creature* creature)
    {
        mOpponents.erase(creature);
    }

    void Combat::addAlly(Creature* creature)
    {
        mAllies.insert(creature);
    }

    void Combat::removeAlly(Creature* creature)
    {
        mAllies.erase(creature);
    }

    const Combat::CreatureSet& Combat::getAllOpponents() const
    {
        return mOpponents;
    }

    const Combat::CreatureSet& Combat::getAllAllies() const
    {
        return mAllies;
    }
}
