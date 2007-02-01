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

#ifndef __CharacterStateWindow_H__
#define __CharacterStateWindow_H__

#include "UiPrerequisites.h"

#include "CeGuiWindow.h"
#include "Eigenschaft.h"
#include "ObjectStateChangeEventSource.h"

namespace rl {

	class Creature;

	class _RlUiExport CharacterStateWindow : public CeGuiWindow, public ObjectStateChangeListener
	{
	public:
		CharacterStateWindow();
		~CharacterStateWindow();

		void setCharacter(Creature* character);
		void update();
		void setVisible(bool visible, bool destroyAfterHide = false);

		void objectStateChanged(ObjectStateChangeEvent* evt);

	private:
		Creature* mCharacter;

		CEGUI::ProgressBar* mLP;
		CEGUI::ProgressBar* mAP;
		CEGUI::ProgressBar* mAU;
		CEGUI::Window* mName;
		
	};
}


#endif

