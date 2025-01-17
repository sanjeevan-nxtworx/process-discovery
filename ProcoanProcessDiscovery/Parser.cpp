#include "stdafx.h"
#include "Parser.h"
#include "Expression.h"
#include "Tokenizer.h"

Parser::Parser(char* line)
	: tokens(DBG_NEW Tokenizer(line))
{
}

Parser::~Parser()
{
	delete tokens;
}

Expression* Parser::statement() {
	Expression *exp = nullptr;
	exp=calculation();
	return exp;
}

Expression* Parser::calculation()
{
	Expression *result;
	//int mark = tokens->mark();
	result = logicalOR();
	if (result != NULL)
		return result;
	delete result;
	result = nullptr;
	return result;
}

Expression* Parser::logicalOR()
{
	int mark = tokens->mark();
	Expression *lhs = logicalAND();
	Expression *rhs;
	while (lhs) {
		int mark2 = tokens->mark();
		if (tokens->token() == "OR") {
			rhs = logicalAND();
			if (rhs != NULL)
				lhs = DBG_NEW LogicalExpression('|', lhs, rhs);
			else {
				delete lhs;
				lhs = nullptr;
			}
		}
		else
		{
			tokens->reset(mark2);
			break;
		}
	}
	if (lhs == nullptr)
		tokens->reset(mark);
	return lhs;
}

Expression* Parser::logicalAND()
{
	int mark = tokens->mark();
	Expression *lhs = relational();
	Expression *rhs;
	while (lhs) {
		int mark2 = tokens->mark();
		if (tokens->token() == "AND") {
			if (rhs = relational())
				lhs = DBG_NEW LogicalExpression('&', lhs, rhs);
			else {
				delete lhs;
				lhs = nullptr;
			}
		}
		else
		{
			tokens->reset(mark2);
			break;
		}
	}
	if (lhs == nullptr)
		tokens->reset(mark);
	return lhs;
}

Expression* Parser::relational()
{
	int mark = tokens->mark();
	Expression *lhs = sum();
	Expression *rhs;
	while (lhs) {
		int mark2 = tokens->mark();
		std::string token = tokens->token();
		if (token == "GT" || token == "LT" || token == "GTE" || token == "LTE" || token == "EQ" || token == "NEQ") {
			if (rhs = sum())
			{
				if (token == "GT")
					lhs = DBG_NEW RelationalExpression('>', lhs, rhs);
				else if (token == "LT")
					lhs = DBG_NEW RelationalExpression('<', lhs, rhs);
				else if (token == "GTE")
					lhs = DBG_NEW RelationalExpression('M', lhs, rhs);
				else if (token == "LTE")
					lhs = DBG_NEW RelationalExpression('N', lhs, rhs);
				else if (token == "EQ")
					lhs = DBG_NEW RelationalExpression('S', lhs, rhs);
				else if (token == "NEQ")
					lhs = DBG_NEW RelationalExpression('T', lhs, rhs);
			}
			else {
				delete lhs;
				lhs = nullptr;
			}
		}
		else
		{
			tokens->reset(mark2);
			break;
		}
	}
	if (lhs == nullptr)
		tokens->reset(mark);
	return lhs;
}

Expression* Parser::sum()
{
	int mark = tokens->mark();
	Expression *lhs = product();
	Expression *rhs;

	while (lhs) {
		if (tokens->character('+')) {
			rhs = product();
			if (rhs != NULL)
				lhs = DBG_NEW ArithmeticExpression('+', lhs, rhs);
			else {
				delete lhs;
				lhs = nullptr;
			}
		}
		else if (tokens->character('-')) {
			rhs = product();
			if (rhs != NULL)
				lhs = DBG_NEW ArithmeticExpression('-', lhs, rhs);
			else {
				delete lhs;
				lhs = nullptr;
			}
		}
		else
			break;
	}

	if (lhs == nullptr)
		tokens->reset(mark);
	return lhs;
}

Expression* Parser::product()
{
	int mark = tokens->mark();
	Expression *lhs = unary();
	Expression *rhs;
	while (lhs) {
		if (tokens->character('*')) {
			rhs = unary();
			if (rhs != NULL)
				lhs = DBG_NEW ArithmeticExpression('*', lhs, rhs);
			else {
				delete lhs;
				lhs = nullptr;
			}
		}
		else if (tokens->character('/')) {
			rhs = unary();
			if (rhs != NULL)
				lhs = DBG_NEW ArithmeticExpression('/', lhs, rhs);
			else {
				delete lhs;
				lhs = nullptr;
			}
		}
		else
			break;
	}
	if (lhs == nullptr)
		tokens->reset(mark); 
	return lhs;
}

Expression* Parser::unary()
{
	bool bUnary = false;
	int mark = tokens->mark();
	std::string token = tokens->token();
	if (token == "NOT")
	{
		bUnary = true;
	}
	else
		tokens->reset(mark);
	Expression *rhs;
	if (rhs = postfix())
	{
		if (bUnary)
		{
			Expression *lhs = DBG_NEW UnaryExpression('!', rhs);
			return lhs;
		}
	}

	return rhs;
}

Expression* Parser::postfix()
{
	int mark = tokens->mark();
	Expression *lhs = primary();
	Expression *rhs;
	while (lhs) 
	{
		if (tokens->character('('))
		{
			rhs = logicalOR();
			if (rhs != NULL)
				lhs = DBG_NEW FunctionCallExpression(lhs, rhs);
			else {
				delete lhs;
				lhs = nullptr;
			}
		}
		else if (tokens->character('.')) 
		{
			rhs = primary();
			if (rhs != NULL)
			{
				lhs = DBG_NEW ObjectReferenceExpression(lhs, rhs);
			}
			else {
				delete lhs;
				lhs = nullptr;
			}
		}
		else
			break;
	}
	if (lhs == nullptr)
		tokens->reset(mark);
	return lhs;
}


Expression *Parser::primary()
{
	int mark = tokens->mark();

	Expression *rhs;
	if (rhs = group())
	{
		return rhs;
	}
	
	tokens->reset(mark);
	rhs = tokens->stringConstant();
	if (rhs != NULL)
		return rhs;

	tokens->reset(mark);
	rhs = tokens->number();
	if (rhs != NULL)
		return rhs;

	tokens->reset(mark);

	rhs = tokens->boolConstant();
	if (rhs != NULL)
		return rhs;

	tokens->reset(mark);

	rhs = tokens->label();
	if (rhs != NULL)
		return rhs;
	return nullptr;

}

Expression* Parser::group() {
	int mark = tokens->mark();
	Expression *exp = nullptr;
	if (tokens->character('('))
	{
		exp = logicalOR();
		if (exp != NULL)
			tokens->character(')');
	}
	else {
		exp = nullptr;
		tokens->reset(mark);
	}
	return exp;
}
