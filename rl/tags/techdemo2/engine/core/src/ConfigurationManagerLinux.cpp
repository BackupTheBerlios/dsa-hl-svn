/* This source file is part of Rastullahs Lockenpracht.
 * Copyright (C) 2003-2005 Team Pantheon. http://www.team-pantheon.de
 * 
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the Perl Artistic License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  Perl Artistic License for more details.
 *
 *  You should have received a copy of the Perl Artistic License
 *  along with this program; if not you can get it here
 *  http://www.perldoc.com/perl5.6/Artistic.html.
 */

#include "ConfigurationManagerLinux.h"
#include <iostream>
#include <fstream>

rl::ConfigurationManager* rl::ConfigurationManagerLinux::sInstance = NULL;

using namespace std;

namespace rl
{
	ConfigurationManager* ConfigurationManagerLinux::getSingletonPtr()
	{
		if(sInstance == 0)
		{
			sInstance = new ConfigurationManagerLinux;
		}
		return sInstance;
	}
	
	ConfigurationManager& ConfigurationManagerLinux::getSingleton()
	{
		if(sInstance == 0)
		{
			sInstance = new ConfigurationManagerLinux;
		}
		return *sInstance;
	}
	
	ConfigurationManagerLinux::ConfigurationManagerLinux()
	{
		mModulesRootDirectory = findModulesRootDir();
		mPluginCfgPath = mModulesRootDirectory + 
			"/modules/common/conf/plugins-linux.cfg";
		mSoundCfgPath = mModulesRootDirectory +
			"/modules/common/conf/sound.cfg";
		mRastullahCfgPath = findRastullahConf( "rastullah_ogre.cfg" );
		mRastullahSystemCfgPath = findRastullahConf( "rastullah_system.cfg" );
        if (!checkForFile(mRastullahSystemCfgPath))
        {
            string filename = mModulesRootDirectory + "/modules/common/conf/rastullah_system.cfg";
            if (checkForFile(filename))
            {
                mRastullahSystemCfgPath = filename;
            }
        }
		mOgreLogPath = mModulesRootDirectory+"/logs/ogre.log";
		mRastullahLogPath = mModulesRootDirectory+"/logs/rastullah.log";
		mModulesCfgPath = mModulesRootDirectory + "/modules/modules.cfg";
		mModuleconfigCfgPath = "/conf/moduleconfig.cfg";
	}
	
	ConfigurationManagerLinux::~ConfigurationManagerLinux()
	{
	}
	
	string ConfigurationManagerLinux::findModulesRootDir()
    {
    	// ModulesRootDir ist in rl.conf definiert, also suchen wir danach
    	
    	// erster Ansatz wie immer das HOME des users (getenv gefährlich)
    	string filename = string(::getenv("HOME")) + "/.rl.conf";
    	
    	if(checkForFile(filename))
    	{
    		// wir geben die erste nichtleere Zeile aus dem File zurück, 
    		// da diese den gesuchten Pfad repräsentiert
    		// später mit tags die einzelnen Variablen beschreiben, 
    		// die dann vernünftig geparst werden!
    		ifstream file(filename.c_str());
    		string line;
    		while(getline(file, line))
    		{
    			if(line.length() > 0)
    			{
    				file.close();
    				return line;
    			}
    		}
    		file.close();
    	}
    	
        // der nächste Versuch die Datei zu finden ist /etc/rl/rl.conf
        filename = "/etc/rl/rl.conf";
        if(checkForFile(filename))
    	{
    		// wir geben die erste nichtleere Zeile aus dem File zurück, 
    		// da diese den gesuchten Pfad repräsentiert
    		// später mit tags die einzelnen Variablen beschreiben, 
    		// die dann vernünftig geparst werden!
    		ifstream file(filename.c_str());
    		string line;
    		while(getline(file, line))
    		{
    			if(line.length() > 0)
    			{
    				file.close();
    				return line;
    			}
    		}
    		file.close();
    	}
        
        // alles war erfolglos, also gehen wir mal vom aktuellen Verzeichnis
        // aus
        return ".";
    }

	//Später die gesamten ausgeschriebenen Pfade durch das Install-
	//prefix welches vom configure script gesetzt wird ersetzen
    string ConfigurationManagerLinux::findRastullahConf( const string& cfg_name )
    {
        // Erstmal im Homeverzeichnis suchen.
        // getenv(home) nicht ungefährlich...
        string filename = string(::getenv("HOME")) + "/." + cfg_name;
        
        //wenn die Datei existiert ist das unsere favorisierte Wahl
        if(checkForFile(filename)) return filename;
       
        // Als nächstes schauen wir mal ob in /etc/rl eine standard
        // config vorhanden ist
        filename = "/etc/rl/" + cfg_name;
        if(checkForFile(filename)) return filename;
        
        // /usr/local/share/rl und /usr/share/rl wären noch Kandidaten
        filename = "/usr/local/share/rl/" + cfg_name;
        if(checkForFile(filename)) return filename;
        
        filename = "/usr/share/rl/" + cfg_name;
        if(checkForFile(filename)) return filename;
        
        //nix gefunden, also geben wir das home des Benutzers zurück, damit eine
        //neue Datei angelegt werden kann.
        //TODO: Es sollte IMMER wenn im home des current calling users keine 
        //.rastullah_ogre.cfg ist eine für diesen Benutzer mit seinen persönlichen
        //settings angelegt werden, oder aber aus den oben genannten Standard
        //pfade eine kopiert und angepasst!!!
        return string(::getenv("HOME")) + "/." + cfg_name;
    }
    
    bool ConfigurationManagerLinux::checkForFile(const std::string& filename)
    {
    	ifstream file(filename.c_str());
    	bool retval = file.is_open();
    	if(retval) file.close();
    	return retval;
    }
}
