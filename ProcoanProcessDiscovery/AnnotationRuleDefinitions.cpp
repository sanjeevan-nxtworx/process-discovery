#include "stdafx.h"
#include "AnnotationRuleDefinitions.h"
#include "Parser.h"
#include "Utility.h"

/****************************************************************************************************************
Constant Name						Value		Description
UIA_ButtonControlTypeId				50000	Identifies the Button control type.
UIA_CalendarControlTypeId			50001	Identifies the Calendar control type.
UIA_CheckBoxControlTypeId			50002	Identifies the CheckBox control type.
UIA_ComboBoxControlTypeId			50003	Identifies the ComboBox control type.
UIA_EditControlTypeId				50004	Identifies the Edit control type.
UIA_HyperlinkControlTypeId			50005	Identifies the Hyperlink control type.
UIA_ImageControlTypeId				50006	Identifies the Image control type.
UIA_ListItemControlTypeId			50007	Identifies the ListItem control type.
UIA_ListControlTypeId				50008	Identifies the List control type.
UIA_MenuControlTypeId				50009	Identifies the Menu control type.
UIA_MenuBarControlTypeId			50010	Identifies the MenuBar control type.
UIA_MenuItemControlTypeId			50011	Identifies the MenuItem control type.
UIA_ProgressBarControlTypeId		50012	Identifies the ProgressBar control type.
UIA_RadioButtonControlTypeId		50013	Identifies the RadioButton control type.
UIA_ScrollBarControlTypeId			50014	Identifies the ScrollBar control type.
UIA_SliderControlTypeId				50015	Identifies the Slider control type.
UIA_SpinnerControlTypeId			50016	Identifies the Spinner control type.
UIA_StatusBarControlTypeId			50017	Identifies the StatusBar control type.
UIA_TabControlTypeId				50018	Identifies the Tab control type.
UIA_TabItemControlTypeId			50019	Identifies the TabItem control type.
UIA_TextControlTypeId				50020	Identifies the Text control type.
UIA_ToolBarControlTypeId			50021	Identifies the ToolBar control type.
UIA_ToolTipControlTypeId			50022	Identifies the ToolTip control type.
UIA_TreeControlTypeId				50023	Identifies the Tree control type.
UIA_TreeItemControlTypeId			50024	Identifies the TreeItem control type.
UIA_CustomControlTypeId				50025	Identifies the Custom control type.For more information, see Custom Properties, Events, and Control Patterns.
UIA_GroupControlTypeId				50026	Identifies the Group control type.
UIA_ThumbControlTypeId				50027	Identifies the Thumb control type.
UIA_DataGridControlTypeId			50028	Identifies the DataGrid control type.
UIA_DataItemControlTypeId			50029	Identifies the DataItem control type.
UIA_DocumentControlTypeId			50030	Identifies the Document control type.
UIA_SplitButtonControlTypeId		50031	Identifies the SplitButton control type.
UIA_WindowControlTypeId				50032	Identifies the Windows control Type
UIA_PaneControlTypeId				50033	Identifies the Pane control type.
UIA_HeaderControlTypeId				50034	Identifies the Header control type.
UIA_HeaderItemControlTypeId			50035	Identifies the HeaderItem control type.
UIA_TableControlTypeId				50036	Identifies the Table control type.
UIA_TitleBarControlTypeId			50037	Identifies the TitleBar control type.
UIA_SeparatorControlTypeId			50038	Identifies the Separator control type.
UIA_SemanticZoomControlTypeId		50039	Identifies the SemanticZoom control type.Supported starting with Windows 8.
UIA_AppBarControlTypeId				50040	Identifies the AppBar control type.Supported starting with Windows 8.1.
*****************************************************************************************************************/

AnnotationRuleDefinitions::AnnotationRuleDefinitions()
{

}


AnnotationRuleDefinitions::~AnnotationRuleDefinitions()
{
	map <string, struct AnnotationRule *>::iterator itRules = allRules.begin();

	while (itRules != allRules.end())
	{
		struct AnnotationRule *pRule = itRules->second;
		if (pRule->expRule != NULL)
			delete pRule->expRule;
		if (pRule->expFalseValue != NULL)
			delete pRule->expFalseNextRule;
		if (pRule->expTrueValue != NULL)
			delete pRule->expTrueValue;
		if (pRule->expFalseNextRule != NULL)
			delete pRule->expFalseNextRule;
		if (pRule->expTrueNextRule != NULL)
			delete pRule->expTrueNextRule;

		delete pRule;
		itRules++;
	}

	allRules.clear();
	mainRules.clear();
}

void AnnotationRuleDefinitions::ParseRules()
{
	map <string, struct AnnotationRule *>::iterator itRules = allRules.begin();

	while (itRules != allRules.end())
	{
		struct AnnotationRule *pRule = itRules->second;
		Parser p((char *)(pRule->rule.c_str()));
		pRule->expRule = p.statement();
		itRules++;
	}
}

string AnnotationRuleDefinitions::GetAdditionalInfo(struct eventDetails *pEvent, map <ULONG, struct widgetDetails *> *pWidgetMap)
{
	string strResult = "";
	struct widgetDetails *pWidget = NULL;

	map<ULONG, struct widgetDetails *>::iterator itWidget = pWidgetMap->find(pEvent->elementID);
	if (itWidget == pWidgetMap->end())
		return strResult;

	pWidget = itWidget->second;
	char str[100];
	sprintf_s(str, "\"CType\": %d,\n", pWidget->controlType);
	strResult += "\"PName\": \"" + EscapeJSonAnnotationString(pWidget->processName) + "\",\n";
	strResult += str;
	strResult += "\"Class\": \"" + pWidget->className + "\",\n";

	int parentID = pWidget->parentID;
	while (parentID != 0)
	{
		itWidget = pWidgetMap->find(parentID);
		if (itWidget == pWidgetMap->end())
			break;
		pWidget = itWidget->second;
		if (pWidget->controlType == UIA_WindowControlTypeId)
		{
			string name = "";
			map<int, string>::iterator itName = pWidget->nameMap.find(pEvent->elementID);
			if (itName != pWidget->nameMap.end())
			{
				name = itName->second;
			}
			strResult += "\"WName\": \"" + EscapeJSonAnnotationString(name) + "\",\n";
			sprintf_s(str, "\"WRect\": {\"Left\": %d, \"Top\": %d, \"Right\": %d, \"Bottom\": %d},\n", pWidget->boundingRect.left, pWidget->boundingRect.top, pWidget->boundingRect.right, pWidget->boundingRect.bottom);
			strResult += str;
			break;
		}
		parentID = pWidget->parentID;
	}

	return strResult;
}

string AnnotationRuleDefinitions::GetAnnotationForEvent(struct eventDetails *pEvent, map <ULONG, struct widgetDetails *> *pWidgetMap)
{
	string strResult = "";
	struct widgetDetails *pWidget = NULL;
	if (pEvent->elementID > 1)
	{
		map<ULONG, struct widgetDetails *>::iterator itWidget = pWidgetMap->find(pEvent->elementID);
		if (itWidget == pWidgetMap->end())
			return strResult;
		pWidget = itWidget->second;
	}
	if (pWidget == NULL && (pEvent->nEventCode != 21 && pEvent->nEventCode != 22))
		return strResult;
	//int elementID = pEvent->elementID;
	//while (true)
	//{
	//	itWidget = pWidgetMap->find(elementID);
	//	pWidget = itWidget->second;
	//	if (pWidget->nameMap.size() == 0)
	//		elementID = pWidget->parentID;
	//	else
	//		break;
	//	if (elementID == 0)
	//		break;
	//}
	//pEvent->elementID = elementID;
	// Go through each rule matrix for each level 1 rule
	list <struct AnnotationRule *>::iterator itMainRules = mainRules.begin();
	while (itMainRules != mainRules.end())
	{
		struct AnnotationRule *pRule = *itMainRules;

		bool bDone = EvaluateRule(pRule, pEvent, pWidget, pWidgetMap, &strResult);
		if (bDone)
		{
			break;
		}
		itMainRules++;
	}

	return strResult;
}

bool AnnotationRuleDefinitions::EvaluateRule(struct AnnotationRule *pRule, struct eventDetails *pEvent, struct widgetDetails *pWidget, map <ULONG, struct widgetDetails *> *pWidgetMap, string *strExpResult)
{
	bool bValue = EvaluateBoolExpression(&pRule->expRule, pRule->rule, pEvent, pWidget, pWidgetMap);
	string strResult = "";

	if (bValue)
	{
		// If rule evaluates to True
		strResult = EvaluateStringExpression(&pRule->expTrueValue, pRule->trueValueExpression, pEvent, pWidget, pWidgetMap);
		if (strResult == "")
		{
			// if no true value expression then take the true path
			strResult = EvaluateStringExpression(&pRule->expTrueNextRule, pRule->trueNextRule, pEvent, pWidget, pWidgetMap);
			// this is the key to the next rule

			map <string, struct AnnotationRule *>::iterator itRule = allRules.find(strResult);
			if (itRule != allRules.end())
			{
				struct AnnotationRule *pNextRule = itRule->second;
				bValue = EvaluateRule(pNextRule, pEvent, pWidget, pWidgetMap, &strResult);
			}
		}
	}
	else
	{
		// If rule evaluates to False
		strResult = EvaluateStringExpression(&pRule->expFalseValue, pRule->falseValueExpression, pEvent, pWidget, pWidgetMap);
		if (strResult == "")
		{
			// if no false value expression then take the fase path
			strResult = EvaluateStringExpression(&pRule->expFalseNextRule, pRule->falseNextRule, pEvent, pWidget, pWidgetMap);
			// this is the key to the next rule

			map <string, struct AnnotationRule *>::iterator itRule = allRules.find(strResult);
			if (itRule != allRules.end())
			{
				struct AnnotationRule *pNextRule = itRule->second;
				bValue = EvaluateRule(pNextRule, pEvent, pWidget, pWidgetMap, &strResult);
			}
		}

	}
	*strExpResult = strResult;
	return bValue;

}

bool AnnotationRuleDefinitions::EvaluateBoolExpression(Expression **ppExp, string strExp, struct eventDetails *pEvent, struct widgetDetails *pWidget, map <ULONG, struct widgetDetails *> *pWidgetMap)
{
	Expression *pRule = *ppExp;
	bool bResult = false;

	if (pRule == NULL&& strExp != "")
	{
		Parser p((char *)strExp.c_str());

		*ppExp = p.statement();
	}

	if (*ppExp != NULL)
	{
		pRule = *ppExp;
		Value valRule = pRule->evaluate(pEvent, pWidget, pWidgetMap);
		if (valRule.GetType() == 3)
		{
			valRule.GetValue(&bResult);
		}
	}

	return bResult;
}

string AnnotationRuleDefinitions::EvaluateStringExpression(Expression **ppExp, string strExp, struct eventDetails *pEvent, struct widgetDetails *pWidget, map <ULONG, struct widgetDetails *> *pWidgetMap)
{
	Expression *pRule = *ppExp;
	string strResult = "";

	if (pRule == NULL && strExp != "")
	{
		Parser p((char *)strExp.c_str());

		*ppExp = p.statement();
	}

	if (*ppExp != NULL)
	{
		pRule = *ppExp;
		Value valRule = pRule->evaluate(pEvent, pWidget, pWidgetMap);
		if (valRule.GetType() == 2)
		{
			valRule.GetValue(&strResult);
		}
	}

	return strResult;
}


void AnnotationRuleDefinitions::ReadRules(char *fileName)
{

	xml_parse_result result = doc.load_file(fileName);

	xml_node root = doc.child("ARRuleSet");

	for (xml_node arRule = root.first_child(); arRule; arRule = arRule.next_sibling())
	{
		string name = arRule.name();
		struct AnnotationRule *arRuleRec = DBG_NEW struct AnnotationRule;
		arRuleRec->expRule = NULL;
		arRuleRec->expFalseValue = NULL;
		arRuleRec->expTrueValue = NULL;
		arRuleRec->expFalseNextRule = NULL;
		arRuleRec->expTrueNextRule = NULL;
		arRuleRec->falseNextRule = "";
		arRuleRec->falseValueExpression = "";
		arRuleRec->rule = "";
		arRuleRec->ruleLevel = 0;
		arRuleRec->trueNextRule = "";
		arRuleRec->ruleName = "";
		arRuleRec->trueValueExpression = "";

		for (xml_node arRuleProperties = arRule.first_child(); arRuleProperties; arRuleProperties = arRuleProperties.next_sibling())
		{
			name = arRuleProperties.name();
			string value = arRuleProperties.child_value();

			if (name == "RuleName")
				arRuleRec->ruleName = value;
			else if (name == "RuleLevel")
				arRuleRec->ruleLevel = atoi(value.c_str());
			else if (name == "Rule")
				arRuleRec->rule = value;
			else if (name == "TrueValue")
				arRuleRec->trueValueExpression = value;
			else if (name == "FalseValue")
				arRuleRec->falseValueExpression = value;
			else if (name == "TrueNextPath")
				arRuleRec->trueNextRule = value;
			else if (name == "FalseNextPath")
				arRuleRec->falseNextRule = value;
		}
		if (arRuleRec->ruleLevel == 1)
			mainRules.push_back(arRuleRec);
		allRules.insert(pair<string, struct AnnotationRule *>(arRuleRec->ruleName, arRuleRec));
	}
}

