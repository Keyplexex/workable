#pragma once

#include <string>
#include <vector>
#include <memory>
#include <iomanip>

#include "../lexer/lexer.h"
#include "../interpreter/value.h"

class Interpreter;

// Base
struct ASTNode;					
struct ExpressionNode;			
struct StatementNode;			

// Expressions 
struct NumberLiteralNode;		
struct StringLiteralNode;		
struct BooleanLiteralNode;		
struct NilLiteralNode;		
struct IdentifierNode;			
struct ListLiteralNode;		
struct FunctionLiteralNode;		

// Operations 
struct BinaryOpNode;
struct UnaryOpNode;			
struct AssignmentNode;		
struct FunctionCallNode;		
struct IndexExpressionNode;	
struct SliceExpressionNode;		// [from:to]

// Statements 
struct ExpressionStatementNode;	
struct BlockNode;				
struct IfStatementNode;			
struct WhileStatementNode;	
struct ForStatementNode;	
struct ReturnStatementNode;		
struct BreakStatementNode;		
struct ContinueStatementNode;	
struct ProgramNode;			


// Base AST
struct ASTNode{
	int line = 0;
	virtual ~ASTNode() = default;
	virtual Value accept(Interpreter& interpreter) const = 0;
	virtual std::string toString(int indent = 0) const = 0;
};

// Expressions
struct ExpressionNode : public ASTNode{};

struct NumberLiteralNode : public ExpressionNode{
	double value;
	std::string rawLexeme;

	explicit NumberLiteralNode(double val, const std::string& lexeme, int l);
	std::string toString(int indent = 0) const override;
	Value accept(Interpreter& interpreter) const override;
};

struct StringLiteralNode : public ExpressionNode{
	std::string value;

	explicit StringLiteralNode(const std::string& val, int l);
	std::string toString(int indent = 0) const override;
	Value accept(Interpreter& interpreter) const override;
};

struct BooleanLiteralNode : public ExpressionNode{
	bool value;

	explicit BooleanLiteralNode(bool val, int l);
	std::string toString(int indent = 0) const override;
	Value accept(Interpreter& interpreter) const override;
};

struct NilLiteralNode : public ExpressionNode{
	explicit NilLiteralNode(int l);
	std::string toString(int indent = 0) const override;
	Value accept(Interpreter& interpreter) const override;
};

struct IdentifierNode : public ExpressionNode{
	std::string name;

	explicit IdentifierNode(const std::string& n, int l);
	std::string toString(int indent = 0) const override;
	Value accept(Interpreter& interpreter) const override;
};

struct ListLiteralNode : public ExpressionNode{
	std::vector<std::unique_ptr<ExpressionNode>> elements;

	explicit ListLiteralNode(std::vector<std::unique_ptr<ExpressionNode>> elems, int l);
	std::string toString(int indent = 0) const override;
	Value accept(Interpreter& interpreter) const override;
};

struct BinaryOpNode : public ExpressionNode{
	TokenType op;
	std::unique_ptr<ExpressionNode> left;
	std::unique_ptr<ExpressionNode> right;

	BinaryOpNode(TokenType o, std::unique_ptr<ExpressionNode> l, std::unique_ptr<ExpressionNode> r, int l_num);
	std::string toString(int indent = 0) const override;
	Value accept(Interpreter& interpreter) const override;
};

struct UnaryOpNode : public ExpressionNode{
	TokenType op;
	std::unique_ptr<ExpressionNode> operand;

	UnaryOpNode(TokenType o, std::unique_ptr<ExpressionNode> r_val, int l_num);
	std::string toString(int indent = 0) const override;
	Value accept(Interpreter& interpreter) const override;
};

struct AssignmentNode : public ExpressionNode{
	std::unique_ptr<ExpressionNode> expression_l; // LValue
	TokenType assignmentOp;
	std::unique_ptr<ExpressionNode> expression_r; // RValue

	AssignmentNode(std::unique_ptr<ExpressionNode> id, TokenType op_type, std::unique_ptr<ExpressionNode> expr, int l_num);
	std::string toString(int indent = 0) const override;
	Value accept(Interpreter& interpreter) const override;
};

struct FunctionCallNode : public ExpressionNode{
	std::unique_ptr<ExpressionNode> callee;
	std::vector<std::unique_ptr<ExpressionNode>> arguments;

	FunctionCallNode(std::unique_ptr<ExpressionNode> cal, std::vector<std::unique_ptr<ExpressionNode>> args, int l_num);
	std::string toString(int indent = 0) const override;
	Value accept(Interpreter& interpreter) const override;
};

struct IndexExpressionNode : public ExpressionNode{
	std::unique_ptr<ExpressionNode> object;
	std::unique_ptr<ExpressionNode> index;

	IndexExpressionNode(std::unique_ptr<ExpressionNode> obj, std::unique_ptr<ExpressionNode> idx, int l);
	std::string toString(int indent = 0) const override;
	Value accept(Interpreter& interpreter) const override;
};

struct SliceExpressionNode : public ExpressionNode{
	std::unique_ptr<ExpressionNode> object;
	std::unique_ptr<ExpressionNode> start;
	std::unique_ptr<ExpressionNode> end;

	SliceExpressionNode(std::unique_ptr<ExpressionNode> obj, std::unique_ptr<ExpressionNode> st, std::unique_ptr<ExpressionNode> ed, int l);
	std::string toString(int indent = 0) const override;
	Value accept(Interpreter& interpreter) const override;
};

// Statements
struct StatementNode : public ASTNode{};

struct BlockNode : public StatementNode{
	std::vector<std::unique_ptr<StatementNode>> statements;

	explicit BlockNode(int l = 0);
	std::string toString(int indent = 0) const override;
	Value accept(Interpreter& interpreter) const override;
};

struct FunctionLiteralNode : public ExpressionNode{
	std::vector<std::unique_ptr<IdentifierNode>> parameters;
	std::unique_ptr<BlockNode> body;

	FunctionLiteralNode(std::vector<std::unique_ptr<IdentifierNode>> params, std::unique_ptr<BlockNode> b, int l_num);
	std::string toString(int indent = 0) const override;
	Value accept(Interpreter& interpreter) const override;
};

struct ExpressionStatementNode : public StatementNode{
	std::unique_ptr<ExpressionNode> expression;

	explicit ExpressionStatementNode(std::unique_ptr<ExpressionNode> expr);
	std::string toString(int indent = 0) const override;
	Value accept(Interpreter& interpreter) const override;
};

struct IfStatementNode : public StatementNode{
	std::unique_ptr<ExpressionNode> condition;
	std::unique_ptr<BlockNode> thenBranch;
	std::unique_ptr<StatementNode> elseBranch;

	IfStatementNode(std::unique_ptr<ExpressionNode> cond, std::unique_ptr<BlockNode> thenB, std::unique_ptr<StatementNode> elseB, int l_num);
	std::string toString(int indent = 0) const override;
	Value accept(Interpreter& interpreter) const override;
};

struct WhileStatementNode : public StatementNode{
	std::unique_ptr<ExpressionNode> condition;
	std::unique_ptr<BlockNode> body;

	WhileStatementNode(std::unique_ptr<ExpressionNode> cond, std::unique_ptr<BlockNode> b, int l_num);
	std::string toString(int indent = 0) const override;
	Value accept(Interpreter& interpreter) const override;
};

struct ForStatementNode : public StatementNode{
	std::unique_ptr<IdentifierNode> loopVariable;
	std::unique_ptr<ExpressionNode> iterable;
	std::unique_ptr<BlockNode> body;

	ForStatementNode(std::unique_ptr<IdentifierNode> var, std::unique_ptr<ExpressionNode> iter, std::unique_ptr<BlockNode> b, int l_num);
	std::string toString(int indent = 0) const override;
	Value accept(Interpreter& interpreter) const override;
};

struct ReturnStatementNode : public StatementNode{
	std::unique_ptr<ExpressionNode> returnValue;

	explicit ReturnStatementNode(int l_num, std::unique_ptr<ExpressionNode> val = nullptr);
	std::string toString(int indent = 0) const override;
	Value accept(Interpreter& interpreter) const override;
};

struct BreakStatementNode : public StatementNode{
	explicit BreakStatementNode(int l_num);
	std::string toString(int indent = 0) const override;
	Value accept(Interpreter& interpreter) const override;
};

struct ContinueStatementNode : public StatementNode{
	explicit ContinueStatementNode(int l_num);
	std::string toString(int indent = 0) const override;
	Value accept(Interpreter& interpreter) const override;
};


// Root node
struct ProgramNode : public ASTNode{
	std::vector<std::unique_ptr<StatementNode>> statements;
	std::string toString(int indent = 0) const override;
	Value accept(Interpreter& interpreter) const override;
};