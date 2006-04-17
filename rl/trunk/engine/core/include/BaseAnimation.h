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

#ifndef __BASEANIMATION_H__
#define __BASEANIMATION_H__

#include "CorePrerequisites.h"

#include <set>
#include <map>
#include "AnimationListener.h"
#include "PlaylistObject.h"


namespace rl {

/** 
	Diese Klasse erm�glicht eine einfache Steuerung von Animationseinstellungen
	und ist die Basisklasse erweiterter Animationen.
	@remarks Instanzen werden �ber den AnimationManager erzeugt
	@see AnimationManager
*/
class _RlCoreExport BaseAnimation : public virtual EventSource, public virtual PlaylistObject
{
    public:
		/**
            @param speed		Geschwindigkeit, auch negativ
			@param timesToPlay	Abspielanzahl, 0 = unendlich
            @param paused       Startet pausiert wenn true
		*/
        BaseAnimation(Ogre::Real length, Ogre::Real speed, unsigned int timesToPlay, bool paused );
		/**	Ein Konstruktor, f�r eine sp�ter festlegbare Animation
  		 */
		BaseAnimation();

		/// Virtueller Destruktor
		virtual ~BaseAnimation( );
        
		/// Gibt zur�ck ob die Animation pausiert ist
        virtual bool isPaused() const;
		/** Pausieren/Fortsetzen der Animation
			@param	isPaused	Zuk�nftiger Status
			@remarks	L�st einen AnimationPaused/Unpaused Event aus
		*/
        virtual void setPaused( bool isPaused );

		/// Gibt zur�ck ob die globale Beschleunigung ignoriert wird
		virtual bool isIgnoringGlobalSpeed() const;
		/**	Setzt die Ignoranz
			@param		isIgnoringGlobalSpeed Die zuk�nftige Ignoranz der globalen Geschwindigkeit
			@remarks	M�glichkeit die globale SlowMotion zu umgehen
						N�tzlich f�r Statusanzeigen, oder �hnliche konstante Animationen
		*/
		virtual void setIgnoringGlobalSpeed( bool isIgnoringGlobalSpeed );

		/// Gibt die aktuelle Geschwindigkeit zur�ck
		virtual Ogre::Real getSpeed() const;
		/**	Setzt die aktuelle Geschwindigkeit der Animation
			@param speed die Geschwindigkeit
			@remarks	1.0 ist die normale Geschwindigkeit der Animation 
						(mLength in Sekunden), negative Werte spielen die 
						Animation r�ckw�rts ab. Bei 0 herrscht Stillstand.
		*/
		virtual void setSpeed( Ogre::Real speed );
		/// Negiert die aktuelle Geschwindigkeit
		virtual void reverseAnimation();

		/** Setzt die maximalen Wiederholungszahl der Animation
			@param	timesToPlay		Die nicht negative Anzahl der Wiederholungen
			@remarks	Bei 0 wird die Animation beliebig oft wiederholt
		*/
		virtual void setTimesToPlay(unsigned int timesToPlay);
		/// Gibt die Anzahl der bereits vollst�ndig abgespielten Wiederholungen zur�ck
		virtual unsigned int getTimesPlayed() const;
		/** Setzt die Abspielzeit zur�ck
			L�st dabei auch eine m�gliche Pause auf, und spult die Animation zur�ck
		*/
		virtual void resetTimesPlayed();
		/// Gibt zur�ck wieviel Wiederholungen insgesamt abzuspielen sind
		virtual unsigned int getTimesToPlay() const;
		/// Gibt zur�ck wieviele Wiederholungen noch durchzuf�hren sind
		virtual unsigned int getTimesToPlayLeft() const;
		/// Gibt die Abspieldauer zur�ck (intern)
		virtual Ogre::Real getTimePlayed() const;

		/** Setzt das Delay vor dem ersten Abspielen der Animation.
		    @param delay Die Verz�gerung in Sekunden  */
		virtual void setDelay( Ogre::Real delay );
		/// Gibt die Verz�gerung vor dem Ersten Abspielen in Sekunden zur�ck.
		virtual Ogre::Real getDelay() const;

		/// Zeit hinzuf�gen - wird vom AnimationManager aufgerufen
		void addTime( Ogre::Real timePassed );

        /// addTime f�r alle Unterklassen
		virtual void doAddTime( Ogre::Real timePassed ) = 0;

		/** F�gt einen AnimationListener hinzu
			@param listener Der hinzuzuf�gende Listener
			@remarks Der Listener wird benachrichtigt, wenn
					  * die Animation pausiert/fortgesetzt wird
					  * die Animation ihr gesamten Wiederholungen vollendet hat
		*/
		virtual void addAnimationListener( AnimationListener *listener);
		/// Entfernt einen AnimationListener
		virtual void removeAnimationListener( AnimationListener *listener);

		/** F�gt einen AnimationFrameListener hinzu
			@param listener Der hinzuzuf�gende Listener
			@param frameNumber Die zu �berwachende Zeitindex
			@remarks	Der Listener wird benachrichtigt, wenn der Zeitindex erreicht oder 
						�bersprungen wird. Dabei wird, falls der Fortschritt gr��er als die
						L�nge der Animation korrekt gepr�ft, so dass keine Events verloren 
						gehen
		*/
		virtual void addAnimationFrameListener( AnimationFrameListener *listener, Ogre::Real frameNumber );
		/// Entfernt einen AnimationListener an allen Zeitindizes
		virtual void removeAnimationFrameListener( AnimationFrameListener *listener );
		/// Entfernt einen AnimationListener an einem bestimmtem Zeitindex
		virtual void removeAnimationFrameListener( AnimationFrameListener *listener, Ogre::Real frameNumber );
        
        /// Lade die Animation
        virtual void load();
        /// Entlade die Animation
        virtual void unload();
        /// Starte die Animation
        virtual void start();
        /// Stoppe die Animation
        virtual void stop();
        /// Pausiere die Animation
        virtual void pause();

        virtual Ogre::Real getLength() const;
    protected:
		/// Ein Delay vor dem ersten Abspielen
		Ogre::Real mDelay;
        /// Dauer
        Ogre::Real mLength;
		/// Pause
        bool mPaused;
		/// Ignoriert die globale Geschwindigkeit
		bool mIgnoringGlobalSpeed;
		/// Eigene Geschwindigkeit
		Ogre::Real mSpeed;
		/// Gesamte Abspielwiederholungen
		unsigned int mTimesToPlay;
		/// Bisherige Abspielzeit
		Ogre::Real mTimePlayed;

		/// EventCaster
		EventCaster<AnimationEvent> mAnimationCaster;

        typedef std::multimap<Ogre::Real,AnimationFrameListener*> 
            AnimationFrameListenerMap;
		/// Die Multimap mit den FrameNummern und den dazugeh�rigen Listenern
		AnimationFrameListenerMap mAnimationFrameListener;

        /// Looping setzen
		virtual void setLoop( bool loop );
		/// Looping zur�ckgeben
		virtual bool isLoop() const;
	private:
		/// �berwacht das erreichen der einzelnen Frames f�r die Listener
		void checkAnimationFrameListeners( Ogre::Real timePassed );
        /// Entfernt alle Listener
        void removeAllListeners();
};

}
#endif
