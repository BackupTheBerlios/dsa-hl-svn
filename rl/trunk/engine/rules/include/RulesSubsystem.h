#ifndef __RULESSUBSYSTEM_H__
#define __RULESSUBSYSTEM_H__

#include "RulesPrerequisites.h"

#include <string>
#include <CEGUIString.h>

using namespace Ogre;

namespace rl
{
    class _RlRulesExport RulesSubsystem : public Singleton<RulesSubsystem>
    {
    public:
        RulesSubsystem();
        ~RulesSubsystem();
	
        void log(const char *msg);
		void log(const std::string& msg);
		void log(const CeGuiString& msg);
	

        static RulesSubsystem& getSingleton(void);
        static RulesSubsystem* getSingletonPtr(void);
	private:
		Ogre::Log* mLog;
    };
}

#endif
