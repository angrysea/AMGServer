#pragma once

#include <string>

#include "export.h"

enum PARSER_API AttributeTypes
{
	UNKNOWN, ABSTRACT, ATTRIBUTEFORMDEFAULT, BASE, BLOCK, BLOCKDEFAULT, 
		ELEMENTFORMDEFAULT, FINAL, FINALDEFAULT, FIXED, FORM, ITEMTYPE, MEMBERTYPES,
		MAXOCCURS, MINOCCURS, MIXED, NAME, NAMESPACE, NONAMESPACESCHEMALOCATION,
		XSINULL, NULLABLE, PROCESSCONTENTS, REF, SCHEMALOCATION, XSISCHEMALOCATION,
		SUBSTITUTIONGROUP, TARGETNAMESPACE, TYPE, XSITYPE, USE, VALUE, XPATH
};

class PARSER_API Attribute
{
public:
	static std::string LNULLATTRIBUTE;
	static std::string LABSTRACT;
	static std::string LATTRIBUTEFORMDEFAULT;
	static std::string LBASE;
	static std::string LBLOCK;
	static std::string LBLOCKDEFAULT;
	static std::string LELEMENTFORMDEFAULT;
	static std::string LFINAL;
	static std::string LFINALDEFAULT;
	static std::string LFIXED;
	static std::string LFORM;
	static std::string LITEMTYPE;
	static std::string LMEMBERTYPES;
	static std::string LMAXOCCURS;
	static std::string LMINOCCURS;
	static std::string LMIXED;
	static std::string LNAME;
	static std::string LNAMESPACE;
	static std::string LNONAMESPACESCHEMALOCATION;
	static std::string LXSINULL;
	static std::string LNULLABLE;
	static std::string LPROCESSCONTENTS;
	static std::string LREF;
	static std::string LSCHEMALOCATION;
	static std::string LXSISCHEMALOCATION;
	static std::string LSUBSTITUTIONGROUP;
	static std::string LTARGETNAMESPACE;
	static std::string LTYPE;
	static std::string LXSITYPE;
	static std::string LUSE;
	static std::string LVALUE;
	static std::string LXPATH;

	Attribute(const std::string & type, const std::string & value) :
		value{ value },
		type{ convertType(type) }
	{
	}

	static short convertType(const std::string & t) 
	{
		short retType = UNKNOWN;

		if (t == LNAME)
			retType = NAME;
		else if (t == LREF)
			retType = REF;
		else if (t == LABSTRACT)
			retType = ABSTRACT;
		else if (t == LATTRIBUTEFORMDEFAULT)
			retType = ATTRIBUTEFORMDEFAULT;
		else if (t == LBASE)
			retType = BASE;
		else if (t == LBLOCK)
			retType = BLOCK;
		else if (t == LBLOCKDEFAULT)
			retType = BLOCKDEFAULT;
		else if (t == LELEMENTFORMDEFAULT)
			retType = ELEMENTFORMDEFAULT;
		else if (t == LFINAL)
			retType = FINAL;
		else if (t == LFINALDEFAULT)
			retType = FINALDEFAULT;
		else if (t == LFIXED)
			retType = FIXED;
		else if (t == LFORM)
			retType = FORM;
		else if (t == LITEMTYPE)
			retType = ITEMTYPE;
		else if (t == LMEMBERTYPES)
			retType = MEMBERTYPES;
		else if (t == LMAXOCCURS)
			retType = MAXOCCURS;
		else if (t == LMINOCCURS)
			retType = MINOCCURS;
		else if (t == LMIXED)
			retType = MIXED;
		else if (t == LNAMESPACE)
			retType = NAMESPACE;
		else if (t == LNONAMESPACESCHEMALOCATION)
			retType = NONAMESPACESCHEMALOCATION;
		else if (t == LXSINULL)
			retType = XSINULL;
		else if (t == LNULLABLE)
			retType = NULLABLE;
		else if (t == LPROCESSCONTENTS)
			retType = PROCESSCONTENTS;
		else if (t == LSCHEMALOCATION)
			retType = SCHEMALOCATION;
		else if (t == LXSISCHEMALOCATION)
			retType = XSISCHEMALOCATION;
		else if (t == LSUBSTITUTIONGROUP)
			retType = SUBSTITUTIONGROUP;
		else if (t == LTARGETNAMESPACE)
			retType = TARGETNAMESPACE;
		else if (t == LTYPE)
			retType = TYPE;
		else if (t == LXSITYPE)
			retType = XSITYPE;
		else if (t == LUSE)
			retType = USE;
		else if (t == LVALUE)
			retType = VALUE;
		else if (t == LXPATH)
			retType = XPATH;

		return retType;
	}

	static const std::string & convertType(short t) 
	{
		switch (t) 
		{
		case NAME:
			return LNAME;
		case REF:
			return LREF;
		case ABSTRACT:
			return LABSTRACT;
		case ATTRIBUTEFORMDEFAULT:
			return LATTRIBUTEFORMDEFAULT;
		case BASE:
			return LBASE;
		case BLOCK:
			return LBLOCK;
		case BLOCKDEFAULT:
			return LBLOCKDEFAULT;
		case ELEMENTFORMDEFAULT:
			return LELEMENTFORMDEFAULT;
		case FINAL:
			return LFINAL;
		case FINALDEFAULT:
			return LFINALDEFAULT;
		case FIXED:
			return LFIXED;
		case FORM:
			return LFORM;
		case ITEMTYPE:
			return LITEMTYPE;
		case MEMBERTYPES:
			return LMEMBERTYPES;
		case MAXOCCURS:
			return LMAXOCCURS;
		case MINOCCURS:
			return LMINOCCURS;
		case MIXED:
			return LMIXED;
		case NAMESPACE:
			return LNAMESPACE;
		case NONAMESPACESCHEMALOCATION:
			return LNONAMESPACESCHEMALOCATION;
		case XSINULL:
			return LXSINULL;
		case NULLABLE:
			return LNULLABLE;
		case PROCESSCONTENTS:
			return LPROCESSCONTENTS;
		case SCHEMALOCATION:
			return LSCHEMALOCATION;
		case XSISCHEMALOCATION:
			return LXSISCHEMALOCATION;
		case SUBSTITUTIONGROUP:
			return LSUBSTITUTIONGROUP;
		case TARGETNAMESPACE:
			return LTARGETNAMESPACE;
		case TYPE:
			return LTYPE;
		case XSITYPE:
			return LXSITYPE;
		case USE:
			return LUSE;
		case VALUE:
			return LVALUE;
		case XPATH:
			return LXPATH;
		default:
			return LNULLATTRIBUTE;
		}
	}

	const std::string & getValue() 
	{
		return value;
	}

	short getType() 
	{
		return type;
	}

private:
	short type;
	std::string value;
};
