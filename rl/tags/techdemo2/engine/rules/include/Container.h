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

#ifndef __CONTAINER_H__
#define __CONTAINER_H__

#include "RulesPrerequisites.h"

#include "Item.h"

namespace rl
{
    /// Behaelter f�r Items.
    class _RlRulesExport Container : public Item
    {
    public:
        Container(const CeGuiString name, const CeGuiString description);
        virtual ~Container(void);

        /// Fassungsvermoegen in Unzen
        int getCapacity() const;

        /// Fassungsvermoegen in Unzen
        void setCapacity(int capacity);

		// Volumen in x (breite) * y (hoehe)
		void setVolume(int x, int y);

		pair<int,int> getVolume();

        /// Liefert Gesamtgewicht des Inhalts.
        int getContentWeight() const;

        void addItem(Item* item);
        Item* removeItem(int itemId);

        ItemSet getItems() const;

		int getItemCount() const;

		const Item* getItem(int itemId) const;

		virtual void close() = 0;

    private:
        int mCapacity;
		pair<int,int> mVolume;
		
		// Speichert, wo die Items sich im Container befinden.
		// Speichert also die IDs der Objekte in die einzelnen Volumenfelder
		int objIDMap [1][1];

        ItemSet mItems;
    };
}

#endif //__CONTAINER_H__
