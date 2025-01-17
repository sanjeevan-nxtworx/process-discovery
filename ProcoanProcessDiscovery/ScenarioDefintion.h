#pragma once
#include <map>
#include <string>
using namespace std;

class ScenarioDefintion
{
	string scenarioName;
	string scenarioDesc;

	map <ULONG, short> path;

public:
	ScenarioDefintion();
	~ScenarioDefintion();
	void SetName(string value);
	void SetDesc(string value);
	void SetPath(ULONG index, short value);

	string GetName();
	string GetDesc();
	short GetPath(ULONG index);
	map <ULONG, short> *GetPaths();
};

