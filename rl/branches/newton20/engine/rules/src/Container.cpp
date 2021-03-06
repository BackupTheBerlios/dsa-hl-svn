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

#include "Container.h"

#include <algorithm>

#include "Exception.h"
#include "GameObjectManager.h"
#include "Item.h"
#include "Slot.h"

using namespace std;

namespace rl {

	const IntPair Container::NO_SPACE_FOR_ITEM = make_pair(1999999999, 1999999999);

    const Ogre::String Container::CLASS_NAME = "Container";

    const Ogre::String Container::PROPERTY_CAPACITY = "capacity";
    const Ogre::String Container::PROPERTY_VOLUME = "volume";
	const Ogre::String Container::PROPERTY_CONTENT = "content";
    const Ogre::String Container::PROPERTY_CONTENT_OBJECTS = "objects";
    const Ogre::String Container::PROPERTY_CONTENT_POSITIONS = "positions";

    Container::Container(int id)
        : Item(id),
          mCapacity(0.0),
		  mVolume(std::make_pair(1,1))
    {
        mQueryFlags |= QUERYFLAG_CONTAINER;
    }

    Container::~Container()
    {
        // set the state of all child items to GOS_LOADED
        ItemSet::iterator it;
        ItemSet items(mItems);
        for( it = items.begin(); it != items.end(); it++ )
            (*it)->removeOldState();
    }

    Ogre::Real Container::getCapacity() const
    {
        return mCapacity;
    }

    void Container::setCapacity(const Ogre::Real capacity)
    {
        mCapacity = capacity;
    }

    void Container::setVolume(int x, int y)
    {
        mVolume = make_pair(x, y);
    }

    std::pair<int,int> Container::getVolume() const
    {
        return mVolume;
    }

    Ogre::Real Container::getContentWeight() const
    {
        Ogre::Real rval = 0;
        for (ItemSet::const_iterator it = mItems.begin(); it != mItems.end(); it++)
        {
            rval += (*it)->getMass();
        }
        return rval;
    }

    bool Container::isContainer() const
    {
        return true;
    }

    ItemSet Container::getItems() const
    {
        return mItems;
    }

    bool Container::isParent(Container* cont) const
    {
        if (cont == this)
            return true;

        Item* parent = this->getParentContainer();
        while (parent)
        {
            if (cont == parent)
                return true;
            parent = parent->getParentContainer();
        }
         
         // c++ allows nasty accesses like this ;-)
        for (ItemSet::iterator iter = cont->mItems.begin(); 
            iter != cont->mItems.end(); ++iter)
        {
            if ((*iter)->isContainer())
            {
                if (isParent(dynamic_cast<Container*>(*iter)))
                {
                    return true;
                }
            }
        }
        return false;
    }

    bool Container::addItem(Item* item, IntPair position)
    {
        if (item == NULL)
        {
            Throw(NullPointerException, "Item ist null.");
        }

        // find recursions, we cannot place the container in the same container etc
        if (item->isContainer())
        {
            if (isParent(dynamic_cast<Container*>(item)))
            {
                return false;
            } 
        }

        IntPair pos = position;
		if (!canPlaceAt(item, pos.first, pos.second))
		{
			pos = findPositionWithEnoughSpace(item->getSize());
		}

		if (pos != NO_SPACE_FOR_ITEM)
		{
            item->removeOldState();

            mItemPositions[item] = pos;
			mItems.insert(item);

            item->setParentContainer(this);
            if (mOwner)
                item->setOwner(mOwner);
            else
                item->setOwner(this);
			item->setState(GOS_IN_POSSESSION);
            item->setScene(mScene);

			return true;
		}
		else
		{
			return false;
		}
    }

    void Container::_doRemoveItem(Item* item)
    {
        ItemSet::iterator it = mItems.find(item);
        if (it != mItems.end())
        {
            mItems.erase(it);
            mItemPositions.erase(mItemPositions.find(item));
        }
        else
        {
            Throw(IllegalArgumentException, "Item not in Container.");
        }
    }

    void Container::removeItem(Item* item)
    {
        ItemSet::iterator it = mItems.find(item);
        if (it != mItems.end())
        {
            item->removeOldState();
            item->setScene("");
        }
        else
        {
            Throw(IllegalArgumentException, "Item not in Container.");
        }
    }

	int Container::getItemCount() const
	{
		return mItems.size();
	}

    bool Container::canPlaceAt(Item* item, int xPos, int yPos) const
	{
		int xSize = item->getSize().first;
		int ySize = item->getSize().second;

		bool free = true;

        // Es wird versucht, das Item ausserhalb des Containers zu platzieren
        if (xPos + xSize > mVolume.first
            || yPos + ySize > mVolume.second)
        {
            return false;
        }

		for (int x = xPos; x < (xPos + xSize); ++x)
        {
			for (int y = yPos; y < (yPos + ySize); ++y)
            {
                LOG_DEBUG2(Logger::RULES,
					Ogre::String("Checking Point in Backpack: Point x:")
					+ Ogre::StringConverter::toString(x)
					+ ", Point y:"
					+ Ogre::StringConverter::toString(y),
                    "Container::canPlaceAt()");

                Item* itemFound = getItemAt(x, y);
                if (itemFound == NULL)
                    continue;
                if (itemFound == item)
                    continue;

				return false;
			}
		}

		return true;
	}

    void Container::setItemPosition(Item* item, int xPos, int yPos)
    {
        if (mItemPositions.find(item) == mItemPositions.end())
        {
            Throw(IllegalArgumentException, "Item not in Container.");
        }

        mItemPositions[item] = make_pair(xPos, yPos);
    }

	IntPair Container::getItemPosition(Item* item) const
    {
		std::map<Item*, IntPair>::const_iterator it =
			mItemPositions.find(item);
        if (it == mItemPositions.end())
        {
            Throw(IllegalArgumentException, "Item not in Container.");
        }

		return (*it).second;
    }

    IntPair Container::findPositionWithEnoughSpace(IntPair space) const
    {
		for (int x = 0; x < mVolume.first; x++)
        {
			for (int y = 0; y < mVolume.second; y++)
            {
				if (isFree(x, y) && checkSpace(x, y, space))
                {
					return make_pair(x,y);
				}
			}
		}
		return NO_SPACE_FOR_ITEM;
	}

    bool Container::checkSpace(int xStart, int yStart, pair<int,int> space) const
    {
		// Falls Kaestchen nicht mehr im Rucksack, ist auch kein Platz mehr :)
        if ((xStart+space.first) > mVolume.first
            || (yStart+space.second) > mVolume.second)
        {
			return false;
		}

		for (int x = 0; x < space.first; x++)
        {
			for (int y = 0; y < space.second; y++)
            {
                if (!isFree(xStart+x, yStart+y))
                {
                    return false;
                }
			}
		}
		return true;
	}

    bool Container::isFree(int x, int y) const
    {
        return getItemAt(x, y) == NULL;
    }

    Item* Container::getItemAt(int x, int y) const
    {
        for (map<Item*, IntPair >::const_iterator iter = mItemPositions.begin();
            iter != mItemPositions.end(); iter++)
        {
            Item* item = (*iter).first;
            IntPair pos = (*iter).second;

            if (pos.first <= x
                && pos.second <= y
                && x < pos.first + item->getSize().first
                && y < pos.second + item->getSize().second)
            {
                return item;
            }
        }

        return NULL;
    }

    Ogre::Real Container::getMass() const
    {
		return Item::getMass() + getContentWeight();
    }

    void Container::setProperty(const CeGuiString& key, const Property& value)
    {
        if (key == Container::PROPERTY_CAPACITY)
        {
            if (value.isInt())
            {
                mCapacity = value.toInt();
            }
            else if (value.isReal())
            {
                mCapacity = value.toReal();
            }
        }
        else if (key == Container::PROPERTY_VOLUME)
        {
            mVolume = value.toIntPair();
        }
		else if (key == Container::PROPERTY_CONTENT)
        {
			PropertyMap contentMap = value.toMap();
			PropertyArray objects = contentMap[Container::PROPERTY_CONTENT_OBJECTS].toArray();
			PropertyArray positions = contentMap[Container::PROPERTY_CONTENT_POSITIONS].toArray();

			for (size_t idx = 0; idx < objects.size(); ++idx)
			{
				Property curObjProp = objects[idx];
				IntPair curObjPos = positions[idx].toIntPair();

				Item* cur = dynamic_cast<Item*>(
					GameObjectManager::getSingleton().createGameObjectFromProperty(curObjProp));

				addItem(cur, curObjPos);
			}
        }
        else
        {
            Item::setProperty(key, value);
        }
    }

    const Property Container::getProperty(const CeGuiString& key) const
    {
        if (key == Container::PROPERTY_CAPACITY)
        {
            return Property(mCapacity);
        }
        else if (key == Container::PROPERTY_VOLUME)
        {
            return Property(make_pair((int)mVolume.first, (int)mVolume.second));
        }
		else if (key == Container::PROPERTY_CONTENT)
        {
			PropertyArray positions;
			PropertyArray objects;

			for (ItemSet::const_iterator it = mItems.begin(); it != mItems.end(); ++it)
			{
				Item* cur = *it;
				IntPair pos = getItemPosition(cur);
				objects.push_back(GameObjectManager::getSingleton().toProperty(cur));
				positions.push_back(Property(pos));
			}

			PropertyMap contentMap;
			contentMap[Container::PROPERTY_CONTENT_OBJECTS] = objects;
			contentMap[Container::PROPERTY_CONTENT_POSITIONS] = positions;

			return Property(contentMap);
        }
        else
        {
            return Item::getProperty(key);
        }
    }

    PropertyKeys Container::getAllPropertyKeys() const
    {
        PropertyKeys keys(Item::getAllPropertyKeys());
        keys.insert(Container::PROPERTY_CAPACITY);
        keys.insert(Container::PROPERTY_VOLUME);
        keys.insert(Container::PROPERTY_CONTENT);
        return keys;
    }

    bool Container::canHold(Item* item) const
    {
        return findPositionWithEnoughSpace(item->getSize()) != NO_SPACE_FOR_ITEM;
    }

    void Container::setOwner(GameObject *go)
    {
        if( mOwner == go )
            return;

        mOwner = go;
        GameObject *newChildOwner = mOwner;
        if( newChildOwner == NULL )
            newChildOwner = this;

        ItemSet::iterator it;
        for( it = mItems.begin(); it != mItems.end(); it++ )
            (*it)->setOwner(newChildOwner);
    }
}
