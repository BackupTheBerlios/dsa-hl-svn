/* This source file is part of Rastullahs Lockenpracht.
 * Copyright (C) 2003-2008 Team Pantheon. http://www.team-pantheon.de
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

#include "TimeSource.h"

#include "GameLoop.h"
#include "SaveGameManager.h"

template<> rl::TimeSourceManager* Ogre::Singleton<rl::TimeSourceManager>::ms_Singleton = 0;

namespace rl
{

    TimeSource::TimeSource(bool interruptable)
        : GameTask(interruptable)
    {
        GameLoop::getSingleton().addTask(this, GameLoop::TG_LOGIC);
    }

    TimeSource::~TimeSource()
    {
        GameLoop::getSingleton().removeTask(this);
    }

    RealTimeContinuous::RealTimeContinuous()
        : TimeSource(false),
        mTime(0.0)
    {
    }

    TimeSource::TimeSourceType RealTimeContinuous::getType() const
    {
        return TimeSource::REALTIME_CONTINUOUS;
    }

    Time RealTimeContinuous::getClock() const
    {
        return mTime;
    }

    void RealTimeContinuous::setClock(const Time& time)
    {
        mTime = time;
    }

    void RealTimeContinuous::setTimeFactor(Ogre::Real factor)
    {
        // can't be accelerated or slowed down
    }

    void RealTimeContinuous::run(Ogre::Real elapsedTime)
    {
        mTime += elapsedTime * 1000;
    }

    const Ogre::String& RealTimeContinuous::getName() const
    {
        static Ogre::String NAME = "Continuous real time clock";
        return NAME;
    }

    RealTimeInterruptable::RealTimeInterruptable()
        : TimeSource(true),
        mTimeFactor(1.0),
        mTime(0.0)
    {
    }

    RealTimeInterruptable::~RealTimeInterruptable()
    {
    }

    TimeSource::TimeSourceType RealTimeInterruptable::getType() const
    {
        return TimeSource::REALTIME_INTERRUPTABLE;
    }

    Time RealTimeInterruptable::getClock() const
    {
        return mTime;
    }

    void RealTimeInterruptable::setClock(const Time& time)
    {
        mTime = time;
    }

    void RealTimeInterruptable::setTimeFactor(Ogre::Real factor)
    {
        mTimeFactor = factor;
    }

    void RealTimeInterruptable::run(Ogre::Real elapsedTime)
    {
        if (!isPaused())
        {
            mTime += mTimeFactor * elapsedTime * 1000;
        }
    }

    const Ogre::String& RealTimeInterruptable::getName() const
    {
        static Ogre::String NAME = "Pausable real time clock";
        return NAME;
    }

    TimeSourceManager::TimeSourceManager()
    {
		SaveGameManager::getSingleton().registerSaveGameData(this);
    }

	TimeSourceManager::~TimeSourceManager()
	{
		SaveGameManager::getSingleton().unregisterSaveGameData(this);
	}

    void TimeSourceManager::registerTimeSource(TimeSource* ts)
    {
        mTimeSources[ts->getType()] = ts;
    }

    TimeSource* TimeSourceManager::getTimeSource(
        const TimeSource::TimeSourceType& type) const
    {
        std::map<TimeSource::TimeSourceType, TimeSource*>::const_iterator it
            = mTimeSources.find(type);

        if (it == mTimeSources.end())
        {
            return NULL;
        }

        return it->second;
    }

    void TimeSourceManager::setTimeFactor(const Ogre::Real& factor)
    {
        for (std::map<TimeSource::TimeSourceType, TimeSource*>::iterator it
            = mTimeSources.begin(); it != mTimeSources.end(); ++it)
        {
            it->second->setTimeFactor(factor);
        }
    }

    void TimeSourceManager::setPaused(bool paused)
    {
        for (std::map<TimeSource::TimeSourceType, TimeSource*>::iterator it
            = mTimeSources.begin(); it != mTimeSources.end(); ++it)
        {
            it->second->setPaused(paused);
        }
    }

	CeGuiString TimeSourceManager::getXmlNodeIdentifier() const
	{
		return "time_sources";
	}

	using namespace XERCES_CPP_NAMESPACE;

    void TimeSourceManager::writeData(SaveGameFileWriter* writer)
	{
		DOMElement* timesources = writer->appendChildElement(writer->getDocument(), 
			writer->getDocument()->getDocumentElement(), getXmlNodeIdentifier().c_str());

		for(std::map<TimeSource::TimeSourceType, TimeSource*>::const_iterator it_time_sources = mTimeSources.begin(); 
			it_time_sources != mTimeSources.end(); it_time_sources++)
        {
            DOMElement* timesource = writer->appendChildElement(writer->getDocument(), timesources, "time_source");
			writer->setAttributeValueAsInteger(timesource, "ID", it_time_sources->first);
			Property time((int)it_time_sources->second->getClock());

            PropertyMap map;
			map["time"] = time;
            writer->writeEachPropertyToElem(timesource, map);
        } 
	}

	void TimeSourceManager::readData(SaveGameFileReader* reader)
	{
		reader->initializeXml();

		DOMNodeList* rootNodeList = reader->getDocument()->getDocumentElement()->getElementsByTagName(AutoXMLCh(getXmlNodeIdentifier().c_str()).data());

		if(rootNodeList->getLength())
        {
			DOMNodeList* xmlTimeSources = static_cast<DOMElement*>(rootNodeList->item(0))->getElementsByTagName(AutoXMLCh("gameobject").data()); //there should be only one "gameobjects" node
            if(xmlTimeSources->getLength())
            {
				for(XMLSize_t childIdx = 0; childIdx < xmlTimeSources->getLength(); childIdx++)
				{
					DOMNode* xmlTimeSource = xmlTimeSources->item(childIdx);
					if(xmlTimeSource->getNodeType() == DOMNode::ELEMENT_NODE)
                    {
						TimeSource::TimeSourceType ID = (TimeSource::TimeSourceType)reader->getAttributeValueAsInteger(
							static_cast<DOMElement*>(xmlTimeSource), "ID");
                        PropertyRecord properties = reader->getPropertiesAsRecord(static_cast<DOMElement*>(xmlTimeSource));
	
						std::map<TimeSource::TimeSourceType, TimeSource*>::const_iterator it_time_sources = mTimeSources.find(ID);
						if(it_time_sources != mTimeSources.end())
							it_time_sources->second->setClock(properties.toPropertyMap()["time"].toInt());
                    }
				}
			}
		}

		reader->shutdownXml();
	}
    
    int TimeSourceManager::getPriority() const
	{
		return 10000;
	}
}
