/* SoundManager.h - Spielt verschiedene Sound nach Belieben.
 * (C) 2003-2005. Team Pantheon. www.team-pantheon.de
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

#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include <Ogre.h>
#include <list>
#include <boost/thread/mutex.hpp>
#include "SoundPrerequisites.h"
#include "SoundSubsystem.h"

namespace rl {

typedef std::list<Ogre::String> StringList;

/**
 * Der SoundManager verwaltet die Sounds, die das Spiel benutzt.
 * Die Sounds werden geladen und entladen, je nachdem, ob die
 * Speichergrenze ueberschritten wurde.
 * @author JoSch
 * @version 1.0
 * @date 04-26-2004
 */ 
class _RlSoundExport SoundManager: public Ogre::ResourceManager,
        public Ogre::Singleton<SoundManager> {
    private:
        /// Ein Mutex, um das Hinzuf�gen der Sounds zu synchronisieren.
        boost::mutex mResListMutex;
        /// Welche Dateiendung soll verwendet werden.
        virtual StringList getExtension();
    public:
        /// Gibt das Singleton zurueck.
        static SoundManager& getSingleton();
        /// Gibt einen Zeiger auf das Singleton zurueck.
        static SoundManager* getSingletonPtr();
        /// Eine Resource erzeugen
        Ogre::Resource* create(const Ogre::String& resName);
        /// Konstruktor
        SoundManager();
        /// Alle Sounds in die Resourcenliste eintragen.
        virtual void addSounds();
        /// einen Sound hinzufuegen (Mit Mutex)
        virtual void add(Ogre::Resource *song);
        /// erzeugt einen Sound und f�gt ihn hinzufuegen (Mit Mutex mittelbar)
        virtual void add(const Ogre::String& filename);
        /// Eine Namesliste erzeugen
        StringList getSounds();
};

}
#endif
