#pragma once

#include <string>
#include <functional>
#include <algorithm>
#include <cctype>

inline bool equalsIgnoreCase(const std::string & value, const char * lower)
{
	std::string lowerValue;
	lowerValue.resize(value.length());
	std::transform(value.begin(), value.end(), lowerValue.begin(), std::ptr_fun<int, int>(std::tolower));
	return lowerValue == lower;
}

