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

#include <xercesc/dom/DOM.hpp>

#include "SaveGameFileReader.h"
#include "SaveGameManager.h"
#include "SaveGameData.h"

#include "CoreSubsystem.h"

using namespace XERCES_CPP_NAMESPACE;

namespace rl
{
    SaveGameFileReader::SaveGameFileReader()
    {
    }

    void SaveGameFileReader::parseSaveGameFile(SaveGameFile* file, const SaveGameDataOrderMap &map)
    {
        initializeXml();

        mDocument = loadDocument(file->getDataStream());

        int version = getAttributeValueAsInteger(mDocument->getDocumentElement(), "Engineversion");
        if(version >= CoreSubsystem::getSingleton().getEngineBuildNumber())
            LOG_MESSAGE(Logger::CORE, "Loading save game: Engine version is ok");
        else
            LOG_ERROR(Logger::CORE, "Loading save game: Save game version is newer then engine version! Loading save game could crash");

        for(SaveGameDataOrderMap::const_reverse_iterator data_iter = map.rbegin(); data_iter != map.rend(); data_iter++)
        {
            data_iter->second->readData(this);
        }


        file->closeDataStream(); //make the save game writable

        mDocument = NULL;

        shutdownXml();
    }

    void SaveGameFileReader::parseSaveGameFileHeader(Ogre::DataStreamPtr &stream, const Ogre::String &groupName, SaveGameFile* file)
    {
        initializeXml();

        if(stream->size())
        {
            DOMDocument* doc = loadDocument(stream);

             DOMNodeList* headerDefsXml = doc->getDocumentElement()->getElementsByTagName(AutoXMLCh("header").data());
             if(headerDefsXml->getLength())
             {
                 DOMElement* elem = static_cast<DOMElement*>(headerDefsXml->item(0));
                 /*DOMNodeList* headerDefChildren = elem->getChildNodes();
                 for(XMLSize_t childIdx = 0; childIdx < headerDefChildren->getLength(); childIdx++)
                 {
                     DOMNode* curChild = headerDefChildren->item(childIdx);
                     if (curChild->getNodeType() == DOMNode::ELEMENT_NODE)
                     {
                         PropertyEntry entry = processProperty(static_cast<DOMElement*>(curChild));
                         if(entry.first != "")
                         {
                            file->setProperty(entry.first, entry.second);
                         }
                     }
                 }*/
                 PropertyRecordPtr set = getPropertiesAsRecord(elem);
                 file->setProperties(set);
             }
        }

        shutdownXml();
    }

    PropertyRecordPtr SaveGameFileReader::getAllPropertiesAsRecord(SaveGameData* data)
    {
        PropertyRecordPtr properties(new PropertyRecord());

        initializeXml();

        DOMNodeList* rootNodeList = getDocument()->getDocumentElement()->getElementsByTagName(AutoXMLCh(data->getXmlNodeIdentifier().c_str()).data());

        if (rootNodeList->getLength())
        {
            DOMNode* xmlPropElem = rootNodeList->item(0);
            properties = getPropertiesAsRecord(static_cast<DOMElement*>(xmlPropElem));
        }

        shutdownXml();

        return properties;
    }
}
