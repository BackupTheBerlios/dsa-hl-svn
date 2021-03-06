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

#include "Logger.h"

#include <OgreLogManager.h>


using Ogre::LogManager;
using Ogre::Singleton;

template<> rl::Logger* Singleton<rl::Logger>::ms_Singleton = 0;
const char* rl::Logger::RULES = "Rules";
const char* rl::Logger::CORE = "Core";
const char* rl::Logger::DIALOG = "Dialog";
const char* rl::Logger::AI = "Ai";
const char* rl::Logger::UI = "Ui";
const char* rl::Logger::MULTIMEDIA = "Multimedia";
const char* rl::Logger::MAIN = "Main";
const char* rl::Logger::SCRIPT = "Script";

namespace rl
{
	
Logger& Logger::getSingleton(void)
{
    return Singleton<Logger>::getSingleton();
}

Logger* Logger::getSingletonPtr(void)
{
    return Singleton<Logger>::getSingletonPtr();
}

Logger::Logger(const Ogre::String& logPath, const Ogre::String& ogreLogPath)
    : mLog(0),
      mLogLevel(LL_ERROR),
      mErrorBuffer(""),
	  mErrorPresent(false)
{
    if (LogManager::getSingletonPtr() == 0)
    {
	    new LogManager();
    }

	//Log f�r Ogre
	LogManager::getSingleton().setDefaultLog(LogManager::getSingleton().createLog(ogreLogPath));

	//Log f�r RL
	mLog = LogManager::getSingleton().createLog(logPath);
}

Logger::~Logger()
{
	delete LogManager::getSingletonPtr();
}

void Logger::log(const Ogre::String& component, const Logger::LogLevel level, 
			const Ogre::String& message, const Ogre::String& ident)
{
	if (ident.length() == 0)
		log(level, "[" + component + "] " + message);
	else
		log(level, "[" + component + "] (" + ident + ") " + message);
}

void Logger::log(const Ogre::String& component, const Logger::LogLevel level, 
			const CeGuiString& message, const Ogre::String& ident)
{
	if (ident.length() == 0)
		log(level, "[" + component + "] " + message.c_str());
	else
		log(level, "[" + component + "] (" + ident + ") " + message.c_str());
}

void Logger::log(const Ogre::String& component, const Logger::LogLevel level, 
			const char* message, const Ogre::String& ident)
{
	if (ident.length() == 0)
		log(level, "[" + component + "] " + message);
	else
		log(level, "[" + component + "] (" + ident + ") " + message);
}

void Logger::log(const Logger::LogLevel level, const Ogre::String& msg )
{
	if (level >= mLogLevel)
	{
		mLog->logMessage(msg, Ogre::LML_TRIVIAL);
	
		if (level >= Logger::LL_ERROR) // Fehler
		{
			mErrorBuffer.append("\n");
			mErrorBuffer.append(msg);
			mErrorPresent = true;
		}
	}
}

void Logger::setLogDetail(const Logger::LogLevel level)
{
	mLogLevel = level;	
	mLog->setLogDetail(Ogre::LL_BOREME);
}

const CEGUI::LoggingLevel Logger::getCeGuiLogDetail() const
{
	if (mLogLevel > Logger::LL_ERROR)
		return CEGUI::Errors;
	else if (mLogLevel > Logger::LL_NORMAL)
		return CEGUI::Standard;
	else
		return CEGUI::Insane;

	return CEGUI::Errors;
}

bool Logger::isErrorPresent() const
{
	return mErrorPresent;
}

void Logger::resetErrorState()
{
	mErrorPresent = false;
}

const Ogre::String& Logger::getErrorLog() const
{
	return mErrorBuffer;
}

}
