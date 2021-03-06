/* This source file is part of Rastullahs Lockenpracht.
 * Copyright (C) 2003-2006 Team Pantheon. http://www.team-pantheon.de
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

namespace rl 
{
	%feature("director") DialogCharacter;
	class DialogCharacter
	{
	public:
		DialogCharacter();
		virtual ~DialogCharacter();

		const rl::CeGuiString getName() const;
		rl::Creature* getDialogCharacter() const;
		void setDialogCharacter(rl::Creature* dialogCharacter);
		rl::Creature* getDialogPartner() const;
		void setDialogPartner(rl::Creature* dialogPartner);
		
		rl::Creature* getNonPlayerCharacter()const;
		void setNonPlayerCharacter(rl::Creature* npc);
		
		rl::Creature* getPlayerCharacter()const;
		void setPlayerCharacter(rl::Creature* pc);
		
		virtual int calcOptionValue(const rl::CeGuiString& optionName)=0;	
		virtual int calcResponseValue(const rl::CeGuiString& responseName)=0;
		virtual int calcSelectionValue(const rl::CeGuiString& selectionName)=0;
	};

	class DialogSubsystem 
	{
	public:
		static DialogSubsystem& getSingleton(void);
		rl::DialogCharacter* getBot(const rl::CeGuiString& botName);
		rl::DialogCharacter* loadBot(const rl::CeGuiString& fileName, const rl::CeGuiString& botName = "");
		void setCurrentDialogCharacter(rl::DialogCharacter* bot);
	};
}
