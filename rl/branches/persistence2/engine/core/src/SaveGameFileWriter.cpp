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

#include "stdinc.h"

#include "SaveGameFileWriter.h"

#include "SaveGameManager.h"

#include <XmlProcessor.h>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMAttr.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <Properties.h>

#ifdef __APPLE__
#   include <CEGUI/CEGUIPropertyHelper.h>
#else
#   include <CEGUIPropertyHelper.h>
#endif

#include <CoreSubsystem.h>
#include <ContentModule.h>
#include <TimeSource.h>

#include <ctime>

using namespace XERCES_CPP_NAMESPACE;
using namespace Ogre;

namespace rl
{

	void SaveGameFileWriter::buildSaveGameFile(rl::SaveGameFile *file)
	{
		initializeXml();

		XMLCh tempStr[100];
        XMLString::transcode("LS", tempStr, 99);
        mImplementation = DOMImplementationRegistry::getDOMImplementation(tempStr);
		mTarget = file->getFormatTarget();
		mWriter = static_cast<DOMImplementationLS*>(mImplementation)->createDOMWriter();
		mDocument = static_cast<DOMImplementation*>(mImplementation)->createDocument(0, XMLString::transcode("SaveGameFile"), 0);

		 if (mWriter->canSetFeature(XMLUni::fgDOMWRTDiscardDefaultContent, true))
            mWriter->setFeature(XMLUni::fgDOMWRTDiscardDefaultContent, true);

        if (mWriter->canSetFeature(XMLUni::fgDOMWRTFormatPrettyPrint, true))
             mWriter->setFeature(XMLUni::fgDOMWRTFormatPrettyPrint, true);

		mDocument->setNodeValue(XMLString::transcode("SaveGameFile")); //Set name of document root node

		setAttributeValueAsString(mDocument->getDocumentElement(), "SaveGameFormatVersion", "0.8");
		setAttributeValueAsInteger(mDocument->getDocumentElement(), "Engineversion", CoreSubsystem::getSingleton().getEngineBuildNumber());

        DOMElement* header = appendChildElement(mDocument, mDocument->getDocumentElement(), "header");
        PropertyRecordPtr headerSet = file->getAllProperties();
        for (PropertyRecord::PropertyRecordMap::const_iterator it_header = headerSet->begin(); it_header != headerSet->end(); it_header++)
        {
            this->processProperty(header, PropertyEntry(it_header->first.c_str(), it_header->second));
        }


        mWriter->writeNode(mTarget, *mDocument);

        mWriter->release();

        delete mDocument;

		shutdownXml();
	}

    //void SaveGameFileWriter::buildSaveGameFile(SaveGameFile *file, const SaveGameDataOrderMap &map)
    //{
    //    //@toto: build
    //    initializeXml();

    //    XMLCh tempStr[100];
    //    XMLString::transcode("LS", tempStr, 99);
    //    mImplementation = DOMImplementationRegistry::getDOMImplementation(tempStr);
    //    mWriter = static_cast<DOMImplementationLS*>(mImplementation)->createDOMWriter();
    //    mTarget = file->getFormatTarget();
    //    mDocument = static_cast<DOMImplementation*>(mImplementation)->createDocument(0, XMLString::transcode("SaveGameFile"), 0);

    //    if (mWriter->canSetFeature(XMLUni::fgDOMWRTDiscardDefaultContent, true))
    //        mWriter->setFeature(XMLUni::fgDOMWRTDiscardDefaultContent, true);

    //    if (mWriter->canSetFeature(XMLUni::fgDOMWRTFormatPrettyPrint, true))
    //         mWriter->setFeature(XMLUni::fgDOMWRTFormatPrettyPrint, true);

    //    mDocument->setNodeValue(XMLString::transcode("SaveGameFile")); //Set name of document root node

    //    //Write SaveGameVersion
    //    setAttributeValueAsString(mDocument->getDocumentElement(), "SaveGameFormatVersion", "0.5");
    //    setAttributeValueAsInteger(mDocument->getDocumentElement(), "Engineversion", CoreSubsystem::getSingleton().getEngineBuildNumber());

    //    //Write modul of save game
    //    DOMElement* header = appendChildElement(mDocument, mDocument->getDocumentElement(), "header");
    //    
    //    PropertyRecordPtr headerSet = file->getAllProperties();
    //    for (PropertyRecord::PropertyRecordMap::const_iterator it_header = headerSet->begin(); it_header != headerSet->end(); it_header++)
    //    {
    //        this->processProperty(header, PropertyEntry(it_header->first.c_str(), it_header->second));
    //    }

    //    ////Write globals
    //    //DOMElement* globals = appendChildElement(mDocument, mDocument->getDocumentElement(), "globals");
    //    //DOMElement* gameTime = appendChildElement(mDocument, globals, "gametime");
    //    //TimeSource* gameTimeSource = TimeSourceManager::getSingleton().getTimeSource(TimeSource::GAMETIME);
    //    //setAttributeValueAsInteger(gameTime, "milliseconds", gameTimeSource->getClock());

    //    for(SaveGameDataOrderMap::const_iterator data_iter = map.begin(); data_iter != map.end(); data_iter++)
    //    {
    //        data_iter->second->writeData(this);
    //    }

    //    mWriter->writeNode(mTarget, *mDocument);


    //    mWriter->release();

    //    delete mDocument;
    //    delete mTarget;

    //    shutdownXml();
    //}

    //void SaveGameFileWriter::writeEachProperty(SaveGameData* data, const rl::PropertyMap &map)
    //{
    //    DOMElement* saveElem = appendChildElement(getDocument(), getDocument()->getDocumentElement(), data->getXmlNodeIdentifier().c_str());

    //    XmlPropertyWriter::writeEachPropertyToElem(saveElem, map);
    //}
}
