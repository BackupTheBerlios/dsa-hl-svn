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

#include "Fmod3SoundChannel.h"

#include <OgreMovableObject.h>
#include "Fmod3Sound.h"
#include "SoundDriver.h"
#include "SoundEvents.h"

extern "C" {
    #include <fmod.h>
}

Ogre::String rl::Fmod3SoundChannel::msMovableType = "Fmod3SoundChannel";

using namespace Ogre; 

namespace rl
{

Fmod3SoundChannel::Fmod3SoundChannel(SoundDriver* driver, Sound *sound, const Ogre::String &name)
 : SoundChannel(driver, sound, name),
   mChannel(NO_CHANNEL)
{
	mRolloffStartDistance = 1.0f;
	mRolloffEndDistance = 1000000000.0f;
}

Fmod3SoundChannel::~Fmod3SoundChannel()
{
    FSOUND_StopSound(getChannel());
}

/**
 * @author JoSch
 * @date 07-23-2005
 */
void Fmod3SoundChannel::play()
{
    if (!getSound()->isValid())
    {
        getSound()->load();
    }
    setChannel(dynamic_cast<Fmod3Sound*>(getSound())->createChannel());

	float vol;
	if (is3d())
	{
		vol = getDriver()->getDefaultSoundVolume();
	}
	else
	{
		vol = getDriver()->getDefaultMusicVolume();
	}
	setVolume(vol);

    setPosition(mPosition);
    setDirection(mDirection);
	setVelocity(mVelocity); 
    FSOUND_3D_SetMinMaxDistance(mChannel, mRolloffStartDistance, mRolloffEndDistance);
    pause(false);
    SoundPlayEvent event = SoundPlayEvent(this, SoundPlayEvent::STARTEVENT);
    dispatchEvent(&event);
}

/**
 * @author JoSch
 * @date 07-04-2005
 * @return Der Soundkanal
 */
const signed int Fmod3SoundChannel::getChannel() const
{
    return mChannel;
}

/**
 * @author JoSch
 * @date 07-21-2005
 * @param channel Der Soundkanal
 */
void Fmod3SoundChannel::setChannel(signed int channel)
{
    mChannel = channel;
}

/**
 * @author JoSch
 * @date 03-11-2005
 * @return Den Objekttypen
 */
const String& Fmod3SoundChannel::getMovableType() const
{
    return msMovableType;
}


/**
 * @return Die aktuelle Richtung der Soundquelle
 * @author JoSch
 * @date 07-23-2004
 */
const Quaternion Fmod3SoundChannel::getDirection() const
{
    return mDirection;
}

/**
 * @param direction Die neue Richtung der Soundquelle.
 * @author JoSch
 * @date 07-23-2004
 */
void Fmod3SoundChannel::setDirection (const Quaternion& direction)
{
    mDirection = direction;
}

/**
 * @return Ist der Kanal gueltig?
 * @author JoSch
 * @date 08-05-2005
 */
bool Fmod3SoundChannel::isValid() const
{
    return SoundChannel::isValid() && (mChannel > 0);
}

/**
 * @return Spielt die Soundquelle noch?
 * @author JoSch
 * @date 07-04-2005
 */
const bool Fmod3SoundChannel::isPlaying() const
{
    if (isValid())
    {
        return FSOUND_IsPlaying(getChannel());
    }
    return false;
}



/**
 * @return Die aktuelle Position der Soundquelle
 * @author JoSch
 * @date 07-04-2005
 */
const Vector3 Fmod3SoundChannel::getPosition() const
{
    if (isValid())
    {
        float pos[3];
        FSOUND_3D_GetAttributes(getChannel(), pos, 0);
        Vector3 result(pos);
        return result;
    }
    return mPosition;
}

class FmodSoundSample;
/**
 * @param position Die neue Position der Soundquelle.
 * @author JoSch
 * @date 07-04-2005
 */
void Fmod3SoundChannel::setPosition(const Vector3& position)
{
    if (isValid())
    {
		// Bei Fmod ist +z "nach vorn", im Gegensatz zu -z bei Ogre
        float pos[] = {position.x, position.y, -position.z}; 

        Logger::getSingleton().log(Logger::MULTIMEDIA, Logger::LL_TRIVIAL, "Pos: "
            + StringConverter::toString(position.x)
            + StringConverter::toString(position.y)
            + StringConverter::toString(position.z));
        FSOUND_3D_SetAttributes(getChannel(), pos, 0);
    }
    mPosition = position;
}

/**
 * @return Die aktuelle Geschwindigkeit der Soundquelle
 * @author JoSch
 * @date 07-04-2005
 */
const Vector3 Fmod3SoundChannel::getVelocity() const
{
    if (isValid())
    {
        float vel[3];
        FSOUND_3D_GetAttributes(getChannel(), 0, vel);
        Vector3 result(vel);
        return result;
    }
    return mVelocity;
}

/**
 * @param velocity Die neue Geschwindigkeit der Soundquelle.
 * @author JoSch
 * @date 07-04-2005
 */
void Fmod3SoundChannel::setVelocity(const Vector3& velocity)
{
    if (isValid())
    {
        float vel[] = {velocity.x, velocity.y, -velocity.z};
        FSOUND_3D_SetAttributes(getChannel(), 0, vel);
    }
    mVelocity = velocity;
}

/**
 * @return Die aktuelle Lautstaerke der Soundquelle
 * @author JoSch
 * @date 07-04-2005
 */
const Ogre::Real Fmod3SoundChannel::getVolume() const
{
    if (isValid())
    {
        return (float)FSOUND_GetVolume(getChannel()) / 255.0;
    }
    return mVolume;
}

/**
 * @param gain Die neue Lautstarke der Soundquelle.
 * @author JoSch
 * @date 07-04-2005
 */
void Fmod3SoundChannel::setVolume(const Ogre::Real gain)
{
	float volume = gain;
	if (volume > 1.0)
		volume = 1.0;
	if (volume < 0.0)
		volume = 0.0;

    mVolume = volume;
    if (isValid())
    {
        FSOUND_SetVolume(getChannel(), int(volume * 255.0));
    }
}

/**
 * @param pausing TRUE laesst den Sound unterbrechen.
 * @author JoSch
 * @date 07-04-2005
 */
void Fmod3SoundChannel::pause(bool pausing)
{
    if (isValid())
    {
        FSOUND_SetPaused(getChannel(), pausing);
        if (pausing)
        {
            SoundPlayEvent event = SoundPlayEvent(this, SoundPlayEvent::PAUSEEVENT);
            dispatchEvent(&event);
        }
    }
}

/**
 * @author JoSch
 * @date 07-23-2004
 */
void Fmod3SoundChannel::stop()
{
    if (isValid())
    {
        FSOUND_StopSound(getChannel());
    }
    SoundPlayEvent event = SoundPlayEvent(this, SoundPlayEvent::STOPEVENT);
    dispatchEvent(&event);
}

/**
 * @return TRUE wenn der Sound unterbrochen wurde.
 * @author JoSch
 * @date 07-04-2005
 */
bool Fmod3SoundChannel::isPaused()
{
    if (isValid())
    {
        return FSOUND_GetPaused(getChannel());
    }
    return true;
}

void Fmod3SoundChannel::setRolloffStartDistance(const Ogre::Real& distance)
{
	SoundChannel::setRolloffStartDistance(distance);

	if (isValid())
	{
		float min, max;
		FSOUND_3D_GetMinMaxDistance(mChannel, &min, &max);
		FSOUND_3D_SetMinMaxDistance(mChannel, distance, max);
	}
}

const Ogre::Real Fmod3SoundChannel::getRolloffStartDistance()
{
	if (isValid())
	{
		float min, max;
		FSOUND_3D_GetMinMaxDistance(mChannel, &min, &max);
		return min;
	}
	return SoundChannel::getRolloffStartDistance();
}

void Fmod3SoundChannel::setRolloffEndDistance(const Ogre::Real& distance)
{
	SoundChannel::setRolloffEndDistance(distance);

	if (isValid())
	{
		float min, max;
		FSOUND_3D_GetMinMaxDistance(mChannel, &min, &max);
		FSOUND_3D_SetMinMaxDistance(mChannel, min, distance);
	}
}

const Ogre::Real Fmod3SoundChannel::getRolloffEndDistance()
{
	if (isValid())
	{
		float min, max;
		FSOUND_3D_GetMinMaxDistance(mChannel, &min, &max);
		return max;
	}
	return SoundChannel::getRolloffEndDistance();
}

};
