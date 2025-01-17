#include "stdafx.h"
#include "ScenarioFacade.h"
#include "ScenarioDefintion.h"
#include "json11.hpp"


using namespace json11;

ScenarioFacade::ScenarioFacade(string jSonSenarioList)
{
	string err;
	Json jSonElement;

	jSonElement = Json::parse(jSonSenarioList, err);
	vector <Json> prcScenarioDefs = jSonElement.array_items();
	for (int i = 0; i < prcScenarioDefs.size(); i++)
	{
		ScenarioDefintion *pScenarioDef = DBG_NEW ScenarioDefintion;

		Json scenarioDef = prcScenarioDefs[i];
		pScenarioDef->SetName(scenarioDef["Name"].string_value());
		pScenarioDef->SetDesc(scenarioDef["Desc"].string_value());
		vector <Json> pathDefs = scenarioDef["Paths"].array_items();
		for (int j = 0; j < pathDefs.size(); j++)
		{
			Json pathDef = pathDefs[j];
			ULONG nIndex = pathDef["Index"].int_value();
			short nPath = pathDef["Path"].int_value();
			pScenarioDef->SetPath(nIndex, nPath);
		}
		scenarioList.push_back(pScenarioDef);
	}


}


ScenarioFacade::~ScenarioFacade()
{
	vector <ScenarioDefintion *>::iterator itScenario = scenarioList.begin();

	while (itScenario != scenarioList.end())
	{
		ScenarioDefintion *pScenarioDef = *itScenario;

		delete pScenarioDef;
		itScenario++;
	}

	scenarioList.clear();
}

void ScenarioFacade::AddScenario(ScenarioDefintion *pScenario)
{
	scenarioList.push_back(pScenario);
}

string ScenarioFacade::GetScenarioJSON()
{
	string sceneJSON = "";

	vector <ScenarioDefintion *>::iterator itScene = scenarioList.begin();
	if (itScene == scenarioList.end())
		return sceneJSON;
	sceneJSON += "[";
	while (itScene != scenarioList.end())
	{
		ScenarioDefintion *pScenarioDefn = *itScene;
		if (itScene == scenarioList.begin())
			sceneJSON += "{";
		else
			sceneJSON += ",{";
		sceneJSON += "\"Name\": \"" + pScenarioDefn->GetName();
		sceneJSON += "\",";
		sceneJSON += "\"Paths\": [";
		map <ULONG, short> *pathMap = pScenarioDefn->GetPaths();
		map <ULONG, short>::iterator itPathMap = pathMap->begin();
		while (itPathMap != pathMap->end())
		{
			char str[100];
			ULONG nIndex = itPathMap->first;
			short nPath = itPathMap->second;
			if (itPathMap == pathMap->begin())
				sprintf_s(str, "{\"Index\":%d, \"Path\":%d}", nIndex, nPath);
			else
				sprintf_s(str, ",{\"Index\":%d, \"Path\":%d}", nIndex, nPath);
			sceneJSON += str;
			itPathMap++;
		}
		sceneJSON += "],";
		sceneJSON += "\"Desc\": \"" + pScenarioDefn->GetDesc();
		sceneJSON += "\"}";
		itScene++;
	}
	sceneJSON += "]";
	return sceneJSON;
}


vector <string> ScenarioFacade::GetScenarioNames()
{
	vector <string> names;
	names.clear();
	vector <ScenarioDefintion *>::iterator itScenario = scenarioList.begin();

	while (itScenario != scenarioList.end())
	{
		ScenarioDefintion *pScenarioDef = *itScenario;
		names.push_back(pScenarioDef->GetName());
		itScenario++;
	}

	return names;
}


ScenarioDefintion *ScenarioFacade::GetScenario(string value)
{
	vector <ScenarioDefintion *>::iterator itScenario = scenarioList.begin();

	while (itScenario != scenarioList.end())
	{
		ScenarioDefintion *pScenarioDef = *itScenario;
		if (pScenarioDef->GetName() == value)
			return pScenarioDef;
		itScenario++;
	}

	return NULL;

}