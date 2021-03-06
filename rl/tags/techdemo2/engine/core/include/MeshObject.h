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
        Ogre::AxisAlignedBox getDefaultSize() const;
        Ogre::AxisAlignedBox getPoseSize(const Ogre::String& animationName);
        
		/// Returns whether a mesh has an animation called animName or not.
        bool hasAnimation(const Ogre::String& animName);

		/** Returns a MeshAnimation for the given animation name.
         *  Either returns an already created MeshAnimation or creates it ad-hoc in a paused state.
         *  @throw IllegalArgumentException, if animName is not an animation of the Mesh.
         */
        MeshAnimation* getAnimation(const Ogre::String& animName);

		/** Creates and returns a MeshAnimation for the given animation name.
         *  This animation is returned unpaused.
         *  @param animName name of the animation to be started.
         *  @param speed scale factor for animation speed. 1.0 is normal time as defined.
         *  @param timesToPlay 0 means infinitly looped.
         *
         *  @throw IllegalArgumentException, if animName is not an animation of the Mesh.
         */
		MeshAnimation* startAnimation(const Ogre::String& animName, 
		    Ogre::Real speed=1.0, unsigned int timesToPlay=0);

		/** First stops oldAnimName, then starts newAnimName
         *  @see rl::MeshObject::startAnimation
         *  @see rl::MeshObject::stopAnimation
         *  @throw IllegalArgumentException, if either anim name is not an animation of the Mesh.
         */
		MeshAnimation* replaceAnimation(const Ogre::String& oldAnimName,
            const Ogre::String& newAnimName, 
		    Ogre::Real speed=1.0, unsigned int timesToPlay=0);

		/** Stops an animation.
         *  @param animName name of the animation to be stopped.
         *
         *  @throw IllegalArgumentException, if animName is not an animation of the Mesh.
         */
        void stopAnimation(const Ogre::String& animName);

        void stopAllAnimations( );

        void setCastShadows (bool enabled);
        bool getCastShadows (void) const;


		const Ogre::String &getMeshName();

		virtual bool isMeshObject();

        /// Per Skeleton-Blend-Setting
        bool getBlendCumulative(void) const;
        void setBlendCumulative(bool cumulative);

        void setHighlighted( bool highlight );
        
        virtual Ogre::String getObjectType();
    private:
		Ogre::String mMeshName;
        Ogre::AxisAlignedBox mSize;
        typedef std::map<Ogre::String, Ogre::AxisAlignedBox> PoseMap;
		PoseMap mPoseSizes;

		Ogre::AxisAlignedBox calculateSize();
        Ogre::AxisAlignedBox calculateSizeFromPose(const Ogre::String& animationName);
		Ogre::AxisAlignedBox getAabbFromVertexData(Ogre::VertexData*);
    };
}
#endif
