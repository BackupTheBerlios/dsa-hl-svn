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
#include <CEGUIWindowManager.h>
#include "UiPrerequisites.h"
#include "Exception.h"

#include "UiSubsystem.h"
#include "CeGuiWindow.h"
#include "InputManager.h"
#include "WindowManager.h"

using namespace std;
using namespace CEGUI;
using namespace Ogre;

namespace rl
{

int CeGuiWindow::sNumCeGuiWindows = 0;

CeGuiWindow::CeGuiWindow(const CeGuiString& xmlfile, WindowType type, bool modal)
: mVisible(false),
	mModal(modal),
	mWindowType(type)
{
	Logger::getSingleton().log(Logger::UI, 
		LML_TRIVIAL, "Lade Fenster '"+Ogre::String(xmlfile.c_str())+"'");
   	mWindow = CeGuiWindow::loadWindow(xmlfile, mNamePrefix);
	if (mWindow == NULL)
	{
		Throw(rl::IllegalStateException, Ogre::String("Could not load window '")+xmlfile.c_str()+"'.");
	}
	mWindow->hide();
	
	if (modal)
	{
		mWindow->setAlwaysOnTop(true);
		mWindow->setModalState(true);
	}

	mName = mWindow->getName();
	getRoot()->addChildWindow(mWindow);
	WindowManager::getSingleton().registerWindow(this);
	mWindow->subscribeEvent(Window::EventActivated, 
		boost::bind(
			&rl::WindowManager::handleMovedToFront,
			rl::WindowManager::getSingletonPtr(),
			this));
}

CeGuiWindow::~CeGuiWindow()
{
}

CEGUI::Window* CeGuiWindow::loadWindow(const CeGuiString& xmlfile)
{
	CeGuiString prefix = "";
	return loadWindow(xmlfile, prefix);
}


CEGUI::Window* CeGuiWindow::loadWindow(const CeGuiString& xmlfile, CeGuiString& prefix)    
{
	CeGuiString namePrefix;
	if (prefix == "")
		prefix.assign(StringConverter::toString(sNumCeGuiWindows));
	sNumCeGuiWindows++;

	CEGUI::Window* window = NULL;		
	try 
	{
		window = CEGUI::WindowManager::getSingleton().loadWindowLayout(xmlfile, 
			prefix);
	}
	catch(...)
	{
	}

	return window;
}

bool CeGuiWindow::isVisible()
{
    return mVisible;
}

void CeGuiWindow::setVisible(bool visible)
{
    if(mVisible != visible)
    {
        if (visible)
            show();
        else
            hide();
    }
}

bool CeGuiWindow::isModal()
{
	return mModal;
}

void CeGuiWindow::show()
{
	if (!mVisible)
	{
		if (!beforeShow())
			return;

		InputManager::getSingleton().registerCeGuiWindow(this);

		mWindow->show();
        mVisible = true;
    }
}

void CeGuiWindow::hide()
{
	if (mVisible)
	{
		if (!beforeHide())
			return;
	
		mWindow->hide();
		
		InputManager::getSingleton().unregisterCeGuiWindow(this);

		mVisible = false;
	}
}

CeGuiWindow::WindowType CeGuiWindow::getWindowType()
{
	return mWindowType;
}

bool CeGuiWindow::beforeHide()
{
	return true;
}

bool CeGuiWindow::beforeShow()
{
	return true;
}

CEGUI::Window* CeGuiWindow::getRoot()
{
	return CEGUI::WindowManager::getSingleton().getWindow((utf8*)UiSubsystem::CEGUI_ROOT);
}

Window* CeGuiWindow::getWindow(const char* name, const char* requiredClass)
{
	CEGUI::Window* wnd = CEGUI::WindowManager::getSingleton().getWindow(mNamePrefix + (utf8*)name);
	
	if (wnd == NULL)
		Throw(
			rl::NullPointerException, 
			"Window "+Ogre::String(name)+" is NULL");

	if (requiredClass != NULL && !wnd->testClassName(requiredClass))
		Throw(
			rl::NullPointerException, 
			"Window "+Ogre::String(name)+" has not the required class " + Ogre::String(requiredClass));

	return wnd;
}

Editbox* CeGuiWindow::getEditbox(const char* name)
{
	return static_cast<Editbox*>(getWindow(name, "Editbox"));
}

TabPane* CeGuiWindow::getTabPane(const char* name)
{
	return static_cast<TabPane*>(getWindow(name, "TabPane"));
}

Listbox* CeGuiWindow::getListbox(const char* name)
{
	return static_cast<Listbox*>(getWindow(name, "Listbox"));
}

StaticText* CeGuiWindow::getStaticText(const char* name)
{
	return static_cast<StaticText*>(getWindow(name, "StaticText"));
}

StaticImage* CeGuiWindow::getStaticImage(const char* name)
{
	return static_cast<StaticImage*>(getWindow(name, "StaticImage"));
}

MultiColumnList* CeGuiWindow::getMultiColumnList(const char* name)
{
	return static_cast<MultiColumnList*>(getWindow(name, "MultiColumnList"));
}

MultiLineEditbox* CeGuiWindow::getMultiLineEditbox(const char* name)
{
	return static_cast<MultiLineEditbox*>(getWindow(name, "MultiLineEditbox"));
}

ProgressBar* CeGuiWindow::getProgressBar(const char* name)
{
	return static_cast<ProgressBar*>(getWindow(name, "ProgressBar"));
}

MenuBase* CeGuiWindow::getMenu(const char* name)
{
	return static_cast<MenuBase*>(getWindow(name, "MenuBase"));
}

MenuItem* CeGuiWindow::getMenuItem(const char* name)
{
	return static_cast<MenuItem*>(getWindow(name, "MenuItem"));
}

PushButton* CeGuiWindow::getPushButton(const char* name)
{
	return static_cast<PushButton*>(getWindow(name, "PushButton"));
}

const CeGuiString& CeGuiWindow::getName() const
{
	return mName;
}

void CeGuiWindow::centerWindow()
{
	CEGUI::Point pos(
		(1.0-mWindow->getRelativeWidth())/2, 
		(1.0-mWindow->getRelativeHeight())/2);
	mWindow->setPosition(Relative, pos);
}

void CeGuiWindow::bindClickToCloseWindow(CEGUI::Window* button)
{
	button->subscribeEvent(Window::EventMouseClick,
		boost::bind(&CeGuiWindow::destroyWindow, this));
}

void CeGuiWindow::bindCloseToCloseButton()
{
	mWindow->subscribeEvent(FrameWindow::EventCloseClicked,
		boost::bind(&CeGuiWindow::destroyWindow, this));
}

bool CeGuiWindow::destroyWindow()
{
	setVisible(false);
	CEGUI::WindowManager::getSingleton().destroyWindow(mWindow);
	return true;
}

CEGUI::Window* CeGuiWindow::getWindow()
{
	return mWindow;
}

const CeGuiString& CeGuiWindow::getNamePrefix() const
{
	return mNamePrefix;
}

}
