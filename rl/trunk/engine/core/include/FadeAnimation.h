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

#ifndef __FadeAnimation_H__
#define __FadeAnimation_H__

#include <set>

#include "MeshAnimation.h"
#include "CorePrerequisites.h"

namespace rl {

class AnimationFadeOptions
{
    public:
        AnimationFadeOptions( MeshAnimation* anim, Ogre::Real timeStart, 
            Ogre::Real timeEnd, Ogre::Real weightStart, Ogre::Real weightEnd );
        virtual ~AnimationFadeOptions() {};

        inline MeshAnimation* getAnimation() const { return mAnimation;};
        inline Ogre::Real getTimeStart() const { return mTimeStart;};
        inline Ogre::Real getTimeEnd() const { return mTimeEnd;};
        inline Ogre::Real getWeightStart() const { return mWeightStart;};
        inline Ogre::Real getWeightEnd() const { return mWeightEnd;};
    private:
        MeshAnimation* mAnimation;
        Ogre::Real mTimeStart; 
        Ogre::Real mTimeEnd;
        Ogre::Real mWeightStart;
        Ogre::Real mWeightEnd;
};

/** Eine Klasse zum automatischen �berblenden zwischen zwei (oder mehr) Animationen.
  * Zur Zeit nur lineares �berblenden. 
  * @todo M�glicherweise mit Animation verschmelzen oder selbe Basis geben...
  * Jede Animation ben�tigt - FadeAnfang, FadeEnde
  */
class _RlCoreExport FadeAnimation : public BaseAnimation
{
public:
	// Kon-/Destruktor
	FadeAnimation();
	virtual ~FadeAnimation();

    /// F�gt eine Animtion hinzu, die zwischen d
    void addAnimation( MeshAnimation* anim, Ogre::Real timeStart, 
        Ogre::Real timeEnd, Ogre::Real weightStart, Ogre::Real weightEnd );
    void removeAnimation( MeshAnimation* anim );
    /// �berschrieben von MeshAnimation
	virtual void doAddTime( Ogre::Real timePassed );

	/// Gibt zur�ck ob dieser Fader nach Beendigung automatisch gel�scht wird
	bool isDeleteOnFinish() const;
	/**	L�scht diesen Fader nach Beendigung automatisch */
	void setDeleteOnFinish( bool deleteOnFinish );
    /// Checks if one of the blended Animations is anim
    bool containsAnimation( BaseAnimation* anim ) const;
private:
    typedef std::set<AnimationFadeOptions*> FadeOptionsSet;

    FadeOptionsSet mFadeOptions;
	/// L�schen wenn das Faden beendet ist
	bool mDeleteOnFinish;	
    bool mFadeSoft;
    bool mDeleteAnimationsOnFinish;
};

}
#endif
