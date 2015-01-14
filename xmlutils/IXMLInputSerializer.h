#pragma once

#include <string>
#include <istream>
#include <exception>

#include "../entityobjects/DataArray.h"

class IXMLInputSerializer
{
public:
	/**
	 * Marshals the incoming XML to the appropriate object
	 * @param is Source of XML
	 * @return The object created from the incoming XML
	 * @throws Exception
	 */
	virtual DATAARRAY * get(std::istream * is) = 0;

	/**
   * Marshals the incoming XML to the appropriate object
   * @param in Source of XML
   * this object.
   * @return The object created from the incoming XML
   * @throws Exception
   */
	virtual DATAARRAY * get(const std::string & in) = 0;

	/**
   * This methods allows you to set the repository package name which
   * will be used to later locate the classes.
   * @param newValue
   */
	virtual void setPackage(const std::string & newValue) = 0;

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