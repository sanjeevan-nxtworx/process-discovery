#pragma once

#include <string>
#include <map>
#include "MMHeader.h"

using namespace std;

union valueData
{
	int intVal;
	std::string *strVal;
	bool boolVal;
	void *objectPtr;
};

class Value
{
private:
	int ValueType; /* 1 - int, 2 - String 3-Bool 4-EventPtr, 5 - Control*/
	union valueData dataValue;
public:
	Value();
	Value(int value); // Type of Value
	Value(std::string val); // Type of Value
	Value(bool value); // Type of Value
	Value(struct eventDetails *ptr); // Event Ptr
	Value(struct widgetDetails *ptr); // Control and Window ptrs

	~Value();
	Value(const Value &val);
	void SetValue(int value);
	void SetValue(std::string value);
	void SetValue(bool value);
	void SetValue(struct eventDetails *ptr); // Event Ptr
	void SetValue(struct widgetDetails *ptr); // Control and Window ptrs
	Value& operator=(const Value&rhs);
	void GetValue(int *value);
	void GetValue(bool *value);
	void GetValue(std::string *value);
	void GetValue(struct eventDetails **ptr); // Event Ptr
	void GetValue(struct widgetDetails **ptr); // Control and Window ptrs
	int GetType();
};

class Expression
{
public:
	Expression();
	virtual ~Expression();
	virtual Value evaluate(struct eventDetails *pEvent, struct widgetDetails *pWidget, map <ULONG, struct widgetDetails *> *pWidgetMap, Value *argument = NULL) = 0;
};

class NumberExpression : public Expression
{
public:
	NumberExpression(int val);

	virtual Value evaluate(struct eventDetails *pEvent, struct widgetDetails *pWidget, map <ULONG, struct widgetDetails *> *pWidgetMap, Value *argument = NULL);
private:
	Value value;
};

class ArithmeticExpression : public Expression
{
public:
	ArithmeticExpression(char op, Expression *left, Expression *right);
	virtual ~ArithmeticExpression();

	virtual Value evaluate(struct eventDetails *pEvent, struct widgetDetails *pWidget, map <ULONG, struct widgetDetails *> *pWidgetMap, Value *argument = NULL);
private:
	Expression *left, *right;
	char op;
};

class LogicalExpression : public Expression
{
public:
	LogicalExpression(char op, Expression *left, Expression *right);
	virtual ~LogicalExpression();

	virtual Value evaluate(struct eventDetails *pEvent, struct widgetDetails *pWidget, map <ULONG, struct widgetDetails *> *pWidgetMap, Value *argument = NULL);
private:
	Expression *left, *right;
	char op;
};


class RelationalExpression : public Expression
{
public:
	RelationalExpression(char op, Expression *left, Expression *right);
	virtual ~RelationalExpression();

	virtual Value evaluate(struct eventDetails *pEvent, struct widgetDetails *pWidget, map <ULONG, struct widgetDetails *> *pWidgetMap, Value *argument = NULL);
private:
	Expression *left, *right;
	char op;
};

class UnaryExpression : public Expression
{
public:
	UnaryExpression(char op, Expression *right);
	virtual ~UnaryExpression();

	virtual Value evaluate(struct eventDetails *pEvent, struct widgetDetails *pWidget, map <ULONG, struct widgetDetails *> *pWidgetMap, Value *argument = NULL);
private:
	Expression *right;
	char op;
};


class BooleanExpression : public Expression
{
public:
	BooleanExpression(bool val);

	virtual Value evaluate(struct eventDetails *pEvent, struct widgetDetails *pWidget, map <ULONG, struct widgetDetails *> *pWidgetMap, Value *argument = NULL);
private:
	Value value;
};

class StringExpression : public Expression
{
public:
	StringExpression(std::string val);

	virtual Value evaluate(struct eventDetails *pEvent, struct widgetDetails *pWidget, map <ULONG, struct widgetDetails *> *pWidgetMap, Value *argument = NULL);
private:
	Value value;
};

class FunctionCallExpression : public Expression
{
public:
	FunctionCallExpression(Expression *left, Expression *right);
	virtual ~FunctionCallExpression();

	virtual Value evaluate(struct eventDetails *pEvent, struct widgetDetails *pWidget, map <ULONG, struct widgetDetails *> *pWidgetMap, Value *argument = NULL);
private:
	Expression *left, *right;
};


class ObjectReferenceExpression : public Expression
{
public:
	ObjectReferenceExpression(Expression *left, Expression *right);
	virtual ~ObjectReferenceExpression();
	virtual Value evaluate(struct eventDetails *pEvent, struct widgetDetails *pWidget, map <ULONG, struct widgetDetails *> *pWidgetMap, Value *argument = NULL);
	Value GetEventDetails(struct eventDetails *pLeftEvent, Expression *right, struct eventDetails *pEvent, struct widgetDetails *pWidget, map <ULONG, struct widgetDetails *> *pWidgetMap);
	Value GetWidgetDetails(struct widgetDetails *pLeftWidget, Expression *right, struct eventDetails *pEvent, struct widgetDetails *pWidget, map <ULONG, struct widgetDetails *> *pWidgetMap);
	string GetKeyEventDescriptor(struct eventDetails *pEvent, string strName);
private:
	Expression *left, *right;
	string GetControlName(struct eventDetails *pEvent, map <ULONG, struct widgetDetails *> *pWidgetMap);

};



class LabelExpression : public Expression
{
public:
	LabelExpression(std::string val);

	virtual Value evaluate(struct eventDetails *pEvent, struct widgetDetails *pWidget, map <ULONG, struct widgetDetails *> *pWidgetMap, Value *argument = NULL);
private:
	std::string value;
};


