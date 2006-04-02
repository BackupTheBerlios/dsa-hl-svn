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

#include "LogWindow.h"

#include <boost/bind.hpp>

#include "Logger.h"

namespace rl
{
	LogWindow::LogWindow()
		: CeGuiWindow("logwindow.xml", WND_MOUSE_INPUT)
	{
		mRastullahLog = getMultiLineEditbox("LogWindow/RastullahLog/Text");
		mOgreLog = getMultiLineEditbox("LogWindow/OgreLog/Text");
		mErrorLog = getMultiLineEditbox("LogWindow/ErrorLog/Text");

		getWindow("LogWindow/UpdateButton")->subscribeEvent(
			CEGUI::Window::EventMouseClick,
			boost::bind(&LogWindow::update, this));
		bindCloseToCloseButton();
		bindClickToCloseWindow(getWindow("LogWindow/CloseButton"));

		centerWindow();
	}
	
	LogWindow::~LogWindow()
	{
	}
	
	bool LogWindow::update()
	{
		mErrorLog->setText(Logger::getSingleton().getErrorLog());
		mErrorLog->setCaratIndex(1999999999);
		mErrorLog->ensureCaratIsVisible();
		//TODO: ogre.log und rastullah.log
		
		return true;
	}
	
	void LogWindow::setVisible(bool visible, bool destroyAfterHide)
	{
		CeGuiWindow::setVisible(visible, destroyAfterHide);

		if (visible)
		{
			update();
		}
	}
	
	
} // namespace rl
