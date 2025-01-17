#include "stdafx.h"
#include "ScenarioDefintion.h"


ScenarioDefintion::ScenarioDefintion()
{
}


ScenarioDefintion::~ScenarioDefintion()
{
}


void ScenarioDefintion::SetName(string value)
{
	scenarioName = value;
}

void ScenarioDefintion::SetDesc(string value)
{
	scenarioDesc = value;
}

void ScenarioDefintion::SetPath(ULONG index, short value)
{
	map <ULONG, short>::iterator itPath = path.find(index);
	if (itPath != path.end())
	{
		itPath->second = value;
	}
	else
	{
		path.insert({ index,value });
	}

}

string ScenarioDefintion::GetName()
{
	return scenarioName;
}

string ScenarioDefintion::GetDesc()
{
	return scenarioDesc;
}

short ScenarioDefintion::GetPath(ULONG index)
{
	map <ULONG, short>::iterator itPath = path.find(index);
	if (itPath != path.end())
	{
		return itPath->second;
	}

	return 0; // default is down
}

map <ULONG, short> *ScenarioDefintion::GetPaths()
{
	return &path;
}


