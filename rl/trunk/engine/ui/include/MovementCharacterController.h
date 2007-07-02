/* This source file is part of Rastullahs Lockenpracht.
 * Copyright (C) 2003-2007 Team Pantheon. http://www.team-pantheon.de
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

#ifndef __MovementCharacterController_H__
#define __MovementCharacterController_H__

#include "UiPrerequisites.h"
#include "GameTask.h"
#include "MovingCreature.h"
#include "PhysicsController.h"
#include "PhysicsGenericContactCallback.h"
#include "ControlState.h"
#include "DebugVisualisable.h"
#include "Selector.h"

#include <deque>

namespace rl {

    class Actor;
	class Creature;
    class MeshObject;
	class PhysicsMaterialRaycast;

    /**
     * This class handles character control via user input.
     */
    class _RlUiExport MovementControlState
	  : public ControlState,
		public PhysicsController,
        public PhysicsGenericContactCallback,
        public DebugVisualisable
    {
    public:
        typedef enum {VM_THIRD_PERSON, VM_FIRST_PERSON, VM_FREE_CAMERA} ViewMode;
        /**
         *  @throw NullPointerException if camera or character is NULL.
         *  @throw InvalidArgumentException if character is not placed in the scene.
         */
        MovementControlState(CommandMapper* cmdMapper, Actor* camera, Person* character);
        virtual ~MovementControlState();

        virtual void pause();
        virtual void resume();

        void run(Ogre::Real elapsedTime);

        /// This is the OgreNewt contact process callback for the combination
        /// Character <-> Level
        int userProcess();

        /// Newton force and torque callback
        void OnApplyForceAndTorque(PhysicalThing* thing);

        /// First oder Third person view.
        void setViewMode(ViewMode mode);
        ViewMode getViewMode();
        void toggleViewMode();

        /** Setzt die Camera in einen 30-Grad-Winkel dem Helden auf den Hinterkopf
        *  schauend im aktuellen Abstand vom Helden, wie durch den Spieler bestimmt.
        */
        void resetCamera();

        virtual bool mouseReleased(const OIS::MouseEvent& evt, OIS::MouseButtonID id);
        virtual bool keyPressed(const OIS::KeyEvent& evt);
        virtual bool keyReleased(const OIS::KeyEvent& evt);

        // Overrides from DebugVisualisable
        virtual DebugVisualisableFlag getFlag() const;
        virtual void updatePrimitive();

    protected:
        virtual void doCreatePrimitive();
        MovingCreature *mMovingCreature;

    private:

		/// private struct for holding state info of the controller
		struct CharacterState
		{
            CharacterState();
			int mCurrentMovementState;
			int mLastMovementState;
		};

        static Ogre::String msDebugWindowPageName;

		CharacterState mCharacterState;// does only refer to the movement caused by the keyboard

        // camera control params
        /// optimal distance to the character
        Ogre::Real mDesiredDistance;
        std::pair<Ogre::Real, Ogre::Real> mDistanceRange;
        Ogre::Degree mCamYaw; // f�r VM_FREE_CAMERA
        Ogre::Degree mCamVirtualYaw; // helps to simulate strafe+forward/backward movement
        Ogre::Degree mPitch;
        Ogre::Degree mRoll;
        std::pair<Ogre::Degree, Ogre::Degree> mPitchRange;

        Ogre::Vector3 mLookAtOffset;
        Ogre::Radian mRotationSpeed;
        Ogre::Real mMouseSensitivity;
        bool mInvertedMouse; // like in old games

        ViewMode mViewMode;

        int mObstractedFrameCount;
        Ogre::Real mObstractedTime;
        int mCameraJammedFrameCount;
        Ogre::Real mCameraJammedTime;

        PhysicsMaterialRaycast* mRaycast;
        HalfSphereSelector mSelector;
        HalfSphereSelector mCombatSelector;


        /// Camera Spring-Damping System (smooth movement) spring-factor
        Ogre::Real mLinearSpringK;
        /// Camera Spring-Damping System (smooth movement) damping-factor
        Ogre::Real mLinearDampingK;

        /// with this velocity the optimal Position of the cam moves away from the char
        Ogre::Real mCamMoveAwayVelocity;
        /// if there was no collision of the cam for this time, the cam can securely move backward
        Ogre::Real mCamMoveAwayStartTime;
        /// if the angle between the last camera pos and the character and the new one is smaller than this value, the camera can move away from the character
        Ogre::Radian mCamMoveAwayRange;

        void updateSelection();
        bool isEnemyNear();


        void updateCameraLookAt(Ogre::Real elapsedTime);

        void updateCharacter(Ogre::Real elapsedTime);

        //void interpolateAnimationLookAtOffset(std::string actAnim, std::string newAnim, Ogre::Real factor);


        /** Does all camera-stuff, moves the camera to the right position 
        * and does pathfinding (in a very simple way)
        * @warning this does only work well, if the character's material is not used for other objects!
        */
		void calculateCamera(const Ogre::Real& timestep);

        /** Calculates the position, 
        * the camera should move to.
        * @param SlowlyMoveBackward if set, the camera moves more slowly away from the character then toward it.
        * @param timestep in order to reset the camera (no valid last position) the timestep can be 0.
        */
        Ogre::Vector3 calculateOptimalCameraPosition(bool SlowlyMoveBackward, const Ogre::Real &timestep);
    };
}
#endif
