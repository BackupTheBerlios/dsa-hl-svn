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

#include <xercesc/framework/LocalFileInputSource.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>

#include "DotSceneLoader.h"
#include "World.h"

#include "PhysicsManager.h"
#include "ActorManager.h"
#include "CoreSubsystem.h"
#include "ConfigurationManager.h"

#include "XmlHelper.h"
#include "XmlResourceManager.h"

#include "Exception.h"

#include <cstdlib>
#include <ctime>

using namespace XERCES_CPP_NAMESPACE;
using namespace std;

namespace rl {
	using XERCES_CPP_NAMESPACE::DOMDocument; //XXX: Warum brauche ich das unter VS 2003?

    DotSceneLoader::DotSceneLoader(const string& filename, const string& resourceGroup)
        : mSceneName(filename),
          mResourceGroup(resourceGroup),
          mSceneManager(CoreSubsystem::getSingleton().getWorld()->getSceneManager()),
		  mStaticNodes(),
		  mRenderingDistance( ActorManager::getSingleton().getDefaultActorRenderingDistance() ),
		  mStaticgeomRenderingDistances()
	{
        srand(static_cast<unsigned int>(time(NULL)));

		initializeScene();
	}

	DotSceneLoader::~DotSceneLoader()
	{
        XmlResourceManager::getSingleton().unload(mSceneName);
	}

	void DotSceneLoader::initializeScene()
	{
		XMLPlatformUtils::Initialize();
		XmlHelper::initializeTranscoder();

		Logger::getSingleton().log(Logger::CORE, Ogre::LML_TRIVIAL, "Lade Szenenbeschreibung aus " + mSceneName );
		DOMDocument* doc = openSceneFile();

		// Durch alle Unterelemente iterieren
		DOMNode* child = doc->getDocumentElement()->getFirstChild();

		

		while( child != NULL )
		{
			// Ein Node
			XMLCh* nodeTag = XMLString::transcode("userData");
			if( XMLString::compareIString(child->getNodeName(), nodeTag) == 0  )
			{
				processSceneUserData( reinterpret_cast<DOMElement*>(child) );
			}
			XMLString::release(&nodeTag);
			child = child->getNextSibling();
		} 

		Logger::getSingleton().log(Logger::CORE, Ogre::LML_TRIVIAL, " Beginne parsen der Unterelemente" );
		DOMElement* nodes = XmlHelper::getChildNamed(doc->getDocumentElement(), "nodes");

		// Eine .scene wird in einem SceneNode mit dem Namen der .scene befestigt
		mSceneNode = mSceneManager->getRootSceneNode()->createChildSceneNode(mSceneName);
		processNodes( nodes, mSceneNode );


        if( ConfigurationManager::getSingleton().shouldUseStaticGeometry() )
        {    
			for( std::map<int,Ogre::SceneNode*>::iterator it = mStaticNodes.begin();
				it != mStaticNodes.end(); ) 
			{
				Ogre::SceneNode* staticNode = it->second;
				string staticName = Ogre::StringConverter::toString(it->first);
				Ogre::Real renderDist = mRenderingDistance;

				if( mStaticgeomRenderingDistances.find( it->first ) != mStaticgeomRenderingDistances.end() )
					renderDist = mStaticgeomRenderingDistances[it->first];

				StaticGeometry* staticGeom = mSceneManager->createStaticGeometry( mSceneName + staticName  );
				
				staticGeom->setRenderingDistance( renderDist );
				staticGeom->addSceneNode( staticNode );
				staticGeom->setRegionDimensions( 1000 * Vector3::UNIT_SCALE );
				// Statische Geometrie bauen
				staticGeom->build();
				// Nicht mehr den Original-Knoten rendern, da dieser noch erhalten ist.			
				staticNode->setVisible( false );
				staticNode->removeAndDestroyAllChildren();
				mSceneManager->destroySceneNode( staticNode->getName() );				
				staticNode = NULL;
				Logger::getSingleton().log(Logger::CORE, Ogre::LML_TRIVIAL, " Statische Geometrie "+staticName+" erstellt" );
				++it;
			}
        }
        else
            Logger::getSingleton().log(Logger::CORE, Ogre::LML_TRIVIAL, " Keine statischen Geometrien erstellt" );

		doc->release();
		XMLPlatformUtils::Terminate();		
		Logger::getSingleton().log(Logger::CORE, Ogre::LML_TRIVIAL, "Szenenbeschreibung aus " + mSceneName +" fertig geparst" );
	}

	DOMDocument* DotSceneLoader::openSceneFile( )
	{

		XercesDOMParser* parser = new XercesDOMParser();
        parser->setValidationScheme(XercesDOMParser::Val_Always);
        parser->setDoNamespaces(true);
		
		XmlPtr res = 
			XmlResourceManager::getSingleton().create(
			mSceneName, 
			mResourceGroup);
		res.getPointer()->parseBy(parser);
		return parser->getDocument();
	}

	// Iteriert durch die einzelnen Nodes
	void DotSceneLoader::processNodes(DOMElement* rootNodesXml, 
		Ogre::SceneNode* parentNode )
	{
		if ( rootNodesXml == NULL )
			return;
		if ( parentNode == NULL )
			Throw( NullPointerException, "parentNode darf nicht null sein" );
	

		// Durch alle Unterelemente iterieren
		DOMNode* child = rootNodesXml->getFirstChild();

		while( child != NULL )
		{
			// Ein Node
			XMLCh* nodeTag = XMLString::transcode("node");
			if( XMLString::compareIString(child->getNodeName(), nodeTag) == 0  )
			{
				processNode( reinterpret_cast<DOMElement*>(child) , parentNode, NULL );
			}
			XMLString::release(&nodeTag);
			child = child->getNextSibling();
		} 
	}

	// Befasst sich mit einem Node
	void DotSceneLoader::processNode(DOMElement* rootNodeXml, Ogre::SceneNode* parentNode, NodeUserData* parentUserData)
	{
		if ( rootNodeXml == NULL )
			return;
		if ( parentNode == NULL )
			Throw( NullPointerException, "parentNode darf nicht null sein" );

		string nodeName = XmlHelper::getAttributeValueAsStdString( rootNodeXml, 
			"name" );
		
		Ogre::SceneNode* newNode;
		// Wurde dem Node ein Name zugewiesen?
		if( nodeName.length() > 0 )
        {
            try
            {
                // Dann versuche einen Knoten mit dem Namen zu erstellen
                newNode = parentNode->createChildSceneNode(nodeName);
            }
            catch( Ogre::Exception& e )
            {
                // Name schon vergeben?
                if (e.getNumber() == Ogre::Exception::ERR_DUPLICATE_ITEM)
                {
                    newNode = parentNode->createChildSceneNode();
                    Logger::getSingleton().log(Logger::CORE, Ogre::LML_NORMAL, 
                        " NodeName '"+nodeName+"' war schon vergeben! Es wurde der Name '"+newNode->getName()+"' benutzt." );
                }
                else
                {
                    // Andere Exception-Ursache - weiterwerfen.
                    throw e;
                }
            }
        }
		else
        {
            newNode = parentNode->createChildSceneNode();
        }	

		Logger::getSingleton().log(Logger::CORE, Ogre::LML_TRIVIAL, 
            " Node '"+newNode->getName()+"' als Unterknoten von '"+parentNode->getName()+"' erstellt." );		
		
        DOMNode* child = rootNodeXml->getFirstChild();
        NodeUserData userData;
        // Defaults einstellen
		if( parentUserData == NULL || !parentUserData->is_inheriting )
		{
			userData.is_dynamic = false;
			userData.is_inheriting = false;
			userData.physical_body = "mesh";
			userData.staticgeom_group = -1;
			userData.renderingdistance = mRenderingDistance;
		}
		// Vom Vorg�nger erben
		else
		{
			userData.is_dynamic = parentUserData->is_dynamic;
			userData.is_inheriting = parentUserData->is_inheriting;
			userData.physical_body = parentUserData->physical_body;
			userData.staticgeom_group = parentUserData->staticgeom_group;
			userData.renderingdistance = parentUserData->renderingdistance;
		}

        // Durch alle Unterelemente iterieren, um die userDatas zu finden
        while( child != NULL )
        {
            // Ein selbstdefinierter Bereich
            if( XMLString::compareIString(child->getNodeName(), 
                XMLString::transcode("userData") ) == 0 )
                processNodeUserData( reinterpret_cast<DOMElement*>(child) , &userData );

            child = child->getNextSibling();
        } 

		if( userData.staticgeom_group > -1 )
		{
			newNode->getParentSceneNode()->removeChild( newNode );

			// Existiert noch nicht
			if( mStaticNodes.find( userData.staticgeom_group ) == mStaticNodes.end() )
			{
				mStaticNodes[userData.staticgeom_group] = 
					mSceneManager->getRootSceneNode()->createChildSceneNode(
						mSceneName+"_static_"+Ogre::StringConverter::toString(userData.staticgeom_group ) );
			}

			mStaticNodes[userData.staticgeom_group]->addChild( newNode );
		}

        // Muss f�r Meshes in diesem Node ein TriMeshBody erstellt werden?
        bool createMeshPhysicalBody = userData.physical_body.compare("mesh") == 0;

        child = rootNodeXml->getFirstChild();
        // Durch alle Unterelemente iterieren
		while( child != NULL )
		{
			// Ein Node
			if( XMLString::compareIString(child->getNodeName(), 
				XMLString::transcode("node") ) == 0 )
				processNode( reinterpret_cast<DOMElement*>(child) , newNode, &userData );
			// Position des Nodes
			else if( XMLString::compareIString(child->getNodeName(), 
				XMLString::transcode("position") ) == 0  )
				newNode->setPosition( processPosition( reinterpret_cast<DOMElement*>(child) ) );
			// Rotation des Nodes
			else if( XMLString::compareIString(child->getNodeName(), 
				XMLString::transcode("rotation") ) == 0  )
				newNode->setOrientation( processRotation( reinterpret_cast<DOMElement*>(child) ) );
			// Skalierung des Nodes
			else if( XMLString::compareIString(child->getNodeName(), 
				XMLString::transcode("scale") ) == 0  )
				newNode->setScale( processScale( reinterpret_cast<DOMElement*>(child) ) );
			// Eine Entity
			else if( XMLString::compareIString(child->getNodeName(), 
				XMLString::transcode("entity") ) == 0  )                
				processEntity( reinterpret_cast<DOMElement*>(child), newNode, 
					createMeshPhysicalBody, userData.renderingdistance );

			child = child->getNextSibling();
		} 
	}

	void DotSceneLoader::processSceneUserData( XERCES_CPP_NAMESPACE::DOMElement* rootUserDataXml )
	{
		DOMNode* child = rootUserDataXml->getFirstChild();
        Logger::getSingleton().log(Logger::CORE, Ogre::LML_TRIVIAL, " SceneUserData gefunden");

        // Durch alle Unterelemente iterieren, um die properties zu finden
        while( child != NULL )
        {
            // Ein selbstdefinierter Bereich
            if( XMLString::compareIString(child->getNodeName(), 
                XMLString::transcode("property") ) == 0 )
            {
                DOMElement* propertyXml = reinterpret_cast<DOMElement*>(child);
                string propertyName = XmlHelper::getAttributeValueAsStdString( propertyXml, 
			        "name" );

                try
                {
					if( propertyName.compare("default_renderingdistance") == 0 )
						mRenderingDistance = XmlHelper::getAttributeValueAsReal( 
                            propertyXml, "data" );
					else if( Ogre::StringUtil::startsWith(propertyName,"staticgeom_renderingdistance_") )
					{
						int groupId = Ogre::StringConverter::parseInt( propertyName.substr( 29 ) );
						
						mStaticgeomRenderingDistances[groupId] = XmlHelper::getAttributeValueAsReal( 
                            propertyXml, "data" );
					}
                }
                catch(...)
                {
                    Logger::getSingleton().log(Logger::CORE, Ogre::LML_TRIVIAL, 
                        " > Parse Error beim �bernehmen der Property '"+propertyName+"'!");
                }

            }

            child = child->getNextSibling();
        } 
	}

    void DotSceneLoader::processNodeUserData( XERCES_CPP_NAMESPACE::DOMElement* rootUserDataXml, 
        NodeUserData* userData )
    {
        DOMNode* child = rootUserDataXml->getFirstChild();
        Logger::getSingleton().log(Logger::CORE, Ogre::LML_TRIVIAL, " NodeUserData gefunden");

        // Durch alle Unterelemente iterieren, um die properties zu finden
        while( child != NULL )
        {
            // Ein selbstdefinierter Bereich
            if( XMLString::compareIString(child->getNodeName(), 
                XMLString::transcode("property") ) == 0 )
            {
                DOMElement* propertyXml = reinterpret_cast<DOMElement*>(child);
                string propertyName = XmlHelper::getAttributeValueAsStdString( propertyXml, 
			        "name" );

                try
                {
                    if( propertyName.compare("physical_body") == 0 )
                        userData->physical_body = XmlHelper::getAttributeValueAsStdString( 
                            propertyXml, "data" );
                    else if( propertyName.compare("dynamic") == 0 )
                        userData->is_dynamic = XmlHelper::getAttributeValueAsInteger( 
                        propertyXml, "data" ) != 0;
					else if( propertyName.compare("staticgeom_group") == 0 )
						userData->staticgeom_group = XmlHelper::getAttributeValueAsInteger( 
                        propertyXml, "data" );
					else if( propertyName.compare("renderingdistance") == 0 )
						userData->renderingdistance = XmlHelper::getAttributeValueAsReal( 
                        propertyXml, "data" );
                }
                catch(...)
                {
                    Logger::getSingleton().log(Logger::CORE, Ogre::LML_TRIVIAL, 
                        " > Parse Error beim �bernehmen der Property '"+propertyName+"'!");
                }

            }

            child = child->getNextSibling();
        } 
    }

	// Eine Entity
	void DotSceneLoader::processEntity( DOMElement* rootEntityXml,
		Ogre::SceneNode* parentNode, bool createMeshPhysicalBody, Ogre::Real renderingDistance )
	{
		string entName = XmlHelper::getAttributeValueAsStdString( 
			rootEntityXml, "name" );
		string meshName = XmlHelper::getAttributeValueAsStdString( 
			rootEntityXml, "meshFile" );

		Ogre::Entity* newEnt = NULL;

        // Wurde der Entity bisher kein Name zugewiesen
		if( entName.length() == 0 )
        {
            entName = getRandomName(mSceneName+"_"+parentNode->getName());
        }

        bool isEntityCreated = false;
        ResourceGroupManager& resGroupMgr = ResourceGroupManager::getSingleton();
        while(!isEntityCreated)
        {
            // Erschaffen versuchen
            try
            {
                // if this mesh exists in our module's resource group: preload it
                if (resGroupMgr.resourceExists(mResourceGroup, meshName))
                {
                    MeshManager::getSingleton().load(meshName, mResourceGroup);
                }
                // if not, it is now loaded implicitly from the default group
                newEnt = mSceneManager->createEntity(entName, meshName);
                parentNode->attachObject( newEnt );
                isEntityCreated = true;

                Logger::getSingleton().log(Logger::CORE, Ogre::LML_TRIVIAL, " Entity '"+meshName+"' mit dem Namen '"+entName+"' in den Knoten '"+parentNode->getName()+"' eingef�gt." );

                // Zur Physik des Levels hinzuf�gen
                if( createMeshPhysicalBody )
                {                
                    PhysicsManager::getSingleton().addLevelGeometry( newEnt );
                    Logger::getSingleton().log(Logger::CORE, Ogre::LML_TRIVIAL, " Entity '"+entName+"' als TriMesh in levelGeometry geladen");
                }

				newEnt->setRenderingDistance( renderingDistance );
                newEnt->setCastShadows( false );
            }
            catch (Ogre::Exception& e) 
            {
                if (e.getNumber() == Ogre::Exception::ERR_DUPLICATE_ITEM)
                {
                    // Ok, gab es schon. Neuen Namen probieren.
                    entName = getRandomName(entName);
                }
                else
                {
                    Logger::getSingleton().log(Logger::CORE, Ogre::LML_CRITICAL, 
                        " Laden der Entity '"+meshName+"' gescheitert!" );
                    // Nicht weiter versuchen
                    break;
                }
            }
        }
	}

    string DotSceneLoader::getRandomName(const string& baseName)
    {
        int rnd = rand();
        stringstream rval;
        rval << baseName << "_" << rnd;
        return rval.str();
    }

	Ogre::Vector3 DotSceneLoader::processPosition( DOMElement* rootPositionXml )
	{
		Logger::getSingleton().log(Logger::CORE, Ogre::LML_TRIVIAL, " Position gefunden");

		try
		{
			if( rootPositionXml->hasAttribute( XMLString::transcode("x") ) && 
				rootPositionXml->hasAttribute( XMLString::transcode("y") ) && 
				rootPositionXml->hasAttribute( XMLString::transcode("z") ) )
			{
				return Ogre::Vector3( 
					XmlHelper::getAttributeValueAsReal( rootPositionXml, "x" ),
					XmlHelper::getAttributeValueAsReal( rootPositionXml, "y" ),
					XmlHelper::getAttributeValueAsReal( rootPositionXml, "z" ) );
			}
		}
		catch(...) { }
         
		Logger::getSingleton().log(Logger::CORE, Ogre::LML_TRIVIAL, " > Parse Error beim �bernehmen der Position! ");

		return Ogre::Vector3::ZERO;
	}


	Ogre::Vector3 DotSceneLoader::processScale( DOMElement* rootScaleXml )
	{
		Logger::getSingleton().log(Logger::CORE, Ogre::LML_TRIVIAL, " Skalierung gefunden");

		try
		{
			if( rootScaleXml->hasAttribute( XMLString::transcode("x") ) && 
				rootScaleXml->hasAttribute( XMLString::transcode("y") ) && 
				rootScaleXml->hasAttribute( XMLString::transcode("z") ) )
			{
				return Ogre::Vector3( 
					XmlHelper::getAttributeValueAsReal( rootScaleXml, "x" ),
					XmlHelper::getAttributeValueAsReal( rootScaleXml, "y" ),
					XmlHelper::getAttributeValueAsReal( rootScaleXml, "z" ) );
			}
		}
        catch(...) { }

		Logger::getSingleton().log(Logger::CORE, Ogre::LML_TRIVIAL, " > Parse Error beim �bernehmen der Skalierung! ");

		return Ogre::Vector3::UNIT_SCALE;
	}

	/// @TODO Sollten drei M�glichkeiten sein...
	Ogre::Quaternion DotSceneLoader::processRotation( DOMElement* rootQuatXml )
	{
		Logger::getSingleton().log(Logger::CORE, Ogre::LML_TRIVIAL, " Rotation gefunden");

		try
		{
			// Durch w,x,y,z definiert
			if( rootQuatXml->hasAttribute( XMLString::transcode("qw") ) && 
				rootQuatXml->hasAttribute( XMLString::transcode("qx") ) && 
				rootQuatXml->hasAttribute( XMLString::transcode("qy") ) && 
				rootQuatXml->hasAttribute( XMLString::transcode("qz") ) )
			{

				return Ogre::Quaternion( 
					XmlHelper::getAttributeValueAsReal( rootQuatXml, "qw" ),
					XmlHelper::getAttributeValueAsReal( rootQuatXml, "qx" ),
					XmlHelper::getAttributeValueAsReal( rootQuatXml, "qy" ),
					XmlHelper::getAttributeValueAsReal( rootQuatXml, "qz" ) );
			}

			// Durch axisX,axisY,axisZ,angle definiert
			if( rootQuatXml->hasAttribute( XMLString::transcode("angle") ) &&
				rootQuatXml->hasAttribute( XMLString::transcode("axisX") ) && 
				rootQuatXml->hasAttribute( XMLString::transcode("axisY") ) && 
				rootQuatXml->hasAttribute( XMLString::transcode("axisZ") )  )
			{
				return Ogre::Quaternion( 
					Ogre::Degree( XmlHelper::getAttributeValueAsReal( rootQuatXml, "angle" ) ),
					Ogre::Vector3(
					XmlHelper::getAttributeValueAsReal( rootQuatXml, "axisX" ),
					XmlHelper::getAttributeValueAsReal( rootQuatXml, "axisY" ),
					XmlHelper::getAttributeValueAsReal( rootQuatXml, "axisZ" ) ) );
			}

			// Durch angleX,angleY,angleZ definiert
			if( rootQuatXml->hasAttribute( XMLString::transcode("angleX") ) && 
				rootQuatXml->hasAttribute( XMLString::transcode("angleY") ) && 
				rootQuatXml->hasAttribute( XMLString::transcode("angleZ") )  )
			{
				Ogre::Matrix3 mat;
				mat.FromEulerAnglesXYZ(
					Degree(XmlHelper::getAttributeValueAsReal(rootQuatXml, "angleX")),
					Degree(XmlHelper::getAttributeValueAsReal(rootQuatXml, "angleY")),
					Degree(XmlHelper::getAttributeValueAsReal(rootQuatXml, "angleZ")));
				return Quaternion(mat);
			}
		}
		catch(...) {}

        Logger::getSingleton().log(Logger::CORE, Ogre::LML_TRIVIAL, " > Parse Error beim �bernehmen der Rotation! ");

		return Ogre::Quaternion::IDENTITY;
	}

}
