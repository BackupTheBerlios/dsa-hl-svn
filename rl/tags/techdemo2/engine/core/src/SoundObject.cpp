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

#include "SoundObject.h"
#include "Actor.h"
#include "SoundResource.h"
#include "SoundManager.h"
#include <Ogre.h>
#include "Sound.h"
#include "SoundChannel.h"
#include "SoundDriver.h"
#include "SoundManager.h"
#include "Logger.h"

using namespace Ogre;

namespace rl {
   
/**
 * @param sound. Der Sound, Besitz wird übernommen.
 * @author JoSch
 * @date 03-11-2005
 */   
SoundObject::SoundObject(Sound *sound, const Ogre::String &name)
    : ActorControlledObject(),
	PlaylistObject(),
	EventListener<SoundEvent>()
{
	SoundDriver *driver = SoundManager::getSingleton().getActiveDriver();
	if (driver != 0)
	{
    	SoundChannel *sc = driver->createChannel(sound, name);
        sc->addEventListener(this);
    	mMovableObject = sc;
	}
}

/**
 * @author JoSch
 * @date 03-11-2005
 */   
SoundObject::~SoundObject()
{
    if (mMovableObject)
    {
        SoundChannel *sc = static_cast<SoundChannel*>(mMovableObject);
        if (sc)
        {
            sc->stop();
            sc->removeEventListener(this);
			SoundManager::getSingleton().getActiveDriver()->removeFromLists(sc);
        }
        delete mMovableObject;
    }
}

/**
 * @return Immer false, weil kein Meshobjekt.
 * @author JoSch
 * @date 03-11-2005
 */   
bool SoundObject::isMeshObject()
{
	return false;
}

/**
 * @author JoSch
 * @date 03-11-2005
 */   
void SoundObject::_update()
{
    ActorControlledObject::_update();
    SoundChannel *channel = getSoundChannel();
    Actor *actor = getActor();
    if (!channel || !actor) // Einer ist Null
    {
        return;
    }
    if (!channel->isValid())
    {
        return;
    }
    if (isAttached())
    {
        channel->setPosition(actor->getWorldPosition());
        channel->setDirection(actor->getWorldOrientation()); 

       Logger::getSingleton().log(Logger::CORE, Logger::LL_MESSAGE, "Pos SoundObject: "
        + StringConverter::toString(actor->getWorldPosition().x) + " "
        + StringConverter::toString(actor->getWorldPosition().y) + " "
        + StringConverter::toString(actor->getWorldPosition().z));
    }
}

void SoundObject::play( )
{
    getSoundChannel()->play();
    if (is3d())
    {
	   _update();
    }
}

void SoundObject::pause(bool pausing)
{
    getSoundChannel()->pause(pausing);
}

bool SoundObject::isPaused()
{
    return getSoundChannel()->isPaused();
}

void SoundObject::stop()
{
	PlaylistObject::stop();
    getSoundChannel()->stop();
}


bool SoundObject::isLooping() const
{
    return getSoundChannel()->isLooping();
}

void SoundObject::setLooping( bool looping )
{
    getSoundChannel()->setLooping( looping );
}

void SoundObject::setVolume(float volume)
{
	if (volume > 1.0)
		volume = 1.0;
	else if (volume < 0.0)
		volume = 0.0;

	getSoundChannel()->setVolume(int (volume * 100.0));
}

bool SoundObject::is3d() const
{
	return getSoundChannel()->is3d();
}

void SoundObject::set3d( bool is3d )
{
	getSoundChannel()->set3d(is3d);
}

void SoundObject::load()
{
	PlaylistObject::load();
	getSoundChannel()->load();
}

void SoundObject::unload()
{
	PlaylistObject::unload();
    getSoundChannel()->unload();
}

/**
 * @param Der bewegbare Sound
 * @author JoSch
 * @date 03-11-2005
 */   
SoundChannel* SoundObject::getSoundChannel() const
{
    return static_cast<SoundChannel*>(mMovableObject);
}

/**
 * @return Immer "SoundObject"
 * @author JoSch
 * @date 03-11-2005
 */   
String SoundObject::getObjectType()
{
    return "SoundObject";
}

bool SoundObject::eventRaised(SoundEvent *event)
{
	int newReason = PlaylistEvent::NOPEVENT;
	switch(event->getReason())
	{
		case SoundPlayEvent::PAUSEEVENT:
			newReason = PlaylistEvent::PAUSEEVENT;
			break;
		case SoundPlayEvent::STARTEVENT:
			newReason = PlaylistEvent::STARTEVENT;
			break;
		case SoundPlayEvent::STOPEVENT:
			newReason = PlaylistEvent::STOPEVENT;
			break;
		default:
			break;
	}
	PlaylistEvent newEvent = PlaylistEvent(event->getSource(), newReason);
    dispatchEvent(&newEvent);
    return true;
}

void SoundObject::pause()
{
	PlaylistObject::pause();
    pause(true);
}

void SoundObject::start()
{
	PlaylistObject::start();
    pause(false);
}

float SoundObject::getLength() const
{
	return getSoundChannel()->getSound()->getLength();
}

}
