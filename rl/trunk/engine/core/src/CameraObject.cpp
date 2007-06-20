/* This source file is part of Rastullahs Lockenpracht.
* Copyright (C) 2003-2007 Team Pantheon. http://www.team-pantheon.de
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

#include "CameraObject.h"
#include "Actor.h"
#include "CoreSubsystem.h"
#include "World.h"
#include "Exception.h"

using namespace Ogre;

namespace rl {
    CameraObject::CameraObject(const String& name)
    {
        Camera* camera = CoreSubsystem::getSingleton().getWorld()->
            getSceneManager()->createCamera(name);

        camera->setNearClipDistance(0.1);
        camera->setFarClipDistance(5000);
        camera->setOrientation( Quaternion::IDENTITY );
        camera->setPosition( Vector3::ZERO );
        mMovableObject = camera;
    }

    CameraObject::~CameraObject()
    {
        CoreSubsystem::getSingleton().getWorld()->getSceneManager()->destroyCamera(getCamera());
    }

    Camera* CameraObject::getCamera() const
    {
        return static_cast<Camera*>(mMovableObject);
    }

    String CameraObject::getObjectType() const
    {
        return "CameraObject";
    }

    AxisAlignedBox CameraObject::getDefaultSize() const
    {
        Real r = getCamera()->getNearClipDistance();
        return AxisAlignedBox(2.0*Vector3(-r, -r, -r), 2.0*Vector3(r, r, r));
    }

	Vector3 CameraObject::getPointOnScreen(const Ogre::Vector3& worldCoords) const
	{
		const Camera* camera = getCamera();
        const Matrix4& viewMatrix = camera->getViewMatrix(true);
        const Matrix4& projMatrix = camera->getProjectionMatrix();

        Vector3 eyeSpacePos = viewMatrix.transformAffine(worldCoords);
        Vector3 screenSpacePos =  projMatrix * eyeSpacePos;
		screenSpacePos.z = eyeSpacePos.z;

        return screenSpacePos;
	}
}
