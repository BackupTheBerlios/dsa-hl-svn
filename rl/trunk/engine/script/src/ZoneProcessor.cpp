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

#include "ZoneProcessor.h"

#include "ActorManager.h"
#include "RulesConstants.h"
#include "ScriptSubsystem.h"
#include "Trigger.h"
#include "TriggerFactory.h"
#include "Zone.h"
#include "ZoneManager.h"

using namespace Ogre;

namespace rl
{

	bool ZoneProcessor::processNode(const TiXmlElement* zonesElem, const Ogre::String& resourceGroup, bool loadGameObjects)
	{
		if (zonesElem == NULL)
		{
			return false; // no zones
		}

        for (const TiXmlNode* cur = zonesElem->FirstChild(); cur != NULL; cur = cur->NextSibling())
        {
            if (cur->Type() == TiXmlNode::ELEMENT
				&& hasNodeName(cur, "zone"))
            {
            	const TiXmlElement* curZoneElem = cur->ToElement();
				if (hasAttribute(curZoneElem, "name"))
				{
					Ogre::String name = getAttributeValueAsStdString(curZoneElem, "name");
                    Zone* zone = NULL;
                    if (name == "default")
                    {
                        zone = ZoneManager::getSingleton().getDefaultZone();
                    }
                    else
                    {
                        // get the zone with that name or create one
                        zone = ZoneManager::getSingleton().getZone(name);
                        if (!zone)
                        {
                            zone = ZoneManager::getSingleton().createZone(name, false);
                        }

                        // multiple areas
                        for (const TiXmlNode* curArea = cur->FirstChild(); curArea != NULL; curArea = curArea->NextSibling())
                        {
                            if (curArea->Type() == TiXmlNode::ELEMENT
                                && hasNodeName(curArea, "area"))
                            {
                            	const TiXmlElement *curAreaElem = curArea->ToElement();
                                if (hasAttribute(curAreaElem, "type"))
                                {
                                    // type
                                    Ogre::String type;
                                    type = getAttributeValueAsStdString(curAreaElem, "type");

                                    // add or subtract?
                                    bool subtract = false;
                                    if (hasAttribute(curAreaElem, "subtract"))
                                    {
                                        subtract = getAttributeValueAsBool(curAreaElem, "subtract");
                                    }

                                    // position
                                    Vector3 position = Vector3::ZERO;
                                    const TiXmlElement* positionElem = getChildNamed(curAreaElem, "position");
                                    if (positionElem)
                                    {
                                        position = getValueAsVector3(positionElem);
                                    }

                                    //scale, rotation, offset
                                    Vector3 scale = Vector3::UNIT_SCALE;
                                    const TiXmlElement* scaleElem = getChildNamed(curAreaElem, "scale");
                                    if (!scaleElem)
                                    {
                                        scaleElem = getChildNamed(curAreaElem, "size");
                                    }
                                    
                                    if (scaleElem)
                                    {
                                        scale = getValueAsVector3(scaleElem);
                                    }

                                    Vector3 offset = Vector3::ZERO;
                                    const TiXmlElement* offsetElem = getChildNamed(curAreaElem, "offset");
                                    if (offsetElem)
                                    {
                                        offset = getValueAsVector3(offsetElem);
                                    }

                                    Quaternion rotation = Quaternion::IDENTITY;
                                    const TiXmlElement* rotationElem = getChildNamed(curAreaElem, "rotation");
                                    if (rotationElem)
                                    {
                                        rotation = getValueAsQuaternion(rotationElem);
                                    }

                                    //transition distance
                                    Real transitionDistance = 0;
                                    const TiXmlElement* transitionElem = getChildNamed(curAreaElem, "transition_distance");
                                    if (transitionElem)
                                    {
                                        transitionDistance = getValueAsReal(transitionElem);
                                    }
					            
                                    if (type == "mesh")
    					            {
                                        Ogre::String meshName;
                                        if (hasAttribute(curAreaElem, "meshfile"))
                                        {
                                            meshName = getAttributeValueAsStdString(curAreaElem, "meshfile");
                                            if (subtract)
                                            {
                                                ZoneManager::getSingleton().subtractMeshAreaFromZone(name,
                                                    meshName, GT_CONVEXHULL, position, scale, offset, rotation, transitionDistance, QUERYFLAG_PLAYER);
                                            }
                                            else
                                            {
                                                ZoneManager::getSingleton().addMeshAreaToZone(name,
                                                    meshName, GT_CONVEXHULL, position, scale, offset, rotation, transitionDistance, QUERYFLAG_PLAYER);
                                            }
                                        }
                                        else
                                        {
                                            LOG_ERROR(Logger::SCRIPT, "an <area> element with type=\"mesh\" must have attribute 'meshfile'");
                                        }
                                    }
    					            else
                                    {
                                        GeometryType geom = GT_NONE;
                                        if (type == "sphere")
                                        {
                                            geom = GT_SPHERE;
                                        }
                                        else if (type == "box")
                                        {
                                            geom = GT_BOX;
                                        }
                                        else if (type == "ellipsoid")
                                        {
                                            geom = GT_ELLIPSOID;
                                        }
                                        else if (type == "pyramid")
                                        {
                                            geom = GT_PYRAMID;
                                        }
                                        else if (type == "capsule")
                                        {
                                            geom = GT_CAPSULE;
                                        }
                                        else
                                        {
                                            LOG_ERROR(Logger::SCRIPT, "Unknown area type '" + type + "' !");
                                        }
                                        
                                        if ( geom != GT_NONE)
                                        {
                                            Ogre::AxisAlignedBox aabb;
                                            aabb.setMinimum( - scale / 2.0f);
                                            aabb.setMaximum( + scale / 2.0f);

                                            if (subtract)
                                                ZoneManager::getSingleton().subtractAreaFromZone(name,
                                                    aabb, geom, position, offset, rotation, transitionDistance, QUERYFLAG_PLAYER);
                                            else
                                                ZoneManager::getSingleton().addAreaToZone(name,
                                                    aabb, geom, position, offset, rotation, transitionDistance, QUERYFLAG_PLAYER);
                                        }
                                    }
                                }
                                else
                                {
                                    LOG_ERROR(Logger::SCRIPT, "<area> elemt must have attribute 'type'");
                                }
                            }
                        }
                    }

					if (zone)
					{
						for (const TiXmlNode* cur = curZoneElem->FirstChild(); cur != NULL; cur = cur->NextSibling())
						{
							if (cur->Type() == TiXmlNode::ELEMENT)
							{
								const TiXmlElement* curElem = cur->ToElement();
								if (hasNodeName(curElem, "light"))
								{
									Ogre::String name = getAttributeValueAsStdString(curElem, "name");
									zone->addLight(ActorManager::getSingleton().getActor(name));
								}
								else if (hasNodeName(curElem, "sound"))
								{
									Ogre::String name = getAttributeValueAsStdString(curElem, "name");
									zone->addSound(name);
								}
								else if (hasNodeName(curElem, "trigger"))
								{
									Ogre::String classname =
										getAttributeValueAsStdString(curElem, "classname");

									Ogre::String name =
										getAttributeValueAsStdString(curElem, "name");

									Trigger* trigger = ScriptSubsystem::getSingleton().getTriggerFactory()
										->createTrigger(classname, name);

                                    // add trigger properties
                                    for (const TiXmlNode* curProperty = cur->FirstChild(); curProperty != NULL; curProperty = curProperty->NextSibling())
                                    {
                                        if (hasNodeName(curProperty, "property"))
                                        {
                                            PropertyEntry propEntry = processProperty(curProperty->ToElement());
                                            if (propEntry.first != "")
                                            {
                                                trigger->setProperty(propEntry.first, propEntry.second);
                                            }
                                        }
                                    }

                                    zone->addTrigger(trigger);
								}
                                else if (hasNodeName(curElem, "eaxpreset"))
                                {
									Ogre::String name = getAttributeValueAsStdString(curElem, "name");
                                    zone->setEaxPreset(name);
                                }
							}
						}
					}
					else
					{
						LOG_ERROR(Logger::SCRIPT, "Zone named '"+name+"' could not be processes.");
					}
				}
				else
				{
					LOG_ERROR(Logger::SCRIPT, "<zone> element must have attribute 'name'.");
				}
			}
		}

        ZoneManager::getSingleton().update();
		return true;
	}

}
