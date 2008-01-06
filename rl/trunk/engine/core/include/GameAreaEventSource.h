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

#ifndef __GameAreaEventSource_H__
#define __GameAreaEventSource_H__

#include "EventSource.h"
#include "EventCaster.h"

#include "GameAreaListener.h"
#include "GameAreaEvent.h"
#include "GameAreaTypes.h"

#include "CorePrerequisites.h"
#include "Properties.h"

namespace rl {

class ZoneManager;
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
     *  @param act Der Actor, an den das Zentrum des Areals gekn�pft ist, kann NULL sein f�r unbewegte areas
     */
    GameAreaEventSource( GameAreaType* areaType, Actor* act = NULL);
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
    GameAreaType* getGameAreaType() { return mAreaType; };
    /// Gibt die Actoren die bei der letzten Abfrage innerhalb des Areals waren zur�ck
    const ActorMap& getInsideAreaList() const { return mInsideAreaList; };
    /// Gibt den Actor zur�ck, den das Areal umgibt
    Actor* getActor() const { return mActor; };
    /// gibt an ob die ga gel�scht werden kann, wenn die dazugeh�rige zone gel�scht wird
    virtual bool destroyIfZoneDestroyed() const {return true;}
    /// loescht den Actor aus der liste der actoren, die sich gerade in der gamearea befinden
    void notifyActorDeleted(Actor* actor);
protected:
    /// helps the zonemanager to associate zones with areas
    friend class ZoneManager;
    void setId(long id) {mId = id;}
    long getId() const {return mId;}
    /// helps the zonemanger to get the values for saving the GameAreaEventSource
    PropertyRecord mProperties;
private:
    long mId;
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
