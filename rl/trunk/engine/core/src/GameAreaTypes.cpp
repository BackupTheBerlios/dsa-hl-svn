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

#include "GameAreaTypes.h"

#include "World.h"
#include "CoreSubsystem.h"

using namespace Ogre;

namespace rl {
    void GameAreaType::addQueryFlag( unsigned long flag  )
    {
        setQueryMask(  getQueryMask() | flag );
    }

    void GameAreaType::removeQueryFlag( unsigned long flag )
    {
        setQueryMask(  getQueryMask() &~ flag );
    }
}


namespace rl {

    GameSphereAreaType::GameSphereAreaType(Vector3 center, Real radius, unsigned long mask)
    {
        mSphereQuery = CoreSubsystem::getSingleton().getWorld()->
            getSceneManager()->createSphereQuery( Sphere(center,radius), mask );

        // Keine Welt-Geometrie erw�nscht, nur Movables
        mSphereQuery->setWorldFragmentType( SceneQuery::WFT_NONE );
    }

    GameSphereAreaType::~GameSphereAreaType()
    {
        mSphereQuery->clearResults();
        CoreSubsystem::getSingleton().getWorld()->getSceneManager()->destroyQuery( mSphereQuery );
    }

    ActorMap GameSphereAreaType::performQuery(  )
    {
        SceneQueryResult rs = mSphereQuery->execute();
        SceneQueryResultMovableList movList = rs.movables;
        
        ActorMap retMap;
        
        // Durch die Ergebnis Liste iterieren
        SceneQueryResultMovableList::iterator it;
        for (it = movList.begin(); it != movList.end(); ++it) 
        {
            MovableObject* mov = *it;  
            if( mov->getUserObject() != NULL )
            {
                // Zur Zeit sind die einzigen an Movables gekn�pfte Objekte Actoren
                Actor* act = dynamic_cast<Actor*>( mov->getUserObject() );
                retMap.insert(ActorPair(act->getName(),act)); 
            }
        }

        return retMap;
    }

    unsigned long GameSphereAreaType::getQueryMask() const
    {
        return mSphereQuery->getQueryMask();
    }

    void GameSphereAreaType::setQueryMask( unsigned long mask ) 
    {
        mSphereQuery->setQueryMask( mask );
    }

    void GameSphereAreaType::setQueryPosition( const Ogre::Vector3& vec )
    {
        // Wurde es bewegt?
        if( vec != mSphereQuery->getSphere().getCenter( ) )
            // Kugel muss neu erzeugt werden :(
            mSphereQuery->setSphere( Sphere(vec,mSphereQuery->getSphere().getRadius()) );
    }

    const Ogre::Vector3& GameSphereAreaType::getQueryPosition() const
    {
        
        return mSphereQuery->getSphere().getCenter( );
    }

}

