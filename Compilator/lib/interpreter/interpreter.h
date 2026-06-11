#pragma once

#include <memory>
#include <iostream>

#include "value.h"
#include "scope.h"
#include "errorManager.h"
#include "../lexer/token.h"
#include "../lexer/lexer.h"
#include "../parser/ASTNode.h"
#include "../parser/parser.h"

struct ReturnValue{
	Value value;

	ReturnValue(const Value& v)
		: value(v)
	{}
};

struct ContinueSignal{
	ContinueSignal() = default;
};
struct BreakSignal{
	BreakSignal() = default;
};

class Interpreter;

struct StandardLibrary{
	StandardLibrary(Scope& globals, Interpreter& interpreter);
};

class Interpreter{
private:
	// scopes
	std::shared_ptr<Scope> m_current_scope;
	std::shared_ptr<Scope> m_global_scope;

	// recursion depth
	int m_recursion_depth = 0;
	const int MAX_RECURSION_DEPTH = 1000;

	// stacktrace
	std::vector<std::string> call_stack_trace;

public:
	Interpreter(std::unique_ptr<ProgramNode> start)
		: m_global_scope(std::make_shared<Scope>(std::move(start)))
	{
		m_current_scope = m_global_scope;
		StandardLibrary std_lib(*m_global_scope, *this);
		visit(m_global_scope->getAstRoot().get());
	}

	// BinaryOP (math?)
	Value add(const Value& left, const Value& right);
	Value subtract(const Value& left, const Value& right);
	Value multiply(const Value& left, const Value& right);
	Value divide(const Value& left, const Value& right);
	Value modulo(const Value& left, const Value& right);
	Value power(const Value& left, const Value& right);

	// BinaryOP equals
	Value equal(const Value& left, const Value& right, bool isnot = false);
	Value notEqual(const Value& left, const Value& right);
	Value lessThan(const Value& left, const Value& right);
	Value greaterThan(const Value& left, const Value& right);
	Value lessThanOrEqual(const Value& left, const Value& right);
	Value greaterThanOrEqual(const Value& left, const Value& right);

	// execute
	Value evaluate(const ASTNode* node);				// выполнение ExpressionNode
	void visitAndExecute(const StatementNode* node);	// выполнение StatementNode

	// visit -> execute
	void visit(const ProgramNode* node);
	void visit(const ExpressionStatementNode* node);

	// Block Node
	void executeBlock(const BlockNode* block_node, std::shared_ptr<Scope> env_for_block);
	void visit(const BlockNode* node);

	// if | while | for
	void visit(const IfStatementNode* node);
	void visit(const WhileStatementNode* node);
	void visit(const ForStatementNode* node);

	// literals
	Value visit(const NumberLiteralNode* node);
	Value visit(const StringLiteralNode* node);
	Value visit(const BooleanLiteralNode* node);
	Value visit(const NilLiteralNode* node);
	Value visit(const IdentifierNode* node);
	Value visit(const ListLiteralNode* node);
	Value visit(const FunctionLiteralNode* node);

	// return | break | continue
	Value visit(const ReturnStatementNode* node);
	void visit(const BreakStatementNode* node);
	void visit(const ContinueStatementNode* node);

	// BinaryOp Node
	Value visit(const BinaryOpNode* node);
	Value applyBinaryOperator(const TokenType& type, const Value& left, const Value& right);

	// UnaryOp Node
	Value visit(const UnaryOpNode* node);
	Value applyUnaryOperator(const TokenType& type, const Value& operand);

	// AssignmentNode (=)
	Value visit(const AssignmentNode* node);

	// FunctionCallNode (func())
	Value visit(const FunctionCallNode* node);

	// IndexExpression (list[index])
	Value visit(const IndexExpressionNode* node);

	// SliceExpression (list[start:end])
	long long resolve_slice_index(const std::unique_ptr<ExpressionNode>& expr_node, long long size, long long default_val);
	Value visit(const SliceExpressionNode* node);

	// stacktrace()
	void pushCall(const std::string& name);
	void popCall();
	Value getStackTrace();
};

// only for test
bool interpret(std::istream& input, std::ostream& output);