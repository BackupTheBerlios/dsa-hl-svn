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

#include <xercesc/dom/DOM.hpp>

#include "DotSceneLoader.h"
#include "World.h"

#include "PhysicsManager.h"
#include "ActorManager.h"
#include "CoreSubsystem.h"
#include "ConfigurationManager.h"

#include "Property.h"

#include "Exception.h"

#include <cstdlib>
#include <ctime>

using namespace XERCES_CPP_NAMESPACE;
using namespace std;
using namespace Ogre;

namespace rl {
    using XERCES_CPP_NAMESPACE::DOMDocument;

    DotSceneLoader::DotSceneLoader(const string& filename, const string& resourceGroup)
        : mSceneName(filename),
          mResourceGroup(resourceGroup),
          mStaticNodes(),
          mRenderingDistance(ActorManager::getSingleton().getDefaultActorRenderingDistance()),
          mStaticgeomRenderingDistances(),
          mStaticgeomBatchSizes(),
          mRessource(NULL),
          mParser(NULL),
          mErrorCount(0)
    {
        srand(static_cast<unsigned int>(time(NULL)));
    }

    DotSceneLoader::~DotSceneLoader()
    {
        if (!mRessource.isNull() && mRessource->isLoaded())
        {
            XmlResourceManager::getSingleton().unload(mSceneName);
            XmlResourceManager::getSingleton().remove(mRessource->getHandle());
        }
    }

    void DotSceneLoader::initializeScene(SceneManager* sceneManager)
    {
        initializeXml();

        LOG_MESSAGE(Logger::CORE, "Loading Scene from " + mSceneName);
        DOMDocument* doc = loadDocument(mSceneName, mResourceGroup);

        // Durch alle Unterelemente iterieren
        DOMNode* child = doc->getDocumentElement()->getFirstChild();

        while(child != NULL)
        {
            // Ein Node
            if (XMLString::compareIString(child->getNodeName(), AutoXMLCh("userData").data()) == 0 )
            {
                processSceneUserData(static_cast<DOMElement*>(child));
            }
            child = child->getNextSibling();
        }

        LOG_DEBUG(Logger::CORE, " Beginne parsen der Unterelemente");
        DOMElement* nodes = getChildNamed(doc->getDocumentElement(), "nodes");

        // Eine .scene wird in einem SceneNode mit dem Namen der .scene befestigt
        mSceneNode = sceneManager->getRootSceneNode()->createChildSceneNode(mSceneName);
        processNodes(nodes, sceneManager, mSceneNode);

        // Find out, if static geometry should be used
        Ogre::String temp = ConfigurationManager::getSingleton().getStringSetting(
            "Video", "Use Static Geometry");
        bool useStaticGeometry;

        if (temp == "yes")
        {
            useStaticGeometry = true;
        }
        else if (temp == "no")
        {
            useStaticGeometry = false;
        }
        else
        {
            // Check, if Renderer supports vertex buffer
            useStaticGeometry = Ogre::Root::getSingleton().getRenderSystem()->getCapabilities()->hasCapability(Ogre::RSC_VBO);
        }

        if (useStaticGeometry)
        {
            for(std::map<int,Ogre::SceneNode*>::iterator it = mStaticNodes.begin();
                it != mStaticNodes.end();)
            {
                Ogre::SceneNode* staticNode = it->second;
                string staticName = Ogre::StringConverter::toString(it->first);
                Ogre::Real renderDist = mRenderingDistance;
                Ogre::Real batchSize = 25.0;

                if (mStaticgeomRenderingDistances.find(it->first) != mStaticgeomRenderingDistances.end())
                    renderDist = mStaticgeomRenderingDistances[it->first];
                if (mStaticgeomBatchSizes.find(it->first) != mStaticgeomBatchSizes.end())
                    batchSize = mStaticgeomBatchSizes[it->first];

                StaticGeometry* staticGeom = sceneManager->createStaticGeometry(mSceneName + staticName );

                staticGeom->setRenderingDistance(renderDist);
                staticGeom->addSceneNode(staticNode);
                staticGeom->setRegionDimensions(batchSize * Vector3::UNIT_SCALE);
                // Statische Geometrie bauen
                staticGeom->build();
                // Nicht mehr den Original-Knoten rendern, da dieser noch erhalten ist.
                staticNode->setVisible(false);
                staticNode->removeAndDestroyAllChildren();
                sceneManager->destroySceneNode(staticNode->getName());
                staticNode = NULL;
                LOG_DEBUG(Logger::CORE, " Statische Geometrie "+staticName+" erstellt");
                ++it;
            }
        }
        else
            LOG_DEBUG(Logger::CORE, " Keine statischen Geometrien erstellt");

        shutdownXml();
        LOG_DEBUG(Logger::CORE, "Szenenbeschreibung aus " + mSceneName +" fertig geparst");
    }

    // Iteriert durch die einzelnen Nodes
    void DotSceneLoader::processNodes(DOMElement* rootNodesXml, SceneManager* sceneManager,
        Ogre::SceneNode* parentNode)
    {
        if (rootNodesXml == NULL)
            return;
        if (parentNode == NULL)
            Throw(NullPointerException, "parentNode darf nicht null sein");


        // Durch alle Unterelemente iterieren
        DOMNode* child = rootNodesXml->getFirstChild();

        while(child != NULL)
        {
            // Ein Node
            if (XMLString::compareIString(child->getNodeName(), AutoXMLCh("node").data()) == 0 )
            {
                processNode(static_cast<DOMElement*>(child), sceneManager, parentNode, NULL);
            }
            child = child->getNextSibling();
        }
    }

    // Befasst sich mit einem Node
    void DotSceneLoader::processNode(DOMElement* rootNodeXml, SceneManager* sceneManager, Ogre::SceneNode* parentNode, NodeUserData* parentUserData)
    {
        if (rootNodeXml == NULL)
            return;
        if (parentNode == NULL)
            Throw(NullPointerException, "parentNode darf nicht null sein");

        string nodeName = getAttributeValueAsStdString(rootNodeXml,
                        "name");

        Ogre::SceneNode* newNode;
        // Wurde dem Node ein Name zugewiesen?
        if (nodeName.length() > 0)
        {
            if (!parentNode->getCreator()->hasSceneNode(nodeName))
            {
                // Dann versuche einen Knoten mit dem Namen zu erstellen
                newNode = parentNode->createChildSceneNode(nodeName);
            }
            else
            {
                // Name schon vergeben
                newNode = parentNode->createChildSceneNode();
                LOG_DEBUG(Logger::CORE,
                    " NodeName '"+nodeName+"' war schon vergeben! Es wurde der Name '"+newNode->getName()+"' benutzt.");
            }
        }
        else
        {
            newNode = parentNode->createChildSceneNode();
        }

        LOG_DEBUG(Logger::CORE,
                    " Node '"+newNode->getName()+"' als Unterknoten von '"+parentNode->getName()+"' erstellt.");


        NodeUserData userData;
        // Defaults einstellen
        if (parentUserData == NULL || !parentUserData->is_inheriting)
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


        mCollisions.clear();
        DOMNode* child = rootNodeXml->getFirstChild();
        DOMNode *childScale = NULL, *childPosition = NULL,
        *childRotation = NULL;
        // Durch alle Unterelemente iterieren und gefundenes speichern
        while(child != NULL)
        {
            // Ein selbstdefinierter Bereich
            if (XMLString::compareIString(child->getNodeName(),
                AutoXMLCh("userData").data()) == 0)
            {
                // UserData direkt auslesen
                processNodeUserData(static_cast<DOMElement*>(child) , &userData);
            }
            else if (XMLString::compareIString(child->getNodeName(),
                     AutoXMLCh("position").data()) == 0)
                childPosition = child;//rootNodeXml->removeChild(child);
            else if (XMLString::compareIString(child->getNodeName(),
                     AutoXMLCh("rotation").data()) == 0)
                childRotation = child;//rootNodeXml->removeChild(child);
            else if (XMLString::compareIString(child->getNodeName(),
                     AutoXMLCh("scale").data()) == 0)
                childScale = child;//rootNodeXml->removeChild(child);

            child = child->getNextSibling();
        }
        // so jetzt stehen nur noch entities und nodes in der liste





        if (userData.staticgeom_group > -1)
        {
            newNode->getParentSceneNode()->removeChild(newNode);

            // Existiert noch nicht
            if (mStaticNodes.find(userData.staticgeom_group) == mStaticNodes.end())
            {
                mStaticNodes[userData.staticgeom_group] =
                sceneManager->getRootSceneNode()->createChildSceneNode(
                mSceneName+"_static_"+Ogre::StringConverter::toString(userData.staticgeom_group));
            }

            mStaticNodes[userData.staticgeom_group]->addChild(newNode);
        }


        // Einzelne weitere childs auslesen, falls n�tig, in der richtigen Reihenfolge

        // Position des Nodes
        if (childPosition != NULL)
            newNode->setPosition(processPosition(static_cast<DOMElement*>(childPosition)));

        // Rotation des Nodes
        if (childRotation != NULL)
            newNode->setOrientation(processRotation(static_cast<DOMElement*>(childRotation)));

        // Skalierung des Nodes
        if (childScale != NULL)
            newNode->setScale(processScale(static_cast<DOMElement*>(childScale)));

/*      // nun nicht mehr n�tig!
        {
            // Skalierung auf alle Entities �bertragen
            for(unsigned short i = 0; i < newNode->numAttachedObjects(); i++)
            {
                MovableObject* mo = newNode->getAttachedObject(i);
                if (mo->getMovableType().compare("Entity") == 0)
                    static_cast<Entity*>(mo)->setNormaliseNormals(newNode->getScale() != Vector3::UNIT_SCALE);
            }
        }
*/



        // Alle Entities und Subnodes auslesen
        child = rootNodeXml->getFirstChild();
        while(child != NULL)
        {
            // geschachteltes weiteres Node
            if (XMLString::compareIString(child->getNodeName(),
                AutoXMLCh("node").data()) == 0)
                processNode(static_cast<DOMElement*>(child), sceneManager, newNode, &userData);
            // Eine Entity
            else if (XMLString::compareIString(child->getNodeName(),
                     AutoXMLCh("entity").data()) == 0)
                processEntity(static_cast<DOMElement*>(child),
                                sceneManager,
                                newNode,
                                userData.renderingdistance,
                                userData.physical_body);

            child = child->getNextSibling();
        }
    }

    void DotSceneLoader::processSceneUserData(XERCES_CPP_NAMESPACE::DOMElement* rootUserDataXml)
    {
        DOMNode* child = rootUserDataXml->getFirstChild();
        LOG_DEBUG(Logger::CORE, " SceneUserData gefunden");
        XmlPropertyReader* propertyReader = new XmlPropertyReader();

        // Durch alle Unterelemente iterieren, um die properties zu finden
        while(child != NULL)
        {
            // Ein selbstdefinierter Bereich
            if (XMLString::compareIString(child->getNodeName(),
                AutoXMLCh("property").data()) == 0)
            {
                DOMElement* propertyXml = static_cast<DOMElement*>(child);

                PropertyEntry entry = propertyReader->processProperty(propertyXml);

                try
                {
                    if (entry.first == "default_renderingdistance")
                    {
                        mRenderingDistance = entry.second.toReal();
                    }
                    else if (Ogre::StringUtil::startsWith(entry.first, "staticgeom_renderingdistance_"))
                    {
                        int groupId = Ogre::StringConverter::parseInt(entry.first.substr(29));
                        mStaticgeomRenderingDistances[groupId] = entry.second.toReal();
                    }
                    else if (Ogre::StringUtil::startsWith(entry.first, "staticgeom_batchsize_"))
                    {
                        int groupId = Ogre::StringConverter::parseInt(entry.first.substr(21));
                        mStaticgeomBatchSizes[groupId] = entry.second.toReal();
                    }
                }
                catch(...)
                {
                    LOG_DEBUG(Logger::CORE,
                        " > Parse Error beim �bernehmen der Property '"+entry.first+"'!");
                }

            }
            // Manuell definiertes LOD
            else if (XMLString::compareIString(child->getNodeName(),
                AutoXMLCh("manualLOD").data()) == 0)
            {
                DOMElement* lodXml = static_cast<DOMElement*>(child);
                string meshName = getAttributeValueAsStdString(lodXml,
                    "mesh");

                try
                {
                    Ogre::MeshPtr mesh = Ogre::MeshManager::getSingleton().getByName(meshName);
                    mesh->removeLodLevels();

                    DOMNode* lodchild = child->getFirstChild();
                    LOG_DEBUG(Logger::CORE, " LOD-Bereich f�r "+meshName+" gefunden");
                    Ogre::Real loddist = 10.0;
                    string lodmeshName = "";

                    // Durch alle Unterelemente iterieren, um die LODs zu finden
                    while(lodchild != NULL)
                    {
                        if (XMLString::compareIString(lodchild->getNodeName(),
                            AutoXMLCh("LOD").data()) == 0)
                        {
                            loddist = 0.0;
                            lodmeshName = "";

                            try
                            {
                                lodXml = static_cast<DOMElement*>(lodchild);
                                loddist = getAttributeValueAsReal(lodXml,
                                    "distance");
                                lodmeshName = getAttributeValueAsStdString(lodXml,
                                    "mesh");

                                if (lodmeshName.length() > 0 && loddist > 0)
                                {
                                    mesh->createManualLodLevel(loddist, lodmeshName);
                                    LOG_DEBUG(Logger::CORE,
                                        " LOD f�r bei '"+Ogre::StringConverter::toString(loddist)+
                                        "' als '"+lodmeshName+"' gesetzt!");
                                }
                            }
                            catch(...) { }
                        }

                        lodchild = lodchild->getNextSibling();
                    }
                }
                catch(...)
                {
                    LOG_MESSAGE(Logger::CORE,
                        " > Parse Error beim Setzen der LOD f�r '"+meshName+"'!");
                }
            }
            child = child->getNextSibling();
        }

        delete propertyReader;
    }

    void DotSceneLoader::processNodeUserData(XERCES_CPP_NAMESPACE::DOMElement* rootUserDataXml,
        NodeUserData* userData)
    {
        DOMNode* child = rootUserDataXml->getFirstChild();
        LOG_DEBUG(Logger::CORE, " NodeUserData gefunden");

        // Durch alle Unterelemente iterieren, um die properties zu finden
        while(child != NULL)
        {
            // Ein selbstdefinierter Bereich
            if (XMLString::compareIString(child->getNodeName(),
                AutoXMLCh("property").data()) == 0)
            {
                DOMElement* propertyXml = static_cast<DOMElement*>(child);
                PropertyEntry entry = processProperty(propertyXml);

                try
                {
                    if (entry.first == "physical_body")
                        userData->physical_body = entry.second.toString().c_str();
                    else if (entry.first == "dynamic")
                        userData->is_dynamic = entry.second.toInt() != 0;
                    else if (entry.first == "staticgeom_group")
                        userData->staticgeom_group = entry.second.toInt();
                    else if (entry.first == "renderingdistance")
                        userData->renderingdistance = entry.second.toReal();
                }
                catch(...)
                {
                    LOG_MESSAGE(Logger::CORE,
                        " > Parse Error beim �bernehmen der Property '"+entry.first+"'!");
                }

            }
            else if (XMLString::compareIString(child->getNodeName(),
                        AutoXMLCh("collisions").data()) == 0)
                    processCollisions(static_cast<DOMElement*> (child));


            child = child->getNextSibling();
        }
    }

    // eine benutzerdefinierte Collision
    void DotSceneLoader::processCollisions(XERCES_CPP_NAMESPACE::DOMElement *rootCollisionXml)
    {
#ifdef OGRENEWT_COLLISION_USE_SHAREDPTR
        OgreNewt::CollisionPtr collision;
#else
        OgreNewt::CollisionPtr collision = NULL;
#endif
        DOMNode* child = rootCollisionXml->getFirstChild();
        OgreNewt::World *thisWorld = PhysicsManager::getSingleton()._getNewtonWorld();

        std::vector<Ogre::Vector3> vec3Vector;
        Ogre::Vector3 scale, offset;
        Ogre::Quaternion rotation;


        LOG_DEBUG(Logger::CORE, " collisions in NodeUserData gefunden");


        while(child != NULL)
        {
            if (child->getNodeType() != DOMNode::ELEMENT_NODE)
            {
                child = child->getNextSibling();
                continue;
            }
#ifdef OGRENEWT_COLLISION_USE_SHAREDPTR
            collision.reset();
#else
            collision = NULL;
#endif
            // am Anfang steht ein Node mit dem Typ
            std::string typeAsString = transcodeToStdString(child->getNodeName());

            scale = Ogre::Vector3::UNIT_SCALE;
            offset = Ogre::Vector3::ZERO;
            rotation = Ogre::Quaternion::IDENTITY;
            vec3Vector.clear();

            DOMNode *childChild = static_cast<DOMElement*>(child)->getFirstChild();
            while(childChild != NULL)
            {
                if (childChild->getNodeType() == DOMNode::ELEMENT_NODE)
                {
                    if (XMLString::compareIString(childChild->getNodeName(),
                        AutoXMLCh("scale").data()) == 0)
                        scale = processScale(static_cast<DOMElement*>(childChild));
                    else if (XMLString::compareIString(childChild->getNodeName(),
                             AutoXMLCh("offset").data()) == 0)
                        offset = processPosition(static_cast<DOMElement*>(childChild));
                    else if (XMLString::compareIString(childChild->getNodeName(),
                             AutoXMLCh("rotation").data()) == 0)
                        rotation = processRotation(static_cast<DOMElement*>(childChild));

                    else if (XMLString::compareIString(childChild->getNodeName(),
                             AutoXMLCh("vertices").data()) == 0)
                    {
                        DOMNode *childChildChild = static_cast<DOMElement*>(childChild)->getFirstChild();
                        while(childChildChild != NULL)
                        {
                            if (childChild->getNodeType() == DOMNode::ELEMENT_NODE)
                            {
                                if (XMLString::compareIString(childChildChild->getNodeName(),
                                    AutoXMLCh("vertex").data()) == 0)
                                {
                                    bool error = false;
                                    Ogre::Vector3 vec3 = processVector(static_cast<DOMElement*>(childChildChild), error);
                                    if (!error)
                                        vec3Vector.push_back(vec3);
                                }
                            }
                            childChildChild = childChildChild->getNextSibling();
                        }
                    }
                }
                childChild = childChild->getNextSibling();
            }

            // typangabe aus String extrahieren
            if (typeAsString == "convexhull")
            {
                int vertcount = vec3Vector.size();
                Ogre::Vector3 *vertices = new Ogre::Vector3[ vertcount ];
                Ogre::Vector3 vec3Min(0,0,0), vec3Max(0,0,0);
                for(int i = 0; i < vertcount; i++)
                {
                    vertices[i] = vec3Vector[i] * scale; // in array �bertragen


                    if (vertices[i].x < vec3Min.x)   // und gr��e des k�rpers bestimmen
                        vec3Min.x = vertices[i].x;
                    if (vertices[i].y < vec3Min.y)
                        vec3Min.y = vertices[i].y;
                    if (vertices[i].z < vec3Min.z)
                        vec3Min.z = vertices[i].z;

                    if (vertices[i].x > vec3Max.x)
                        vec3Max.x = vertices[i].x;
                    if (vertices[i].y > vec3Max.y)
                        vec3Max.y = vertices[i].y;
                    if (vertices[i].z > vec3Max.z)
                        vec3Max.z = vertices[i].z;
                }

                Ogre::Vector3 size = vec3Max - vec3Min;
                // Gr��e �berpr�fen
                if (size.x < PhysicsManager::NEWTON_GRID_WIDTH ||
                    size.y < PhysicsManager::NEWTON_GRID_WIDTH ||
                    size.z < PhysicsManager::NEWTON_GRID_WIDTH ||
                    vertcount < 4)
                {
                    LOG_MESSAGE(Logger::CORE,
                        " physical_body 'convexhull' in <collisions> konnte nicht erstellt werden; der K�rper ist zu klein!");

                    // Minimale Gr��e verwenden
                    if (size.x < PhysicsManager::NEWTON_GRID_WIDTH)
                        size.x = PhysicsManager::NEWTON_GRID_WIDTH;
                    if (size.y < PhysicsManager::NEWTON_GRID_WIDTH)
                        size.y = PhysicsManager::NEWTON_GRID_WIDTH;
                    if (size.z < PhysicsManager::NEWTON_GRID_WIDTH)
                        size.z = PhysicsManager::NEWTON_GRID_WIDTH;

                    collision = OgreNewt::CollisionPtr(new OgreNewt::CollisionPrimitives::Box(
                                     thisWorld, size, rotation, offset));

                    LOG_MESSAGE(Logger::CORE, " stattdessen physical_body 'box' erstellt. ");
                }
                else
                {
                    collision = OgreNewt::CollisionPtr(new OgreNewt::CollisionPrimitives::ConvexHull(
                                    thisWorld,
                                    vertices,
                                    vertcount,
                                    rotation, offset));

                    LOG_DEBUG(Logger::CORE, " physical_body 'convexhull' in <collisions> erstellt. ");
                }

                delete [] vertices;
            }
            else
            {
                if (!vec3Vector.empty())   // fehler!
                {
                    LOG_MESSAGE(Logger::CORE, " > Parse Error beim Einlesen einer Collision; Es k�nne nur f�r Convexhull Vektoren definiert werden!");
                }

                if (typeAsString.compare("box") == 0)
                {
                    collision = OgreNewt::CollisionPtr(new OgreNewt::CollisionPrimitives::Box(
                                    thisWorld, scale, rotation, offset));
                    LOG_DEBUG(Logger::CORE, " physical_body 'box' in <collisions> erstellt. ");
                }
                else if (typeAsString.compare("sphere") == 0)
                {
                    collision = OgreNewt::CollisionPtr(new OgreNewt::CollisionPrimitives::Ellipsoid(
                                    thisWorld, scale, rotation, offset));
                    LOG_DEBUG(Logger::CORE, " physical_body 'sphere' in <collisions> erstellt. ");
                }
                else if (typeAsString.compare("ellipsoid") == 0)
                {
                    collision = OgreNewt::CollisionPtr(new OgreNewt::CollisionPrimitives::Ellipsoid(
                    thisWorld, scale, rotation, offset));
                    LOG_DEBUG(Logger::CORE, " physical_body 'ellipsoid' in <collisions> erstellt. ");
                }
                else if (typeAsString.compare("pyramid") == 0)
                {
                    collision = OgreNewt::CollisionPtr(new OgreNewt::CollisionPrimitives::Pyramid(
                    thisWorld, scale, rotation, offset));
                    LOG_DEBUG(Logger::CORE, " physical_body 'pyramid' in <collisions> erstellt. ");
                }
                else if (typeAsString.compare("capsule") == 0)
                {
                    double radius = std::max(scale.x, scale.z) / 2.0;
                    double height = scale.y;
                    offset.x -= scale.y/2;
                    Ogre::Quaternion tempRot;
                    tempRot.FromAngleAxis(Degree(90), Ogre::Vector3::UNIT_Z);
                    rotation = (rotation * tempRot);

                    collision = OgreNewt::CollisionPtr(new OgreNewt::CollisionPrimitives::Capsule(
                                    thisWorld,
                                    radius,
                                    height,
                                    rotation, offset));
                    LOG_DEBUG(Logger::CORE, " physical_body 'capsule' in <collisions> erstellt. ");
                }
                else if (typeAsString.compare("cone") == 0)
                {
                    double radius = std::max(scale.x, scale.z) / 2.0;
                    double height = scale.y;
                    offset.x -= scale.y/2;
                    Ogre::Quaternion tempRot;
                    tempRot.FromAngleAxis(Degree(90), Ogre::Vector3::UNIT_Z);
                    rotation = (rotation * tempRot);

                    collision = OgreNewt::CollisionPtr(new OgreNewt::CollisionPrimitives::Cone(
                                    thisWorld,
                                    radius,
                                    height,
                                    rotation, offset));
                    LOG_DEBUG(Logger::CORE, " physical_body 'cone' in <collisions> erstellt. ");
                }
                else if (typeAsString.compare("cylinder") == 0)
                {
                    double radius = std::max(scale.x, scale.z) / 2.0;
                    double height = scale.y;
                    offset.x -= scale.y/2;
                    Ogre::Quaternion tempRot;
                    tempRot.FromAngleAxis(Degree(90), Ogre::Vector3::UNIT_Z);
                    rotation = (rotation * tempRot);

                    collision = OgreNewt::CollisionPtr(new OgreNewt::CollisionPrimitives::Cylinder(
                                    thisWorld,
                                    radius,
                                    height,
                                    rotation, offset));
                    LOG_DEBUG(Logger::CORE, " physical_body 'cylinder' in <collisions> erstellt. ");
                }
                else if (typeAsString.compare("chamfer_cylinder") == 0)
                {
                    double radius = std::max(scale.x, scale.z) / 2.0;
                    double height = scale.y;
                    offset.x -= scale.y/2;
                    Ogre::Quaternion tempRot;
                    tempRot.FromAngleAxis(Degree(90), Ogre::Vector3::UNIT_Z);
                    rotation = (rotation * tempRot);

                    collision = OgreNewt::CollisionPtr(new OgreNewt::CollisionPrimitives::ChamferCylinder(
                                    thisWorld,
                                    radius,
                                    height,
                                    rotation, offset));
                    LOG_DEBUG(Logger::CORE, " physical_body 'chamfer_cylinder' in <collisions> erstellt. ");
                }
                else if (typeAsString.compare("mesh") == 0)
                {
                    LOG_MESSAGE(Logger::CORE,
                        " Mesh-Collisions in <collisions> werden momentan noch nicht unterst�tzt (Eintrag wird ignoriert).");
#ifdef OGRENEWT_COLLISION_USE_SHAREDPTR
                    collision.reset();
#else
                    collision = NULL;
#endif
                }
/*
                else if (typeAsString.compare("meshhull") == 0)  // automatische convexhull mit daten aus mesh
                {
                    LOG_MESSAGE(Logger::CORE,
                        " Entityhull-Collisions werden momentan noch nicht unterst�tzt (Eintrag wird ignoriert).");
                    collision = NULL;
                }
*/
                else
                {
                    LOG_MESSAGE(Logger::CORE,
                        " > Parse Error beim Erstellen einer Collision; ung�ltiger typ: '"+typeAsString+"' !");
#ifdef OGRENEWT_COLLISION_USE_SHAREDPTR
                    collision.reset();
#else
                    collision = NULL;
#endif
                }

            }


            // Collision dem vektor hinzuf�gen
            if ( !collision )
                mCollisions.push_back(collision);

            child = child->getNextSibling();
        }
    }

    // Eine Entity
    void DotSceneLoader::processEntity(DOMElement* rootEntityXml,
        SceneManager* sceneManager, Ogre::SceneNode* parentNode,
        Ogre::Real renderingDistance, const std::string &physical_body)
    {
        string entName = getAttributeValueAsStdString(
            rootEntityXml, "name");
        string meshName = getAttributeValueAsStdString(
            rootEntityXml, "meshFile");

        Ogre::Entity* newEnt = NULL;

        // Wurde der Entity bisher kein Name zugewiesen
        if (entName.length() == 0)
        {
            entName = getRandomName(mSceneName+"_"+parentNode->getName());
        }

        ResourceGroupManager& resGroupMgr = ResourceGroupManager::getSingleton();
        while(parentNode->getCreator()->hasEntity(entName))
        {
            entName = getRandomName(entName);
        }

        try
        {
            // if this mesh exists in our module's resource group: preload it
            if (resGroupMgr.resourceExists(mResourceGroup, meshName))
            {
                MeshManager::getSingleton().load(meshName, mResourceGroup);
            }

            // If not, it is now loaded implicitly from the default group
            newEnt = sceneManager->createEntity(entName, meshName);
        }
        catch(...)
        {
              LOG_ERROR(Logger::CORE, " Entity '"+meshName+"' mit dem Namen '"+entName+"' konnte nicht geladen werden.");
              return;
        }

        /*if (parentNode->getScale() != Vector3::UNIT_SCALE) 
		{
            newEnt->setNormaliseNormals(true);
		}*/

        parentNode->attachObject(newEnt);

        LOG_DEBUG(Logger::CORE, " Entity '"+meshName+"' mit dem Namen '"+entName+"' in den Knoten '"+parentNode->getName()+"' eingef�gt.");


        // ------- Falls n�tig automatisch bodyproxy erstellen -------------
        // (wenn physical_body gesetzt wurde)
        OgreNewt::CollisionPtr collision = OgreNewt::CollisionPtr();
        OgreNewt::World *thisWorld = PhysicsManager::getSingleton()._getNewtonWorld();

        if (physical_body.compare("none") != 0)
        {
            const AxisAlignedBox &aab = newEnt->getMesh()->getBounds();
            Ogre::Vector3 size = (aab.getMaximum() - aab.getMinimum()) * parentNode->getScale();
            bool forceBox = false;

            if ((size.x < PhysicsManager::NEWTON_GRID_WIDTH ||
                 size.y < PhysicsManager::NEWTON_GRID_WIDTH ||
                 size.z < PhysicsManager::NEWTON_GRID_WIDTH) &&
                 physical_body.compare("convexhull") == 0)
            {
                if (size.x < PhysicsManager::NEWTON_GRID_WIDTH)
                    size.x = PhysicsManager::NEWTON_GRID_WIDTH;
                if (size.y < PhysicsManager::NEWTON_GRID_WIDTH)
                    size.y = PhysicsManager::NEWTON_GRID_WIDTH;
                if (size.z < PhysicsManager::NEWTON_GRID_WIDTH)
                    size.z = PhysicsManager::NEWTON_GRID_WIDTH;

                LOG_MESSAGE(Logger::CORE, " Die Entity '"+entName+"' liegt in einer Ebene, verwende 'box' f�r physical_body '"+physical_body+"' ");
                forceBox = true;
            }
            const Quaternion orientation(0,0,0,0);// = parentNode->getOrientation();
            const Ogre::Vector3 pos = aab.getMinimum()* parentNode->getScale() + (size/2.0);


            // Pr�fen, ob schon ein identischer Proxy erstellt wurde um diesen erneut zu verwenden
            AlreadyUsedCollision &aucol (mAutoCreatedCollisions[meshName]);
            if (aucol.Type.compare(physical_body) == 0  &&
                aucol.Scale == parentNode->getScale() &&
                (!forceBox)) // sicherheitshalber
            {
                collision = aucol.ColPtr;
                LOG_DEBUG(Logger::CORE, " Schon fr�her erstellten physical_body f�r Entity '"+entName+"' wieder verwendet. ");
            }
            else
            {

                if (physical_body.compare("box") == 0 || forceBox)
                {
                    collision = OgreNewt::CollisionPtr(new OgreNewt::CollisionPrimitives::Box(
                                     thisWorld, size, orientation, pos));
                    LOG_DEBUG(Logger::CORE, " physical_body 'box' f�r Entity '"+entName+"' erstellt. ");
                }
                else if (physical_body.compare("pyramid") == 0)
                {
                    collision = OgreNewt::CollisionPtr(new OgreNewt::CollisionPrimitives::Pyramid(
                                    thisWorld, size, orientation, pos));
                    LOG_DEBUG(Logger::CORE, " physical_body 'pyramid' f�r Entity '"+entName+"' erstellt. ");
                }
                else if (physical_body.compare("sphere") == 0)
                {
                    double radius = std::max(size.x, std::max(size.y, size.z)) / 2.0;
                    collision = OgreNewt::CollisionPtr(new OgreNewt::CollisionPrimitives::Ellipsoid(
                                    thisWorld, Vector3(radius, radius, radius),
                                    orientation, pos));
                    LOG_DEBUG(Logger::CORE, " physical_body 'sphere' f�r Entity '"+entName+"' erstellt. ");
                }
                else if (physical_body.compare("ellipsoid") == 0)
                {
                    // set the size x/z values to the maximum
                    Ogre::Vector3 s(size/2.0);
                    s.x = std::max(s.x, s.z);
                    s.z = s.x;
                    collision = OgreNewt::CollisionPtr(new OgreNewt::CollisionPrimitives::Ellipsoid(
                                    thisWorld, s,
                                    orientation, pos));
                    LOG_DEBUG(Logger::CORE, " physical_body 'ellipsoid' f�r Entity '"+entName+"' erstellt. ");
                }
                else if (physical_body.compare("capsule") == 0)
                {
                    double radius = std::max(size.x, size.z) / 2.0;
                    double height = size.y;

                    collision = OgreNewt::CollisionPtr(new OgreNewt::CollisionPrimitives::Capsule(
                                    thisWorld,
                                    radius,
                                    height,
                                    orientation, pos));
                    LOG_DEBUG(Logger::CORE, " physical_body 'capsule' f�r Entity '"+entName+"' erstellt. ");
                }
                else if (physical_body.compare("convexhull") == 0)
                {
                    collision = OgreNewt::CollisionPtr(new OgreNewt::CollisionPrimitives::ConvexHull(
                                    thisWorld,
                                    newEnt));
                    LOG_DEBUG(Logger::CORE, " physical_body 'convexhull' f�r Entity '"+entName+"' erstellt. ");
                }
                else if (physical_body.compare("mesh") == 0 || physical_body.compare("auto") == 0)
                {
                    collision = OgreNewt::CollisionPtr(new OgreNewt::CollisionPrimitives::TreeCollision(
                                    thisWorld, newEnt, true));
                    LOG_DEBUG(Logger::CORE, " physical_body 'mesh' f�r Entity '"+entName+"' erstellt. ");
                }
                else
                    LOG_MESSAGE(Logger::CORE,
                        " Der bodyproxy_type '"+physical_body+"'(aus userData) der Entity '"+meshName+"' ist ung�ltig.");

                // proxy in die liste der schon erstellten proxies hinzuf�gen
                aucol.ColPtr = collision;
                aucol.Scale = parentNode->getScale();
                aucol.Type = physical_body;
            }
        }

        // zur liste hinzuf�gen
        if (collision != NULL)
        {
            mCollisions.push_back(collision);
        }


        // Zur Physik des Levels hinzuf�gen
        if (mCollisions.size() > 0)
        {
            PhysicsManager::getSingleton().addLevelGeometry(newEnt, mCollisions);
            LOG_DEBUG(Logger::CORE, " Entity '"+entName+"' in levelGeometry geladen");
        }


        // wieder aus der liste entfernen, falls mehrere entities hier definiert werden
        if (collision != NULL)
        {
            mCollisions.pop_back();
        }

        // Renderingdistanz berechnen
        if (renderingDistance == mRenderingDistance)
        {
            Ogre::Real diameter
                = (newEnt->getBoundingBox().getMaximum() - newEnt->getBoundingBox().getMinimum()).length();

            // Gerade mal 10cm� => 10m
            if (diameter <= 0.5)
                renderingDistance = 15;
            // Gerade mal 1,5m� => 25m
            else if (diameter <= 1.5)
                renderingDistance = 30;
            // Gerade mal 2,5m� => 50m
            else if (diameter <= 2.5)
                renderingDistance = 60;
            else if (diameter <= 10)
                renderingDistance = 150;
            else if (diameter <= 50)
                renderingDistance = 250;
            else if (diameter <= 100)
                renderingDistance = 450;
            else
                renderingDistance = 1500;

            newEnt->setRenderingDistance(renderingDistance);
        }
        else
            newEnt->setRenderingDistance(renderingDistance);
        newEnt->setCastShadows(false);
    }

    string DotSceneLoader::getRandomName(const string& baseName)
    {
        int rnd = rand();
        stringstream rval;
        rval << baseName << "_" << rnd;
        return rval.str();
    }

    Ogre::Vector3 DotSceneLoader::processPosition(DOMElement* rootPositionXml)
    {
        LOG_DEBUG(Logger::CORE, " Position gefunden");

        try
        {
            if (hasAttribute(rootPositionXml, "x") &&
                hasAttribute(rootPositionXml, "y") &&
                hasAttribute(rootPositionXml, "z"))
            {
                return Ogre::Vector3(
                    getAttributeValueAsReal(rootPositionXml, "x"),
                    getAttributeValueAsReal(rootPositionXml, "y"),
                    getAttributeValueAsReal(rootPositionXml, "z"));
            }
        }
        catch(...) { }

        LOG_MESSAGE(Logger::CORE, " > Parse Error beim �bernehmen der Position! ");

        return Ogre::Vector3::ZERO;
    }


    Ogre::Vector3 DotSceneLoader::processScale(DOMElement* rootScaleXml)
    {
        LOG_DEBUG(Logger::CORE, " Skalierung gefunden");

        try
        {
            if (hasAttribute(rootScaleXml, "x") &&
                hasAttribute(rootScaleXml, "y") &&
                hasAttribute(rootScaleXml, "z"))
            {
                return Ogre::Vector3(
                    getAttributeValueAsReal(rootScaleXml, "x"),
                    getAttributeValueAsReal(rootScaleXml, "y"),
                    getAttributeValueAsReal(rootScaleXml, "z"));
            }
        }
        catch(...) { }

        LOG_MESSAGE(Logger::CORE, " > Parse Error beim �bernehmen der Skalierung! ");

        return Ogre::Vector3::UNIT_SCALE;
    }

    Ogre::Vector3 DotSceneLoader::processVector(DOMElement* rootScaleXml, bool &error)
    {
        LOG_DEBUG(Logger::CORE, " Vector gefunden");

        try
        {
            if (hasAttribute(rootScaleXml, "x") &&
                hasAttribute(rootScaleXml, "y") &&
                hasAttribute(rootScaleXml, "z"))
            {
                error = false;
                return Ogre::Vector3(
                    getAttributeValueAsReal(rootScaleXml, "x"),
                    getAttributeValueAsReal(rootScaleXml, "y"),
                    getAttributeValueAsReal(rootScaleXml, "z"));
            }
        }
        catch(...) { }

        LOG_MESSAGE(Logger::CORE, " > Parse Error beim Lesen eines Vectors! ");
        error = true;

        return Ogre::Vector3::UNIT_SCALE;
    }

    /// @TODO Sollten drei M�glichkeiten sein...
    Ogre::Quaternion DotSceneLoader::processRotation(DOMElement* rootQuatXml)
    {
        LOG_DEBUG(Logger::CORE, " Rotation gefunden");

        try
        {
            // Durch w,x,y,z definiert
            if (hasAttribute(rootQuatXml, "qw") &&
                hasAttribute(rootQuatXml, "qx") &&
                hasAttribute(rootQuatXml, "qy") &&
                hasAttribute(rootQuatXml, "qz"))
            {

                return Ogre::Quaternion(
                    getAttributeValueAsReal(rootQuatXml, "qw"),
                    getAttributeValueAsReal(rootQuatXml, "qx"),
                    getAttributeValueAsReal(rootQuatXml, "qy"),
                    getAttributeValueAsReal(rootQuatXml, "qz"));
            }

            // Durch axisX,axisY,axisZ,angle definiert
            if (hasAttribute(rootQuatXml, "angle") &&
                hasAttribute(rootQuatXml, "axisX") &&
                hasAttribute(rootQuatXml, "axisY") &&
                hasAttribute(rootQuatXml, "axisZ"))
            {
                return Ogre::Quaternion(
                    Ogre::Degree(getAttributeValueAsReal(rootQuatXml, "angle")),
                    Ogre::Vector3(
                    getAttributeValueAsReal(rootQuatXml, "axisX"),
                    getAttributeValueAsReal(rootQuatXml, "axisY"),
                    getAttributeValueAsReal(rootQuatXml, "axisZ")));
            }

            // Durch angleX,angleY,angleZ definiert
            if (hasAttribute(rootQuatXml, "angleX") &&
                hasAttribute(rootQuatXml, "angleY") &&
                hasAttribute(rootQuatXml, "angleZ") )
            {
                Ogre::Matrix3 mat;
                mat.FromEulerAnglesXYZ(
                    Degree(getAttributeValueAsReal(rootQuatXml, "angleX")),
                    Degree(getAttributeValueAsReal(rootQuatXml, "angleY")),
                    Degree(getAttributeValueAsReal(rootQuatXml, "angleZ")));
                return Quaternion(mat);
            }
        }
        catch(...) {}

        LOG_MESSAGE(Logger::CORE, " > Parse Error beim �bernehmen der Rotation! ");

        return Ogre::Quaternion::IDENTITY;
    }
}

