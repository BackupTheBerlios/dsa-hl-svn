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

#ifndef SOUNDCONFIG_H_
#define SOUNDCONFIG_H_

#include "CeGuiWindow.h"

namespace rl
{

	class SoundDriverConfigWindow;

	class SoundConfig : public rl::CeGuiWindow
	{
	public:
		SoundConfig();
		virtual ~SoundConfig();

		void registerSoundDriverConfigWindow(SoundDriverConfigWindow* wnd);
		
	private:
		bool handleOK();
		bool handleVolumeMusicChanged();
		bool handleVolumeSoundChanged();
		bool handleVolumeMasterChanged();
		bool handleConfig();
		bool handleClose();
		
		// Die Combobox, aus der gew�hlt wird.
		CEGUI::Combobox*	mDriverBox;
		CEGUI::Slider*		mVolumeSound;
		CEGUI::Slider*		mVolumeMusic;
		CEGUI::Slider*		mVolumeMaster;
		CEGUI::Window*		mDriverConfig;
		
		SoundDriverConfigWindow* mCurrentConfig;

		std::list<SoundDriverConfigWindow*> mDriverConfigs;
	};

}

#endif /*SOUNDCONFIG_H_*/
