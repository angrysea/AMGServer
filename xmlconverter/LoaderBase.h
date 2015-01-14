#pragma once

#include <string>
#include <vector>
#include <memory>
#include <istream>
#include <exception>

#include "export.h"
#include "ElementType.h"

class XMLCONVERTER_API LoaderBase
{
public:
	virtual void xmlLoad(std::istream * inputstream) = 0;
	virtual void xmlLoad(const std::string & s) = 0;
	virtual std::shared_ptr<ElementType> getElementType(const std::string & type) = 0;
	virtual std::map<const std::string, std::shared_ptr<ElementType>> & getElementTypes() = 0;
	//virtual int start() = 0;
	//virtual void end() = 0;
	//virtual const std::string & next(bool externalizable) = 0;
};
