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
#include "MultimediaSubsystem.h"
#include "SoundManager.h"
#include "Logger.h"
#include "Video.h"
#include <stdio.h>
#include "SoundUpdateTask.h"
#include "SoundDriver.h"
#include "NullDriver.h"
#ifdef WITH_FMOD3
#include "Fmod3Driver.h"
#endif
#ifdef WITH_OAL
#include "OalDriver.h"
#endif
#include "ConfigFile.h"


using namespace Ogre;

template<> rl::MultimediaSubsystem* Singleton<rl::MultimediaSubsystem>::ms_Singleton = 0;

namespace rl {

/**
 * @return Das Singleton-Objekt
 * @author JoSch
 * @date 05-26-2004
 */
MultimediaSubsystem& MultimediaSubsystem::getSingleton(void)
{
	return Singleton<MultimediaSubsystem>::getSingleton();
}

/**
 * @return Das Singleton-Zeiger
 * @author JoSch
 * @date 05-26-2004
 */
MultimediaSubsystem* MultimediaSubsystem::getSingletonPtr(void)
{
    return Singleton<MultimediaSubsystem>::getSingletonPtr();
}

/**
 * @author JoSch
 * @date 05-26-2004
 */
MultimediaSubsystem::MultimediaSubsystem():
	mElapsedTime(0),
    mVideoList(),
    mDriverList(),
	mActiveDriver(0),
    mActiveListener(0),
    mSoundManager(0),
    mSoundUpdateTask(0)
{   
    SoundDriver *driver = NULL;
    // Die Treiberliste ermitteln.
    mDriverList.clear();
    // Immer Nulltreiber
    SoundDriver *nullDriver = new NullDriver();
    mDriverList.push_back(nullDriver);
#ifdef WITH_FMOD3
    // Fmod testen.
    driver = new Fmod3Driver();
    if (driver->isDriverAvailable())
    {
        mDriverList.push_back(driver);
        setActiveDriver(driver);
    } else {
        delete driver;
        setActiveDriver(nullDriver);
    }
#else
    // Nulltreiber als aktiven Treiber setzen.
    setActiveDriver(nullDriver);
#endif
#ifdef WITH_OAL
    // OpenAL testen.
    driver = new OalDriver();
    if (driver->isDriverAvailable())
    {
        mDriverList.push_back(driver);
    } else {
        delete driver;
    }
#endif
  	//Singletons erzeugen 
   	mSoundManager = new SoundManager();
    
    // SoundUpdates anschmeissen.	
    mSoundUpdateTask = new SoundUpdateTask();
}

/**
 * @author JoSch
 * @date 05-26-2004
 */
MultimediaSubsystem::~MultimediaSubsystem()
{
    shutdown();
}

void MultimediaSubsystem::shutdown()
{
    delete mSoundManager;
    mSoundManager = 0;
    delete mSoundUpdateTask;
    mSoundUpdateTask = 0;
    if (mActiveDriver != 0)
    {
        mActiveDriver->deInit();
        mActiveDriver = 0;
    }
    while (!mDriverList.empty())
    {
        delete *mDriverList.begin();
        mDriverList.erase(mDriverList.begin());
    }
}

/**
 * @author JoSch
 * @date 07-03-2005
 */
void MultimediaSubsystem::update(Real elapsedTime)
{
    setElapsedTime(elapsedTime);
    if (mActiveDriver != 0)
    {
        mActiveDriver->update();
    }
}

/**
 * @author JoSch
 * @date 07-24-2005
 */
Real MultimediaSubsystem::getElapsedTime() const
{
    return mElapsedTime;
}

/**
 * @author JoSch
 * @date 07-24-2005
 */
void MultimediaSubsystem::setElapsedTime(Real elapsedTime)
{
    mElapsedTime = elapsedTime;
}

/**
 * @param video Das Video, das hinzugef�gt werden soll.
 * @author JoSch
 * @date 09-06-2005
 */
void MultimediaSubsystem::addVideo(Video *video)
{
    mVideoList.push_back(video);
}

/**
 * @return Alle Videos in der Liste l�schen
 * @author JoSch
 * @date 09-06-2005
 */
void MultimediaSubsystem::clearVideos()
{
    mVideoList.clear();
}

/**
 * @param Das Video, das gel�scht werden soll
 * @author JoSch
 * @date 09-06-2005
 */
void MultimediaSubsystem::removeVideo(Video *video)
{
    for(VideoList::iterator i = mVideoList.begin();
        i != mVideoList.end();)
    {
        if (*i == video)
        {
            i = mVideoList.erase(i);
        } else {
            i ++;
        }
    }
}

/**
 * @return Die Liste der funktionierenden Treiber.
 * @author JoSch
 * @date 12-23-2005
 */
const DriverList& MultimediaSubsystem::getSoundDriverList() const
{
    return mDriverList;
}

/**
 * @param Der neue Treiber.
 * @author JoSch
 * @date 12-23-2005
 */
void MultimediaSubsystem::setActiveDriver(SoundDriver *driver)
{
    if (mActiveDriver != NULL && driver != NULL)
    {
		Logger::getSingleton().log(Logger::MULTIMEDIA, Logger::LL_NORMAL, (CeGuiString("Soundtreiber wird gewechselt von ")
            + mActiveDriver->getName() + CeGuiString(" zu ") + driver->getName()).c_str());
    } 
	else if (mActiveDriver != NULL)
    {
        Logger::getSingleton().log(Logger::MULTIMEDIA, Logger::LL_NORMAL, (CeGuiString("Soundtreiber wird gewechselt von ")
            + mActiveDriver->getName()).c_str());
    } 
	else 
	{
		Logger::getSingleton().log(Logger::MULTIMEDIA, Logger::LL_NORMAL, (CeGuiString("Soundtreiber wird gewechselt zu ")
             + driver->getName()).c_str());
    }
    mActiveDriver = driver;
    if (mActiveDriver != NULL)
    {
        mActiveDriver->init();
    }
}

/**
 * @return Der aktive Treiber.
 * @author JoSch
 * @date 12-23-2005
 */
SoundDriver* MultimediaSubsystem::getActiveDriver() const
{
    return mActiveDriver;
}

/**
 * @param Der neue Treiber.
 * @author JoSch
 * @date 12-23-2005
 */
void MultimediaSubsystem::setActiveListener(ListenerMovable *listener)
{
    mActiveListener = listener;
}

/**
 * @return Der aktive Listener.
 * @author JoSch
 * @date 04-04-2006
 */
ListenerMovable* MultimediaSubsystem::getActiveListener() const
{
    return mActiveListener;
}

/** 
 * Hole einen Treiber durch Angabe seines Namens
 * @param name Der name des gesuchten Treibers.
 * @return Der gesuchte Treiber
 * @author JoSch
 * @date 05-10-2006
 */
SoundDriver *MultimediaSubsystem::getDriverByName(const String  &name)
{
	DriverList::const_iterator it;
	for(it = mDriverList.begin(); it != mDriverList.end(); it++)
	{
		if ((*it)->getName() == name)
		{
			return *it;
		}
	}
	return 0;
}

/**
 * Hole die Soundkonfiguration
 * 
 * @author JoSch
 * @date 05-08-2006
 */
void MultimediaSubsystem::loadConf(const Ogre::String &filename)
{
	ConfigFile conf;
	conf.load(filename);
	String drivername = conf.getValue(String("Nulltreiber"), "ActiveDriver", "General");
	SoundDriver *driver = getDriverByName(drivername);
	RlAssert(driver != 0, "Beim Laden des Treibers ist ein Fehler aufgetreten");
	setActiveDriver(driver);
	getActiveDriver()->loadConf(conf);
}

/**
 * Schreibe die Soundkonfiguration
 * @author JoSch
 * @date 05-08-2006
 * @param filename Der Dateiname der Konfiguration
 */
void MultimediaSubsystem::saveConf(const Ogre::String &filename) const
{
	RlAssert(getActiveDriver() != 0, "Kein aktiver Soundtreiber");
	ConfigFile conf;
	conf.setValue(String(getActiveDriver()->getName().c_str()), "ActiveDriver", "General");
	getActiveDriver()->saveConf(conf);
	conf.save(filename);
}


}
