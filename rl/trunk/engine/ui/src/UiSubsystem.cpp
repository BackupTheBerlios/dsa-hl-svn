#include <xercesc/util/XMemory.hpp>	// Muss vor Ogre stehen (zumindest f�r VS)
#include <renderers/OgreGUIRenderer/ogrerenderer.h>
#include "UiSubsystem.h"
#include "CoreSubsystem.h"
#include "RubyInterpreter.h"
#include "Console.h"
#include "DebugWindow.h"
#include "ThirdPersonGameController.h"
#include "InputManager.h"
#include "CommandMapper.h"
#include "MessageWindow.h"
#include "WindowManager.h"

#include "GameLoop.h"
#include "ActorManager.h"
#include "GameActor.h"
#include "CameraActor.h"
#include "World.h"

// BEGIN TEST
#include "Person.h"
#include "CharacterSheetWindow.h"
#include "GameObject.h"
#include "Action.h"
#include "ActionChoiceWindow.h"
#include "ActionManager.h"
#include "DsaManager.h"
#include "DialogWindow.h"
// END TEST

template<> rl::UiSubsystem* Singleton<rl::UiSubsystem>::ms_Singleton = 0;

namespace rl {
	const char* UiSubsystem::CEGUI_ROOT = "RootWindow";

	UiSubsystem& UiSubsystem::getSingleton(void)
	{
		return Singleton<UiSubsystem>::getSingleton();
	}

	UiSubsystem* UiSubsystem::getSingletonPtr(void)
	{
		return Singleton<UiSubsystem>::getSingletonPtr();
	}

	UiSubsystem::UiSubsystem()
	{
        mRequestExit = false;

        initializeUiSubsystem();
	}

    UiSubsystem::~UiSubsystem() 
    {  
		delete Console::getSingletonPtr();
        delete InputManager::getSingletonPtr();

        GameLoop::getSingleton().removeSynchronizedTask(mGameController);
    }

    void UiSubsystem::initializeUiSubsystem( void )
    {
		using namespace CEGUI;

        World* world = CoreSubsystem::getSingleton().getWorld();
        SceneManager* sceneMgr = CoreSubsystem::getSingleton().getWorld()->getSceneManager();

		CEGUI::OgreRenderer* rend = 
			new CEGUI::OgreRenderer(Ogre::Root::getSingleton().getAutoCreatedWindow(), 
									Ogre::RENDER_QUEUE_OVERLAY, 
									false, 
									3000,
									sceneMgr);

		new System(rend, NULL, (utf8*)"modules/common/gui/cegui.config");

		// load scheme and set up defaults
		System::getSingleton().setDefaultMouseCursor((utf8*)"TaharezLook", (utf8*)"MouseArrow");
		Window* sheet = CEGUI::WindowManager::getSingleton().createWindow((utf8*)"DefaultGUISheet", (utf8*)CEGUI_ROOT);
		sheet->setSize(
			Absolute, 
			Size(Ogre::Root::getSingleton().getAutoCreatedWindow()->getWidth(), 
				Ogre::Root::getSingleton().getAutoCreatedWindow()->getHeight()));
		sheet->setPosition(Absolute, Point(0, 0));
		System::getSingleton().setGUISheet(sheet);

		//Initializing InputManager
		new CommandMapper();
        new InputManager();
		new WindowManager();

		new DebugWindow();
		new Console();
		((RubyInterpreter*)CoreSubsystem::getSingleton().getInterpreter() )->initializeInterpreter( (VALUE(*)(...))&UiSubsystem::consoleWrite );
			      
        //runTest();
    }

    void UiSubsystem::requestExit()
    {
        mRequestExit = true;
    }
    
    bool UiSubsystem::isRequestingExit() const
    {
        return mRequestExit;
    }

	void UiSubsystem::writeToConsole(std::string text)
	{
		Console::getSingleton().write(text);
	}

	VALUE UiSubsystem::consoleWrite(VALUE self, VALUE str)
	{
		Console::getSingleton().write(RubyInterpreter::val2str(str) + " \n");
		return Qnil;
	}

	Person* UiSubsystem::getActiveCharacter()
	{
		return mCharacter;
	}

	void UiSubsystem::setActiveCharacter(Person* person)
	{
		mCharacter = person;
		 
		CameraActor* camera = dynamic_cast<CameraActor*>(
            ActorManager::getSingleton().getActor("DefaultCamera"));
		mGameController = new ThirdPersonGameController(
            camera->getOgreCamera(), person->getActor());
        GameLoop::getSingleton().addSynchronizedTask(mGameController);
		World* world = CoreSubsystem::getSingleton().getWorld();
		world->setActiveActor(person->getActor());
	}

	void UiSubsystem::showActionChoice(GameObject* obj)
	{
		ActionChoiceWindow* w = new ActionChoiceWindow(UiSubsystem::getSingleton().getActiveCharacter());
		w->showActionsOfObject(obj);
		w->setVisible(true);
	}
	
	void UiSubsystem::showCharacterActionChoice()
	{
		showActionChoice(getActiveCharacter());
	}

	void UiSubsystem::showMessageWindow(const CeGuiString& message)
	{
		MessageWindow* w = new MessageWindow();
		w->setText(message);
		w->setVisible(true);
	}

	void UiSubsystem::setBattleMode(bool inBattle)
	{
		mInBattle = inBattle;
		//TODO: Irgendwann sp�ter, UI auf Kampfmodus umstellen
	}

	bool UiSubsystem::isInBattleMode()
	{
		return mInBattle;
	}

	void UiSubsystem::runTest()
	{
		InputManager::getSingleton().setObjectPickingActive(true);
	}
}
