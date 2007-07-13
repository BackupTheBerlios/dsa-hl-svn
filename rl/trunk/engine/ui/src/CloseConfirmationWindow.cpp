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
#include "stdinc.h" //precompiled header

#include <boost/bind.hpp>

#include "CloseConfirmationWindow.h"
#include "GameLoop.h"

namespace rl {

CloseConfirmationWindow::CloseConfirmationWindow()
: AbstractWindow("closeconfirmationwindow.xml", WIT_MOUSE_INPUT | WIT_KEYBOARD_INPUT, true, true)
{
	getPushButton("CloseConfirmationWindow/YesButton")->subscribeEvent(
		CEGUI::Window::EventMouseClick,
		boost::bind(
			&CloseConfirmationWindow::handleConfirm,
			this
		));

    bindDestroyWindowToXButton();
	bindDestroyWindowToClick(getPushButton("CloseConfirmationWindow/NoButton"));
	centerWindow();
}

CloseConfirmationWindow::~CloseConfirmationWindow()
{
}

bool CloseConfirmationWindow::handleConfirm()
{
	GameLoop::getSingleton().quitGame();
	return true;
}

}
