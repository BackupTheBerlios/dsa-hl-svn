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

#ifndef __PhysicalThing_H__
#define __PhysicalThing_H__

#include "CorePrerequisites.h"
#include "PhysicsContactListener.h"
#include "PhysicsManager.h"

#include <OgreNewt.h>

namespace rl {

	class Actor;
	class MeshObject;

    class _RlCoreExport PhysicalThing
    {
    public:
		PhysicalThing(
			PhysicsManager::GeometryTypes geomType,
			PhysicalObject* po, 
			Real mass, 
			bool hullModifier = false);

        /// Klasse Polymorph machen, damit SWIG gl�cklich ist.
        virtual ~PhysicalThing();

        Ogre::Vector3 getPosition() const;
        void setPosition(const Ogre::Vector3& pos);
        void setPosition(Ogre::Real x, Ogre::Real y, Ogre::Real z);

        Ogre::Quaternion getOrientation() const;
        void setOrientation(const Ogre::Quaternion& orienation);
        void setOrientation(Ogre::Real w, Ogre::Real x, Ogre::Real y, Ogre::Real z);

        void setVelocity(const Ogre::Vector3& velocity);
        Ogre::Vector3 getVelocity();

        // Sets the vector, that will always point up.
        void setUpConstraint(const Ogre::Vector3& upVector = Ogre::Vector3::UNIT_Y);
        Ogre::Vector3 getUpConstraint() const;
        void clearUpConstraint();

        // Sets whether to use default gravity, or override it with its own
        void setGravityOverride(bool override, const Ogre::Vector3& gravity);
        void setGravityOverride(bool override,
            Ogre::Real x = 0.0f, Ogre::Real y = 0.0f, Ogre::Real z = 0.0f);

        Actor* getActor() const;
		OgreNewt::Body* _getBody() const;
        
        void _update();
        void _setActor(Actor* actor);
        void _attachToSceneNode(Ogre::SceneNode* node);
		void _attachToBone(MeshObject* object, const std::string& boneName);
        void _detachFromSceneNode(Ogre::SceneNode* node);

        void onApplyForceAndTorque();
        void addForce(const Ogre::Vector3& force);

        void freeze();
        void unfreeze();

        Ogre::Real getMass() const;
        void setMass(Ogre::Real mass);

		void createPhysicsProxy(SceneNode* node);

        /** Called to update the collision of the physical thing, in order to adapt
         *  to a new animation state.
         *  @warning This is only applicable to ConvexHullCollisions.
         *  @throw IllegalStateException, if PhysicalThing does not represent a ConvexHullCollision
         */
        void updateCollisionHull();

        /** Called to update the collision of the physical thing, in order to adapt
         *  to a new animation state.
         */
        void fitToPose(const Ogre::String& name);

        void setContactListener(PhysicsContactListener* listener);
        PhysicsContactListener* getContactListener() const;

		void prepareUserControl(OgreNewt::MaterialID* material);
        void unprepareUserControl();

    private:
        Actor* mActor;
        OgreNewt::Body* mBody;
        OgreNewt::BasicJoints::UpVector* mUpVectorJoint;
        Ogre::Vector3 mOffset;
        Ogre::Quaternion mOrientationBias;
        Ogre::Vector3 mPendingForce;
        bool mOverrideGravity;
        Ogre::Vector3 mGravity;
        PhysicsContactListener* mContactListener;

        typedef std::map<Ogre::String, OgreNewt::CollisionPtr> CollisionMap;
        CollisionMap mPoseCollisions;

		Ogre::Real mMass;
		PhysicsManager::GeometryTypes mGeometryType;
		PhysicalObject* mPhysicalObject;
		bool mHullModifier;

		void setOffset(const Ogre::Vector3& offset);
		void setOrientationBias(const Ogre::Quaternion& orientation);
		PhysicsManager::GeometryTypes getGeometryType() const;
		void setBody(OgreNewt::Body* body);
		OgreNewt::CollisionPtr createCollision(
			const AxisAlignedBox& aabb, Vector3* offset = NULL, 
			Quaternion* orientation = NULL, Vector3* inertiaCoefficients = NULL) const;
	};
}

#endif
