// TestHarness.cpp : Defines the entry point for the console application.
//
#include "../xmlconverter/XmlSchemaLoader.h"

int main(int argc, char * argv[])
{
	if (argc > 2)
	{

		bool cacheable = true;
		if (argc == 4)
		{
			cacheable = strcmp(argv[3], "true");
		}

		XmlSchemaLoader loader;

		loader.generateClasses(
			/* rootDirectory */argv[1],
			/* schemaFileName */argv[2],
			cacheable);
	}
	return 0;
}

