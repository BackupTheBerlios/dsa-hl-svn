#ifndef __XmlHelper_h__
#define __XmlHelper_h__

#include "CommonPrerequisites.h"

#include <xercesc/dom/DOMElement.hpp>

namespace rl {

class _RlCommonExport XmlHelper
{
public:
	static XERCES_CPP_NAMESPACE::DOMElement* getChildNamed(XERCES_CPP_NAMESPACE::DOMElement* parent, const char* name);
};

}

#endif