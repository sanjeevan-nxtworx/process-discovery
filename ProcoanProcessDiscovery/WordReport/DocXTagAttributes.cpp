#include "stdafx.h"
#include "DocXTagAttributes.h"


DocXTagAttributes::DocXTagAttributes(string key, string value)
{
	strKey = key;
	strValue = value;
}


DocXTagAttributes::~DocXTagAttributes()
{
}

string DocXTagAttributes::GetKey()
{
	return strKey;
}

string DocXTagAttributes::GetValue()
{
	return strValue;
}
