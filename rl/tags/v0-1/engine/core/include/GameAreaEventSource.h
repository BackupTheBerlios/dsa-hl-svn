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

#ifndef __GameAreaEventSource_H__
#define __GameAreaEventSource_H__

#include "EventSource.h"
#include "EventCaster.h"

#include "GameAreaListener.h"
#include "GameAreaEvent.h"
#include "GameAreaTypes.h"

#include "CorePrerequisites.h"

namespace rl {

/** 
 * GameAreaEventSource
 * Die Quelle f�r Ereignisse die das betreten/verlassen eines Areals betreffen.
 * Hier werden die Actoren die sich zur letzten Abfrage innerhalb des Areals befanden
 * aufbewahrt, um die Differenzen bestimmen zu k�nnen, und die Abfragemethode verwaltet
 *
 *  @see GameAreaListener, GameAreaEvent, GameEventManager, GameAreaTypes
 */
class _RlCoreExport GameAreaEventSource : public virtual EventSource
{
public:
    /** Konstruktor
     *  @param areaType Die Art des abzufragenden Areals 
     *  @param act Der Actor, an den das Zentrum des Areals gekn�pft ist
     */
    GameAreaEventSource( GameAreaType* areaType, Actor* act );
    /// Dekonstruktor
    virtual ~GameAreaEventSource();

    /** Wird vom GameEventManager aufgerufen um die Szenenabfrage zu starten
     *  Berechnet die verlassenden/betretenden Aktoren und l�st mittels
     *  doDispatchEvents die Ereignisse aus.
     *
     *  @notice Sollte nicht eigenst�ndig aufgerufen werden
     */
    void performQuery( Ogre::Real timePassed );

    /** F�gt einen GameAreaListener hinzu, der zuk�nftig bei GameAreaEvents benachrichtigt wird 
     *
     * @param list Der hinzuzuf�gende Listener
     */
    void addAreaListener( GameAreaListener*  list );
    /** Entfernt einen GameAreaListener
    *
    * @param list Der zu entfernende Listener
    */
    void removeAreaListener( GameAreaListener* list );

    /// Gibt zur�ck ob sich Listener angemeldet haben
    bool hasListeners( ) const;

    /// Gibt die Art des Areals zur�ck
    GameAreaType* getGameAreaType() const { return mAreaType; };
    /// Gibt die Actoren die bei der letzten Abfrage innerhalb des Areals waren zur�ck
    const ActorMap& getInsideAreaList() const { return mInsideAreaList; };
    /// Gibt den Actor zur�ck, den das Areal umgibt
    Actor* getActor() const { return mActor; };
private: 
    /** Verteilt die Events an die angef�gten Listener
    * F�r jeden Actor wird ein einzelnes Ereigniss generiert, zuerst f�r alle
    * verlassenden Actoren, dann f�r die betretenden
    *
    * @param enteringActors Die neu hinzugekommenen Actoren
    * @param leavingActors Die verlassenden Actoren
    */
    void doDispatchEvents( const ActorMap& enteringActors, const ActorMap& leavingActors );

    /// Entfernt alle AreaListener
    void removeAllAreaListeners(  );

    /// Der Typ des Areals
    GameAreaType* mAreaType;

    /// Der EventCaster der die Verteilung an die Listener �bernimmt
    EventCaster<GameAreaEvent> mAreaEventCaster;
    /// Die Aktoren innerhalb des Areals
    ActorMap mInsideAreaList;
    /// Der Aktor den das Areal umgibt
    Actor* mActor;
};

}

#endif
