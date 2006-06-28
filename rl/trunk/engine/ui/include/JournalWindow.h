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

#ifndef __JOURNALWINDOW_H__
#define __JOURNALWINDOW_H__

#include "UiPrerequisites.h"

#include "CeGuiWindow.h"
#include "QuestBookChangeListener.h"

namespace rl {

    class JournalWindow : public CeGuiWindow, public QuestBookChangeListener {
    public:
        JournalWindow();
        virtual ~JournalWindow();

        virtual void questStateChanged(QuestBookChangeEvent* anEvent);
        virtual void questPartsDoneChanged(QuestBookChangeEvent* anEvent);
        virtual void questKnownChanged(QuestBookChangeEvent* anEvent);
        virtual void questSubquestAdded(QuestBookChangeEvent* anEvent);
        virtual void journalEntryAdded(JournalEvent* anEvent);

    private:
        CEGUI::Listbox* mQuests;
        CEGUI::StaticText* mQuestTitle;
        CEGUI::StaticText* mQuestState;
        CEGUI::MultiLineEditbox* mQuestDescription;

        CEGUI::Listbox* mJournalEntries;
        CEGUI::StaticText* mJournalEntryTitle;
        CEGUI::MultiLineEditbox* mJournalEntryText;

        CeGuiString mSelectionBrush;
        CeGuiString mSelectionImageset;
        CEGUI::colour mSelectionColour;

        void updateQuests();
        void selectQuest(CEGUI::ListboxItem* item);
        bool updateQuestSelection();
        void addQuest(Quest* quest, int level = 0);

        void updateJournal();
        void selectJournalEntry(CEGUI::ListboxItem* item);
        bool updateJournalEntrySelection();
        void addJournalEntry(JournalEntry* entry);
    };
}

#endif //__JOURNALWINDOW_H__
