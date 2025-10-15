#include "value.h"

#include "errorManager.h"

Value::Value(double val)
	: data(val)
{}

Value::Value(const std::string& val)
	: data(std::make_shared<std::string>(val))
{}

Value::Value(const char* val)
	: data(std::make_shared<std::string>(val))
{}

Value::Value(bool val)
	: data(val)
{}

Value::Value()
	: data(nullptr)
{}

Value::Value(std::shared_ptr<ListType> val)
	: data(val)
{}

Value::Value(std::shared_ptr<Function> val)
	: data(val)
{}


ValueType Value::getType() const{
	switch(data.index()){
		case 0: return ValueType::kDouble;
		case 1: return ValueType::kString;
		case 2: return ValueType::kBool;
		case 3: return ValueType::kNil;
		case 4: return ValueType::kList;
		case 5: return ValueType::kFunc;
		default:
			return ValueType::kNil;
	}
}

double Value::asNumber() const{
	if(auto val = std::get_if<double>(&data)){
		return *val;
	}

	ErrorManager("Value is not a number");
	return 0;
}

std::shared_ptr<std::string> Value::asString() const{
	if(auto val = std::get_if<std::shared_ptr<std::string>>(&data)){
		return *val;
	}

	ErrorManager("Value is not a string");
	return nullptr;
}

bool Value::asBool() const{
	if(auto val = std::get_if<bool>(&data)){
		return *val;
	}

	ErrorManager("Value is not a boolean");
	return false;
}

std::shared_ptr<ListType> Value::asList() const{
	if(auto val = std::get_if<std::shared_ptr<ListType>>(&data)){
		return *val;
	}

	ErrorManager("Value is not a list");
	return nullptr;
}

std::shared_ptr<Function> Value::asFunction() const{
	if(auto val = std::get_if<std::shared_ptr<Function>>(&data)){
		return *val;
	}

	ErrorManager("Value is not a function");
	return nullptr;
}



std::string Value::toString() const{
	switch(getType()){
		case ValueType::kDouble:{
			double num = asNumber();

			std::string str = std::to_string(num);
			str.erase(str.find_last_not_of('0') + 1, std::string::npos);
			if(str.back() == '.') str.pop_back();

			return str;
		}

		case ValueType::kString: return *(asString());
		case ValueType::kBool: return asBool() ? "true" : "false";
		case ValueType::kNil: return "nil";
		case ValueType::kList:{
			auto list = asList();

			std::string out;
			out.reserve(list->size() * 10);
			out += "[";

			for(size_t i = 0; i < list->size(); ++i){
				if(i > 0) out += ", ";

				if((*list)[i].getType() == ValueType::kString){
					out += "\""+(*list)[i].toString()+"\"";
				}
				else{
					out +=(*list)[i].toString();
				}
			}
			out += "]";

			return out;
		}

		case ValueType::kFunc:{
			std::shared_ptr<Function> func = asFunction();

			return "<function at "+std::to_string(reinterpret_cast<uintptr_t>(func.get()))+">";
		}

		default: return "";
	}
}

bool Value::isTruthy() const{
	switch(getType()){
		case ValueType::kDouble: return asNumber() != 0.0;
		case ValueType::kString: return !asString()->empty();
		case ValueType::kBool: return asBool();
		case ValueType::kNil: return false;
		case ValueType::kList: return !asList()->empty();
		case ValueType::kFunc: return true; // function is always true
		default: return false;
	}
}