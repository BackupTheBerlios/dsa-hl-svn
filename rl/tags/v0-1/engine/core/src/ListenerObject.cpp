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

#include "ListenerObject.h"
#include "Actor.h"
#include <Ogre.h>
#include "ListenerMovable.h"

using namespace Ogre;

namespace rl {
   
/**
 * @param listener. Der Listener, Besitz wird übernommen.
 * @author JoSch
 * @date 03-16-2005
 */   
ListenerObject::ListenerObject(ListenerMovable *listener) : ActorControlledObject()
{
    mMovableObject = dynamic_cast<MovableObject*>(listener);
}

/**
 * @author JoSch
 * @date 03-16-2005
 */   
ListenerObject::~ListenerObject()
{
    if (mMovableObject)
    {
        delete mMovableObject;
    }
}

/**
 * @return Immer false, weil kein Meshobjekt.
 * @author JoSch
 * @date 03-16-2005
 */   
bool ListenerObject::isMeshObject()
{
	return false;
}

/**
 * @author JoSch
 * @date 03-16-2005
 */   
void ListenerObject::_update()
{
    ActorControlledObject::_update();
    ListenerMovable *listener = getListener();
    Actor *actor = getActor();
    if (!listener || !actor) // Einer ist Null
    {
        return;
    }
    listener->setPosition(actor->getPosition());
    Vector3 *temp1 = new Vector3();
    Vector3 *temp2 = new Vector3(actor->getPosition());
    Real length = temp2->normalise();
    actor->getOrientation().ToAxes(temp1);
    *temp1 += *temp2;
    *temp1 *= length;
    listener->setOrientation(*temp1, *temp1);
    // TODO Orientation korrigieren
}

/**
 * @param Der bewegbare Listener
 * @author JoSch
 * @date 03-16-2005
 */   
ListenerMovable* ListenerObject::getListener()
{
    return reinterpret_cast<ListenerMovable*>(mMovableObject);
}

/**
 * @return Immer "ListenerObject"
 * @author JoSch
 * @date 03-16-2005
 */   
String ListenerObject::getObjectType()
{
    return "ListenerObject";
}


}
