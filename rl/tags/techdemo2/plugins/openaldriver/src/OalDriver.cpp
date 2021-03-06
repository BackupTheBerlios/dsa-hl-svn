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
#include "OalDriver.h"

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    // OpenAL 1.1 unter Windows
	extern "C" {
		#include "al.h"
		#include "alc.h"
	}
#else
    // OpenAL 1.0 unter Linux
	extern "C" {
		#include "AL/al.h"
		#include "AL/alc.h"
	}
#endif

#include "OalSoundSample.h"
#include "OalSoundStream.h"
#include "OalSoundChannel.h"
#include "OalListener.h"

rl::CeGuiString rl::OalDriver::NAME = "OpenAL";

namespace rl
{

/** 
 * Konstruktor
 * @author JoSch
 * @date 12-23-2005
 */
OalDriver::OalDriver(Ogre::ResourceManager* soundResourceManager)
: SoundDriver(soundResourceManager)
{
}

/** 
 * Destruktor
 * @author JoSch
 * @date 12-23-2005
 */
OalDriver::~OalDriver()
{
}

/** 
 * Zeigt an, ob der Treiber funktionsfähig ist.
 * @return Immer TRUE.
 * @author JoSch
 * @date 12-23-2005
 */
bool OalDriver::isDriverAvailable()
{
    try {
        // Clear error code
        alGetError();
        ALCdevice *device = alcOpenDevice(0);
        
        if (device == 0)
        {
            return false;
        }
        
        //Create context(s)
        ALCcontext *context = alcCreateContext(device, 0);
        //Set active context
        alcMakeContextCurrent(context);
        // Get Error Code
        ALenum success = alGetError();
        //Disable context
        alcMakeContextCurrent(0);
        //Release context(s)
        alcDestroyContext(context);
        //Close device
        alcCloseDevice(device);
        
        return (success == AL_NO_ERROR);
    } catch (...)
    {
        return false;
    }
}

/** 
 * Starte den Nulltreiber und mache alle Initialisierung.
 * @author JoSch
 * @date 12-23-2005
 */
void OalDriver::init()
{
}

/** 
 * Beeende den Nulltreiber und gebe alle Resourcen frei.
 * @author JoSch
 * @date 12-23-2005
 */
void OalDriver::deInit()
{
}

/** 
 * Gibt den Anzeigename des Treiber zurück.
 * @return Anzeigename
 * @author JoSch
 * @date 12-23-2005
 */
CeGuiString OalDriver::getName() const
{
    return NAME;
}

/**
 * Update-Aufgaben erledigen
 * @author JoSch
 * @date 03-06-2006
 */
 void OalDriver::update()
 {
 	// Erstmal nichts zu tun.
 }
 
 /**
  * Einen Sound-Stream mit Resource erzeugen
  * @return Der erzeugte Stream
  * @author JoSch
  * @date 03-06-2006
  */
Sound *OalDriver::createStream(const SoundResourcePtr &res)
{
 	Sound *sound = new OalSoundStream(this, res);
 	return sound;
}

/**
 * Einen Sound-Sample mit Resource erzeugen
 * @return Das erzeugte Sample
 * @author JoSch
 * @date 03-06-2006
 */
Sound *OalDriver::createSample(const SoundResourcePtr &res)
{
 	Sound *sound = new OalSoundSample(this, res);
 	return sound;
}

/**
 * Einen Sound-Channel erzeugen
 * @return Das erzeugte Sample
 * @param sound Der Sound, der kapselt wird.
 * @param name Der Name des Channels.
 * @author JoSch
 * @date 03-06-2006
 */
SoundChannel *OalDriver::createChannel(Sound *sound, const Ogre::String &name)
{
 	SoundChannel *channel = new OalSoundChannel(this, sound, name);
    if (sound->is3d())
    {
        channel->setVolume(mDefaultSoundVolume);
        mSoundSet.insert(channel);
    } else
    {
        channel->setVolume(mDefaultMusicVolume);
        mMusicSet.insert(channel);
    }
 	return channel;
}

/**
 * Einen Soundlistener erzeugen
 * @return Der erzeugte Listener
 * @param name Der Name des Channels.
 * @author JoSch
 * @date 04-04-2006
 */
ListenerMovable *OalDriver::createListener(const Ogre::String &name)
{
    ListenerMovable *listener = new OalListener(name);
    return listener;
}

/**
 * Ausdruck einiger Werte des Soundsystems.
 * @author JoSch
 * @date 01-20-2006
 */
void OalDriver::printData() const
{
}

/*
 * Die Konfiguration in Datei schreiben.
 * @author JoSch
 * @date 05-07-2006
 * @param conf Die Konfigurationdatei zum Schreiben.
 */
void OalDriver::saveConf(ConfigFile &conf) const
{
	SoundDriver::saveConf(conf);
    // DO NOTHING
}

/*
 * Die Konfiguration lesen
 * @author JoSch
 * @date 05-07-2006
 * @param conf Die Konfigurationdatei, aus der gelesen werden soll
 */
void OalDriver::loadConf(ConfigFile &conf)
{
	SoundDriver::loadConf(conf);
    // DO NOTHING
}

/*
 * Den Konfigurationsdialog aufrufen
 * @author JoSch
 * @date 05-07-2006
 */
void OalDriver::doConfig()
{
    // DO NOTHING
}

}
