#pragma once

#include "pugixml.hpp"
#include <list>
#include <map>
#include <string>
#include "Expression.h"
#include "MMHeader.h"

using namespace std;
using namespace pugi;

struct  AnnotationRule
{
	string ruleName;
	int ruleLevel;
	string rule;
	string trueValueExpression;
	string falseValueExpression;
	string trueNextRule;
	string falseNextRule;
	Expression *expRule;
	Expression *expTrueValue;
	Expression *expFalseValue;
	Expression *expTrueNextRule;
	Expression *expFalseNextRule;
};

class AnnotationRuleDefinitions
{
	xml_document doc;
	list <struct AnnotationRule *> mainRules;
	map <string, struct AnnotationRule *> allRules;

public:
	AnnotationRuleDefinitions();
	void ReadRules(char *fileName);
	void ParseRules();
	string GetAnnotationForEvent(struct eventDetails *pEvent, map <ULONG, struct widgetDetails *> *pWidgetMap);
	string GetAdditionalInfo(struct eventDetails *pEvent, map <ULONG, struct widgetDetails *> *pWidgetMap);
	bool EvaluateRule(struct AnnotationRule *pRule, struct eventDetails *pEvent, struct widgetDetails *pWidget, map <ULONG, struct widgetDetails *> *pWidgetMap, string *strExpResult);
	bool EvaluateBoolExpression(Expression **ppExp, string strExp, struct eventDetails *pEvent, struct widgetDetails *pWidget, map <ULONG, struct widgetDetails *> *pWidgetMap);
	string EvaluateStringExpression(Expression **ppExp, string strExp, struct eventDetails *pEvent, struct widgetDetails *pWidget, map <ULONG, struct widgetDetails *> *pWidgetMap);
	~AnnotationRuleDefinitions();
};

