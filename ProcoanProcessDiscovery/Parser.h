#pragma once
class Expression;
class Tokenizer;

class Parser
{
public:
	Parser(char* line);
	~Parser();

	Expression* statement();
private:
	Tokenizer* tokens;

	Expression* calculation();
	Expression* sum();
	Expression* product();
	Expression* group();
	Expression* unary();
	Expression* postfix();
	Expression* primary();
	Expression* logicalOR();
	Expression* logicalAND();
	Expression* relational();
};
