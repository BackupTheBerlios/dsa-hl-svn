#ifndef __UiPrerequisites_H__
#define __UiPrerequisites_H__

#include "RastullahPrerequisites.h"

#undef _assert

#if OGRE_PLATFORM == PLATFORM_WIN32
// Export control
#   if defined( RLUI_EXPORTS )
#       define _RlUiExport __declspec( dllexport )
#   else
#       define _RlUiExport __declspec( dllimport )
#   endif
#else // Linux / Mac OSX etc
#   define _RlUiExport
#endif


#include <vector>
#include <CEGUIString.h>

typedef CEGUI::String CeGuiString;

namespace rl
{
    typedef std::vector<CeGuiString> StringVector;
}

#endif
