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

#ifndef __MeshObject_H__
#define __MeshObject_H__

#include "CorePrerequisites.h"
#include "PhysicalObject.h"

#include <OgreEntity.h>

namespace rl {

	class MeshAnimation;

	class _RlCoreExport MeshObject : public PhysicalObject
    {
    public:
        MeshObject(const Ogre::String& name, const Ogre::String& meshname);
        virtual ~MeshObject();

        /// Wie ActorControlledObject::getMovableObject()
        /// Nur schon gebrauchsfertig gecastet.
        Ogre::Entity* getEntity() const;
        
        /// Groesse der Boundingbox
        Ogre::Vector3 getDefaultSize() const;
		Ogre::Vector3 getPoseSize(const Ogre::String& animationName);
        Ogre::Vector3 getCenter();
        Ogre::Real getRadius();
        Ogre::Real getHeight();
        
		MeshAnimation* getAnimation(const Ogre::String& animName);
		MeshAnimation* startAnimation(const Ogre::String& animName, 
		    Ogre::Real speed=1.0, unsigned int timesToPlay=0);
		MeshAnimation* replaceAnimation(const Ogre::String& oldAnimName, const Ogre::String& newAnimName, 
		    Ogre::Real speed=1.0, unsigned int timesToPlay=0);
        void stopAnimation(const Ogre::String&);
        void stopAllAnimations( );

        void setCastShadows (bool enabled);
        bool getCastShadows (void) const;

		virtual bool isMeshObject();

        /// Per Skeleton-Blend-Setting
        bool getBlendCumulative(void) const;
        void setBlendCumulative(bool cumulative);

        void setHighlighted( bool highlight );
        
        virtual Ogre::String getObjectType();
    private:
        Ogre::Vector3 mSize;
		typedef std::map<Ogre::String, Ogre::Vector3> PoseMap;
		PoseMap mPoseSizes;

		Ogre::Vector3 calculateSize();
		Ogre::Vector3 calculateSizeFromPose(const Ogre::String& animationName);
		Ogre::AxisAlignedBox getAabbFromVertexData(Ogre::VertexData*);
    };
}
#endif
