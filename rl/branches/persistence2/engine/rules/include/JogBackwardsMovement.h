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

#ifndef __JogBackwardsMovement_H__
#define __JogBackwardsMovement_H__

#include "JogMovement.h"


namespace rl
{
    class JogBackwardsMovement : public JogMovement
    {
    public:
        JogBackwardsMovement(CreatureController *creature) : JogMovement(creature)
        {
            mAnim = creature->getCreature()->getAnimation("joggen_rueckwaerts");
        }
        virtual CreatureController::MovementType getId() const {return CreatureController::MT_RUECKWAERTS_JOGGEN;}
        virtual CreatureController::MovementType getFallBackMovement() const {return CreatureController::MT_RUECKWAERTS_GEHEN;}
        virtual bool calculateBaseVelocity(Ogre::Real &velocity)
        {
            velocity = mMovingCreature->getCurrentGS() / 3.2;
            return isPossible();
        }
    };
}

#endif

