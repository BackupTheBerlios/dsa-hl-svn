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

#ifndef __Actor_H__
#define __Actor_H__

#include "CorePrerequisites.h"
#include <OgreUserDefinedObject.h>

namespace rl {

    class PhysicalThing;
    class ActorControlledObject;

    class _RlCoreExport Actor : public Ogre::UserDefinedObject
    {
    public:
        static Ogre::String DEFAULT_SLOT_NAME;

        enum QueryGroupFlags
        {
            QGF_NONE = 0,
            QGF_DEFAULT = 1,
            QGF_PLAYER = 2,
            QGF_ALL = 0xFFFFFFFF
        };

        Actor(const Ogre::String& name,
            ActorControlledObject* aco = NULL,
            PhysicalThing* pt = NULL,
            Ogre::UserDefinedObject* go = NULL);

        /** Dekonstruktor
          * @remark Nicht direkt aufrufen,
          * sondern ActorManager::destroyActor() benutzen.
          */
        ~Actor();

        /// Gibt den eindeutigen Namen des Aktors zur�ck
        const Ogre::String& getName() const;
        
        /// Gibt das UserdefinedObject der n�chsten Schicht zur�ck
        Ogre::UserDefinedObject* getGameObject() const;
        /// Setzt das UserdefinedObject der n�chsten Schicht
        void setGameObject(Ogre::UserDefinedObject* uo);
        
        /// Gibt die Physikalische Repr�sentation zur�ck
        PhysicalThing* getPhysicalThing() const;
        /// Setzt die Physikalische Repr�sentation
        void setPhysicalThing( PhysicalThing* pt );

        /// Gibt das kontrollierte Objekt zur�ck
        ActorControlledObject* getControlledObject() const;
        /// Setzt das kontrollierte Objekt
        void setControlledObject( ActorControlledObject* act );
        

        /** F�gt diesen Aktor in einen SzenenKnoten ein.
          * 
          * @param parent Der SzenenKnoten in den der Aktor eingef�gt werden soll
          * @param offsetPosition Die Verschiebung
          * @param offsetOrientation Die Drehung
          * @param physicsBone Der Knochen an den die 
          *        Physikalische Verkn�pfung gebunden werden soll
          *	
          */
        void placeIntoNode( 
            Ogre::SceneNode* parent,
            const Ogre::Vector3& offsetPosition = Ogre::Vector3::ZERO,
            const Ogre::Quaternion& offsetOrientation = Ogre::Quaternion::IDENTITY,
            const Ogre::String& physicsBone = "");
        /** F�gt diesen Aktor in die Szene (der RootNode) ein.
         * 
         * @param offsetPosition Die Verschiebung
         * @param offsetOrientation Die Drehung
         * @param physicsBone Der Knochen an den die 
         *        Physikalische Verkn�pfung gebunden werden soll
         *	
         */
        void placeIntoScene(
            const Ogre::Vector3& position = Ogre::Vector3::ZERO,
            const Ogre::Quaternion& orientation = Ogre::Quaternion::IDENTITY,
            const Ogre::String& physicsBone = "");   
        /// F�gt diesen Aktor in die Szene (der RootNode) ein.
        void placeIntoScene(
            Ogre::Real px, Ogre::Real py, Ogre::Real pz,
			Ogre::Real ow, Ogre::Real ox, Ogre::Real oy, Ogre::Real oz,
            const Ogre::String& physicsBone = "");
            
        /// Entfernt den Aktor aus der Szene
        void removeFromScene();


        /// Gibt die Anfrage-Maske zur�ck,
        unsigned long getQueryMask() const;
        /// Setzt die Anfrage-Maske
        void setQueryMask( unsigned long mask = 0xFFFFFFFF );

        /// F�gt der Anfrage-Maske ein Flag hinzu
        void addQueryFlag( unsigned long flag  );
        /// Entfernt ein Flag aus der Anfrage-Maske
        void removeQueryFlag( unsigned long flag );



        /// Gibt die aktuelle Position des Aktors relativ zu seinem Parent zur�ck
        const Ogre::Vector3& getPosition(void) const;
        /// Gibt die aktuelle Position des Aktors relativ zur Welt zur�ck
        const Ogre::Vector3& getWorldPosition(void) const;

        /// Setzt die Position des Aktors relativ zu seinem Parent
        void setPosition(const Ogre::Vector3& vec);
        /// Setzt die Position des Aktors relativ zu seinem Parent
        void setPosition(Ogre::Real x, Ogre::Real y, Ogre::Real z);

        /// Skaliert den Aktor
        void setScale( Ogre::Real sx, Ogre::Real sy, Ogre::Real sz );

        /// Gibt die Orientierung des Aktors relativ zu seinem Parent zur�ck
        const Ogre::Quaternion& getOrientation(void) const;
        /// Gibt die Orientierung des Aktors relativ zur Welt zur�ck
        const Ogre::Quaternion& getWorldOrientation(void) const;

        /// Setzt die Orientierung des Aktors
        void setOrientation(const Ogre::Quaternion& orientation);

        /// Bewegt den Aktor an seiner Achse entlang
        void translate(const Ogre::Vector3& d, Ogre::Node::TransformSpace ts);

        /// Dreht den Aktor um seine Z-Achse
        void roll(Ogre::Real angleunits);
        /// Dreht den Aktor um seine Z-Achse
        void pitch(Ogre::Real angleunits);
        /// Dreht den Aktor um seine Z-Achse
        void yaw(Ogre::Real angleunits);

        /// Dreht den Aktor
        void rotate(const Ogre::Quaternion& q, Ogre::Node::TransformSpace ts);
      

        /**
         *	Befestigt einen anderen Aktor �ber einen UnterNode an diesem Aktors.
         *
         * @param actor Der Aktor
         * @param childSlot Der Slot an dem zu befestigenden Aktor, wenn DEFAULT_SLOT_NAME ignoriert
         *                  verursacht zus�tzliche Offset/Drehung
         * @param offsetPosition Die zus�tzliche Verschiebung
         * @param offsetOrientation Die zus�tzliche Drehung
         */
        void attach( 
            Actor* actor,
            const Ogre::String& childSlot = "SLOT_DEFAULT",
            const Ogre::Vector3& offsetPosition=Ogre::Vector3::ZERO,
            const Ogre::Quaternion& offsetOrientation=Ogre::Quaternion::IDENTITY
            );

        /**
         *	Befestigt einen anderen Aktor �ber einen UnterNode an diesem Aktors.
         *
         * @param actor Der Aktor
         * @param childSlot Der Slot an dem zu befestigenden Aktor, wenn DEFAULT_SLOT_NAME ignoriert,
         *                  verursacht zus�tzliche Offset/Drehung
         * @param offsetPosition Die zus�tzliche Verschiebung
         * @param offsetAxis Die Achse der zus�tzlichen Drehung
         * @param offsetRotation Die zus�tzliche Drehung
         */
        void attachAxisRot( 
            Actor* actor,
            const Ogre::String& childSlot = "SLOT_DEFAULT",            
            const Ogre::Vector3& offsetPosition=Ogre::Vector3::ZERO,
            const Ogre::Vector3& offsetAxis=Ogre::Vector3::UNIT_X,
            const Ogre::Radian& offsetRotation=Ogre::Radian(0) );

        /**
          *	Befestigt einen anderen Aktor an einem SLOT dieses Aktors.
          *
          * @param actor Der Aktor
          * @param slot Der Slot an diesem Aktor, wenn DEFAULT_SLOT_NAME ignoriert
          * @param childSlot Der Slot an dem zu befestigenden Aktor, wenn DEFAULT_SLOT_NAME ignoriert,
          *                  verursacht zus�tzliche Offset/Drehung
          * @param offsetPosition Die zus�tzliche Verschiebung
          * @param offsetOrientation Die zus�tzliche Drehung
          */
        void attachToSlot( 
            Actor* actor,
            const Ogre::String& slot,
            const Ogre::String& childSlot = "SLOT_DEFAULT",
            const Ogre::Vector3& offsetPosition=Ogre::Vector3::ZERO,
			const Ogre::Quaternion& offsetOrientation=Ogre::Quaternion::IDENTITY
			);

        /**
         *	Befestigt einen anderen Aktor an einem SLOT dieses Aktors.
         *
         * @param actor Der Aktor
         * @param slot Der Slot an diesem Aktor, wenn DEFAULT_SLOT_NAME ignoriert
         * @param childSlot Der Slot an dem zu befestigenden Aktor, wenn DEFAULT_SLOT_NAME ignoriert, 
         *                  verursacht zus�tzliche Offset/Drehung
         * @param offsetPosition Die zus�tzliche Verschiebung
         * @param offsetAxis Die Achse der zus�tzlichen Drehung
         * @param offsetRotation Die zus�tzliche Drehung
         */
        void attachToSlotAxisRot( 
            Actor* actor,
            const Ogre::String& slot,
            const Ogre::String& childSlot = "SLOT_DEFAULT",            
			const Ogre::Vector3& offsetPosition=Ogre::Vector3::ZERO,
            const Ogre::Vector3& offsetAxis=Ogre::Vector3::UNIT_X,
            const Ogre::Radian& offsetRotation=Ogre::Radian(0) );

        /** Entfernt einen zuvor befestigten Aktor.
         *  @param actor Der zu entfernende Aktor
         */
        void detach(Actor* actor);
        
        Ogre::SceneNode* _getSceneNode() const;
        Ogre::MovableObject* _getMovableObject() const;
        void _update();

        /// Setzt diesem Aktor ein Highlight
		void setHighlighted(bool highlight);
        /// Gibt zur�ck ob der Aktor gehighlighted ist
        bool isHighlighted() const;
        
        ///@todo mehr Query-Methoden f�r Childs
        Actor* getChildByName(const Ogre::String& name ) const;
        ///@todo Visibility

        /// Entfernt alle Kinder vom Node
        void detachAllChildren( );
    private:
        typedef std::set<Actor*> ChildSet;

        /// Der einzigartige Name
        Ogre::String mName;
        /// Verkn�pfung zur Physikalischen Repr�sentation
        PhysicalThing* mPhysicalThing;
        /// Verkn�pfung zur N�chsten Schicht
        Ogre::UserDefinedObject* mGameObject;
        /// Verkn�pfung zum kontrollierten Objekt
        ActorControlledObject* mActorControlledObject;
        /// Der Parent-Aktor
        Actor* mParent;
        /// Darunterliegende Aktoren
        ChildSet mChilds;
        /// Der SceneNode des Aktors
        Ogre::SceneNode* mSceneNode;
        /// Speichert ob der Aktor zur Zeit leuchtet
        bool mHighlighted;
        /// Ist der Aktor an einem Bone angehangen?
        bool mAttachedToBone;

        void doPlaceIntoScene(
            Ogre::SceneNode* parent,
            const Ogre::Vector3& position = Ogre::Vector3::ZERO,
            const Ogre::Quaternion& orientation = Ogre::Quaternion::IDENTITY,
            const Ogre::String& physicsBone = "" );   

        /**
         *	K�mmert sich um das Durchf�hren des Befestigens
         *
         * @param actor Der Aktor
         * @param slot Der Slot an diesem Aktor, wenn DEFAULT_SLOT_NAME ignoriert
         * @param childSlot Der Slot an dem zu befestigenden Aktor, wenn DEFAULT_SLOT_NAME ignoriert
         *                  verursacht zus�tzliche Offset/Drehung
         * @param offsetPosition Die zus�tzliche Verschiebung
         * @param offsetOrientation Die zus�tzliche Drehung
         */
        void doAttach(
            Actor* actor,
            const Ogre::String& slot, 
            const Ogre::String& childSlot, 
            const Ogre::Vector3& offsetPosition,
			const Ogre::Quaternion& offsetOrientation);

        /**
         *	K�mmert sich um das Durchf�hren des Abnehmens
         */
        void doDetach(Actor* actor);

    };
}
#endif
