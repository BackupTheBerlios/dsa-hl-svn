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
#include "Agent.h"
#include "Exception.h"
#include "Creature.h"
#include "DialogCharacter.h"
#include "SteeringVehicle.h"
#include "SteeringMachine.h"
//#include "PerceptionPool.h"

using namespace rl;

Agent::Agent(Creature* character)
	: mType(AgentManager::AGENT_NONE), mBehaviour(NULL), mVehicle(NULL), mDialogBot(NULL), 
	  mCreature(character)
//	  mPerceptionPool(new PerceptionPool())
{
	initialize();
}

Agent::Agent(Creature* character, SteeringVehicle* vehicle)
	: mType(AgentManager::AGENT_NONE), mBehaviour(NULL), mVehicle(vehicle), 
	  mDialogBot(NULL), mCreature(character)
//	  mPerceptionPool(new PerceptionPool())
{
	initialize();
}

Agent::Agent(DialogCharacter* character)
	: mType(AgentManager::AGENT_STD_NPC), mBehaviour(NULL), mVehicle(NULL),
	  mDialogBot(character),  mCreature(NULL)
//	  mPerceptionPool(new PerceptionPool())
{
    mCreature = character->getNonPlayerCharacter();
	initialize();
}

Agent::~Agent(void)
{
    if(mType == AgentManager::AGENT_STD_NPC)
        PhysicsManager::getSingleton().
			setPhysicsController( mCreature->getActor()->getPhysicalThing(), NULL );

    delete mVehicle;
    delete mBehaviour;
//	delete mPerceptionPool;
}

void Agent::initialize()
{
//  an agent needs a creature it refers to
	if(mCreature == NULL)
	{
		Throw(NullPointerException, "Agent has no creature");
	}
//  if there is no vehicle, create a standard vehicle
	if(mVehicle == NULL)
	{
		mType = AgentManager::AGENT_STD_NPC;
		mVehicle = new SteeringVehicle(this, mCreature->getActor());
	}
    Logger::getSingleton().log(
        Logger::AI, 
        Logger::LL_NORMAL, 
        "created SteeringVehicle for Agent");
    
	mBehaviour = new SteeringMachine(NULL, mVehicle);
    Logger::getSingleton().log(
        Logger::AI, 
        Logger::LL_NORMAL, 
        "created SteeringMachine for Agent");
//  a perceptron should be the controller, and the perceptron calculates
//  the steering force with the help of different steering behaviours
	if(mType == AgentManager::AGENT_STD_NPC)
	{
		PhysicsManager::getSingleton().
			setPhysicsController(mCreature->getActor()->getPhysicalThing(), this);
		Logger::getSingleton().log(
			Logger::AI, 
			Logger::LL_NORMAL, 
			"added Agent to PhysicsManager as PhysicsController");
	}
}

void Agent::addSteeringBehaviour(SteeringBehaviour* behaviour)
{
    behaviour->setParent(mBehaviour);
    behaviour->setController(mVehicle);
    mBehaviour->addState(behaviour);
    Logger::getSingleton().log(
        Logger::AI, 
        Logger::LL_MESSAGE, 
        "added SteeringBehaviour for Agent");
}

void Agent::clearSteeringBehaviours()
{
    mBehaviour->clearStates();
    Logger::getSingleton().log(
        Logger::AI, 
        Logger::LL_MESSAGE, 
        "Cleared all SteeringBehaviours for Agent");
}


void Agent::update(const float elapsedTime)
{
	mBehaviour->update(elapsedTime);
//  currentTime not needed yet, only elapsedTime
    mVehicle->update(0.0f, elapsedTime);
}

void Agent::OnApplyForceAndTorque(PhysicalThing* thing)
{
	OgreNewt::World* world = PhysicsManager::getSingleton()._getNewtonWorld();
	Real elapsedTime = world->getTimeStep();
	update(elapsedTime);
}

bool Agent::isDialogActive()
{
	if(mDialogBot == NULL)
	{
		// Throw(NullPointerException, "No Dialogbot found");
        return false;
	}
	return mDialogBot->isActive();
}
