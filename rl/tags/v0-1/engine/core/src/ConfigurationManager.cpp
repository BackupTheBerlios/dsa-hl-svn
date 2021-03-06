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

#include <OgreRenderSystem.h>
#include <OgreResourceGroupManager.h>

#include "CoreSubsystem.h"


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
	
    ConfigurationManager::ConfigurationManager() :
        mSystemConfig(NULL)
    {
    }

	ConfigurationManager::~ConfigurationManager()
	{
        delete mSystemConfig;
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

    Ogre::String ConfigurationManager::getRastullahSystemCfgPath()
    {
        return mRastullahSystemCfgPath;
    }

    Ogre::ConfigFile* ConfigurationManager::getSystemConfig()
    {
        if( mSystemConfig == NULL )
        {
            mSystemConfig = new Ogre::ConfigFile();

            try
            {            
                mSystemConfig->loadFromResourceSystem( mRastullahSystemCfgPath, 
                    Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, "=" );
            }           
            catch ( Ogre::Exception  ) 
            {
                Logger::getSingleton().log(Logger::CORE, Ogre::LML_CRITICAL, 
                    "Konnte Rastullah-System-Konfiguration aus '" + mRastullahSystemCfgPath + 
                    "' nicht laden! Defaulteinstellungen werden benutzt.","Configuration");
            }
        }
        
        return mSystemConfig;
    }

    bool ConfigurationManager::shouldUseStaticGeometry()
    {
        Ogre::String mode = getSystemConfig()->getSetting( "use_static_geometry" );
        
        if( mode == Ogre::StringUtil::BLANK )
            mode = "auto";
        
        if( mode.compare("yes") == 0 )
        {
            return true;
        }
        else if( mode.compare("no") == 0 )
        {
            return false;
        }        
        else
        {
            // �berpr�fen ob der Renderer VertexBuffer unterst�tzt
            return Ogre::Root::getSingleton().getRenderSystem()
                ->getCapabilities()->hasCapability( Ogre::RSC_VBO );
        }
        
    }

	Ogre::String ConfigurationManager::getEngineVersionString() const
	{
		static Ogre::String version = "0.0.9";
		return version;
	}

	Ogre::String ConfigurationManager::getEngineVersionName() const
	{
		static Ogre::String version = "Internal Build";
		return version;
	}

	static const Ogre::String sMonths[] = 
			{"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug",
             "Sep", "Oct", "Nov", "Dec"};

	/// __DATE__ sieht ca. so aus : Nov 08 2005
	long parseDate(char* date)
	{   
		Ogre::String dateStr = Ogre::String(date);
		Ogre::String monthStr = dateStr.substr(0,3);
		int day = Ogre::StringConverter::parseInt( dateStr.substr(4,2) );
		int year = Ogre::StringConverter::parseInt( dateStr.substr(7,4) );
		int month = 0;

		while( month < 12 && monthStr.compare(sMonths[month]) != 0 )
			month++;

		return /* Jahr */		  year * 100000 +
			   /* Monat */	 (month+1) * 1000 + 
			   /* Tag */		   day * 10 + 
			   /* Sub-Version */	 0;	
	}

	long ConfigurationManager::getEngineBuildNumber() const
	{
		static long buildNumber = parseDate(__DATE__);
		return buildNumber;
	}


	const CeGuiString& ConfigurationManager::getAboutText() const
	{
		static CeGuiString aboutText = 
			"Rastullahs Lockenpracht\n\nCopyright 2003-2005 Team Pantheon\n\nBenutzte Bibliotheken: Ogre, fmod, Newton, boost, ...";
		return aboutText;
	}
}
