 /** 
 * Generated CppUnit test driver template.
 * To build it, add the following line at the end of
 * your existing Makefile:
 *    include rl/engine/sound/src/OalppSoundInterfaceTest.mk
 * Build the rl/engine/sound/src/OalppSoundInterfaceTest target from the Make Target view
 */
#include <cppunit/extensions/HelperMacros.h>
#include "OgreNoMemoryMacros.h"
#include "MultimediaPrerequisites.h"
#ifdef  _MSC_VER
#define _USE_MATH_DEFINES
#endif
#include <math.h>
#include <OgreIteratorWrappers.h>
#include <boost/thread.hpp>
#include "SoundManager.h"
#include "SoundResource.h"
#include "Sound.h"
#include "SoundChannel.h"
#include "ListenerMovable.h"
#include "Logger.h"
#include "SoundDriver.h"


using namespace rl;
using namespace boost;
using namespace Ogre;

class SoundTest8 : public CppUnit::TestFixture {
private:

public:
    SoundTest8()
    {
    }
    
    void setUp()
    {
    }

    void tearDown()
    {
    }

    void test()
    {
		MultimediaSubsystem *mm = MultimediaSubsystem::getSingletonPtr();
		SoundDriver *driver = mm->getActiveDriver();
		
        Logger::getSingleton().log("SoundTest", Ogre::LML_NORMAL, "Starte Test #8");
        Logger::getSingleton().log("SoundTest", Ogre::LML_NORMAL, "Using Driver " + driver->getName());

        Logger::getSingleton().log("SoundTest", Ogre::LML_NORMAL, "Beende Test #8");
        CPPUNIT_ASSERT(true);
    }
 

    CPPUNIT_TEST_SUITE(SoundTest8);
    CPPUNIT_TEST(test);
    CPPUNIT_TEST_SUITE_END();
};
CPPUNIT_TEST_SUITE_REGISTRATION(SoundTest8);
