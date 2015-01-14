#include "NameMangler.h"


int main(char * args[])
{
	NameMangler nameMangler;
	std::string vals[] = { "-.-..:", "Tag-1", "T-a:g.1", "Tag_1D$", "A1234356544334--x" };
	std::string vals2[] = { "Tag_1$003C$", "Tag_1", "Tag-1", "Tag$$", "Tag$1$", "Tag$1E$" };

	std::string str;
	std::string result;
	std::string result2;

	std::cout << "*** Encode Test ***" << std::endl;
	for (auto c : vals)
	{
		str = c;
		result = nameMangler.encode(str);
		std::cout << str << " ==> " << result << std::endl;
		result2 = nameMangler.decode(result);
		std::cout << result << " ==> " << result2 << std::endl;
	}

	std::cout << "*** Decode Test ***" << std::endl;
	for (auto c : vals2)
	{
		str = c;
		result = nameMangler.decode(str);
		std::cout << str << " ==> " << result << std::endl;
	}

	result = "Tagname-1.2";
	result = nameMangler.encode(result);
	std::cout << result << std::endl;

	return 1;
}

