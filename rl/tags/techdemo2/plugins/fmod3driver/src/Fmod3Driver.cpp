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
#include "Fmod3Driver.h"

extern "C" {
    #include <fmod.h>
    #include <fmod_errors.h>
}

#include "ConfigFile.h"
#include "Fmod3SoundSample.h"
#include "Fmod3SoundStream.h"
#include "Fmod3SoundChannel.h"
#include "Fmod3Listener.h"
#include "Logger.h"
#include "SoundResource.h"


using namespace Ogre;

Ogre::String rl::Fmod3Driver::NAME = "FMOD3";

namespace rl
{

/** 
 * Konstruktor
 * @author JoSch
 * @date 12-23-2005
 */
Fmod3Driver::Fmod3Driver(Ogre::ResourceManager* soundResourceManager):
    SoundDriver(soundResourceManager)
{
}

/** 
 * Destruktor
 * @author JoSch
 * @date 12-23-2005
 */
Fmod3Driver::~Fmod3Driver()
{
}

/** 
 * Zeigt an, ob der Treiber funktionsf�hig ist.
 * @return Immer TRUE.
 * @author JoSch
 * @date 12-23-2005
 */
bool Fmod3Driver::isDriverAvailable()
{
    try {
		// TODO Anhand der Konfiguration
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
		FSOUND_SetOutput(FSOUND_OUTPUT_DSOUND);
#else
        FSOUND_SetOutput(FSOUND_OUTPUT_ESD);
#endif
        FSOUND_SetMixer(FSOUND_MIXER_QUALITY_AUTODETECT);
        bool success = FSOUND_Init(44100, 32, 0);
        if (success)
        {
            FSOUND_Close();
        }
        return success; 
    } catch (...)
    {
        return false;
    }
    return false;
}

/** 
 * Starte den Nulltreiber und mache alle Initialisierung.
 * @author JoSch
 * @date 12-23-2005
 */
void Fmod3Driver::init()
{
	if (FSOUND_GetVersion() < FMOD_VERSION)
    {
		Throw(RuntimeException, 
			String("Error : You are using the wrong DLL version!  You should be using FMOD ")
			+ StringConverter::toString(FMOD_VERSION));
    }
	// Daten sammeln
	collectData();

    // fmod initialisieren und Fehler zuruecksetzen.
	// TODO Initialisieren wie in Config steht.
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	FSOUND_SetOutput(FSOUND_OUTPUT_DSOUND);
#else
    FSOUND_SetOutput(FSOUND_OUTPUT_ESD);
#endif

	FSOUND_SetMixer(FSOUND_MIXER_QUALITY_AUTODETECT);

	switch (FSOUND_GetOutput())
    {
        case FSOUND_OUTPUT_NOSOUND:
			Logger::getSingleton().log(Logger::MULTIMEDIA, Logger::LL_MESSAGE, "FMOD Output: NoSound");
            break;
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
        case FSOUND_OUTPUT_WINMM:
            Logger::getSingleton().log(Logger::MULTIMEDIA, Logger::LL_MESSAGE, "FMOD Output: Windows Multimedia Waveout");
            break;
        case FSOUND_OUTPUT_DSOUND:
            Logger::getSingleton().log(Logger::MULTIMEDIA, Logger::LL_MESSAGE, "FMOD Output: Direct Sound");
            break;
        case FSOUND_OUTPUT_ASIO:
            Logger::getSingleton().log(Logger::MULTIMEDIA, Logger::LL_MESSAGE, "FMOD Output: ASIO");
            break;
		default:
            Logger::getSingleton().log(Logger::MULTIMEDIA, Logger::LL_MESSAGE, "FMOD Output: Unknown");
            break;
#else
        case FSOUND_OUTPUT_OSS:
            Logger::getSingleton().log(Logger::MULTIMEDIA, Logger::LL_MESSAGE, "FMOD Output: Open Sound System");
            break;
        case FSOUND_OUTPUT_ESD:
            Logger::getSingleton().log(Logger::MULTIMEDIA, Logger::LL_MESSAGE, "FMOD Output: Enlightment Sound Daemon");
            break;
        case FSOUND_OUTPUT_ALSA:
            Logger::getSingleton().log(Logger::MULTIMEDIA, Logger::LL_MESSAGE, "FMOD Output: Alsa");
            break;
		default:
            Logger::getSingleton().log(Logger::MULTIMEDIA, Logger::LL_MESSAGE, "FMOD Output: Unknown");
            break;
#endif
    }

    unsigned int caps = 0;
    FSOUND_GetDriverCaps(FSOUND_GetDriver(), &caps);
    
    Logger::getSingleton().log(Logger::MULTIMEDIA, Logger::LL_MESSAGE, "FMOD Driver capabilities");
	const char* driverName = FSOUND_GetDriverName(FSOUND_GetDriver());
    if (driverName != NULL)
		Logger::getSingleton().log(Logger::MULTIMEDIA, Logger::LL_MESSAGE, driverName);
	else
		Logger::getSingleton().log(Logger::MULTIMEDIA, Logger::LL_MESSAGE, "No driver");
    if (!caps)
        Logger::getSingleton().log(Logger::MULTIMEDIA, Logger::LL_MESSAGE, "- This driver will support software mode only.\n  It does not properly support 3D sound hardware.");
    if (caps & FSOUND_CAPS_HARDWARE)
        Logger::getSingleton().log(Logger::MULTIMEDIA, Logger::LL_MESSAGE, "- Driver supports hardware 3D sound!");
    if (caps & FSOUND_CAPS_EAX2)
        Logger::getSingleton().log(Logger::MULTIMEDIA, Logger::LL_MESSAGE, "- Driver supports EAX 2 reverb!");
    if (caps & FSOUND_CAPS_EAX3)
        Logger::getSingleton().log(Logger::MULTIMEDIA, Logger::LL_MESSAGE, "- Driver supports EAX 3 reverb!");

    // File Callbacks fuer FMOD setzen
    FSOUND_File_SetCallbacks(
        (FSOUND_OPENCALLBACK)Fmod3Driver::open,
        (FSOUND_CLOSECALLBACK)Fmod3Driver::close,
        (FSOUND_READCALLBACK)Fmod3Driver::read,
        (FSOUND_SEEKCALLBACK)Fmod3Driver::seek,
        (FSOUND_TELLCALLBACK)Fmod3Driver::tell); 

 	if (!FSOUND_Init(44100, 32, 0))
    {
        Throw(RuntimeException, "FMOD not initialized");
    }
	Logger::getSingleton().log(Logger::MULTIMEDIA, Logger::LL_MESSAGE, 
		String("fmod initialisiert, Fehler: ")
		+ FMOD_ErrorString(FSOUND_GetError()));
   
    FSOUND_3D_SetRolloffFactor(1.0);
    FSOUND_SetSFXMasterVolume(255);

    // Wir initialisieren den Listener
    // Position of the listener.
    float v[3] = {0, 0, 0};
    FSOUND_3D_Listener_SetAttributes(v, v, 1, 0, 0, 1, 0, 0); 
    Logger::getSingleton().log(Logger::MULTIMEDIA, Logger::LL_MESSAGE, "FMOD Driver: Listener set");
    
    // Verschiedene Daten ausgeben.
    printData();    

}

/** 
 * Beeende den Nulltreiber und gebe alle Resourcen frei.
 * @author JoSch
 * @date 12-23-2005
 */
void Fmod3Driver::deInit()
{
    FSOUND_Close();
}

/** 
 * Gibt den Anzeigename des Treiber zur�ck.
 * @return Anzeigename
 * @author JoSch
 * @date 12-23-2005
 */
Ogre::String Fmod3Driver::getName() const
{
    return NAME;
}

/**
 * @param handle Das Handle der zu schliessenden Datei
 * @author JoSch
 * @date 08-22-2005
 */
void Fmod3Driver::close(void *handle)
{
    if (handle != 0)
    {
    	SoundResourcePtr *sound = static_cast<SoundResourcePtr*>(handle);
    	if (!sound->isNull())
    	{
    		DataStreamPtr ds = (*sound)->getDataStream();
        	if (!ds.isNull())
        	{
				Logger::getSingleton().log(Logger::MULTIMEDIA, Logger::LL_MESSAGE, "Stream closed");
            	ds->close();
        	}
	        delete sound;
        }
    }
}

/**
 * @param name. Der der Name der zu oeffnenden Datei
 * @author JoSch
 * @date 08-22-2005
 */
void *Fmod3Driver::open(const char *name)
{
	SoundResourcePtr *res = new SoundResourcePtr(sSoundResourceManager->getByName(name));
    (*res)->load();
    Logger::getSingleton().log(
		Logger::MULTIMEDIA, 
		Logger::LL_MESSAGE, 
		"Opened stream " + String((*res)->getOrigin()));
    return res;
}

/**
 * @param buffer. Zu fuellender Buffer
 * @param size. Die Groesse der Daten
 * @param handle. Handle der Datei
 * @return. Anzahl der Bytes, die ERFOLGREICH gelesen wurden.
 * @author JoSch
 * @date 08-22-2005
 */
int Fmod3Driver::read(void *buffer, int size, void *handle)
{
    if (handle != 0)
    {
    	SoundResourcePtr *sound = static_cast<SoundResourcePtr*>(handle);
    	if (!sound->isNull())
    	{
    		DataStreamPtr ds = (*sound)->getDataStream();
        	if (!ds.isNull())
        	{
	            return ds->read(buffer, size);
        	}
        }
    }
    return 0;
}

/**
 * @param handle. Das Handle der Datei.
 * @param pos. Gesuchte Dateiposition.
 * @param mode. Seekmode.
 * @return 0 bei Erfolg, Nicht-0 ansonsten.
 * @author JoSch
 * @date 08-22-2005
 */
int Fmod3Driver::seek(void *handle, int pos, signed char mode)
{
    if (handle != 0)
    {
    	SoundResourcePtr *sound = static_cast<SoundResourcePtr*>(handle);
    	if (!sound->isNull())
    	{
    		DataStreamPtr ds = (*sound)->getDataStream();
        	if (!ds.isNull())
        	{
            	if (mode == SEEK_END)
            	{
                	pos += ds->size();
            	}
            	if (mode == SEEK_CUR)
            	{
                	pos += ds->tell();
            	}
            	ds->seek(pos);
            	return 0;
        	}
        }
    }
    return -1;
}

/**
 * @return Aktuelle Position
 * @author JoSch
 * @date 08-22-2005
 */
int Fmod3Driver::tell(void *handle)
{
    if (handle != 0)
    {
    	SoundResourcePtr *sound = static_cast<SoundResourcePtr*>(handle);
    	if (!sound->isNull())
    	{
    		DataStreamPtr ds = (*sound)->getDataStream();
        	if (!ds.isNull())
        	{
	            return ds->tell();
        	}
        }
    }
    return 0;
}

/**
 * Update-Aufgaben erledigen
 * @author JoSch
 * @date 03-06-2006
 */
 void Fmod3Driver::update()
 {
    Logger::getSingleton().log(Logger::MULTIMEDIA, Logger::LL_TRIVIAL, "Updaten von Fmod3");
 	FSOUND_Update();
 }
 
 /**
  * Einen Sound-Stream mit Resource erzeugen
  * @return Der erzeugte Stream
  * @author JoSch
  * @date 03-06-2006
  */
Sound *Fmod3Driver::createStream(const SoundResourcePtr &res)
{
 	Sound *sound = new Fmod3SoundStream(res);
 	return sound;
}

/**
 * Einen Sound-Sample mit Resource erzeugen
 * @return Das erzeugte Sample
 * @author JoSch
 * @date 03-06-2006
 */
Sound *Fmod3Driver::createSample(const SoundResourcePtr &res)
{
 	Sound *sound = new Fmod3SoundSample(res);
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
SoundChannel *Fmod3Driver::createChannel(Sound *sound, const Ogre::String &name)
{
 	SoundChannel *channel = new Fmod3SoundChannel(this, sound, name);
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
ListenerMovable *Fmod3Driver::createListener(const Ogre::String &name)
{
    ListenerMovable *listener = new Fmod3Listener(name);
    return listener;
}

/**
 * Ausdruck einiger Werte des Soundsystems.
 * @author JoSch
 * @date 01-20-2006
 */
void Fmod3Driver::printData() const
{
	int numDrivers = FSOUND_GetNumDrivers();
	int activeDriver = FSOUND_GetDriver();
	for(int driver = 0; driver < numDrivers; driver++)
	{
		Logger::getSingleton().log(Logger::MULTIMEDIA, 
			Logger::LL_MESSAGE, 
			String("FMOD Driver #")
			+ StringConverter::toString(driver) + ": " +
			+ FSOUND_GetDriverName(driver)
			+ (driver == activeDriver ? "(active)" : ""));
	}

    switch (FSOUND_GetMixer())
    {
        case FSOUND_MIXER_BLENDMODE:
			Logger::getSingleton().log(Logger::MULTIMEDIA, Logger::LL_MESSAGE, "FMOD Mixer: FSOUND_MIXER_BLENDMODE");
            break;
        case FSOUND_MIXER_MMXP5:
            Logger::getSingleton().log(Logger::MULTIMEDIA, Logger::LL_MESSAGE, "FMOD Mixer: FSOUND_MIXER_MMXP5");
            break;
        case FSOUND_MIXER_MMXP6:
            Logger::getSingleton().log(Logger::MULTIMEDIA, Logger::LL_MESSAGE, "FMOD Mixer: FSOUND_MIXER_MMXP6");
            break;
        case FSOUND_MIXER_QUALITY_FPU:
            Logger::getSingleton().log(Logger::MULTIMEDIA, Logger::LL_MESSAGE, "FMOD Mixer: FSOUND_MIXER_QUALITY_FPU");
            break;
        case FSOUND_MIXER_QUALITY_MMXP5:
            Logger::getSingleton().log(Logger::MULTIMEDIA, Logger::LL_MESSAGE, "FMOD Mixer: FSOUND_MIXER_QUALITY_MMXP5");
            break;
        case FSOUND_MIXER_QUALITY_MMXP6:
            Logger::getSingleton().log(Logger::MULTIMEDIA, Logger::LL_MESSAGE, "FMOD Mixer: FSOUND_MIXER_QUALITY_MMXP6");
            break;
        default:
            Logger::getSingleton().log(Logger::MULTIMEDIA, Logger::LL_MESSAGE, "FMOD Mixer: Unknown");
            break;
    };
 
    int num2d, num3d, total;
    FSOUND_GetNumHWChannels(&num2d, &num3d, &total);
    String line = 
		"HW channels 3D: " 
		+ StringConverter::toString(num3d) 
		+ ", 2D: " 
		+ StringConverter::toString(num2d)
		+ ", Total: " 
		+ StringConverter::toString(total);
    Logger::getSingleton().log(Logger::MULTIMEDIA, Logger::LL_MESSAGE, line);
}

void Fmod3Driver::setMasterVolume(const Ogre::Real& vol)
{
	SoundDriver::setMasterVolume(vol);
	FSOUND_SetSFXMasterVolume(int(vol * 255.0f));
}

/*
 * Die Konfiguration in Datei schreiben.
 * @author JoSch
 * @date 05-07-2006
 * @param conf Die Konfigurationdatei zum Schreiben.
 */
void Fmod3Driver::saveConf(ConfigFile &conf) const
{
	SoundDriver::saveConf(conf);
	conf.setValue(mRolloffFactor, "3DRolloffFactor", "Fmod3");
}

/*
 * Die Konfiguration lesen
 * @author JoSch
 * @date 05-07-2006
 * @param conf Die Konfigurationdatei, aus der gelesen werden soll
 */
void Fmod3Driver::loadConf(ConfigFile &conf)
{
	SoundDriver::loadConf(conf);
	mRolloffFactor = conf.getValue(Ogre::Real(1.0), "3DRolloffFactor", "Fmod3");
}

/*
 * Den Konfigurationsdialog aufrufen
 * @author JoSch
 * @date 05-07-2006
 */
void Fmod3Driver::doConfig()
{
    // DO NOTHING FOR NOW
}

/*
 * Die gesammelten Daten zur�ckgeben
 * @return Die gesammelten Daten
 * @author JoSch
 * @date 05-31-2006
 */
const DriverMap &Fmod3Driver::getDriverData() const
{
	return mDriverData;
}

/*
 * Die Treiberdaten f�r einen Output sammeln
 * @param output Die Fmod-Konstante, f�r die wir die Treiberdaten sammeln.
 * @author JoSch
 * @date 05-31-2006
 */
const StringList Fmod3Driver::getDriversForOutput(int output) const
{
	StringList retVal;
	FSOUND_SetOutput(output);
	int numDrivers = FSOUND_GetNumDrivers();
	for(int driver = 0; driver < numDrivers; driver++)
	{
		String drivername = String(FSOUND_GetDriverName(driver));
		retVal.push_back(drivername);
	}
	return retVal;
}

/*
 * Die Treiberdaten sammeln
 * @author JoSch
 * @date 05-31-2006
 */

void Fmod3Driver::collectData()
{
	mDriverData.clear();
	mDriverData["NOSOUND"] = getDriversForOutput(FSOUND_OUTPUT_NOSOUND);
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	mDriverData["WINMM"] = getDriversForOutput(FSOUND_OUTPUT_WINMM);
	mDriverData["DSOUND"] = getDriversForOutput(FSOUND_OUTPUT_DSOUND);
	mDriverData["A3D"] = getDriversForOutput(FSOUND_OUTPUT_A3D);
#else
	mDriverData["ESD"] = getDriversForOutput(FSOUND_OUTPUT_ESD);
	mDriverData["ALSA"] = getDriversForOutput(FSOUND_OUTPUT_ALSA);
	mDriverData["OSS"] = getDriversForOutput(FSOUND_OUTPUT_OSS);
#endif
}

void Fmod3Driver::setRolloffFactor(const Ogre::Real& factor)
{
	mRolloffFactor = factor;
	FSOUND_3D_SetRolloffFactor(factor);
}

const Ogre::Real Fmod3Driver::getRolloffFactor()
{
	return mRolloffFactor;
}


}
