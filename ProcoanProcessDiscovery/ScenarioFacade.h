#pragma once
#include <vector>
#include <string>
using namespace std;

class ScenarioDefintion;

class ScenarioFacade
{
	vector <ScenarioDefintion *> scenarioList;

public:
	ScenarioFacade(string jSonSenarioList);
	~ScenarioFacade();
	vector <string> GetScenarioNames();
	ScenarioDefintion *GetScenario(string value);
	void AddScenario(ScenarioDefintion *pScenario);
	string GetScenarioJSON();
};

