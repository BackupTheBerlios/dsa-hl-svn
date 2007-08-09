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

#ifndef __ListenerObject_H__
#define __ListenerObject_H__

#include "CorePrerequisites.h"
#include "ActorControlledObject.h"


namespace rl {
/// @TODO SharedPtr
    class ListenerMovable;
    
    
    /** Klasse f�r alle Listenerobjekte, die man an einen Aktor
     *  heften kann. Zwischen Actor und ActorControlledObject
     *  besteht eine 1:1-Beziehung.
     */
    class _RlCoreExport ListenerObject : public ActorControlledObject
    {
    private:
        static ListenerObject* gActiveListener;
        
    public:
        ListenerObject(ListenerMovable *listener);
        virtual ~ListenerObject();
        
        /**
         * Interne Methode. Wird vom Aktor aufgerufen, wenn sich dessen
         * Status ge�ndert hat. (Position, Orientierung, etc)
         * Die Standardimplementierung macht nichts, kann aber
         * von abgeleiteten Klassen �berschrieben werden.
         */
        virtual void _update();
        
        /// Wie ActorControlledObject::getMovableObject()
        /// Nur schon gebrauchsfertig gecastet.
        ListenerMovable* getListener() const;
        
        virtual Ogre::String getObjectType() const;

		virtual bool isMeshObject();        
    };
}
#endif
