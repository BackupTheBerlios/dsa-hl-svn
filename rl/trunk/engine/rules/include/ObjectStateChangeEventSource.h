/* This source file is part of Rastullahs Lockenpracht.
 * Copyright (C) 2003-2005 Team Pantheon. http://www.team-pantheon.de
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

#ifndef __ObjectStateChangeEventSource_H__
#define __ObjectStateChangeEventSource_H__

#include "EventSource.h"
#include "EventCaster.h"

#include "ObjectStateChangeListener.h"
#include "ObjectStateChangeEvent.h"

#include "RulesPrerequisites.h"

#include <OgreUserDefinedObject.h>

namespace rl {

class GameObject;
/** 
 * ObjectStateChangeEventSource
 *
 *  @see ObjectStateChangeEvent, ObjectStateChangeListener, GameObject
 */
class _RlRulesExport ObjectStateChangeEventSource : public virtual EventSource
{
public:
    /** Konstruktor
     */
	ObjectStateChangeEventSource( GameObject* obj );
	ObjectStateChangeEventSource( );
	/// Dekonstruktor
    virtual ~ObjectStateChangeEventSource();

    /** F�gt einen ObjectStateChangeListener hinzu, der zuk�nftig bei ObjectStateChangeEvents benachrichtigt wird 
     *
     * @param list Der hinzuzuf�gende Listener
     */
    void addObjectStateChangeListener( ObjectStateChangeListener*  list );
    /** Entfernt einen ObjectStateChangeListener
    *
    * @param list Der zu entfernende Listener
    */
    void removeObjectStateChangeListener( ObjectStateChangeListener* list );

    /// Gibt zur�ck ob sich Listener angemeldet haben
    bool hasListeners( ) const;

    /// Gibt das Objekt zur�ck, das �berwacht wird
    GameObject* getObject() const { return mObject; }

    void fireObjectStateChangeEvent();
protected:
	void setObject(GameObject* obj) { mObject = obj; }

private: 
    /// Der EventCaster der die Verteilung an die Listener �bernimmt
    EventCaster<ObjectStateChangeEvent> mObjectStateChangeEventCaster;
    /// Das Object der �berwacht wird
    GameObject* mObject;
};

}

#endif
