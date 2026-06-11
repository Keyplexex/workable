#include "parser.h"

#include "errorManager.h"

Parser::Parser(Lexer& lexer)
	: m_lexer(lexer)
	, m_current_token(TokenType::tERROR, "", 0)
	, m_previous_token(TokenType::tERROR, "", 0)
{
	advance();
}

void Parser::advance(){
	m_previous_token = m_current_token;
	m_current_token = m_lexer.getNextToken();
}

bool Parser::check(TokenType type) const{
	if(isAtEnd()) return false;
	return m_current_token.type == type;
}

bool Parser::match(TokenType type){
	if(m_current_token.type == TokenType::tERROR && type != TokenType::tERROR){
		error(m_current_token, "Lexicalerror: " + m_current_token.lexema);
	}
	if(check(type)){
		advance();
		return true;
	}
	return false;
}

bool Parser::match(const std::vector<TokenType>& types){
	if(m_current_token.type == TokenType::tERROR){
		bool expectingError = false;
		for(TokenType t : types){
			if(t == TokenType::tERROR){
				expectingError = true;
				break;
			}
		}
		if(!expectingError) error(m_current_token, "Lexicalerror: " + m_current_token.lexema);
	}
	for(TokenType type : types){
		if(check(type)){
			advance();
			return true;
		}
	}
	return false;
}

void Parser::consume(TokenType type, const std::string& message){
	if(m_current_token.type == TokenType::tERROR && type != TokenType::tERROR){
		error(m_current_token, "Lexicalerror: " + m_current_token.lexema);
	}
	if(check(type)){
		advance();
		return;
	}
	error(m_current_token, message);
}

bool Parser::isAtEnd() const{
	return m_current_token.type == TokenType::tEOF;
}

void Parser::error(const Token& error_token, const std::string& message_text){
	std::string full_message;
	if(error_token.type == TokenType::tEOF){
		ErrorManager("SyntaxError at end", message_text, error_token.line);
	}
	else{
		ErrorManager("SyntaxError at '"+error_token.lexema+"'", message_text, error_token.line);
	}
}

std::unique_ptr<ProgramNode> Parser::parseProgram(){
	auto programNode = std::make_unique<ProgramNode>();
	programNode->line = m_current_token.line;

	while(!isAtEnd()){
		auto stmt = parseStatement();
		if(stmt){
			programNode->statements.push_back(std::move(stmt));
		}
	}
	return programNode;
}

std::unique_ptr<BlockNode> Parser::parseBlock(){
	int blockLine = m_current_token.line;
	auto block = std::make_unique<BlockNode>(blockLine);

	while(!isAtEnd() &&
			m_current_token.type != TokenType::tEndIf &&
			m_current_token.type != TokenType::tEndWhile &&
			m_current_token.type != TokenType::tEndFor &&
			m_current_token.type != TokenType::tEndFunc &&
			m_current_token.type != TokenType::tElse){
		auto stmt = parseStatement();
		if(stmt) block->statements.push_back(std::move(stmt));
	}

	return block;
}

std::unique_ptr<StatementNode> Parser::parseStatement(){
	switch(m_current_token.type){
		case TokenType::tIf: return parseIfStatement();
		case TokenType::tWhile: return parseWhileStatement();
		case TokenType::tFor: return parseForStatement();
		case TokenType::tReturn: return parseReturnStatement();
		case TokenType::tBreak: return parseBreakStatement();
		case TokenType::tContinue: return parseContinueStatement();
	}
	
	return parsePossibleAssignmentOrExpressionStatement();
}

std::unique_ptr<StatementNode> Parser::parsePossibleAssignmentOrExpressionStatement(){
	std::unique_ptr<ExpressionNode> expr = parseExpression();
	return std::make_unique<ExpressionStatementNode>(std::move(expr));
}

std::unique_ptr<StatementNode> Parser::parseIfStatementInternal(Token ifOrElseToken, bool isElseIfContinuation){
	if(isElseIfContinuation){
		consume(TokenType::tIf, "Expect 'if' after 'else' for an 'else if' construct.");
	}

	auto condition = parseExpression();
	consume(TokenType::tThen, "Expect 'then' after if condition.");
	auto thenBranch = parseBlock();
	std::unique_ptr<StatementNode> elseNode = nullptr;

	if(match(TokenType::tElse)){
		Token currentElseToken = m_previous_token;
		if(m_current_token.type == TokenType::tIf){
			elseNode = parseIfStatementInternal(currentElseToken, true);
		}
		else{
			elseNode = parseBlock();
		}
	}

	return std::make_unique<IfStatementNode>(std::move(condition), std::move(thenBranch), std::move(elseNode), ifOrElseToken.line);
}

std::unique_ptr<StatementNode> Parser::parseIfStatement(){
	Token ifToken = m_current_token;
	consume(TokenType::tIf, "Expect 'if'.");
	
	auto ifStmtNode = parseIfStatementInternal(ifToken, false);
	
	consume(TokenType::tEndIf, "Expect 'end' to close 'if/else if/else' statement chain.");
	return ifStmtNode;
}


std::unique_ptr<StatementNode> Parser::parseWhileStatement(){
	Token whileToken = m_current_token;
	consume(TokenType::tWhile, "Expect 'while'.");
	auto condition = parseExpression();
	auto body = parseBlock();
	consume(TokenType::tEndWhile, "Expect 'end' to close 'while' statement.");
	return std::make_unique<WhileStatementNode>(std::move(condition), std::move(body), whileToken.line);
}

std::unique_ptr<StatementNode> Parser::parseForStatement(){
	Token forToken = m_current_token;
	consume(TokenType::tFor, "Expect 'for'.");
	Token idToken = m_current_token;

	consume(TokenType::tIdentifier, "Expect identifier for loop variable.");
	auto loopVar = std::make_unique<IdentifierNode>(idToken.lexema, idToken.line);
	
	consume(TokenType::tIn, "Expect 'in' after loop variable.");
	
	auto iterable = parseExpression();
	auto body = parseBlock();
	consume(TokenType::tEndFor, "Expect 'end' to close 'for' statement.");
	return std::make_unique<ForStatementNode>(std::move(loopVar), std::move(iterable), std::move(body), forToken.line);
}

std::unique_ptr<ExpressionNode> Parser::parseFunctionLiteral(){
	Token funcToken = m_current_token;
	consume(TokenType::tFunc, "Expect 'function'.");

	consume(TokenType::tLParenthesis, "Expect '(' after 'function' keyword for parameters.");
	std::vector<std::unique_ptr<IdentifierNode>> parameters;
	if(!check(TokenType::tRParenthesis)){
		do{
			Token paramToken = m_current_token;
			consume(TokenType::tIdentifier, "Expect parameter name.");
			parameters.push_back(std::make_unique<IdentifierNode>(paramToken.lexema, paramToken.line));
		}while(match(TokenType::tComma));
	}
	consume(TokenType::tRParenthesis, "Expect ')' after parameters.");

	auto body = parseBlock();
	consume(TokenType::tEndFunc, "Expect 'end' to close 'function' literal definition.");
	return std::make_unique<FunctionLiteralNode>(std::move(parameters), std::move(body), funcToken.line);
}

std::unique_ptr<StatementNode> Parser::parseReturnStatement(){
	Token retToken = m_current_token;
	consume(TokenType::tReturn, "Expect 'return'.");
	std::unique_ptr<ExpressionNode> value = nullptr;
	
	bool canHaveExpression = true;
	if(check(TokenType::tEndIf) ||check(TokenType::tEndWhile) ||check(TokenType::tEndFor) ||check(TokenType::tEndFunc) || check(TokenType::tElse) || isAtEnd() || check(TokenType::tEOF) ){
		canHaveExpression = false;
	}

	if(canHaveExpression){
		switch(m_current_token.type){
			case TokenType::tIdentifier: case TokenType::tNumber: case TokenType::tString:
			case TokenType::tTrue: case TokenType::tFalse: case TokenType::tNil:
			case TokenType::tLParenthesis: case TokenType::tLBracket: 					// For list literals
			case TokenType::tMinus: case TokenType::tPlus: case TokenType::tNot:	// Unary ops
			case TokenType::tFunc:												// For function literals `return function() ... end`
				value = parseExpression();
				break;
			default:
				break;
		}
	}
	return std::make_unique<ReturnStatementNode>(retToken.line, std::move(value));
}

std::unique_ptr<StatementNode> Parser::parseBreakStatement(){
	Token breakToken = m_current_token;
	consume(TokenType::tBreak, "Expect 'break'.");
	return std::make_unique<BreakStatementNode>(breakToken.line);
}

std::unique_ptr<StatementNode> Parser::parseContinueStatement(){
	Token continueToken = m_current_token;
	consume(TokenType::tContinue, "Expect 'continue'.");
	return std::make_unique<ContinueStatementNode>(continueToken.line);
}

int Parser::getOperatorPrecedence(TokenType type){
	switch(type){
		case TokenType::tAssign:
		case TokenType::tPlusAssign:
		case TokenType::tMinusAssign:
		case TokenType::tMultiplyAssign:
		case TokenType::tDivideAssign:
		case TokenType::tModuleAssign:
		case TokenType::tPowerAssign:
			return 1;

		case TokenType::tOr:
			return 2;

		case TokenType::tAnd:
			return 3;

		case TokenType::tEqual:
		case TokenType::tNotEqual:
			return 4;

		case TokenType::tLess:
		case TokenType::tGreater:
		case TokenType::tLessOrEqual:
		case TokenType::tGreaterOrEqual:
			return 5;

		case TokenType::tPlus: case TokenType::tMinus: return 6; // Бинарные + и -
		case TokenType::tMultiply: case TokenType::tDivide: case TokenType::tModule: return 7;
		case TokenType::tPower: return 8;
		
		default: return 0;
	}
}

std::unique_ptr<ExpressionNode> Parser::parseExpression(int minPrecedence){
	std::unique_ptr<ExpressionNode> left = parseUnary();

	while(!isAtEnd()){
		TokenType operatorType = m_current_token.type;
		int precedence = getOperatorPrecedence(operatorType);

		if(precedence == 0 || precedence < minPrecedence){
			break;
		}

		bool isRightAssociative =
			(operatorType == TokenType::tPower ||
			(operatorType >= TokenType::tAssign && operatorType <= TokenType::tPowerAssign));
		
		int nextMinPrecedence = isRightAssociative ? precedence : precedence + 1;
		
		consume(operatorType, "Expect binary operator.");
		Token opToken = m_previous_token;

		if(opToken.type >= TokenType::tAssign && opToken.type <= TokenType::tPowerAssign){
			auto identifierForAssignment = std::unique_ptr<IdentifierNode>(static_cast<IdentifierNode*>(left.release()));
			auto right = parseExpression(nextMinPrecedence);
			if(!right){
				error(opToken, "Expected expression on the right-hand side of assignment '" + opToken.lexema + "'");
			}
			left = std::make_unique<AssignmentNode>(std::move(identifierForAssignment), opToken.type, std::move(right), opToken.line);
		}
		else{
			auto right = parseExpression(nextMinPrecedence);
			if(!right){
				error(opToken, "Expected expression after binary operator '" + opToken.lexema + "'");
			}
			left = std::make_unique<BinaryOpNode>(opToken.type, std::move(left), std::move(right), opToken.line);
		}
	}
	return left;
}

std::unique_ptr<ExpressionNode> Parser::parseUnary(){
	// +/-/!
	if(match({TokenType::tNot, TokenType::tMinus, TokenType::tPlus})){
		Token opToken = m_previous_token;
		auto operand = parseUnary();
		if(!operand){
			error(opToken, "Expected expression after unary operator '" + opToken.lexema + "'");
		}

		return std::make_unique<UnaryOpNode>(opToken.type, std::move(operand), opToken.line);
	}
	return parsePostfixOperations();
}

std::unique_ptr<ExpressionNode> Parser::parsePostfixOperations(){
	std::unique_ptr<ExpressionNode> expr = parsePrimary();

	while(true){
		if(check(TokenType::tLParenthesis)){
			expr = parseCall(std::move(expr));
		}
		else if(match(TokenType::tLBracket)){
			Token lbracketToken = m_previous_token;
			std::unique_ptr<ExpressionNode> firstArg = nullptr;
			std::unique_ptr<ExpressionNode> secondArg = nullptr;
			bool isSlice = false;

			if(check(TokenType::tRBracket)){
				error(lbracketToken, "Subscript operator [] cannot be empty. For full slice, use [:] or specify indices.");
			}
			
			if(check(TokenType::tColon)){ 
				consume(TokenType::tColon, "Expect ':' for slice start.");
				isSlice = true;
				if(!check(TokenType::tRBracket)){ 
					secondArg = parseExpression();
				} 
			}
			else{ 
				firstArg = parseExpression();
				if(match(TokenType::tColon)) { 
					isSlice = true;
					if(!check(TokenType::tRBracket)){
						secondArg = parseExpression();
					} 
				}
				else{ 
					isSlice = false;
				}
			}

			consume(TokenType::tRBracket, "Expect ']' after index or slice arguments.");

			if(isSlice){
				expr = std::make_unique<SliceExpressionNode>(std::move(expr), std::move(firstArg), std::move(secondArg), lbracketToken.line);
			}
			else{
				if(!firstArg){
					error(lbracketToken, "Index expression missing inside []. This should not happen.");
				}
				expr = std::make_unique<IndexExpressionNode>(std::move(expr), std::move(firstArg), lbracketToken.line);
			}
		}
		else{
			break;
		}
	}
	return expr;
}

std::unique_ptr<ExpressionNode> Parser::parsePrimary(){
	if(match(TokenType::tNumber)){
		try{
			return std::make_unique<NumberLiteralNode>(std::stod(m_previous_token.lexema), m_previous_token.lexema, m_previous_token.line);
		}
		catch(const std::out_of_range&){
			error(m_previous_token, "Number literal out of range.");
		}
		catch(const std::invalid_argument&){
			error(m_previous_token, "Invalid number literal format.");
		}
	}
	if(match(TokenType::tString)){
		return std::make_unique<StringLiteralNode>(m_previous_token.lexema, m_previous_token.line);
	}
	if(match(TokenType::tTrue)) return std::make_unique<BooleanLiteralNode>(true, m_previous_token.line);
	if(match(TokenType::tFalse)) return std::make_unique<BooleanLiteralNode>(false, m_previous_token.line);
	if(match(TokenType::tNil)) return std::make_unique<NilLiteralNode>(m_previous_token.line);
	if(match(TokenType::tIdentifier)) return std::make_unique<IdentifierNode>(m_previous_token.lexema, m_previous_token.line);

	if(match(TokenType::tLBracket)){ // List literal
		Token lbracketToken = m_previous_token;
		std::vector<std::unique_ptr<ExpressionNode>> elements;
		if(!check(TokenType::tRBracket)){
			do{
				elements.push_back(parseExpression());
			}while(match(TokenType::tComma));
		}
		consume(TokenType::tRBracket, "Expect ']' after list elements or '[' for empty list.");
		return std::make_unique<ListLiteralNode>(std::move(elements), lbracketToken.line);
	}

	if(m_current_token.type == TokenType::tFunc){
		return parseFunctionLiteral();
	}

	if(match(TokenType::tLParenthesis)){
		auto expr = parseExpression();
		consume(TokenType::tLParenthesis, "Expect ')' after expression in parentheses.");
		return expr;
	}

	error(m_current_token, "Expected primary expression(literal, identifier, list, function, or grouped expression). Found: " + tokenTypeToStr(m_current_token.type) + "('" + m_current_token.lexema + "')");
	return nullptr;
}

std::unique_ptr<ExpressionNode> Parser::parseCall(std::unique_ptr<ExpressionNode> callee){
	Token lparenToken = m_current_token; 
	consume(TokenType::tLParenthesis, "Expect '(' to start function call arguments.");

	std::vector<std::unique_ptr<ExpressionNode>> arguments;
	if(!check(TokenType::tRParenthesis)){
		do{
			arguments.push_back(parseExpression());
		}while(match(TokenType::tComma));
	}
	consume(TokenType::tRParenthesis, "Expect ')' or ',' in argument list to close function call.");
	return std::make_unique<FunctionCallNode>(std::move(callee), std::move(arguments), lparenToken.line);
}

void Parser::synchronize(){
	advance();

	while(!isAtEnd()){
		if(
			m_previous_token.type == TokenType::tIf ||
			m_previous_token.type == TokenType::tEndWhile ||
			m_previous_token.type == TokenType::tEndFor ||
			m_previous_token.type == TokenType::tEndFunc
		) return;

		switch(m_current_token.type){
			case TokenType::tIf:
			case TokenType::tWhile:
			case TokenType::tFor:
			case TokenType::tReturn:
			case TokenType::tEndIf:
			case TokenType::tEndWhile:
			case TokenType::tEndFor:
			case TokenType::tEndFunc:
				return;
			default:
				break;
		}

		advance();
	}
}