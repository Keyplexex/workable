#pragma once

#include <vector>
#include <memory>
#include <iostream>

#include "../lexer/lexer.h"
#include "ASTNode.h"

class Parser{
private:
	Lexer& m_lexer;			
	Token m_current_token;		
	Token m_previous_token;		

	void advance();											
	bool check(TokenType type) const;						
	bool match(TokenType type);									
	bool match(const std::vector<TokenType>& types);		
	void consume(TokenType type, const std::string& message);	
	bool isAtEnd() const;					

	
	std::unique_ptr<StatementNode> parseStatement();
	std::unique_ptr<BlockNode>     parseBlock();
	std::unique_ptr<StatementNode> parseIfStatement();
	std::unique_ptr<StatementNode> parseIfStatementInternal(Token ifOrElseToken, bool isElseIfContinuation); 
	std::unique_ptr<StatementNode> parseWhileStatement();
	std::unique_ptr<StatementNode> parseForStatement();
	std::unique_ptr<StatementNode> parseReturnStatement();
	std::unique_ptr<StatementNode> parseBreakStatement();
	std::unique_ptr<StatementNode> parseContinueStatement();
	std::unique_ptr<StatementNode> parsePossibleAssignmentOrExpressionStatement();

	// Expression parsing
	std::unique_ptr<ExpressionNode> parseExpression(int minPrecedence = 0);
	std::unique_ptr<ExpressionNode> parseUnary();
	std::unique_ptr<ExpressionNode> parsePostfixOperations();
	std::unique_ptr<ExpressionNode> parsePrimary();
	std::unique_ptr<ExpressionNode> parseCall(std::unique_ptr<ExpressionNode> callee);
	std::unique_ptr<ExpressionNode> parseFunctionLiteral();

	// Precedence Operator
	int getOperatorPrecedence(TokenType type);

	void error(const Token& error_token, const std::string& message_text);
	void synchronize();

public:
	explicit Parser(Lexer& lexer);
	std::unique_ptr<ProgramNode> parseProgram(); // entry
};