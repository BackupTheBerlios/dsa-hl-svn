#ifndef __CeConsole_H__
#define __CeConsole_H__

#include "UiPrerequisites.h"

#include <vector>
#include <string>

#include "CeGuiWindow.h"

#include <OgreSingleton.h>

namespace rl {

    class Interpreter;

	/** This is a Console System
	 */
	class _RlUiExport CeConsole : public Singleton<CeConsole>, public CeGuiWindow
	{
		public:
			CeConsole();
			
			static CeConsole& getSingleton();
			static CeConsole* getSingletonPtr();

			void setInterpreter(Interpreter* interpreter);
			Interpreter* getInterpreter();

			void write(String output);
				
		protected:
			bool beforeShow();

		private:
			CEGUI::Editbox* mCommandLine;
			CEGUI::Listbox* mDisplay;
			
			Interpreter* mInterpreter;
			bool mPrompt;

			bool handleKeyDown(const CEGUI::EventArgs& e);
			bool handleEnter(const CEGUI::EventArgs& e);
			void appendTextRow(CeGuiString& text, const CEGUI::colour color);

			std::vector<std::string> mHistory;
			int mHistoryMarker;
			void cycleHistory(int skip);
	};
}

#endif
