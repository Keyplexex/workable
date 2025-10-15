#pragma once

#include <memory>
#include <vector>
#include <variant>
#include <functional>
#include <cmath>

enum class ValueType{
	kDouble,
	kString,
	kBool,
	kNil,
	kList,
	kFunc
};

class Value;

class Nil{};

class Function;

using ListType = std::vector<Value>;

class Value{
private:
	std::variant<
		double,							// double
		std::shared_ptr<std::string>,	// string
		bool,							// bool
		std::nullptr_t,					// nil
		std::shared_ptr<ListType>,		// list
		std::shared_ptr<Function>		// function
	> data;

public:
	// Constructors
	Value(double val);
	Value(const std::string& val);
	Value(const char* val);
	Value(bool val);
	Value();
	Value(std::shared_ptr<ListType> val);
	Value(std::shared_ptr<Function> val);

	ValueType getType() const; // get ValueType lol

	// конверты с std::variant
	double asNumber() const;
	std::shared_ptr<std::string> asString() const;
	bool asBool() const;
	std::shared_ptr<ListType> asList() const;
	std::shared_ptr<Function> asFunction() const;

	std::string toString() const; // converts everything to string for output
	bool isTruthy() const; // true or false (typical for loops and if)
};

class Function{
private:
	std::function<Value(const std::vector<Value>&)> func;

public:
	Function(std::function<Value(const std::vector<Value>&)> f)
		: func(f)
	{}

	Value operator()(const std::vector<Value>& args){
		return func(args);
	}
};