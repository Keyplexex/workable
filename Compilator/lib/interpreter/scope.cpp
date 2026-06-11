#include "scope.h"

#include "errorManager.h"

Scope::Scope(std::shared_ptr<Scope> outer)
	: outer_scope(outer)
{}

Scope::Scope(std::unique_ptr<ProgramNode> ast_root)
	: outer_scope(nullptr)
	, ast_root(std::move(ast_root))
{}

void Scope::define(const std::string& name, const Value& value){
	variables[name] = value;
}

bool Scope::assign(const std::string& name, const Value& value){
	auto it = variables.find(name);

	if(it != variables.end()){
		it->second = value;
		return true;
	}

	if(outer_scope){
		return outer_scope->assign(name, value);
	}


	return false;
}


Value Scope::get(const std::string& name) const{
	auto it = variables.find(name);

	if(it != variables.end()){
		return it->second;
	}

	if(outer_scope){
		return outer_scope->get(name);
	}

	ErrorManager("Scope", "No access to \""+name+"\"");
	return Value();
}

bool Scope::isDefinedLocally(const std::string& name) const{
	return variables.count(name);
}

const std::unique_ptr<ProgramNode>& Scope::getAstRoot() const{
	return ast_root;
}