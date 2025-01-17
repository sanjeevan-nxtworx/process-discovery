#pragma once

#include <string>
using namespace std;

class DocXTagAttributes
{
	string strKey;
	string strValue;

public:
	DocXTagAttributes(string key, string value);
	~DocXTagAttributes();
	string GetKey();
	string GetValue();
};

