#pragma once

#include <vector>
#include <memory>

#include "export.h"
#include "XmlBase.h";
#include "ElementData.h";

class XMLCONVERTER_API ModelGroup :public XmlBase
{
public:
	ModelGroup() : XmlBase()
	{
	}

	void putElement(std::shared_ptr<ElementData> data) 
	{
		elems.push_back(data);
	}

	std::vector<std::shared_ptr<ElementData>> & getElments() 
	{
		return elems;
	}

private:
	std::vector<std::shared_ptr<ElementData>> elems;
};
