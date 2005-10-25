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

#ifndef __DotSceneLoader_H__
#define __DotSceneLoader_H__

#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMElement.hpp>

#include <OgreSceneNode.h>
#include <string>

namespace rl {

	/** Diese Klasse parst eine .scene.xml Datei.
	 *  
	 *  Zur Zeit sind folgenende Dinge (partiell) unterst�tzt 
	 *   - Nodes ( Name, Hierarchie + Platzierung + Skalierung + Rotation )
	 *   - Entities ( Name, TriMeshPhysik )
	 */
	class DotSceneLoader
	{
	public:
		/// Erstellt einen Dotscene Loader, der das gew�nschte File einliest
        DotSceneLoader(const std::string& filename, const std::string& resourceGroup);
		/// Standard Destruktor
		~DotSceneLoader() {};

		/// Hilfsmethode zur Generierung eines nopch nicht vergebenen Entitynamens
		static void staticGeometryAddSceneNodeWorkaround( Ogre::StaticGeometry* staticGeom , Ogre::SceneNode* baseNode );
	private:
		/// Laden der Szene
		void initializeScene();
		/// �ffnen der XML-Ressource
		XERCES_CPP_NAMESPACE::DOMDocument* openSceneFile();

		/// Geht alle Nodes in der .scene durch
		void processNodes(XERCES_CPP_NAMESPACE::DOMElement* rootNodesXml, Ogre::SceneNode* parentNode );
		/// Node und alle Unterelemente
		void processNode(XERCES_CPP_NAMESPACE::DOMElement* rootNodeXml, Ogre::SceneNode* parentNode );
		/// Eine Entity+Attribute
		void processEntity( XERCES_CPP_NAMESPACE::DOMElement* rootEntityXml, Ogre::SceneNode* parentNode );

		/** Liest einen Vector aus einem XML Element, �ber die Attribute x, y, z
		 *  Sollten die Attribute nicht korrekt definiert sein, gibt es Vector::ZERO zur�ck (0,0,0)
		 */
		Ogre::Vector3 processPosition( XERCES_CPP_NAMESPACE::DOMElement* rootPositionXml );

		/** Liest einen Vector aus einem XML Element, �ber die Attribute x, y, z
		*  Sollten die Attribute nicht korrekt definiert sein, gibt es Vector::UNIT_SCALE zur�ck (1,1,1)
		*/
		Ogre::Vector3 processScale( XERCES_CPP_NAMESPACE::DOMElement* rootPositionXml );
		/** Liest ein Quaternion aus einem XML Element, 
		*  �ber die Attribute qw, qx, qy, qz  ODER angle, axisX, axisY, axisZ
		*  Sollten die Attribute nicht korrekt definiert sein, gibt es Quaternion::IDENTITY zur�ck (1,0,0,0)
		*/
		Ogre::Quaternion processRotation( XERCES_CPP_NAMESPACE::DOMElement* rootQuatXml );

        std::string getRandomName(const std::string& baseName);

		/// Der Name der Scene
		const std::string mSceneName;
        /// ResourceGroup der dotscene-Resource
        const std::string mResourceGroup;
		/// Der SceneManager
		Ogre::SceneManager* const mSceneManager;
	};

}

#endif
