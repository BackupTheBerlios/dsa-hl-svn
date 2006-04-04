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

#ifndef __WindowFactory_H__
#define __WindowFactory_H__

#include "UiPrerequisites.h"
#include <OgreSingleton.h>

#include <OgreNoMemoryMacros.h>
#include "FixRubyHeaders.h"		// die Makros sind hier vor Ruby schon definiert
#include <ruby.h>
#include "FixRubyHeaders.h"
#include <OgreMemoryMacros.h>
#undef min

#include <CEGUIVector.h>
#include "GameTask.h"

namespace rl {

	class CeGuiWindow;
	class CharacterStateWindow;
	class CharacterSheetWindow;
	class Combat;
	class Console;
	class Container;
	class Creature;
	class DialogCharacter;
	class GameLoggerWindow;
	class GameObject;
	class InGameMenuWindow;
	class JournalWindow;
	class LogWindow;
	class Person;
	class WindowUpdater;

	class _RlUiExport WindowFactory : public Ogre::Singleton<WindowFactory>
	{
	public:
		WindowFactory();
		~WindowFactory();

		static WindowFactory& getSingleton();
		static WindowFactory* getSingletonPtr();

		void setActiveCharacter(Creature* character);

		void showActionChoice(GameObject* obj);
		void showAboutWindow();
		void showCharacterActionChoice();
		void showCharacterSheet();
		void showCharacterSheet(Person* chara);
		void showCombatWindow(Combat* combat, Creature* activeCreature);
		void showContainerContent(Container* container);
		void showDescriptionWindow(GameObject* obj);
		void showDialog(DialogCharacter* bot);
        void showExitConfirmation();
		bool showInputOptionsMenu(Creature* actionHolder);
		void showJournalWindow();
		void showMessageWindow(const CeGuiString& message);
		void showMainMenu();
		void showPickedObjectActions();
        void showPlaylist();
		void showTargetWindow();

		void toggleConsole();
		void toggleDebugWindow();
		void toggleGameLogWindow();
		void toggleCharacterStateWindow();
		void toggleInGameGlobalMenu();
		
		void checkForErrors();
		GameLoggerWindow* getGameLogger();

		void update();

		/** Writes Text into the Console or other output media */
		void writeToConsole(std::string text);
		static VALUE consoleWrite(VALUE self, VALUE str);


	private:
		GameLoggerWindow* mGameLogger;
		CharacterStateWindow* mCharacterStateWindow;
		InGameMenuWindow* mInGameMenuWindow;
		CharacterSheetWindow* mCharacterSheet;
		JournalWindow* mJournalWindow;
		LogWindow* mLogWindow;
		Console* mConsole;
	};
}

#endif
