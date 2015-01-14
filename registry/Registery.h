#pragma once
#include "RegFile.h"

class RegisteryNode;

class Registery
{
public:
	Registery(std::string & fileName) ;

	void insertKey(char * regLine);
	void insertValue(std::string & regLine);


	static Registery * open(std::string & fileName)
	{
		if(registery!=0)
		{
			delete registery;
		}
		
		registery = new Registery(fileName);
		registery->openRegistery();

		return registery;
	}

	static void close()
	{
		if(registery!=0)
		{
			delete registery;
		}
	}

	static RegisteryNode * getRootNode()
	{
		return registery->root;
	}

	int saveRegistery();

private:
	Registery();
	int openRegistery();

private:
	RegisteryNode * root;
	RegisteryNode * current;
	std::string & fileName;
	RegFile regFile;
	static Registery * registery;
};
