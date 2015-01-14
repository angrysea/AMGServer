#include <array>

#include "XMLDOMWriter.h"
#include "XMLAttr.h"

void XMLDOMWriter::write(const std::shared_ptr<XMLNode> & node, bool bRootNode)
{
	if (node == nullptr)
	{
		return;
	}

	int type = node->getNodeType();
	switch (type) 
	{
		case NodeType::DOCUMENT_NODE:
		{
			if (canonical) 
			{
				std::vector<std::shared_ptr<XMLNode>> children = node->getChildNodes();
				{
					for (auto & childNode : children)
					{
						if (childNode->getNodeType() == NodeType::PROCESSING_INSTRUCTION_NODE) 
						{
							const std::string & name = childNode->getNodeName();
							const std::string & val = childNode->getNodeValue();
							xmlBuffer.append("<?");
							xmlBuffer.append(name);
							xmlBuffer.append(" ");
							xmlBuffer.append(val);
							xmlBuffer.append("?>");
						}
					}
				}
			}
			std::shared_ptr<XMLDocument> docNode(dynamic_cast<XMLDocument*>(node.get()));
			write(docNode->getDocumentElement(), true);
			break;
		}

		case NodeType::ELEMENT_NODE: 
		{
			xmlBuffer.append("<");
			xmlBuffer.append(node->getNodeName());
			int len = node->getAttributes().size();
			std::shared_ptr<std::shared_ptr<XMLAttr>> attrs = sortAttributes(node->getAttributes());
			if (bRootNode) 
			{
				xmlBuffer.append(" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\"");
			}

			std::shared_ptr<XMLAttr > * p_attrs = attrs.get();
			for (int i = 0; i < len; i++)
			{
				xmlBuffer.append(" ");
				xmlBuffer.append(p_attrs[i]->getNodeName());
				xmlBuffer.append("=\"");
				xmlBuffer.append(normalize(p_attrs[i]->getNodeValue()));
				xmlBuffer.append("\"");
			}

			xmlBuffer.append(">");
			for (auto & child : node->getChildNodes())
			{
				write(child, false);
			}
			break;
		}

		case NodeType::ENTITY_REFERENCE_NODE: 
		{
			if (canonical) 
			{
				for (auto & child : node->getChildNodes())
				{
					write(child, false);
				}
			}
			else 
			{
				xmlBuffer.append("&");
				xmlBuffer.append(node->getNodeName());
				xmlBuffer.append(";");
			}
			break;
		}

		case NodeType::CDATA_SECTION_NODE:
		case NodeType::TEXT_NODE: 
		{
			xmlBuffer.append(normalize(node->getNodeValue()));
			break;
		}

		case NodeType::PROCESSING_INSTRUCTION_NODE: 
		{
			xmlBuffer.append("<?");
			xmlBuffer.append(node->getNodeName());
			const std::string & data = node->getNodeValue();
			if (!data.empty())
			{
				xmlBuffer.append(" ");
				xmlBuffer.append(data);
			}
			xmlBuffer.append("?>");
			break;
		}
	}

	if (type == NodeType::ELEMENT_NODE) 
	{
		xmlBuffer.append("</");
		xmlBuffer.append(node->getNodeName());
		xmlBuffer.append(">");
	}
}

const std::string & XMLDOMWriter::normalize(const std::string & s) const
{
	std::string str;

	for(auto ch : s) 
	{
		switch (ch) 
		{
			case '<': 
			{
				str.append("&lt;");
				break;
			}
			case '>': 
			{
				str.append("&gt;");
				break;
			}
			case '&': 
			{
				str.append("&amp;");
				break;
			}
			case '"': 
			{
				str.append("&quot;");
				break;
			}
			case '\r':
			case '\n': 
			{
				if (canonical) 
				{
					str.append("&#");
					str.append(std::to_string(ch));
					str.append(";");
					break;
				}
			}
			default: 
			{
				str.append(std::to_string(ch));
			}
		}
	}
	return str;
}

std::shared_ptr<std::shared_ptr<XMLAttr>> XMLDOMWriter::sortAttributes(std::vector<std::shared_ptr<XMLAttr>> & attrs) const
{
	int len = attrs.size();
	std::shared_ptr<std::shared_ptr<XMLAttr>> array(new std::shared_ptr<XMLAttr>[len]);
	std::shared_ptr<XMLAttr > * p_attrs = array.get();
	for (int i = 0; i < len; i++)
	{
		p_attrs[i] = attrs[i];
	}

	for (int i = 0; i < len - 1; i++)
	{
		std::string name = p_attrs[i]->getNodeName();
		int index = i;
		for (int j = i + 1; j < len; j++) 
		{
			const std::string & curName = p_attrs[j]->getNodeName();
			if (curName.compare(name) < 0) 
			{
				name = curName;
				index = j;
			}
		}

		if (index != i) 
		{
			std::shared_ptr<XMLAttr> temp = p_attrs[i];
			p_attrs[i] = p_attrs[index];
			p_attrs[index] = temp;
		}
	}

	return array;
}

