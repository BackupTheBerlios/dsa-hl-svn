/* This source file is part of Rastullahs Lockenpracht.
 * Copyright (C) 2003-2004 Team Pantheon. http://www.team-pantheon.de
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

#include "DotSceneOctreeWorld.h"

#include <OgreTextureManager.h>
#include <OgreRoot.h>
#include <OgreException.h>
#include "GameLoop.h"

#include "CameraActor.h"
#include "ActorManager.h"
#include "PhysicsManager.h"

namespace rl {

    DotSceneOctreeWorld::DotSceneOctreeWorld( ) : World(ST_GENERIC)
    {
        // Set up shadowing
        // JoSch hat unter Windows Probleme auf seiner TNT.
        // Eventuell wg. Schattensetup, deshalb testweise aus.
        mSceneMgr->setShadowTechnique(SHADOWTYPE_STENCIL_MODULATIVE);
        mSceneMgr->setShadowColour(ColourValue(0.5, 0.5, 0.5));
        mSceneMgr->setShadowFarDistance(500);
        mSceneMgr->setShadowDirLightTextureOffset(0.9);
		if (StringUtil::startsWith(Root::getSingletonPtr()->
		    getRenderSystem()->getName(), "direct"))
        {
            // In D3D, use a 1024x1024 shadow texture
            mSceneMgr->setShadowTextureSettings(1024, 2);
        }
        else
        {
            // Use 512x512 texture in GL since we can't go higher than the window res
            mSceneMgr->setShadowTextureSettings(512, 2);
        }
    }

    DotSceneOctreeWorld::~DotSceneOctreeWorld()
    {
        clearScene();
    }

    void DotSceneOctreeWorld::initializeDefaultCamera(void)
    {
        // Get random player start point
        mCamera = ActorManager::getSingleton().createCameraActor("DefaultCamera");
        ViewPoint defaultVP = mSceneMgr->getSuggestedViewpoint(true);

        mCamera->setNearClipDistance(10);
        mCamera->setFarClipDistance(10000);
        mCamera->setPosition( defaultVP.position );

        // Quake uses X/Y horizon, Z up
        mCamera->setFOVy(60);
        //mCamera->rotate(defaultVP.orientation);
        
        mCamera->setFixedYawAxis(false);

        // Create one viewport, entire window
        Viewport* newVp = Ogre::Root::getSingletonPtr()->
            getAutoCreatedWindow()->addViewport(mCamera->getOgreCamera(),1);
        newVp->setBackgroundColour(ColourValue(0,0,0));
    }

    void DotSceneOctreeWorld::loadScene(const String& levelName)
    {
        if( mbSceneLoaded )
            clearScene();

        // Add some default lighting to the scene
        mSceneMgr->setAmbientLight(ColourValue(0.55, 0.55, 0.55));

		// Create the visual entity and scene node
		///@todo umgestalten, dass es auch mehrfach aufgerufen werden kann.
		/// und das Level korrekt abgebaut wird.
		Entity* entity = mSceneMgr->createEntity("level", levelName);
		SceneNode* node = mSceneMgr->getRootSceneNode()->createChildSceneNode("level");
		node->attachObject(entity);
		//entity->setCastShadows(false);
		
        initializeDefaultCamera();
        mbSceneLoaded = true;
    }

    void DotSceneOctreeWorld::clearScene()
    {
		try {
		    mSceneMgr->destroySceneNode("level");
		    mSceneMgr->removeEntity("level");
		}
		catch (Ogre::Exception& ex)
		{
		    // ignorieren. Gab es die Elemente halt nicht.
		    // wenn es nur eines nicht gibt, dann ist aber was im Argen.
		    // deshalb:
		    ///@todo separate �berpr�fung f�r die beiden Calls.
		}
		
        Ogre::Root::getSingleton().getAutoCreatedWindow()->removeAllViewports();

        ActorManager::getSingleton().deleteAllActors();

        mSceneMgr = Root::getSingleton().getSceneManager(ST_GENERIC);
        PhysicsManager::getSingleton().setWorldScene(this);
        mbSceneLoaded = false;
    }

    float* DotSceneOctreeWorld::getVerticesPtr()
    {
        return 0;//static_cast<DotSceneManager*>(mSceneMgr)->GetVerticesPtr(0, 0);
    }

	int* DotSceneOctreeWorld::getIndexPtr()
	{
        return 0;//static_cast<DotSceneManager*>(mSceneMgr)->GetIndexPtr(0, 0);
	}

	int DotSceneOctreeWorld::getVerticeCount()
	{
        return 0;//static_cast<DotSceneManager*>(mSceneMgr)->GetNumVertices(0, 0);
	}

	int DotSceneOctreeWorld::getIndexCount()
	{
        return 0;//static_cast<DotSceneManager*>(mSceneMgr)->GetNumIndex(0, 0);
	}
}
