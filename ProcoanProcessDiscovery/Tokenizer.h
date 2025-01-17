#pragma once
#include <sstream>

class NumberExpression;
class VariableExpression;
class StringExpression;
class BooleanExpression;
class LabelExpression;


class Tokenizer
{
public:
	Tokenizer(char* data);

	NumberExpression* number();
	VariableExpression* variable();
	StringExpression* stringConstant();
	BooleanExpression* boolConstant();
	LabelExpression *label();
	bool character(char expected);
	bool delimiter(char data);
	std::string token();
	bool atEnd();

	int mark();
	void reset(int mark);
private:
	std::stringstream str;

	void skipWhiteSpace();
};
