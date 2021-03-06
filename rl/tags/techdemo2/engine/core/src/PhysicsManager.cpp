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

#include "PhysicsManager.h"

#include "CoreSubsystem.h"
#include "World.h"
#include "PhysicalThing.h"
#include "Actor.h"
#include "ActorManager.h"
#include "Exception.h"
#include "PhysicalObject.h"
#include "PhysicsController.h"

using namespace std;
using namespace OgreNewt;

template<> rl::PhysicsManager* Singleton<rl::PhysicsManager>::ms_Singleton = 0;

namespace rl
{
    PhysicsManager& PhysicsManager::getSingleton(void)
    {
        return Singleton<PhysicsManager>::getSingleton();
    }

    PhysicsManager* PhysicsManager::getSingletonPtr(void)
    {
        return Singleton<PhysicsManager>::getSingletonPtr();
    }

    PhysicsManager::PhysicsManager( )
        : mEnabled(false),
        mNewtonDebugger(),
        mPhysicalThings(),
        mControlledThings(),
        mDebugMode(false),
        mGravity(0, -9.81, 0),
        mWorldAABB(Vector3(-100, -100, -100), Vector3(100, 100, 100)),
        mElapsed(0.0f),
        mUpdate(1.0f/60.0f),
        mLevelID(),
        mCharacterID(),
        mDefaultPair(),
        mCharLevelPair(),
        mCharCharPair(),
        mGenericCallback()
    {
		mWorld = new OgreNewt::World();
        mWorld->setFrictionModel(OgreNewt::World::FM_ADAPTIVE);
		mWorld->setSolverModel(OgreNewt::World::SM_ADAPTIVE);
		//mWorld->setSolverModel(4);

        // setup materials: default<->default
        const OgreNewt::MaterialID* defaultID = mWorld->getDefaultMaterialID();
        mDefaultPair = new OgreNewt::MaterialPair(
            mWorld, defaultID, defaultID);
        mGenericCallback = new PhysicsGenericContactCallback();
        mDefaultPair->setContactCallback(mGenericCallback);
        mDefaultPair->setDefaultFriction(0.85f, 0.80f);

        // setup materials: character/cam<->level
        mLevelID = new OgreNewt::MaterialID(mWorld);
        mCharacterID = new OgreNewt::MaterialID(mWorld);
        mCharLevelPair = new OgreNewt::MaterialPair(mWorld, mCharacterID, mLevelID);
        mCharLevelPair->setDefaultFriction(0, 0);

        // setup material: char<->cam. there should be no collision
        mCharCharPair = new OgreNewt::MaterialPair(mWorld, mCharacterID, mCharacterID);
        mCharCharPair->setDefaultCollidable(0);
    }

    PhysicsManager::~PhysicsManager()
    {
        delete mGenericCallback;
        delete mCharCharPair;
        delete mCharLevelPair;
        delete mCharacterID;
        delete mLevelID;
        delete mDefaultPair;
        delete mWorld;
    }

    void PhysicsManager::run(Real elapsedTime)
    {
        // do nothing, if not enabled
        if (!mEnabled) 
			return;

        mElapsed += elapsedTime;
        while (mElapsed >= mUpdate)
        {
	 		mWorld->update(mUpdate);
            mElapsed -= mUpdate;
        }
    }

    void PhysicsManager::setGravity( Real x, Real y, Real z )
    {
        mGravity = Vector3(x, y, z);
    }

    Vector3 PhysicsManager::getGravity() const
    {
        return mGravity;
    }

    PhysicalThing* PhysicsManager::createPhysicalThing(GeometryTypes geomType,
        PhysicalObject* po, Real mass, bool hullModifier)
    {
		PhysicalThing* rval = NULL;
		if (geomType != GT_NONE)
		{
			rval = new PhysicalThing(geomType, po, mass, hullModifier);
		}

		return rval;
    }

    void PhysicsManager::removeAndDestroyPhysicalThing(PhysicalThing* thing)
    {
        vector<PhysicalThing*>::iterator it = find(mPhysicalThings.begin(),
            mPhysicalThings.end(), thing);

        if( it != mPhysicalThings.end() )
        {
            PhysicalThing* phys = *it;
            mPhysicalThings.erase(it);
            delete phys;
        }
    }

    bool PhysicsManager::isEnabled() const
    {
        return mEnabled;
    }

    void PhysicsManager::setEnabled(bool enabled)
    {
        mEnabled = enabled;
    }

    bool PhysicsManager::isDebugMode() const
    {
        return mDebugMode;
    }

    void PhysicsManager::toggleDebugMode()
    {
        if (mDebugMode)
		{
            mNewtonDebugger->hideLines();
		}
        else
        {
            mNewtonDebugger = &OgreNewt::Debugger::getSingleton();

			try
			{
				mNewtonDebugger->init(
					CoreSubsystem::getSingleton().getWorld()->getSceneManager());
			}
			catch(Ogre::Exception) {}

            mNewtonDebugger->showLines(mWorld);
        }
        mDebugMode = !mDebugMode;
    }

    void PhysicsManager::addLevelGeometry( Ogre::Entity* levelEntity )
    {
        RlAssert1(levelEntity);
        RlAssert1(levelEntity->getParentSceneNode());

        SceneNode* node = levelEntity->getParentSceneNode();
        //Level entity has to be attached to a scene node.

        OgreNewt::Collision* collision =
            new OgreNewt::CollisionPrimitives::TreeCollision(mWorld, levelEntity, false);
        OgreNewt::Body* body = new OgreNewt::Body(mWorld, CollisionPtr(collision));

        body->attachToNode(node);
        body->setPositionOrientation(node->getWorldPosition(),
            node->getWorldOrientation());
        body->setMaterialGroupID(mLevelID);

        mLevelBodies.push_back(body);

        // adjust worldAABB
        Vector3 minV(mWorldAABB.getMinimum());
        Vector3 maxV(mWorldAABB.getMaximum());

        AxisAlignedBox entityAABB = levelEntity->getWorldBoundingBox(true);
        minV.makeFloor(entityAABB.getMinimum());
        maxV.makeCeil(entityAABB.getMaximum());
        mWorldAABB.setMinimum(minV - Vector3(50, 50, 50));
        mWorldAABB.setMaximum(maxV + Vector3(50, 50, 50));

        mWorld->setWorldSize(mWorldAABB);
    }

	void PhysicsManager::createPhysicsProxy(PhysicalThing* pt, SceneNode* node)
	{
		pt->createPhysicsProxy(node);
		mPhysicalThings.push_back(pt);
	}

    void PhysicsManager::clearLevelGeometry(  )
    {
        for (size_t i = 0; i < mLevelBodies.size(); i++ )
        {
            delete mLevelBodies[i];
        }
        mLevelBodies.clear();
    }

    // adopted from the chararcter demo in the newton sdk
    // copyright 2000-2004
    // By Julio Jerez
    void PhysicsManager::genericForceCallback(OgreNewt::Body* body)
    {
        // apply saved forces in the PhysicalThing
        PhysicalThing* thing =
            static_cast<Actor*>(body->getUserData())->getPhysicalThing();

        thing->onApplyForceAndTorque();
    }

    void PhysicsManager::controlledForceCallback(OgreNewt::Body* body)
    {
        PhysicalThing* thing =
            static_cast<Actor*>(body->getUserData())->getPhysicalThing();

        ControllerMap::const_iterator it = getSingleton().mControlledThings.find(thing);
        if (it != getSingleton().mControlledThings.end())
        {
            (*it).second->OnApplyForceAndTorque(thing);
        }
        else
        {
            Throw(IllegalStateException,
                "controlledForceCallback called for uncontrolled PhysicalThing.");
        }
    }


    PhysicsController* PhysicsManager::getPhysicsController(PhysicalThing* thing) const
    {
        PhysicsController* rval = 0;

        ControllerMap::const_iterator it = mControlledThings.find(thing);
        if (it != mControlledThings.end())
        {
            rval = (*it).second;
        }

        return rval;
    }

    void PhysicsManager::setPhysicsController(PhysicalThing* thing,
        PhysicsController* controller)
    {
        RlAssert1(thing);

        // first see, if the thing has another controller already
        PhysicsController* oldController = getPhysicsController(thing);
        if (oldController)
        {
            // if so, remove it
            thing->unprepareUserControl();
            mControlledThings.erase(thing);
        }

        if(controller)
        {
            // add it to the map and prepare it for control
            mControlledThings[thing] = controller;
			thing->prepareUserControl(mCharacterID); /** @TODO: Nicht immer nur Char-Material nehmen */
        }
    }

    void PhysicsManager::setCharLevelContactCallback(PhysicsGenericContactCallback* callback)
    {
        if (callback != NULL)
        {
            mCharLevelPair->setContactCallback(callback);
        }
        else
        {
            mCharLevelPair->setContactCallback(PhysicsManager::mGenericCallback);
        }
    }

    OgreNewt::World* PhysicsManager::_getNewtonWorld() const
    {
        return mWorld;
    }

    OgreNewt::MaterialID* PhysicsManager::_getLevelMaterialID() const
    {
        return mLevelID;
    }

	OgreNewt::MaterialID* PhysicsManager::_getCharMaterialID() const
    {
        return mCharacterID;
    }

	const OgreNewt::MaterialID* PhysicsManager::_getDefaultMaterialID() const
    {
        return 	mWorld->getDefaultMaterialID();
    }
}
