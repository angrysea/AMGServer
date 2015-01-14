#pragma once

#include <string>
#include <istream>
#include <exception>

#include "../entityobjects/DataArray.h"

class IXMLOutputSerializer 
{
public:
	/**
	* This method will generate XML from the incoming object
	* @param o The object to marshal XML out of
	* @return This is the string containing the generated XML
	* @throws Exception
	*/
	virtual const std::string & get(DATAARRAY * o) = 0;

	/**
	* This method will generate XML from the incoming object
	* @param o The object to marshal XML out of
	* @param bXMLPI This bool indicates whether proccessing
	* instructions should be included in the XML
	* @return This is the string containing the generated XML
	* @throws Exception
	*/
	virtual const std::string & get(DATAARRAY * o, bool bXMLPI) = 0;

	/**
	* This method will generate XML from the incoming object
	* @param o The object to marshal XML out of
	* @param bXMLPI This bool indicates whether proccessing
	* instructions should be included in the XML
	* @param agentPIAttribs This is a string containing additional
	* proccessing instructions that may be nessary for various clients
	* or server platforms
	* @return This is the string containing the generated XML
	* @throws Exception
	*/
	virtual const std::string & get(DATAARRAY o, bool bXMLPI, const std::string & agentPIAttribs) = 0;

	/**
	* This method will return the last error if one has occured during
	* the XML marshalling
	* @return The exception that had been throw during marshalling
	*/
	virtual std::exception getLastError() = 0;

	/**
	* This method will indicate if there was an error after the last
	* marshalling process.
	* @return true or false indicating whether an error has occured or not
	*/
	virtual bool hasError() = 0;  
};