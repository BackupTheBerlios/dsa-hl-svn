/* This source file is part of Rastullahs Lockenpracht.
 * Copyright (C) 2003-2007 Team Pantheon. http://www.team-pantheon.de
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

#ifndef __XmlHelper_h__
#define __XmlHelper_h__

#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/util/XMLChar.hpp>
#include <xercesc/util/TransService.hpp>
#include <xercesc/sax2/Attributes.hpp>

#include "CommonPrerequisites.h"
#include "XmlErrorHandler.h"

// Da sollten wir uns auf etwas einigen
// So ist das laestig.
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#   pragma warning (push)
#   pragma warning (disable : 4267)
#   include <CEGUIString.h>
#   pragma warning (pop)
#else
#   include <CEGUI/CEGUIString.h>
#endif
using XERCES_CPP_NAMESPACE::XMLTranscoder;
using XERCES_CPP_NAMESPACE::XMLTransService;
using XERCES_CPP_NAMESPACE::Attributes;
using CEGUI::utf8;
using CEGUI::String;

namespace rl {

    class XmlErrorHandler;
/**
 * Diese Klasse bietet Helferfunktionen, um XML einfacher verarbeiten zu k�nnen
 * Alle Methoden sind statisch, es muss also keine Instanz dieser Klasse gebildet werden
 */
class _RlCommonExport XmlHelper
{
public:
    
    /**
	 * Erzeugt einen Kindknoten eines DOM-Elements, das einen bestimmten Namen hat, 
	 * dieser ist wiederum ein Element-Knoten
	 *
     * @param doc Das DOM-Document
	 * @param parent Knoten, dessen Kinder gesucht sind
	 * @param name Name des gesuchten Kindknotens von parent
	 *
	 * @return Der Kindknoten
	 */
    static XERCES_CPP_NAMESPACE::DOMElement* appendChildElement(XERCES_CPP_NAMESPACE::DOMDocument* doc, 
        XERCES_CPP_NAMESPACE::DOMElement* element, const char* const name);

    /**
	 * Ermittelt einen Kindknoten eines DOM-Elements, das einen bestimmten Namen hat, 
	 * dieser ist wiederum ein Element-Knoten
	 *
	 * @param parent Knoten, dessen Kinder gesucht sind
	 * @param name Name des gesuchten Kindknotens von parent
	 *
	 * @return Der Kindknoten
	 */
	static XERCES_CPP_NAMESPACE::DOMElement* getChildNamed(XERCES_CPP_NAMESPACE::DOMElement* parent, const char* const name);
	
    /**
	 * Setzt den Text eines Elementknotens
	 *
	 * @param element Das DOM-Element
     * @param string Der zu setzende String
	 * @return Das DOM-Element
	 */
    static XERCES_CPP_NAMESPACE::DOMElement* setValueAsString(XERCES_CPP_NAMESPACE::DOMDocument* doc, XERCES_CPP_NAMESPACE::DOMElement* element, const CeGuiString &value);
    
    /**
	 * Ermittelt den Text eines Elementknotens
	 *
	 * @param element Das DOM-Element
	 * @return Text innerhalb der Element-Tags, als CeGuiString
	 */
	static CeGuiString getValueAsString(XERCES_CPP_NAMESPACE::DOMElement* element);
	
    /**
	 * Setzt den Text eines Elementknotens, als UTF-8
	 * vor Benutzung muss initializeTranscoder() aufgerufen werden
	 *
	 * @param element Das DOM-Element
     * @param utf Der zu setzende UTF String
	 * @return Das DOM-Element
	 */
	static XERCES_CPP_NAMESPACE::DOMElement* setValueAsUtf8(XERCES_CPP_NAMESPACE::DOMDocument* doc, XERCES_CPP_NAMESPACE::DOMElement* element, utf8* value);
    
    /**
	 * Ermittelt den Text eines Elementknotens, als UTF-8
	 * vor Benutzung muss initializeTranscoder() aufgerufen werden
	 *
	 * @param element Das DOM-Element
	 * @return Text innerhalb der Element-Tags, als utf8* (muss selbst gel�scht werden)
	 * @see initializeTranscoder()
	 */
	static utf8* getValueAsUtf8(XERCES_CPP_NAMESPACE::DOMElement* element);

	/**
	 * Setzt den Text eines Elementknotens und parst ihn, 
	 * um eine Ganzzahl daraus zu machen
	 *
	 * @param element Das DOM-Element
     * @param value Der zu setzende Integer
	 * @return Das DOM-Element
	 */
	static XERCES_CPP_NAMESPACE::DOMElement* setValueAsInteger(XERCES_CPP_NAMESPACE::DOMDocument* doc, XERCES_CPP_NAMESPACE::DOMElement* element, int value);

    
    /**
	 * Ermittelt den Text eines Elementknotens und parst ihn, 
	 * um eine Ganzzahl daraus zu machen
	 *
	 * @param element Das DOM-Element
	 * @return Zahl, die innerhalb der Element-Tags steht
	 */
	static int getValueAsInteger(XERCES_CPP_NAMESPACE::DOMElement* element);

	
    /**
     * Convert a vector3 to an xml tag
	 *
	 * @param element The DOM element
     * @param value The vector input
	 * @return The DOM element
	 */
    static XERCES_CPP_NAMESPACE::DOMElement* setValueAsVector3(XERCES_CPP_NAMESPACE::DOMDocument* doc, XERCES_CPP_NAMESPACE::DOMElement* element, Ogre::Vector3 value);

    /**
	 * Processes an element node of shape <any_node_name x="0" y="0" z="0"/>
	 * into a three component Ogre vector
	 *
	 * @param element The DOM element
	 * @return the vector
	 */
	static Ogre::Vector3 getValueAsVector3(XERCES_CPP_NAMESPACE::DOMElement* element);

    /**
     * Convert a quaternion to an xml tag
	 *
	 * @param element The DOM element
     * @param value The quaternion input
	 * @return The DOM element
	 */
    static XERCES_CPP_NAMESPACE::DOMElement* setValueAsQuaternion(XERCES_CPP_NAMESPACE::DOMDocument* doc, XERCES_CPP_NAMESPACE::DOMElement* element, Ogre::Quaternion value);

    /**
	 * Processes an element node of shape <any_node_name x="0" y="0" z="0" w="1"/>
	 * into a three component Ogre vector
	 *
	 * @param element The DOM element
	 * @return the quaternion
	 */
    static Ogre::Quaternion getValueAsQuaternion(XERCES_CPP_NAMESPACE::DOMElement* element);

	
    /**
	 * Ermittelt, ob ein DOMElement ein bestimmtes Attribut bestitzt
	 *
	 * @param element Das DOM-Element
	 * @param name Name des Attributes
	 * @return <code>true</code>, wenn das Attribut an diesem Element existiert, sonst <code>false</code>
	 */
	static bool hasAttribute(XERCES_CPP_NAMESPACE::DOMElement* element, const char* const name);

    /**
	 * Setzt den Wert von einem Attribut eines Elementes
	 *
	 * @param element Das DOM-Element
	 * @param name Name des Attributes
     * @param value Wert der gesetzt werden muss
	 * @return <code>true</code>, wenn das Attribut an diesem Element existiert, sonst <code>false</code>
	 */
    static void setAttribute(XERCES_CPP_NAMESPACE::DOMElement* element, const char* const name, const char* const value);

    /**
	 * Checks whether a DOMNode has a certain name
	 *
	 * @param node the DOM node
	 * @param name the name to check
	 * @return <code>true</code>, if the DOM node the name <code>name</code>, <code>false</code> otherwise
	 */
	static bool hasNodeName(XERCES_CPP_NAMESPACE::DOMNode* node, const char* const name);

    
    /**
	 * Ermittelt den Text eines DOMElement-Attributes und parst ihn, 
	 * um eine Ganzzahl daraus zu machen
	 *
	 * @param element Das DOM-Element
	 * @param name Name des Attributes
	 * @return Zahlenwert des Attributes
	 */
	static XERCES_CPP_NAMESPACE::DOMElement* setAttributeValueAsInteger(XERCES_CPP_NAMESPACE::DOMElement* element, const char* const name, int value);

    /**
	 * Ermittelt den Text eines DOMElement-Attributes und parst ihn, 
	 * um eine Ganzzahl daraus zu machen
	 *
	 * @param element Das DOM-Element
	 * @param name Name des Attributes
     * @param value das zu setzende Integer
	 * @return Das DOM-Element
	 */
	static int getAttributeValueAsInteger(XERCES_CPP_NAMESPACE::DOMElement* element, const char* const name);
	
	/**
	 * Setzt den Text eines DOMElement-Attributes
	 * 
	 * @param element Das DOM-Element
	 * @param name Name des Attributes
	 * @param value das zu setzende CeGuiString
	 * @return Das DOM-Element
	 */	
	static XERCES_CPP_NAMESPACE::DOMElement* setAttributeValueAsString(XERCES_CPP_NAMESPACE::DOMElement* element, const char* const name, const CeGuiString &value);
    
    /**
	 * Ermittelt den Text eines DOMElement-Attributes und konvertiert ihn zu einem CeGuiString
	 * 
	 * @param element Das DOM-Element
	 * @param name Name des Attributes
	 * @return Konvertierter Text als CeGuiString
	 */	
	static CeGuiString getAttributeValueAsString(XERCES_CPP_NAMESPACE::DOMElement* element, const char* const name);

    /**
	 * Setzt den Text eines DOMElement-Attributes
	 * 
	 * @param element Das DOM-Element
	 * @param name Name des Attributes
	 * @param value der zu setzende std::string
	 * @return Das DOM-Element
	 */	
    static XERCES_CPP_NAMESPACE::DOMElement* setAttributeValueAsStdString(XERCES_CPP_NAMESPACE::DOMElement* element, const char* const name, const std::string &value);

	/**
	 * Ermittelt den Text eines DOMElement-Attributes und konvertiert ihn zu einem std::string
	 * 
	 * @param element Das DOM-Element
	 * @param name Name des Attributes
	 * @return Konvertierter Text als std::string
	 */	
    static std::string getAttributeValueAsStdString(XERCES_CPP_NAMESPACE::DOMElement* element, const char* const name);
	
    /**
	 * Ermittelt den Text eines SAX Element-Attributes und konvertiert ihn zu einem CeGuiString
	 *
	 * @param element Liste aller Attribute des aktuellen Elementes
	 * @param name Name des Attributes
	 * @return Konvertierter Text als CeGuiString
	 */
	static CeGuiString getAttributeValueAsString(const XERCES_CPP_NAMESPACE::Attributes& attributes, const char* const name);

    /**
	 * Setzt den Text eines DOMElement-Attributes
	 * 
	 * @param element Das DOM-Element
	 * @param name Name des Attributes
	 * @param value der zu setzende boolean
	 * @return Das DOM-Element
	 */	
	static XERCES_CPP_NAMESPACE::DOMElement* setAttributeValueAsBool(XERCES_CPP_NAMESPACE::DOMElement* element, const char* const name, bool value);

	/**
	 * Ermittelt den Text eines DOMElement-Attributes und interpretier ihn als bool
	 * 
	 * @param element Das DOM-Element
	 * @param name Name des Attributes
	 * @return Konvertierter Text als bool
	 */	
	static bool getAttributeValueAsBool(XERCES_CPP_NAMESPACE::DOMElement* element, const char* const name);

    /**
	 * Setzt den Text eines DOMElement-Attributes
	 * 
	 * @param element Das DOM-Element
	 * @param name Name des Attributes
	 * @param value der zu setzende Real Wert
	 * @return Das DOM-Element
	 */	
    static XERCES_CPP_NAMESPACE::DOMElement* setAttributeValueAsReal(XERCES_CPP_NAMESPACE::DOMElement* element, const char* const name, Ogre::Real value);

	/**
	 * Ermittelt den Text eines DOMElement-Attributes und konvertiert ihn zu Ogre::Real
	 * 
	 * @param element Das DOM-Element
	 * @param name Name des Attributes
	 * @return Konvertierter Text als Ogre::Real
	 */	
	static Ogre::Real getAttributeValueAsReal(XERCES_CPP_NAMESPACE::DOMElement* element, const char* const name);


	/**
	 * Initialisiert den XML<->UTF-8 Transcoder, 
	 * davor muss XMLPlatformUtils::Initialize() aufgerufen worden sein
	 */
	static void initializeTranscoder();

	/**
	 * Konvertiert einen Xerces-XMLCh* in eine CeGuiString
	 * Intern wird eine Konvertierung in utf8* vorgenommen,
	 * wobei man sich hier um das nachtr�gliche aufr�umen 
	 * nicht mehr zu k�mmern braucht.
	 * 
	 * @param string16 Der zu konvertierende Xerces-XMLCh*
	 * @return Konvertierter Text als CeGuiString
	 * @see transcodeToUtf8(const XMLCh* const string16);
	 */
	static CeGuiString transcodeToString(const XMLCh* const string16);

	static std::string transcodeToStdString(const XMLCh* const string16);

    static XmlErrorHandler* getErrorHandler();

private:
	static XMLTranscoder* sTranscoder;
	static XMLTransService::Codes sFailCode;

    static XmlErrorHandler sErrorHandler;

	/**
	 * Konvertiert ein Xerces-XMLCh* in einen UTF-8-String
	 * Vor Benutzung muss initializeTranscoder() aufgerufen werden
	 *
	 * @param string16 Der zu konvertierende Xerces-XMLCh*
	 * @return Konvertierter Text als utf8* (muss selbst gel�scht werden)
	 * @see initializeTranscoder()
	 */
	static utf8* transcodeToUtf8(const XMLCh* const string16);
};

class _RlCommonExport AutoXMLCh
{
public:
    AutoXMLCh() : mData(0) { }

    AutoXMLCh(const char* rhs) : mData(0)
    {
        mData = XERCES_CPP_NAMESPACE::XMLString::transcode(rhs);
    }

    AutoXMLCh(const AutoXMLCh& rhs) : mData(0)
    {
        mData = XERCES_CPP_NAMESPACE::XMLString::replicate(rhs.mData);
    }

    ~AutoXMLCh()
    {
        XERCES_CPP_NAMESPACE::XMLString::release(&mData);
    }

    const AutoXMLCh& operator=(const AutoXMLCh& rhs)
    {
        if (mData != NULL)
        {
            XERCES_CPP_NAMESPACE::XMLString::release(&mData);
        }
        mData = XERCES_CPP_NAMESPACE::XMLString::replicate(rhs.mData);
        return *this;
    }

    const AutoXMLCh& operator=(const char* rhs)
    {
        if (mData != NULL)
        {
            XERCES_CPP_NAMESPACE::XMLString::release(&mData);
        }
        mData = XERCES_CPP_NAMESPACE::XMLString::transcode(rhs);
        return *this;
    }

    XMLCh* data() const { return mData; }
private:
    XMLCh* mData;
};

class _RlCommonExport AutoChar
{
public:
    AutoChar() : mData(0) { }

    AutoChar(const XMLCh* str) : mData(0)
    {
        mData = XERCES_CPP_NAMESPACE::XMLString::transcode(str);
    }

    AutoChar(const AutoChar& rhs) : mData(0)
    {
        mData = XERCES_CPP_NAMESPACE::XMLString::replicate(rhs.mData);
    }

    ~AutoChar()
    {
        XERCES_CPP_NAMESPACE::XMLString::release(&mData);
    }

    const AutoChar& operator=(const AutoChar& rhs)
    {
        XERCES_CPP_NAMESPACE::XMLString::release(&mData);
        mData = XERCES_CPP_NAMESPACE::XMLString::replicate(rhs.mData);
        return *this;
    }

    const AutoChar& operator=(XMLCh* rhs)
    {
        if (mData != NULL)
        {
            XERCES_CPP_NAMESPACE::XMLString::release(&mData);
        }
        mData = XERCES_CPP_NAMESPACE::XMLString::transcode(rhs);
        return *this;
    }

    char* data() const { return mData; }
private:
    char* mData;
};

}

#endif
