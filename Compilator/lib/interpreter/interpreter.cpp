#include "interpreter.h"

#include "errorManager.h"
#include "../parser/ASTNode.h"

#include <iostream>
#include <memory>

Value Interpreter::evaluate(const ASTNode* node){
	return node->accept(*this);
}

void Interpreter::visitAndExecute(const StatementNode* node){
	node->accept(*this);
}

void Interpreter::visit(const ProgramNode* node){
	for(const auto& t : node->statements){
		if(t) visitAndExecute(t.get());
	}
}

void Interpreter::visit(const ExpressionStatementNode* node){
	if(node->expression){
		evaluate(node->expression.get());
	}
}

void Interpreter::executeBlock(const BlockNode* block_node, std::shared_ptr<Scope> env_for_block){
	std::shared_ptr<Scope> previous_env = m_current_scope;
	m_current_scope = env_for_block;

	for(const auto& t : block_node->statements){
		if(t) visitAndExecute(t.get());
	}

	m_current_scope = previous_env;
}

void Interpreter::visit(const BlockNode* node){
	executeBlock(node, m_current_scope);
}

void Interpreter::visit(const IfStatementNode* node){
	Value condition_val = evaluate(node->condition.get());
	std::shared_ptr<Scope> block_env = std::make_shared<Scope>(m_current_scope);

	if(condition_val.asBool()){
		if(node->thenBranch){
			executeBlock(node->thenBranch.get(), block_env);
		}
	}
	else{
		if(node->elseBranch){
			if(auto else_block = dynamic_cast<const BlockNode*>(node->elseBranch.get())){
				executeBlock(else_block, block_env);
			}
			else if(auto else_if_stmt = dynamic_cast<const IfStatementNode*>(node->elseBranch.get())){
				Interpreter::visit(else_if_stmt);
			}
			else{
				visitAndExecute(node->elseBranch.get());
			}
		}
	}
}

void Interpreter::visit(const WhileStatementNode* node){
	std::shared_ptr<Scope> loop_env_outer = m_current_scope;

	pushCall("while (line "+std::to_string(node->line)+")");

	while(evaluate(node->condition.get()).asBool()){
		std::shared_ptr<Scope> loop_body_env = std::make_shared<Scope>(loop_env_outer);
		m_current_scope = loop_body_env;

		try{
			if(node->body){
				executeBlock(node->body.get(), loop_body_env);
			}
		}
		catch(const BreakSignal&){
			m_current_scope = loop_env_outer;
			popCall();
			return;
		}
		catch(const ContinueSignal&){
			continue;
		}
		catch(...){
			m_current_scope = loop_env_outer;
			popCall();
			throw;
		}
	}

	m_current_scope = loop_env_outer;
	popCall();
}

void Interpreter::visit(const ForStatementNode* node){
	Value iterable_value = evaluate(node->iterable.get());

	if(iterable_value.getType() != ValueType::kList && iterable_value.getType() != ValueType::kString){
		ErrorManager("ForStatementNode", "for loop can only iterate over lists and strings, not "+iterable_value.toString()+".");
	}

	pushCall("for (line "+std::to_string(node->line)+")");

	try{
		// kList
		if(iterable_value.getType() == ValueType::kList){
			const auto& list = *iterable_value.asList();
			for(const auto& element : list){
				auto body_env = std::make_shared<Scope>(m_current_scope);

				body_env->define(node->loopVariable->name, element);

				try{
					executeBlock(node->body.get(), body_env);
				}
				catch(const ContinueSignal&){
					continue;
				}
			}
		}
		// kString
		else if(iterable_value.getType() == ValueType::kString){
			const auto& str = *iterable_value.asString();
			for(char c : str){
				auto body_env = std::make_shared<Scope>(m_current_scope);

				body_env->define(node->loopVariable->name, Value(std::string(1, c)));

				try{
					executeBlock(node->body.get(), body_env);
				}
				catch(const ContinueSignal&){
					continue;
				}
			}
		}
	}
	catch(const BreakSignal&){}
	catch(...){
		popCall();
		throw;
	}

	popCall();
}

Value Interpreter::visit(const NumberLiteralNode* node){
	return Value(node->value);
}

Value Interpreter::visit(const StringLiteralNode* node){
	return Value(node->value);
}

Value Interpreter::visit(const BooleanLiteralNode* node){
	return Value(node->value);
}

Value Interpreter::visit(const NilLiteralNode* node){
	return Value();
}

Value Interpreter::visit(const IdentifierNode* node){
	return m_current_scope->get(node->name);
}

Value Interpreter::visit(const ListLiteralNode* node){
	auto list_values = std::make_shared<ListType>();

	for(const auto& elem_node : node->elements){
		if(elem_node){
			list_values->push_back(evaluate(elem_node.get()));
		}
		else{
			list_values->push_back(Value());
		}
	}

	return Value(list_values);
}

Value Interpreter::visit(const FunctionLiteralNode* node){
	auto func = std::make_shared<Function>([this, node](const std::vector<Value>& args){
		
		if(args.size() != node->parameters.size()){
			ErrorManager("FunctionLiteralNode", "args size hz");
		}

		auto new_scope = std::make_shared<Scope>(m_current_scope);

		for(size_t i=0; i < args.size() && i < node->parameters.size(); ++i){
			new_scope->define(node->parameters[i]->name, args[i]);
		}

		auto old_scope = m_current_scope;
		m_current_scope = new_scope;
		Value result;

		try{
			Interpreter::visit(node->body.get());
		}
		catch(const ReturnValue& ret){
			result = ret.value;
		}

		m_current_scope = old_scope;

		return result;
	});

	return Value(func);
}

Value Interpreter::visit(const ReturnStatementNode* node){
	Value result = (node->returnValue ? evaluate(node->returnValue.get()) : Value());
	throw ReturnValue(result);
}

void Interpreter::visit(const BreakStatementNode* node){
	throw BreakSignal();
}

void Interpreter::visit(const ContinueStatementNode* node){
	throw ContinueSignal();
}

Value Interpreter::visit(const BinaryOpNode* node){
	Value left = evaluate(node->left.get());

	if(node->op == TokenType::tAnd){
		if(!left.asBool()){
			return Value(false);
		}
		return Value(evaluate(node->right.get()).asBool());
	}

	if(node->op == TokenType::tOr){
		if(left.asBool()){
			return Value(true);
		}
		return Value(evaluate(node->right.get()).asBool());
	}

	Value right = evaluate(node->right.get());

	return applyBinaryOperator(node->op, left, right);
}

Value Interpreter::applyBinaryOperator(const TokenType& type, const Value& left, const Value& right){
	switch(type){
		case TokenType::tPlus: return add(left, right);
		case TokenType::tMinus: return subtract(left, right);
		case TokenType::tMultiply: return multiply(left, right);
		case TokenType::tDivide: return divide(left, right);
		case TokenType::tModule: return modulo(left, right);
		case TokenType::tPower: return power(left, right);

		case TokenType::tEqual: return equal(left, right);
		case TokenType::tNotEqual: return notEqual(left, right);
		case TokenType::tLess: return lessThan(left, right);
		case TokenType::tGreater: return greaterThan(left, right);
		case TokenType::tLessOrEqual: return lessThanOrEqual(left, right);
		case TokenType::tGreaterOrEqual: return greaterThanOrEqual(left, right);
	}

	ErrorManager("applyBinaryOperator");
	return Value();
}

Value Interpreter::visit(const UnaryOpNode* node){
	Value operand = evaluate(node->operand.get());
	return applyUnaryOperator(node->op, operand);
}

Value Interpreter::applyUnaryOperator(const TokenType& type, const Value& operand){
	switch(type){
		case TokenType::tMinus:
			if(operand.getType() == ValueType::kDouble) return Value(-operand.asNumber());
			ErrorManager("applyUnaryOperator", "argument must be number");

		case TokenType::tPlus:
			if(operand.getType() == ValueType::kDouble) return Value(operand.asNumber());
			ErrorManager("applyUnaryOperator", "argument must be number");

		case TokenType::tNot:
			return Value(!operand.asBool());
	}

	ErrorManager("applyUnaryOperator");
	return Value();
}

Value Interpreter::visit(const AssignmentNode* node){
	Value rvalue = evaluate(node->expression_r.get());

	// простое присваивание
	if(auto id_node = dynamic_cast<const IdentifierNode*>(node->expression_l.get())){
		if(node->assignmentOp == TokenType::tAssign){
			if(!m_current_scope->assign(id_node->name, rvalue)){
				// если нет, то создаём
				m_current_scope->define(id_node->name, rvalue);
			}
			return rvalue;
		}
		else{
			// for +=, -= etc.
			Value lvalue = m_current_scope->get(id_node->name);
			TokenType type;

			switch(node->assignmentOp){
				case TokenType::tPlusAssign: type = TokenType::tPlus; break;
				case TokenType::tMinusAssign: type = TokenType::tMinus; break;
				case TokenType::tMultiplyAssign: type = TokenType::tMultiply; break;
				case TokenType::tDivideAssign: type = TokenType::tDivide; break;
				case TokenType::tModuleAssign: type = TokenType::tModule; break;
				case TokenType::tPowerAssign: type = TokenType::tPower; break;
				default:
					ErrorManager("AssignmentNode");
			}
			rvalue = applyBinaryOperator(type, lvalue, rvalue);
			m_current_scope->assign(id_node->name, rvalue);
		}
		return rvalue;
	}

	// list[i] = value
	else if(auto index_expr_node = dynamic_cast<const IndexExpressionNode*>(node->expression_l.get())){
		Value object_val = evaluate(index_expr_node->object.get());
		Value index_val = evaluate(index_expr_node->index.get());

		if(object_val.getType() == ValueType::kList){
			if(index_val.getType() != ValueType::kDouble){
				ErrorManager("AssignmentNode", "list index must be a number.");
			}
			double raw_idx = index_val.asNumber();
			if(std::floor(raw_idx) != raw_idx){
				ErrorManager("AssignmentNode", "list index must be a number.");
			}
			int idx = static_cast<int>(raw_idx);
			auto list_ptr = object_val.asList();

			if(idx < 0) idx += list_ptr->size();

			if(idx < 0 || idx >= list_ptr->size()){
				ErrorManager("AssignmentNode", "list index out of bounds.");
			}


			if(node->assignmentOp == TokenType::tAssign){
				(*list_ptr)[idx] = rvalue;
			}
			else{
				TokenType type;

				switch(node->assignmentOp){
					case TokenType::tPlusAssign: type = TokenType::tPlus; break;
					case TokenType::tMinusAssign: type = TokenType::tMinus; break;
					case TokenType::tMultiplyAssign: type = TokenType::tMultiply; break;
					case TokenType::tDivideAssign: type = TokenType::tDivide; break;
					case TokenType::tModuleAssign: type = TokenType::tModule; break;
					case TokenType::tPowerAssign: type = TokenType::tPower; break;
					default:
						ErrorManager("AssignmentNode");
				}
				rvalue = applyBinaryOperator(type, (*list_ptr)[idx], rvalue);
				(*list_ptr)[idx] = rvalue;

			}

			return rvalue;
		}
		else{
			ErrorManager("AssignmentNode", "list[index]");
		}
	}

	ErrorManager("AssignmentNode", "list[index]");
	return Value();
}

Value Interpreter::visit(const FunctionCallNode* node){
	if(m_recursion_depth >= MAX_RECURSION_DEPTH){
		ErrorManager("Stack overflow", "Maximum recursion depth exceeded.");
	}

	Value callee = evaluate(node->callee.get());
	if(callee.getType() != ValueType::kFunc){
		ErrorManager("FunctionCallNode", "\""+callee.toString()+"\" is not a function");
	}

	std::vector<Value> args;
	for(const auto& arg : node->arguments){
		args.push_back(evaluate(arg.get()));
	}

	++m_recursion_depth;
	pushCall("function \""+callee.toString()+"\" (line "+std::to_string(node->line)+")");

	Value v = (*callee.asFunction())(args);

	--m_recursion_depth;
	popCall();

	return v;
}

Value Interpreter::visit(const IndexExpressionNode* node){
	Value object = evaluate(node->object.get());
	Value index_val = evaluate(node->index.get());

	if(index_val.getType() != ValueType::kDouble){
		ErrorManager("IndexExpressionNode", "index must be a number.");
	}

	double raw_idx = index_val.asNumber();
	if(std::floor(raw_idx) != raw_idx){
		ErrorManager("IndexExpressionNode", "index must be an integer.");
	}
	int idx = static_cast<int>(raw_idx);

	// kList
	if(object.getType() == ValueType::kList){
		auto list_ptr = object.asList();
		int size = list_ptr->size();

		if(idx < 0){
			idx += size;
		}

		if(idx < 0 || idx >= size){
			return Value(); 
		}
		return (*list_ptr)[idx];
	}

	// kString
	if(object.getType() == ValueType::kString){
		auto str_ptr = object.asString();
		int size = str_ptr->length();

		if(idx < 0){
			idx += size;
		}

		if(idx < 0 || idx >= size){
			return Value();
		}

		return Value(std::string(1, (*str_ptr)[idx]));
	}

	ErrorManager("IndexExpressionNode", "indexing operator [] can only be applied to lists and strings.");
	return Value();
}

long long Interpreter::resolve_slice_index(const std::unique_ptr<ExpressionNode>& expr_node, long long size, long long default_val){
	if(!expr_node){
		return default_val;
	}

	Value val = evaluate(expr_node.get());
	if(val.getType() != ValueType::kDouble){
		ErrorManager("SliceExpressionNode", "slice indices must be numbers.");
	}
	double raw_idx = val.asNumber();
	 if(std::floor(raw_idx) != raw_idx){
		ErrorManager("SliceExpressionNode", "slice indices must be integers.");
	}

	long long idx = static_cast<long long>(raw_idx);

	if(idx < 0){
		idx += size;
	}

	return std::max(0LL, std::min(idx, size));
}

Value Interpreter::visit(const SliceExpressionNode* node){
	Value object = evaluate(node->object.get());

	// kList
	if(object.getType() == ValueType::kList){
		auto list_ptr = object.asList();
		long long size = list_ptr->size();

		long long start = resolve_slice_index(node->start, size, 0);
		long long end = resolve_slice_index(node->end, size, size);

		auto new_list = std::make_shared<ListType>();

		if(start < end){
			for(long long i = start; i < end; ++i){
				new_list->push_back((*list_ptr)[i]);
			}
		}

		return Value(new_list);
	}

	// kString
	if(object.getType() == ValueType::kString){
		auto str_ptr = object.asString();
		long long size = str_ptr->length();

		long long start = resolve_slice_index(node->start, size, 0);
		long long end = resolve_slice_index(node->end, size, size);

		std::string new_str;

		if(start < end){
			new_str = str_ptr->substr(start, end - start);
		}

		return Value(new_str);
	}

	ErrorManager("SliceExpressionNode", "slicing operator [:] can only be applied to lists and strings.");
	return Value();
}

void Interpreter::pushCall(const std::string& name){
	call_stack_trace.push_back(name);
}

void Interpreter::popCall(){
	if(!call_stack_trace.empty()){
		call_stack_trace.pop_back();
	}
}

Value Interpreter::getStackTrace(){
	auto list_ptr = std::make_shared<ListType>();

	for(const auto& call_info : call_stack_trace){
		list_ptr->push_back(Value(call_info));
	}

	std::reverse(list_ptr->begin(), list_ptr->end());

	return Value(list_ptr);
}


bool interpret(std::istream& in, std::ostream& out){
	std::streambuf* oldCoutBuf = nullptr;
	try{
		std::string source_code;
		source_code.assign((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());

		Lexer lexer(source_code);
		Parser parser(lexer);
		std::unique_ptr<ProgramNode> ast_root = parser.parseProgram();

		oldCoutBuf = std::cout.rdbuf();
		std::cout.rdbuf(out.rdbuf());

		Interpreter interpreter(std::move(ast_root));

		std::cout.rdbuf(oldCoutBuf);

		return true;
	}
	catch(...){
		if(oldCoutBuf){
			std::cout.rdbuf(oldCoutBuf);
		}
		return false;
	}
}