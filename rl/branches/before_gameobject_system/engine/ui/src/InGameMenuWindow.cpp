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
#include <boost/bind.hpp>

#include "UiPrerequisites.h"

#include "UiSubsystem.h"
#include "CoreSubsystem.h"

#include "InGameMenuWindow.h"
#include "Action.h"
#include "ActionManager.h"

#include <map>

using namespace CEGUI;
using namespace Ogre;
using std::map;

namespace rl {

InGameMenuWindow::InGameMenuWindow()
: CeGuiWindow("ingamemenuwindow.xml", WND_MOUSE_INPUT)
{
	update();
}

InGameMenuWindow::~InGameMenuWindow()
{
}

void InGameMenuWindow::createMenu(MenuBase* menu)
{
	CEGUI::WindowManager* windowMan = CEGUI::WindowManager::getSingletonPtr(); 
				
	const ActionVector actions = ActionManager::getSingleton().getInGameGlobalActions();
	map<CeGuiString, PopupMenu*> menuGroups;

	for (ActionVector::const_iterator actIter = actions.begin(); actIter != actions.end(); actIter++)
	{
        Action* action = *actIter;
		ActionGroup* group = action->getGroup();
		if (group != NULL)
		{
			PopupMenu* menuGrp;
			map<CeGuiString, PopupMenu*>::iterator grpIter = menuGroups.find(group->getName());
			if (grpIter != menuGroups.end())
			{
				menuGrp = (*grpIter).second;
			}
			else
			{				
				MenuItem* grpItem = static_cast<MenuItem*>(windowMan->createWindow("RastullahLook/MenuItem", getNamePrefix()+"IngameMenu/"+group->getName()));
				grpItem->setText(group->getName());
				menu->addChildWindow(grpItem);

				menuGrp = static_cast<PopupMenu*>(windowMan->createWindow("RastullahLook/PopupMenu", getNamePrefix()+"IngameMenu/Menu"+group->getName()));
				grpItem->addChildWindow(menuGrp);				

				menuGroups[group->getName()] = menuGrp;
			}

			MenuItem* item = static_cast<MenuItem*>(windowMan->createWindow("RastullahLook/MenuItem", getNamePrefix()+"IngameMenu/"+group->getName()+"/"+action->getName()));
			item->setText(action->getDescription());
			menuGrp->addChildWindow(item);

			setAction(item, action);
		}
	}
}

void InGameMenuWindow::setAction(MenuItem* item, Action* action)
{
	item->subscribeEvent(
		MenuItem::EventClicked, 
		boost::bind(
			&InGameMenuWindow::handleAction, 
			this, 
			action));
}

bool InGameMenuWindow::handleAction(Action* action)
{
	action->doAction(NULL, NULL, NULL);

	setVisible(false);
	
	return true;
}

void InGameMenuWindow::update()
{
	createMenu(getMenu("InGameMenu/Menubar"));	
}

}
