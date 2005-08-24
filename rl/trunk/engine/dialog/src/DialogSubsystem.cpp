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
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include "AimlProcessorManager.h"
#include "DialogSubsystem.h"

#include "XmlHelper.h"
#include "XmlResourceManager.h"
#include "Logger.h"

using namespace Ogre;

template<> rl::DialogSubsystem* Singleton<rl::DialogSubsystem>::ms_Singleton = 0;

namespace rl 
{
	DialogSubsystem& DialogSubsystem::getSingleton(void)
	{
		return Singleton<DialogSubsystem>::getSingleton();
	}

	DialogSubsystem* DialogSubsystem::getSingletonPtr(void)
	{
		return Singleton<DialogSubsystem>::getSingletonPtr();
	}

	DialogSubsystem::DialogSubsystem()
	{
		AimlProcessorManager::addStandardProcessors();
	//  Initialize Xerces if this wasn't done already
		try 
		{
            XMLPlatformUtils::Initialize();
			XmlHelper::initializeTranscoder();
        }
        catch (const XMLException& exc) 
		{
			char* excmsg = XMLString::transcode(exc.getMessage());
			std::string excs="Exception while initializing Xerces: ";
			excs+=excmsg;
			log(Ogre::LML_TRIVIAL, excs);
            XMLString::release(&excmsg);
        }

	//  Load dialog-startup definition with bot properties etc.pp.
	//	if(XmlResourceManager::getSingleton().getByName("dialog-startup.xml")==NULL)
	//			XmlResourceManager::getSingleton().create("dialog-startup.xml");
	//	???	what is this for? should bots be preloaded? i guess not! 
	//  or do we need it for aiml preloading?

	}

    DialogSubsystem::~DialogSubsystem() 
    {  

    }

	void DialogSubsystem::log(const Ogre::LogMessageLevel level, const Ogre::String& msg, const Ogre::String& ident)
	{
		Logger::getSingleton().log(level, "Dialog", msg, ident);
	}
}
