#pragma once

#include <string>
#include <unordered_map>
#include <memory>

#include "value.h"
#include "../parser/ASTNode.h"

class Scope{
private:
	// parent scope
	std::shared_ptr<Scope> outer_scope;

	// variables
	std::unordered_map<std::string, Value> variables;

	// for show_ast
	std::unique_ptr<ProgramNode> ast_root;

public:
	explicit Scope(std::shared_ptr<Scope> outer);
	explicit Scope(std::unique_ptr<ProgramNode> ast_root);

	// variable definition
	void define(const std::string& name, const Value& value);

	// search value in outer_scope and define
	bool assign(const std::string& name, const Value& value);

	// search value in outer_scope and get it (return type)
	Value get(const std::string& name) const;

	// is there a variable locally
	bool isDefinedLocally(const std::string& name) const;

	// show_ast
	const std::unique_ptr<ProgramNode>& getAstRoot() const;
};