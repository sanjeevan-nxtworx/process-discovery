#include "stdafx.h"
#include "Tokenizer.h"
#include "Expression.h"
#include <cctype>

Tokenizer::Tokenizer(char* data) : str(std::string(data)) 
{
}

NumberExpression* Tokenizer::number()
{
	this->skipWhiteSpace();
	char ch = str.peek();
	if (std::isdigit(ch)) {
		int x;
		str >> x;
		return DBG_NEW NumberExpression(x);
	}
	return nullptr;
}

BooleanExpression* Tokenizer::boolConstant()
{
	std::string strToken = token();
	this->skipWhiteSpace();
	if (strToken == "true")
	{
		return DBG_NEW BooleanExpression(true);
	}
	else if (strToken == "false")
	{
		return DBG_NEW BooleanExpression(false);
	}
	return nullptr;
}

LabelExpression* Tokenizer::label()
{
	std::string strLabel = token();
	return DBG_NEW LabelExpression(strLabel);
}


StringExpression* Tokenizer::stringConstant()
{
	std::string strConstant;

	this->skipWhiteSpace();
	char ch = str.peek();
	if (ch == '\"')
	{
		ch = str.get();
		ch = str.get();
		while (ch != '\"' && !atEnd())
		{
			strConstant = strConstant + ch;
			ch = str.get();
		}
		if (!atEnd())
		{
			return DBG_NEW StringExpression(strConstant);
		}
	}

	return nullptr;
}

bool Tokenizer::character(char expected) {
	this->skipWhiteSpace();
	char ch = str.peek();
	if (ch == expected) {
		ch = str.get();
		return true;
	}
	return false;
}

std::string Tokenizer::token()
{
	std::string strToken;

	this->skipWhiteSpace();
	char ch = str.peek();
	while (!delimiter(ch) && !atEnd())
	{
		ch = str.get();
		strToken = strToken + ch;
		ch = str.peek();
	}
	return strToken;
}

bool Tokenizer::delimiter(char data)
{
	switch (data)
	{
	case '.':
	case '+':
	case '-':
	case '*':
	case '/':
	case '\r':
	case '\n':
	case '\t':
	case '\"':
	case '\'':
	case '[':
	case '(':
	case ')':
	case ' ':
	case -1:
		return true;
	default:
		break;
	}

	return false;
}


int Tokenizer::mark() {
	return str.tellg();
}

bool Tokenizer::atEnd() {
	return str.eof();
}

void Tokenizer::reset(int mark) {
	str.clear();
	str.seekg(mark);
}

void Tokenizer::skipWhiteSpace() {
	char ch = str.peek();
	while (ch == ' ' || ch == '\t') {
		ch = str.get();
		ch = str.peek();
	}
}
