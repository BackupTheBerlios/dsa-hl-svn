/* This source file is part of Rastullahs Lockenpracht.
 * Copyright (C) 2003-2006 Team Pantheon. http://www.team-pantheon.de
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
#ifndef __AiSubsystem_H__
#define __AiSubsystem_H__

#include <OgreSingleton.h>
#include "AiPrerequisites.h"
#include "World.h"

namespace rl
{
	class Agent;
	class AgentManager;
	class WayPointGraphManager;
	class AiWorld;

	/** Central core of AI
	 * Handles creation of all AI related object Managers.
	 */
	class _RlAiExport AiSubsystem 
		: public Ogre::Singleton<AiSubsystem>,
		  public SceneChangeListener
	{
	public:
		//! returns the aisubsystem object.
		static AiSubsystem & getSingleton(void);
		//! returns a pointer to the aisubsystem object.
        static AiSubsystem * getSingletonPtr(void);

		//! default constructor
		AiSubsystem(void);
		/** explicit virtual destructor.
		 * Unregisters Scene listener, makes AgentManager drop all Agents,
		 * unregisters AgentManager from GameLoop,
		 * deletes AgentManager, WayPointGraphManager and AiWorld
		 */
		virtual ~AiSubsystem(void);
		
		/** Returns the stored world object representing storing obstacles.
		 */
		AiWorld* getWorld();

		/** Trigger function triggered after scene load.
		 */
		void onAfterSceneLoaded();
		/** Trigger function triggered before scene load.
		 */
        void onBeforeClearScene();
	private:
		/** Initializes the AI subsystem.
		 * Creates AiWorld and AgentManager, registers a scene listener,
		 * adds the AgentManager to the GameLoop as a task.
		 */
		void initialize();

		//! single AgentManager object
		AgentManager* mAgentManager;
		//! single WayPointGraphManager object
		WayPointGraphManager *mWayPointGraphManager;
		//! AiWorld representation - WIP!!! (object to changes)
		AiWorld* mWorld;
	};

	inline AiWorld* AiSubsystem::getWorld()
	{
		return mWorld;
	}
}
#endif
