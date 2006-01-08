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

#ifndef __AnimationManager_H__
#define __AnimationManager_H__

#include <map>
#include <OgreSingleton.h>

#include "GameTask.h"
#include "CorePrerequisites.h"

namespace rl {

class Actor;
class Animation;
class TrackAnimation;
class MeshObject;

/** 
	Diese Klasse verwaltet s�mtliche Animationen und k�mmert sich um das Starten und Stoppen dieser
	@see Animation, TrackAnimation
*/
class _RlCoreExport AnimationManager
  : public GameTask,
    protected Ogre::Singleton<AnimationManager>
{
public:
	/// Interpolations-Modus, Spline ist besser, braucht auch mehr Perfomance
	enum InterpolationMode { IM_LINEAR, IM_SPLINE };
	/// RotationsInterpolations-Modus, Spherical ist genauer, aber aufw�ndiger
	enum RotationInterpolationMode { RIM_LINEAR, RIM_SPHERICAL };

    /** Default Constructor */
	AnimationManager( );
	/** Default Deconstructor */
    virtual ~AnimationManager();

	/** Erzeugt eine Animation, tr�gt diese ein und beginnt
		sofort mit dem Abspielen.
		@param	animState Der AnimationState
		@param	speed	Die Geschwindigkeit
		@param	timesToPlay	Die Wiederholungen
		@see Animation
	*/
    Animation* addAnimation(Ogre::AnimationState* animState, MeshObject* mesh,
        Ogre::Real speed=1.0, unsigned int timesToPlay=0 ); 
	/** Gibt die Animation zur�ck, die zum AnimationState geh�rt 
     *  @returns NULL wenn es die Animation nicht gibt
     */
	Animation* getAnimation(Ogre::AnimationState* animState) const;
	/// Entfernt eine Animation, und stoppt das Abspielen dieser
    void removeAnimation(Ogre::AnimationState* animState);
	/** Erzeugt eine neue, leere TrackAnimation, der der SceneNode
		des Actors zugeordnet wird.
		@param	actor	Der zugeordnete Actor
		@param	name	Der einzigartige Name der Animation
		@param	length	Die gew�nschte L�nge des Tracks
	*/
	TrackAnimation* createTrackAnimation(Actor* actor,
	    const Ogre::String& name, Ogre::Real length );
    /// Entfernt alle Animationen
    void removeAllAnimations();
	/// Entfernt eine Animation
	void removeAnimation(Animation* anim);
    /// Ersetzt eine alte Animation durch eine Neue
    Animation* replaceAnimation(Animation* oldAnim,  
	Ogre::AnimationState* newAnimState, Ogre::Real speed=1.0, unsigned int timesToPlay=0  );
	/// Entfernt eine TrackAnimation dieses Actors
	void removeTrackAnimation( Actor* act, const Ogre::String& name );
	/// Entfernt alle TrackAnimations dieses Actors
	void removeAllTrackAnimations( Actor* act );
	

	/** Globale Beschleunigung, f�r SlowMotion oder andere sinnige Effekte
		@param speed Der Beschleunigungsfaktor. 
		@remarks Negative Werte lassen das Spiel nicht r�ckw�rts laufen, nur
			die Animationen.
	*/
	void setGlobalAnimationSpeed( Ogre::Real speed );
	/// Gibt die globale Beschleunigung zur�ck
	Ogre::Real getGlobalAnimationSpeed( ) const;
	
	/// Setzt den StandardInterpolationsModus f�r neue Animationen
	void setDefaultInterpolationMode( AnimationManager::InterpolationMode im );
	/// Gibt den StandardInterpolationsModus f�r neue Animationen zur�ck
	AnimationManager::InterpolationMode getDefaultInterpolationMode() const;

	/// Setzt den StandardRotationsInterpolationsModus f�r neue Animationen
	void setDefaultRotationInterpolationMode( AnimationManager::RotationInterpolationMode rim );
	/// Gibt den Standard RotationsInterpolationsModus f�r neue Animationen zur�ck
	AnimationManager::RotationInterpolationMode getDefaultRotationInterpolationMode() const;

	/// Geerbt von GameTask, wird in jedem Frame mit der vergangenen Zeit aufgerufen
    virtual void run(Ogre::Real timePassed);

	/// Singleton
    static AnimationManager & getSingleton(void);
	/// Singleton
	static AnimationManager * getSingletonPtr(void);
private:
    static void stopAnimation( Animation* anim );

    typedef std::map<Ogre::AnimationState*,Animation*> AnimMap;
    /// Alle auszuf�hrenden Animationen
    AnimMap mAnimationMap;

	/// Die globale Beschleunigung
	Ogre::Real mGlobalAnimationSpeed;
};
}
#endif
