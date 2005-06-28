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

#ifndef __GameLoop_H__
#define __GameLoop_H__

#include <list>
#include <OgreSingleton.h>
//#include <OpenThreads/Thread>
#include <boost/thread.hpp>

#include "CorePrerequisites.h"

namespace rl {

class GameTask;
class GameLoop;
class SynchronizedGameLoop;
class AsynchronousGameLoop;

class _RlCoreExport GameLoopManager : protected Ogre::Singleton<GameLoopManager>
{
public:
	GameLoopManager(unsigned long millisPerAsyncTick);
	virtual ~GameLoopManager();

    void addSynchronizedTask(GameTask* newTask);
	void addAsynchronousTask(GameTask* newTask);
	void removeSynchronizedTask(GameTask* oldTask);
	void removeAsynchronousTask(GameTask* oldTask);	
      
    bool isPaused();
    void setPaused(bool pause);
	void quitGame();

    static GameLoopManager & getSingleton(void);
	static GameLoopManager * getSingletonPtr(void);

private:
    AsynchronousGameLoop* mAsynchronousGameLoop;    
	SynchronizedGameLoop* mSynchronizedGameLoop;
};

class GameLoop
{
public:
	GameLoop();
	virtual ~GameLoop();

	void add(GameTask* task);
	void remove(GameTask* task);
	bool isPaused();
	void setPaused(bool pause);

protected:
	void loop( Ogre::Real timeSinceLastCall );
	
private:	
	std::list<GameTask*> mTaskList;
	bool mPaused;
};

class SynchronizedGameLoop : public GameLoop, public Ogre::FrameListener
{
private:
	bool mRunning;

public:
	SynchronizedGameLoop();
    
	void quitGame();

	bool frameStarted(const Ogre::FrameEvent & evt);
	bool frameEnded(const Ogre::FrameEvent & evt);
};

class AsynchronousGameLoop
	: public GameLoop, protected Ogre::Singleton<AsynchronousGameLoop>
{
public:
	AsynchronousGameLoop(unsigned long timeTickInMillis);
	void run();
	static void runStatic();

	static AsynchronousGameLoop & getSingleton(void);
	static AsynchronousGameLoop * getSingletonPtr(void);

	static unsigned long sTimeTickInMillis;

private: 
	Ogre::Timer* mTimer;
	
	boost::thread* mThread;
};

}
#endif

