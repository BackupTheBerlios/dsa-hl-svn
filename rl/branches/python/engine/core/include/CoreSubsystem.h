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

#ifndef __CoreSubsystem_H__
#define __CoreSubsystem_H__


#include <OgreSingleton.h>
#include <OgreString.h>

#include "CorePrerequisites.h"

namespace rl {

class Interpreter;
class World;

/** CoreSubsystem. 
	@remarks		
        It follows the Singleton Pattern,
		and initialises the game context.
*/
class _RlCoreExport CoreSubsystem : protected Ogre::Singleton<CoreSubsystem>
{
public:
	/** Default Constructor */
	CoreSubsystem();
    /** Default Deconstructor */
	virtual ~CoreSubsystem();

	/** Starts the Game */
	void startCore();

	/** Returns the Singleton */
	static CoreSubsystem & getSingleton();
	static CoreSubsystem * getSingletonPtr();

	World* getWorld();
	void loadMap(const Ogre::String type, const Ogre::String filename,
	    const Ogre::String startupScript = "");

	void setInterpreter(Interpreter* interpreter);
	Interpreter* getInterpreter();

	Ogre::StringVector getActiveModules() const;
	const Ogre::StringVector& getCommonModules() const;
	const Ogre::StringVector& getActivatableModules() const;
	void setActiveModule(const Ogre::String& module);
	void initializeModule(const std::string& module);
	
	/** Saves a timestamped jpg Screenshot
		@param sName The filename (extended with the timestamp)
	*/
	void  makeScreenshot(const Ogre::String& sName);

    static void log ( const Ogre::String& msg );

	/** Gibt die abgelaufene Spielzeit zur�ck*/
	RL_LONGLONG getClock();

	/** Setzt die abgelaufene Spielzeit wieder auf Null, die DSA-Zeit wird aus der Basiszeit + abgelaufener Spielzeit berechnet
	*
	* @see DsaManager
	*/
	void resetClock();
	
private:  
    /** Runs the setup methods  */
	bool initializeCoreSubsystem();

	/** Loads all needed ressources */
	void initializeResources();
	void initializeModuleTextures(const std::string& module);
	void unloadModule(const std::string& module);
	void addCommonSearchPath(const std::string& path);	

	/** Opens a configuration dialog */
	bool setupConfiguration();

	RL_LONGLONG getCurrentTime();

	World* mWorld;
	Interpreter* mInterpreter;
	Ogre::StringVector mCommonModules;
	Ogre::StringVector mActivatableModules;
	Ogre::String mActiveModule;
    Ogre::String mRootDir;
    Ogre::Timer* mTimer;

	RL_LONGLONG mClockStartTime;
};

}
#endif

