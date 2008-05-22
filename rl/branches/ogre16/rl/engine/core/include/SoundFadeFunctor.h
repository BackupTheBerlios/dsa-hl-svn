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

#ifndef __RL_SOUND_FADE_FUNCTOR_H__
#define __RL_SOUND_FADE_FUNCTOR_H__

#include "CorePrerequisites.h"


namespace rl {
    class SoundObject;

    class _RlCoreExport SoundFadeFunctor
    {
    public:
        SoundFadeFunctor(Ogre::Real duration);

        virtual Ogre::Real operator()(Ogre::Real time) = 0;

    protected:
        Ogre::Real mDuration;
    };
}

#endif
