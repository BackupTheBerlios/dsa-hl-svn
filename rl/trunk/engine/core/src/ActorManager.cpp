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

#include "ActorManager.h"

#include "CoreSubsystem.h"

#include "Actor.h"
#include "World.h"
#include "MeshObject.h"
#include "CameraObject.h"
#include "LightObject.h"
#include "SoundObject.h"
#include "SoundMovable.h"
#include "ListenerMovable.h"
#include "ListenerObject.h"

template<> rl::ActorManager* Singleton<rl::ActorManager>::ms_Singleton = 0;

namespace rl {

    ActorManager& ActorManager::getSingleton(void)
	{
		return Singleton<ActorManager>::getSingleton();
	}

	ActorManager* ActorManager::getSingletonPtr(void)
	{
		return Singleton<ActorManager>::getSingletonPtr();
	}

    ActorManager::ActorManager() : mActors()
    {
        mWorld = CoreSubsystem::getSingleton().getWorld();
    }

    ActorManager::~ActorManager()
    {
    }

    void ActorManager::setWorld( World* world )
    {
        mWorld = world;
    }

	const World* const ActorManager::getWorld() const
	{
		return mWorld;
	}

    Actor* ActorManager::getActor(const String& name)
	{
		ActorPtrMap::const_iterator pActorIter = mActors.find(name);

		if( pActorIter != mActors.end() )
			return pActorIter->second;

		return 0;
	}

	void ActorManager::destroyActor(Actor* actor)
	{
        ///@todo Loesch ihn!
	}

    void ActorManager::destroyAllActors()
	{
        for (ActorPtrMap::iterator it = mActors.begin();
            it != mActors.end();) 
        {
            Actor* actor = it->second;
            if (actor->getControlledObject()->getObjectType() != "CameraObject")
            {
                mActors.erase(it++);
                destroyActor(actor);
            } else {
                ++it;
            }
        }
	}

    Actor* ActorManager::createLightActor(const String& name, rl::LightObject::LightTypes type )
	{
        const String&  uniquename = nextUniqueName(name);

        Actor* actor = 0;
        try
        {
            ///@todo Typen in Einklang bringen
            LightObject* lo = new LightObject(uniquename, type);

            actor = new Actor(uniquename, lo);
            mActors.insert(ActorPtrPair(uniquename,actor)); 
        }
        catch( Ogre::Exception& e)
        {
            CoreSubsystem::log("ActorManager - Das Light '"
                + uniquename + "' konnte nicht erstellt werden. Grund: "
                + e.getFullDescription());
        }

        return actor;
	}

    Actor* ActorManager::createSoundActor(const String& name, const String& soundfile)
    {
        const String&  uniquename = nextUniqueName(name);

        Actor* actor = 0;
        try
        {
            SoundMovable* sm = new SoundMovable(soundfile);
            SoundObject* so = new SoundObject(sm);

            actor = new Actor(uniquename, so);
            mActors.insert(ActorPtrPair(uniquename,actor)); 
        }
        catch( Ogre::Exception& e)
        {
            CoreSubsystem::log("ActorManager - Der Sound '"
                + uniquename + "' konnte nicht erstellt werden. Grund: "
                + e.getFullDescription());
        }

        return actor;
    }

    Actor* ActorManager::createListenerActor(const String& name)
    {
        const String&  uniquename = nextUniqueName(name);

        Actor* actor = 0;
        try
        {
            ListenerMovable* lm = new ListenerMovable(name);
            ListenerObject* lo = new ListenerObject(lm);

            actor = new Actor(uniquename, lo);
            mActors.insert(ActorPtrPair(uniquename,actor)); 
        }
        catch( Ogre::Exception& e)
        {
            CoreSubsystem::log("ActorManager - Der Sound '"
                + uniquename + "' konnte nicht erstellt werden. Grund: "
                + e.getFullDescription());
        }

        return actor;
    }

    Actor* ActorManager::createCameraActor(const String& name)
    {
        const String&  uniquename = nextUniqueName(name);
        Actor* actor = 0;
        try
        {
            CameraObject* co = new CameraObject(uniquename);
            co->getCamera()->setNearClipDistance(10);
            co->getCamera()->setFarClipDistance(10000);
            PhysicalThing* pt = PhysicsManager::getSingleton()
                .createPhysicalThing(PhysicsManager::GT_SPHERE,
                    Vector3(co->getCamera()->getNearClipDistance() * 1.5, 0, 0),
                    0.0f, PhysicsManager::OM_CENTERED);
            actor = new Actor(uniquename, co, pt);

            mActors.insert(ActorPtrPair(uniquename,actor)); 
        }
        catch( Ogre::Exception& e)
        {
            CoreSubsystem::log("ActorManager - Die Camera '"
                + name + "' f�r den Aktor '"
                + uniquename + "' konnte nicht erstellt werden. Grund: "
                + e.getFullDescription());
        }

        return actor;
    }

	Actor* ActorManager::createMeshActor(const String& name,const String& meshname,
	    PhysicsManager::GeometryTypes geomType, Ogre::Real density)
	{
		const String&  uniquename = nextUniqueName(name);
		
		Actor* actor = 0;
        try
        {
		    MeshObject* mo = new MeshObject(uniquename, meshname);
		    PhysicalThing* pt = PhysicsManager::getSingleton()
		        .createPhysicalThing(geomType, mo->getSize(), density);

		    actor = new Actor(uniquename, mo, pt);
		    mActors.insert(ActorPtrPair(uniquename,actor)); 
        }
        catch( Ogre::Exception& e)
        {
            CoreSubsystem::log("ActorManager - Das Mesh '"
                + meshname + "' f�r den Aktor '"
                + uniquename + "' konnte nicht erstellt werden. Grund: "
                + e.getFullDescription());
        }
    
        return actor;
	}

    Actor* ActorManager::createParticleSystemActor(const String& name,const String& partname)
    {
        const String&  uniquename = nextUniqueName(name);

		Actor* actor = 0;
        try
        {
			ParticleSystemObject* po = new ParticleSystemObject(uniquename, partname);

			actor = new Actor(uniquename, po, 0);
			mActors.insert(ActorPtrPair(uniquename,actor)); 
        }
        catch( Ogre::Exception& e )
        {
            CoreSubsystem::log("ActorManager - Das Partikelsystem '"
                + partname + "' f�r den Aktor '"
				+ uniquename + "' konnte nicht erstellt werden. Grund: "
				+ e.getFullDescription());
        }

        return actor;
    }

    String ActorManager::nextUniqueName(const String& basename)
	{
		String newname = basename;
	
		int iID = 0;

		while( this->getActor(newname) != 0 )
		{
			newname = basename + "_" + StringConverter::toString(++iID);
		}
		
		if( basename != newname )
            CoreSubsystem::log("ActorManager - Warnung! '" + basename
                + "' war schon vergeben! Neuer Name ist '" + newname + "'.");
        
		return newname;
	}

	Actor* ActorManager::getActorAt(Real x, Real y, Real width, Real length, bool infinite) const
	{      
        if (getWorld()->getActiveCamera() == NULL ||
			getWorld()->getActiveActor() == NULL)
			return NULL;

		 // Start a new ray query
		Ogre::Ray cameraRay = getWorld()->getActiveCamera()->
			getCameraToViewportRay( x, y );
		//Ogre::Ray selectRay(
		//	cameraRay.getOrigin()/*getWorld()->getActiveActor()->getPosition()*/, 
		//	cameraRay.getDirection());
		//Ogre::RaySceneQuery *raySceneQuery = getWorld()->
		//	getSceneManager()->createRayQuery(selectRay);
		//raySceneQuery->execute();
		//Ogre::RaySceneQueryResult result = raySceneQuery->getLastResults();
		
		Vector3 rayTip = cameraRay.getOrigin();
		Vector3 camUp = getWorld()->getActiveCamera()->getUp();
		Vector3 camRight = getWorld()->getActiveCamera()->getRight();
		if (camUp == Vector3::ZERO || camRight == Vector3::ZERO)
			return NULL;
		Vector3 rayEndMiddle = cameraRay.getOrigin() + length*cameraRay.getDirection();
		Vector3 rayEndTopLeft = rayEndMiddle + width*camUp - width*camRight;
		Vector3 rayEndTopRight = rayEndMiddle + width*camUp + width*camRight;
		Vector3 rayEndBottomLeft = rayEndMiddle - width*camUp - width*camRight;
		Vector3 rayEndBottomRight = rayEndMiddle - width*camUp + width*camRight;

		PlaneBoundedVolume selectionPyramid;
		// Left side
		selectionPyramid.planes.push_back(Plane(rayTip, rayEndBottomLeft, rayEndTopLeft)); 
		// Bottom side
		selectionPyramid.planes.push_back(Plane(rayTip, rayEndBottomRight, rayEndBottomLeft));
		// Right side
		selectionPyramid.planes.push_back(Plane(rayTip, rayEndTopRight, rayEndBottomRight));
		// Top side
		selectionPyramid.planes.push_back(Plane(rayTip, rayEndTopLeft, rayEndTopRight));
		// Far limiting side
		if (!infinite)
			selectionPyramid.planes.push_back(Plane(rayEndTopLeft, rayEndBottomRight, rayEndTopRight));
		PlaneBoundedVolumeList list;
		list.push_back(selectionPyramid);
		PlaneBoundedVolumeListSceneQuery* query = 
			getWorld()->getSceneManager()->createPlaneBoundedVolumeQuery(list);
        SceneQueryResultMovableList result = query->execute().movables;
		
		Ogre::MovableObject *closestObject = NULL;
		Real closestDistance = LONG_MAX;

		
		SceneQueryResultMovableList::iterator resultIterator;
		 
		for ( resultIterator = result.begin(); resultIterator != result.end(); resultIterator++ ) 
		{
			Ogre::MovableObject* movable = *resultIterator;
			if (movable != NULL && 
				movable->getUserObject() != NULL && 
				movable != getWorld()->getActiveCamera() &&
				movable != getWorld()->getActiveActor()->_getMovableObject()) 
			{
				//if (movable->getUserObject()
				//if ((*resultIterator).distance < closestDistance) 
				//{
					closestObject = movable;
				//	closestDistance = (*resultIterator).distance;
				//}
			}
		}

		Actor* rval;
		// No object clicked
		if ( closestObject == NULL) {   
			rval = NULL;
		} else {
			rval = static_cast<Actor*>(closestObject->getUserObject());
		}

		query->clearResults();
		getWorld()->getSceneManager()->destroyQuery(query);

		return rval;
	}
}
