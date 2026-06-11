#pragma once
#include <string>

class ErrorManager{
private:
	std::string getTypeName(ValueType type){
		switch(type){
			case ValueType::kDouble: return "Double type";
			case ValueType::kString: return "String type";
			case ValueType::kList: return "List type";
			case ValueType::kBool: return "Bool type";
			case ValueType::kNil: return "Nil type";
			case ValueType::kFunc: return "Function type";
			default: return "";
		}
	}

	std::string formatError(const std::string& operation, const std::string& message){
		return operation+": "+message;
	}

public:
	// error messages
	ErrorManager(const std::string& message){
		throw std::runtime_error(message);
	}

	// syntax error messages
	ErrorManager(const std::string& operation, const std::string& message, int line){
		throw std::runtime_error(formatError(operation, message)+" (line "+std::to_string(line)+')');
	}

	// function format error messages
	ErrorManager(const std::string& operation, const std::string& message){
		throw std::runtime_error(formatError(operation, message));
	}

	// function format type error
	ErrorManager(const std::string& operation, const Value& left, const Value& right){
		throw std::runtime_error(formatError(operation, "cannot perform operation on " + getTypeName(left.getType()) + " and " + getTypeName(right.getType())));
	}

	// function argument count error
	ErrorManager(const std::string& function, size_t expected, size_t received){
		throw std::runtime_error(formatError(function, "expected " + std::to_string(expected) + " argument(s), got " + std::to_string(received)));
	}

	// function argument type error
	ErrorManager(const std::string& function, size_t argIndex, const std::string& expectedType, ValueType receivedType){
		throw std::runtime_error(formatError(function, "argument " + std::to_string(argIndex + 1) + " must be " + expectedType + ", got " + getTypeName(receivedType)));
	}
};