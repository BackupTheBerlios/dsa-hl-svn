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

#ifndef __InputManager_H__
#define __InputManager_H__

#include <set>
#include <OgreEventQueue.h>
#include <OgreFrameListener.h>
#include <OgreEventListeners.h>
#include <OgreInput.h>

#include "UiPrerequisites.h"
#include "FixRubyHeaders.h"
#include "GameTask.h"

#include <OgreNoMemoryMacros.h>
#include <CEGUI.h>
#include <OgreMemoryMacros.h>

namespace CEGUI {
	enum MouseButton;
}

namespace rl {

	class CeGuiWindow;
	class GameObject;
	class CharacterController;
    class CommandMapper;

    class _RlUiExport InputManager
        :	public GameTask, 
        public Ogre::KeyListener, 
        public Ogre::MouseListener, 
        public Ogre::MouseMotionListener, 
        public Ogre::Singleton<InputManager>
    {
    public:
        InputManager(void);
        ~InputManager();

        static InputManager & getSingleton(void);
        static InputManager * getSingletonPtr(void);

        bool isKeyDown(Ogre::KeyCode kc);
        bool isMouseButtonDown(int iButtonID);

        Ogre::Real getMouseRelativeX(void);
        Ogre::Real getMouseRelativeY(void);
        Ogre::Real getMouseRelativeZ(void);

        void run(Ogre::Real elapsedTime);

        void registerCeGuiWindow(CeGuiWindow* window);
        void unregisterCeGuiWindow(CeGuiWindow* window);
        bool isCeguiActive();

        virtual void mouseClicked(Ogre::MouseEvent* e);
        virtual void mouseEntered(Ogre::MouseEvent* e);
        virtual void mouseExited(Ogre::MouseEvent* e);
        virtual void mousePressed(Ogre::MouseEvent* e);
        virtual void mouseReleased(Ogre::MouseEvent* e);
        virtual void mouseMoved(Ogre::MouseEvent* e);
        virtual void mouseDragged(Ogre::MouseEvent* e);
        virtual void keyPressed(Ogre::KeyEvent* e);
        virtual void keyReleased(Ogre::KeyEvent* e);
        virtual void keyClicked(Ogre::KeyEvent* e);

        CeGuiString getKeyName(int scancode, int syskeys);
        CeGuiString getKeyName(int combinedKeyCode);
        int getScanCode(const CeGuiString& name);
        int getSystemCode(const CeGuiString& name);
        void loadKeyMapping(const Ogre::String& filename);
        void loadCommandMapping(const Ogre::String& filename);
        typedef std::map<int, CEGUI::utf8> KeyCharMap;
        typedef std::map<int, CeGuiString> KeyNameMap;

        void setCharacterController(CharacterController* controller);

    private:
        static const int TIME_SHOW_DESCRIPTION = 4000;

        enum { NUM_MOUSE_BUTTON=4, NUM_KEYS=256 };
        enum InputSwitch { SWITCH_NO_SWITCH, SWITCH_TO_BUFFERED, SWITCH_TO_UNBUFFERED };
        InputSwitch mScheduledInputSwitch;

        Ogre::InputReader* mInputReader;
        Ogre::EventQueue* mEventQueue; 
        Ogre::EventProcessor* mEventProcessor;

        bool mKeyDown[NUM_KEYS];
        bool mMouseButtonDown[NUM_MOUSE_BUTTON];

        unsigned short mScreenX;
        unsigned short mScreenY;
        bool mBuffered, mEventInitialized, mInputInitialized;			

        CommandMapper* mCommandMapper;

        KeyCharMap mKeyMapNormal;
        KeyCharMap mKeyMapShift;
        KeyCharMap mKeyMapAlt;
        KeyNameMap mKeyNames;

        int mNumActiveWindowsMouseInput;
        int mNumActiveWindowsKeyboardInput;
        int mNumActiveWindowsAllInput;

        CharacterController* mCharacterController;

        CEGUI::MouseButton convertOgreButtonToCegui(int ogre_button_id);

        bool sendKeyToCeGui(Ogre::KeyEvent* e);

        void switchMouseToUnbuffered();
        void switchMouseToBuffered();
        void resetPressedKeys( bool up );
        void checkMouseButton(
            const int button, const int buttonMask, int& pressedButtonMask, int& releasedButtonMask);

        CEGUI::utf32 getKeyChar(Ogre::KeyEvent* ke);
    };
}

#endif
