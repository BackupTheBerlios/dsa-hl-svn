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

#ifndef __PhysicsManager_H__
#define __PhysicsManager_H__

#include <map>
#include <vector>
#include <OgreSingleton.h>
#include "GameTask.h"
#include <OgreNewt.h>

#include "CorePrerequisites.h"
#include "PhysicsGenericContactCallback.h"

namespace rl {

    class PhysicalThing;
    class PhysicsController;
    class Actor;
    class World;
	class PhysicalObject;

    class _RlCoreExport PhysicsManager
        :   public GameTask,
            protected Singleton<PhysicsManager>
    {
    public:

        enum GeometryTypes {
            GT_NONE = -1,
            GT_BOX = 0,
            GT_SPHERE = 1,
            GT_CAPSULE = 2,
            GT_MESH = 3,
            GT_ELLIPSOID = 4,
            GT_CONVEXHULL = 5
        };
        
        PhysicsManager();
        virtual ~PhysicsManager();

        virtual void run( Real elapsedTime );

        /**
         * @param geomType Grundform der Geometrie des Objektes
         * @param size Gr��e des Objektes in cm
         * @param mass die Masse in kg, wenn <= 0.0 ist es ein statisches Objekt
         * @param offsetMode bestimmt, wo der lokale Koordinatenursprung sitzt.
         * @param hullModifer soll ein Collision in NewtonConvexHullModifier gewrapped werden?
         *        Das ist bei sich schnell bewegenden bodies hilfreich um tunneling zu verhindern.
         *
         * @todo Geometry-Kapselung verallgemeinern. z.B. funktioniert Capusle
         *       momentan nur dann gut, wenn die H�he die Y-Achse ist.
         */
        /*PhysicalThing* createPhysicalThing(const int geomType, PhysicalObject* po,
            Ogre::Real mass, OffsetMode offsetMode = OM_BOTTOMCENTERED, bool hullModifier = false);*/
		PhysicalThing* createPhysicalThing(GeometryTypes geomType, PhysicalObject* po,
            Ogre::Real mass, bool hullModifier = false);

		/**
		 * Erschafft den entg�ltigen Physikproxy
		 */
		void createPhysicsProxy(PhysicalThing* pt, SceneNode* node);

        //PhysicalThing* createConvexHullPhysicalThing(Ogre::Entity*, Ogre::Real mass = 0.0f,
        //    const Ogre::Vector3& inertiaCoefficients = Ogre::Vector3(1.0f, 1.0f, 1.0f));

        void removeAndDestroyPhysicalThing(PhysicalThing* thing);

        // Global Settings
        void setGravity(Ogre::Real x, Ogre::Real y, Ogre::Real z);
        Vector3 getGravity() const;

        bool isEnabled() const;
        void setEnabled(bool enabled);

        // Singleton Stuff
        static PhysicsManager & getSingleton(void);
        static PhysicsManager * getSingletonPtr(void);

		/// Levelgeometrie hinzuf�gen
		void addLevelGeometry(Ogre::Entity* ent);
		/// Komplette Levelgeometrie aufl�sen
		void clearLevelGeometry();
		
		void toggleDebugMode();
        bool isDebugMode() const;

        PhysicsController* getPhysicsController(PhysicalThing* thing) const;
        /**
         * @param thing, the PhysicalThing to be controlled by controller
         * @param controller, the controller may be NULL.
         *        In this case, thing is not controlled anymore
         */
        void setPhysicsController(PhysicalThing* thing, PhysicsController* controller);

        /**
         * Sets the special contact callback for character-level-interaction.
         * This is handled specifically
         * @todo If other special cases are identified, the interface for
         * material handling has to be generalised.
         * @ param callback, the new contact callback. Can be NULL, in this case the default
         *   callback is set.
         */
        void setCharLevelContactCallback(PhysicsGenericContactCallback* callback);

        // Newton callbacks ...
        /// generic force callback. Gravity is applied and the force,
        /// applied via PhysicalThing interface.
        static void genericForceCallback(OgreNewt::Body* body);

        /// special force callback for controlled characters (or monsters even)
        /// those bodies have up vector and are controlled using inverse dynamics
        static void controlledForceCallback(OgreNewt::Body* body);

        OgreNewt::World* _getNewtonWorld() const;
        OgreNewt::MaterialID* _getLevelMaterialID() const;
        OgreNewt::MaterialID* _getCharMaterialID() const;
		const OgreNewt::MaterialID* _getDefaultMaterialID() const;

    private:
        typedef std::map<PhysicalThing*, PhysicsController*> ControllerMap;

        bool mEnabled;
        OgreNewt::World* mWorld;
        OgreNewt::Debugger* mNewtonDebugger;
        std::vector<PhysicalThing*> mPhysicalThings;
        ControllerMap mControlledThings;
        std::vector<OgreNewt::Body*> mLevelBodies;
        bool mDebugMode;
        Ogre::Vector3 mGravity;
        Ogre::AxisAlignedBox mWorldAABB;
        Ogre::Real mElapsed;
        Ogre::Real mUpdate;

        OgreNewt::MaterialID* mLevelID;
        OgreNewt::MaterialID* mCharacterID;
        OgreNewt::MaterialPair* mDefaultPair;
        OgreNewt::MaterialPair* mCharLevelPair;
        OgreNewt::MaterialPair* mCharCharPair;
        PhysicsGenericContactCallback* mGenericCallback;
    };
}

#endif
