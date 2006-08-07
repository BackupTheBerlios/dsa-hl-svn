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

#ifndef __Sound_H__
#define __Sound_H__

#include "MultimediaPrerequisites.h"
#include "SoundResource.h"
#include "SoundChannel.h"
#include "Exception.h"


namespace rl {

   /** Diese Klasse dient der Interaktion mit dem jeweiligen Soundsystem
    * ein Objekt stellt eine einzelne Sounddatei oder einen Stream dar
    * @author Josch
    * @date 06-29-2005
    * @version 1.0
    * @version 2.0
    */
    class _RlMultimediaExport Sound 
    {
    public:
        /// Konstruktor
        Sound(const SoundResourcePtr &soundres);
        /// Destruktor
        virtual ~Sound();
        /// Name zur�ckgeben
        const Ogre::String& getName() const;
    
        /// Laedt den Sound.
        virtual void load() throw (RuntimeException) = 0;
        /// Entlaedt den Sound.
        virtual void unload() throw (RuntimeException) = 0;

        // SoundResource zur�ckgeben.
        const SoundResourcePtr &getSoundResource() const;
        // Wollen wir 3D?
        bool is3d() const;
        // Setzen des 3D-Flags.
        void set3d(bool is3d);
        // Sind wir gueltig
        virtual bool isValid() const throw (RuntimeException) = 0;
        
        // Sollen der Sound wiederholt werden?
        bool isLooping() const;
        // Setzen des Loop-Flags.
        void setLooping(bool looping);

        virtual float getLength() const = 0;

    private:
        /// Der Name des Sounds
        Ogre::String mName;
        /// Der Soundresource, auf den wir verweisen.
        SoundResourcePtr mSoundResource;

        // Damit die Timings alle 0.5 sek. abgeschickt werden.
        int mTicks;
        // Ob dieser Sound 3D sein soll.
        bool mIs3d;
        // Ob dieser Sound loopen soll.
        bool mIsLooping;
    }; 



}
#endif
