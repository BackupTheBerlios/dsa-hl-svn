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

#ifndef __PropertyReader_H__
#define __PropertyReader_H__

#include "CommonPrerequisites.h"

#include <tinyxml.h>

#include "Properties.h"
#include "XmlProcessor.h"

namespace rl 
{
    class Property;
    class PropertyRecord;

    typedef std::pair<Ogre::String, Property> PropertyEntry;

    class _RlCommonExport XmlPropertyReader
         : public XmlProcessor, public TiXmlVisitor
    {
    public:
        XmlPropertyReader();
        virtual ~XmlPropertyReader();

        void parseGameObjectFile(Ogre::DataStreamPtr &stream, const Ogre::String &groupName);
        PropertyRecordVector getPropertyRecords();

        PropertyEntry processProperty(const TiXmlElement* domElem) const;
		PropertyEntry processProperty(const TiXmlAttribute* domAttr) const;

        virtual PropertyRecordPtr getPropertiesAsRecord(const TiXmlElement* parent);
        virtual bool VisitEnter(const TiXmlElement &element, const TiXmlAttribute *firstAttribute);
    private:
        PropertyRecordVector mPropertyRecords;

        Property getProperty(const Ogre::String& key, const CeGuiString& value);
        void processGameObjectClassNode(const TiXmlElement &element);
    };
} // namespace rl

#endif //__PropertyReader_H__
