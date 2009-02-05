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

#ifndef __JogMovement_H__
#define __JogMovement_H__

#include "WalkMovement.h"


namespace rl
{
    class JogMovement : public WalkMovement
    {
    public:
        JogMovement(CreatureController *creature);
        virtual CreatureController::MovementType getId() const {return CreatureController::MT_JOGGEN;}
        virtual CreatureController::MovementType getFallBackMovement() const {return CreatureController::MT_GEHEN;}
        virtual bool calculateBaseVelocity(Ogre::Real &velocity);
        virtual bool isPossible() const;
        virtual void applyAuChanges(Ogre::Real elapsedTime);
        virtual void setAnimation(Ogre::Real elapsedTime);
    };
}

#endif

