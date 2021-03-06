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
#include "Fmod3Config.h"

#include <boost/bind.hpp>
#include <fmod.h>

#include "Exception.h"
#include "Fmod3Driver.h"
#include "SoundDriver.h"
#include "SoundManager.h"

using namespace CEGUI;
using namespace Ogre;

namespace rl
{

	Fmod3Config::Fmod3Config(Fmod3Driver* driver)
		: SoundDriverConfigWindow("fmod3.xml"),
		mOutput(NULL),
		mDriver(NULL),
		mWasActive(false),
		mFmod3Driver(driver)
	{
		mOutput = getCombobox("Fmod3Config/Output");
		mOutput->subscribeEvent(
			Combobox::EventTextChanged,
			boost::bind(&Fmod3Config::handleOutputChanged, this));

		mSpeaker = getCombobox("Fmod3Config/Speaker");
		mSpeaker->subscribeEvent(
			Combobox::EventTextChanged,
			boost::bind(&Fmod3Config::handleSpeakerChanged, this));

		for (int i = 0; i < FSOUND_GetNumDrivers(); i++)
		{
			ListboxTextItem *item = new ListboxTextItem(
				Ogre::String(FSOUND_GetDriverName(i))
				);
			mDriver->addItem(item);
		}

		DriverMap data = mFmod3Driver->getDriverData();
		for(DriverMap::const_iterator it = data.begin(); it != data.end(); it++)
		{
			ListboxItem *item = new ListboxTextItem(it->first);
			mDriver->addItem(item);
		}

		ListboxItem *item = new ListboxTextItem("Mono");
		mSpeaker->addItem(item);
		item = new ListboxTextItem("Stereo");
		mSpeaker->addItem(item);
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
		item = new ListboxTextItem("Dolby Digital");
		mSpeaker->addItem(item);
		item = new ListboxTextItem("Kopfh�rer");
		mSpeaker->addItem(item);
		item = new ListboxTextItem("Quad");
		mSpeaker->addItem(item);
		item = new ListboxTextItem("Surround");
		mSpeaker->addItem(item);
#endif
		mWindow->setVisible(false);
		getRoot()->removeChildWindow(mWindow);
	}

	bool Fmod3Config::handleOK()
	{
		setVisible(false);
		destroyWindow();
		return true;
	}

	bool Fmod3Config::handleDriverChanged()
	{
		return true;
	}

	Fmod3Config::~Fmod3Config()
	{
		/*	if (mWasActive)
		{
		SoundManager::getSingleton().setActiveDriver(mFmod3Driver);
		}
		*/
	}

	bool Fmod3Config::handleOutputChanged()
	{
		return true;
	}

	bool Fmod3Config::handleSpeakerChanged()
	{
		return true;
	}

	Fmod3Driver* Fmod3Config::getDriver()
	{
		return mFmod3Driver;
	}

}
