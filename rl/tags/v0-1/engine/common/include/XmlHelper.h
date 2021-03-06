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

#ifndef __XmlHelper_h__
#define __XmlHelper_h__

#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/util/XMLChar.hpp>
#include <xercesc/util/TransService.hpp>
#include <xercesc/sax2/Attributes.hpp>

#include "CommonPrerequisites.h"

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

/**
 * Diese Klasse bietet Helferfunktionen, um XML einfacher verarbeiten zu k�nnen
 * Alle Methoden sind statisch, es muss also keine Instanz dieser Klasse gebildet werden
 */
class _RlCommonExport XmlHelper
{
public:

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
	 * Ermittelt den Text eines Elementknotens
	 *
	 * @param element Das DOM-Element
	 * @return Text innerhalb der Element-Tags, als CeGuiString
	 */
	static CeGuiString getValueAsString(XERCES_CPP_NAMESPACE::DOMElement* element);
	
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
	 * Ermittelt den Text eines Elementknotens und parst ihn, 
	 * um eine Ganzzahl daraus zu machen
	 *
	 * @param element Das DOM-Element
	 * @return Zahl, die innerhalb der Element-Tags steht
	 */
	static int getValueAsInteger(XERCES_CPP_NAMESPACE::DOMElement* element);

	/**
	 * Ermittelt den Text eines DOMElement-Attributes und parst ihn, 
	 * um eine Ganzzahl daraus zu machen
	 *
	 * @param element Das DOM-Element
	 * @param name Name des Attributes
	 * @return Zahlenwert des Attributes
	 */
	static int getAttributeValueAsInteger(XERCES_CPP_NAMESPACE::DOMElement* element, const char* const name);
	
	/**
	 * Ermittelt den Text eines DOMElement-Attributes und konvertiert ihn zu einem CeGuiString
	 * 
	 * @param element Das DOM-Element
	 * @param name Name des Attributes
	 * @return Konvertierter Text als CeGuiString
	 */	
	static CeGuiString getAttributeValueAsString(XERCES_CPP_NAMESPACE::DOMElement* element, const char* const name);


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
	 * Ermittelt den Text eines DOMElement-Attributes und interpretier ihn als bool
	 * 
	 * @param element Das DOM-Element
	 * @param name Name des Attributes
	 * @return Konvertierter Text als bool
	 */	
	static bool getAttributeValueAsBool(XERCES_CPP_NAMESPACE::DOMElement* element, const char* const name);

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
	 * Konvertiert ein Xerces-XMLCh* in einen UTF-8-String
	 * Vor Benutzung muss initializeTranscoder() aufgerufen werden
	 *
	 * @param string16 Der zu konvertierende Xerces-XMLCh*
	 * @return Konvertierter Text als utf8* (muss selbst gel�scht werden)
	 * @see initializeTranscoder()
	 */

	static utf8* transcodeToUtf8(const XMLCh* const string16);

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

private:
	static XMLTranscoder* sTranscoder;
	static XMLTransService::Codes sFailCode;

};

}

#endif
