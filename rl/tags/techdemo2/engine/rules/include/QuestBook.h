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

#ifndef __QUESTBOOK_H__
#define __QUESTBOOK_H__

#include "RulesPrerequisites.h"

#include "EventSource.h"
#include "EventCaster.h"

#include "JournalEntry.h"

#include "Quest.h"
#include "QuestEvent.h"
#include "QuestListener.h"

#include <vector>

namespace rl {

///@todo Rename this class to reflect scope changes.
class _RlRulesExport QuestBook : public EventSource
{
public:
	QuestBook();
	~QuestBook();


	/**
	 * Sucht einen Quest anhand einer ID
	 * @return der Quest
	 */
	Quest* getQuest(const CeGuiString id);

	QuestVector getTopLevelQuests();

	/**
	 * Fuegt einen Quest hinzu
	 * @param quest der Qubquest
	 */
	void addQuest(Quest* quest);

    /**
     *  Adds a journal entry to the quest book.
     */
    void addJournalEntry(JournalEntry* entry);
    void addJournalEntry(CeGuiString caption, CeGuiString text);

    unsigned int getNumJournalEntries() const;

    JournalEntry* getJournalEntry(unsigned int index) const;

	void addQuestListener(QuestListener* listener);
	void removeQuestListener(QuestListener* listener);

    void _fireQuestBookChanged(Quest* quest, int reason);

private:
	Quest* getQuest(Quest* parent, const CeGuiString id);
	Quest* mRootQuest;
    std::vector<JournalEntry*> mJournalEntries;
	EventCaster<QuestEvent> mQuestEventCaster;
	EventCaster<JournalEvent> mJournalEventCaster;

    void fireJournalChanged(JournalEntry* entry, int reason);
};

}
#endif
