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
 
#include "ConfigurationManager.h"
#include "ConfigurationManagerLinux.h"
#include "ConfigurationManagerWin32.h"
#include "ConfigurationManagerMac.h"

namespace rl
{
	ConfigurationManager* ConfigurationManager::getSingletonPtr()
	{
#       if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
		    return ConfigurationManagerLinux::getSingletonPtr();
#       elif OGRE_PLATFORM == OGRE_PLATFORM_WIN32
		    return ConfigurationManagerWin32::getSingletonPtr();
#       else
		    return ConfigurationManagerMac::getSingletonPtr();
#       endif
	}
	
	ConfigurationManager& ConfigurationManager::getSingleton()
	{
#       if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
		    return ConfigurationManagerLinux::getSingleton();
#       elif OGRE_PLATFORM == OGRE_PLATFORM_WIN32
		    return ConfigurationManagerWin32::getSingleton();
#       else
		    return ConfigurationManagerMac::getSingleton();
#       endif
	}
	
	ConfigurationManager::~ConfigurationManager()
	{
	}
	
	Ogre::String ConfigurationManager::getOgreLogPath()
	{
		return mOgreLogPath;
	}
	
	Ogre::String ConfigurationManager::getRastullahCfgPath()
	{
		return mRastullahCfgPath;
	}
	
	Ogre::String ConfigurationManager::getPluginCfgPath()
	{
		return mPluginCfgPath;
	}
	
	Ogre::String ConfigurationManager::getRastullahLogPath()
	{
		return mRastullahLogPath;
	}
	
	Ogre::String ConfigurationManager::getModulesCfgPath()
	{
		return mModulesCfgPath;
	}
	
	Ogre::String ConfigurationManager::getModuleconfigCfgPath(const Ogre::String& module)
	{
		return (mModulesRootDirectory + "/modules/" + module + mModuleconfigCfgPath);
	}
	
	Ogre::String ConfigurationManager::getModulesRootDirectory()
	{
		return mModulesRootDirectory;
	}

	Ogre::String ConfigurationManager::getEngineVersionString()
	{
		static Ogre::String version = "Internal Build";
		return version;
	}

	long parseDate(char* date)
	{ //TODO: __DATE__ in ein long verwandeln, damit man 
		return /* Jahr */			2005 * 100000+
			/* Monat */			10 * 1000 + 
			/* Tag */			18 * 10 + 
			/* Sub-Version */	0;	
	}

	long ConfigurationManager::getEngineBuildNumber()
	{
		static long buildNumber = parseDate(__DATE__);
		return buildNumber;
	}


	const CeGuiString& ConfigurationManager::getAboutText()
	{
		static CeGuiString aboutText = 
			"Rastullahs Lockenpracht\n\nCopyright 2003-2005 Team Pantheon\n\nBenutzte Bibliotheken: Ogre, fmod, Newton, boost, ...";
		return aboutText;
	}
}
