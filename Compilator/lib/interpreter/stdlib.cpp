#include "interpreter.h"

#include <iostream>
#include <memory>
#include <ctime>
#include <algorithm>

#include "scope.h"

StandardLibrary::StandardLibrary(Scope& globals, Interpreter& interpreter){
	// random
	std::srand(static_cast<unsigned>(std::time(nullptr)));

	// abs(x)
	globals.define("abs", Value(std::make_shared<Function>([](const std::vector<Value>& args){
		if(args.size() != 1){
			ErrorManager("abs", 1, args.size());
		}
		if(args[0].getType() != ValueType::kDouble){
			ErrorManager("abs", 0, "number", args[0].getType());
		}

		return Value(std::abs(args[0].asNumber()));
	})));

	// ceil(x)
	globals.define("ceil", Value(std::make_shared<Function>([](const std::vector<Value>& args){
		if(args.size() != 1){
			ErrorManager("ceil", 1, args.size());
		}
		if(args[0].getType() != ValueType::kDouble){
			ErrorManager("ceil", 0, "number", args[0].getType());
		}

		return Value(std::ceil(args[0].asNumber()));
	})));

	// floor(x)
	globals.define("floor", Value(std::make_shared<Function>([](const std::vector<Value>& args){
		if(args.size() != 1){
			ErrorManager("floor", 1, args.size());
		}
		if(args[0].getType() != ValueType::kDouble){
			ErrorManager("floor", 0, "number", args[0].getType());
		}

		return Value(std::floor(args[0].asNumber()));
	})));

	// round(x)
	globals.define("round", Value(std::make_shared<Function>([](const std::vector<Value>& args){
		if(args.size() != 1){
			ErrorManager("round", 1, args.size());
		}
		if(args[0].getType() != ValueType::kDouble){
			ErrorManager("round", 0, "number", args[0].getType());
		}

		return Value(std::round(args[0].asNumber()));
	})));

	// sqrt(x)
	globals.define("sqrt", Value(std::make_shared<Function>([](const std::vector<Value>& args){
		if(args.size() != 1){
			ErrorManager("sqrt", 1, args.size());
		}
		if(args[0].getType() != ValueType::kDouble){
			ErrorManager("sqrt", 0, "number", args[0].getType());
		}

		double num = args[0].asNumber();
		if(num < 0) ErrorManager("sqrt", "cannot take square root of negative number");

		return Value(std::sqrt(num));
	})));

	// rnd(n)
	globals.define("rnd", Value(std::make_shared<Function>([](const std::vector<Value>& args){
		if(args.size() != 1){
			ErrorManager("rnd", 1, args.size());
		}
		if(args[0].getType() != ValueType::kDouble){
			ErrorManager("rnd", 0, "number", args[0].getType());
		}

		double n = args[0].asNumber();
		if(n <= 0) ErrorManager("rnd", "argument must be positive");

		return Value(static_cast<double>(std::rand() % static_cast<int>(n)));
	})));

	// parse_num(s)
	globals.define("parse_num", Value(std::make_shared<Function>([](const std::vector<Value>& args){
		if(args.size() != 1){
			ErrorManager("parse_num", 1, args.size());
		}
		if(args[0].getType() != ValueType::kString){
			ErrorManager("parse_num", 0, "string", args[0].getType());
		}

		std::string s = *args[0].asString();

		try{
			size_t pos;
			double num = std::stod(s, &pos);
			if(pos == s.size()) return Value(num);
		}
		catch(...){}

		return Value(); // nil при ошибке
	})));

	// to_string(x)
	globals.define("to_string", Value(std::make_shared<Function>([](const std::vector<Value>& args){
		if(args.size() != 1){
			ErrorManager("parse_num", 1, args.size());
		}

		return Value(args[0].toString());
	})));

	// len(s)
	globals.define("len", Value(std::make_shared<Function>([](const std::vector<Value>& args){
		if(args.size() != 1){
			ErrorManager("len", 1, args.size());
		}
		if(args[0].getType() != ValueType::kString && args[0].getType() != ValueType::kList){
			ErrorManager("len", 0, "string or list", args[0].getType());
		}

		if(args[0].getType() == ValueType::kString){
			return Value(static_cast<double>(args[0].asString()->size()));
		}
		if(args[0].getType() == ValueType::kList){
			return Value(static_cast<double>(args[0].asList()->size()));
		}

		return Value();
	})));

	// lower(s)
	globals.define("lower", Value(std::make_shared<Function>([](const std::vector<Value>& args){
		if(args.size() != 1){
			ErrorManager("lower", 1, args.size());
		}
		if(args[0].getType() != ValueType::kString){
			ErrorManager("lower", 0, "string", args[0].getType());
		}

		std::string str = *(args[0].asString());
		std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c){ return std::tolower(c); });

		return Value(str);
	})));

	// upper(s)
	globals.define("upper", Value(std::make_shared<Function>([](const std::vector<Value>& args){
		if(args.size() != 1){
			ErrorManager("upper", 1, args.size());
		}
		if(args[0].getType() != ValueType::kString){
			ErrorManager("upper", 0, "string", args[0].getType());
		}

		std::string str = *(args[0].asString());
		std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c){ return std::toupper(c); });

		return Value(str);
	})));

	// split(s)
	globals.define("split", Value(std::make_shared<Function>([](const std::vector<Value>& args){
		if(args.size() != 2){
			ErrorManager("split", 2, args.size());
		}
		if(args[0].getType() != ValueType::kString){
			ErrorManager("split", 0, "string", args[0].getType());
		}
		if(args[1].getType() != ValueType::kString){
			ErrorManager("split", 1, "string", args[1].getType());
		}

		std::string s = *args[0].asString();
		std::string delim = *args[1].asString();
		auto list = std::make_shared<ListType>();
		list->reserve(s.size() /(delim.empty() ? 1 : delim.size()) + 1);

		if(delim.empty()){
			for(char c : s){
				list->push_back(Value(std::string(1, c)));
			}
		}
		else{
			size_t start = 0, end;

			while((end = s.find(delim, start)) != std::string::npos){
				list->push_back(Value(s.substr(start, end - start)));
				start = end + delim.size();

			}
			list->push_back(Value(s.substr(start)));
		}

		return Value(list);
	})));

	// join(list, delim)
	globals.define("join", Value(std::make_shared<Function>([](const std::vector<Value>& args){
		if(args.size() != 2){
			ErrorManager("join", 2, args.size());
		}
		if(args[0].getType() != ValueType::kList){
			ErrorManager("join", 0, "list", args[0].getType());
		}
		if(args[1].getType() != ValueType::kString){
			ErrorManager("join", 1, "string", args[1].getType());
		}

		auto list = args[0].asList();
		std::string delim = *args[1].asString();
		std::string result;

		for(size_t i=0; i < list->size(); ++i){
			if(i > 0) result += delim;
			result +=(*list)[i].toString();
		}

		return Value(result);
	})));

	// replace(s, old, new)
	globals.define("replace", Value(std::make_shared<Function>([](const std::vector<Value>& args){
		if(args.size() != 3){
			ErrorManager("replace", 3, args.size());
		}
		if(args[0].getType() != ValueType::kString){
			ErrorManager("replace", 0, "string", args[0].getType());
		}
		if(args[1].getType() != ValueType::kString){
			ErrorManager("replace", 1, "string", args[1].getType());
		}
		if(args[2].getType() != ValueType::kString){
			ErrorManager("replace", 2, "string", args[2].getType());
		}

		std::string s = *args[0].asString();
		std::string oldStr = *args[1].asString();
		std::string newStr = *args[2].asString();

		size_t pos = 0;
		while((pos = s.find(oldStr, pos)) != std::string::npos){
			s.replace(pos, oldStr.size(), newStr);
			pos += newStr.size();
		}

		return Value(s);
	})));

	// range(start, end, step)
	globals.define("range", Value(std::make_shared<Function>([](const std::vector<Value>& args){
		if(args.size() < 1 || args.size() > 3){
			ErrorManager("range", "requires 1 to 3 arguments, got " + std::to_string(args.size()));
		}
		for(size_t i=0; i < args.size(); ++i){
			if(args[i].getType() != ValueType::kDouble){
				ErrorManager("range", i, "number", args[i].getType());
			}
		}

		double start =(args.size() == 1) ? 0 : args[0].asNumber();
		double end =(args.size() == 1) ? args[0].asNumber() : args[1].asNumber();
		double step =(args.size() == 3) ? args[2].asNumber() : 1.0;

		if(step == 0) ErrorManager("range", "step cannot be zero");

		auto list = std::make_shared<ListType>();
		if(step > 0){
			for(double i = start; i < end; i += step){
				list->push_back(Value(i));
			}
		}
		else{
			for(double i = start; i > end; i += step){
				list->push_back(Value(i));
			}
		}

		return Value(list);
	})));

	// push(list, x)
	globals.define("push", Value(std::make_shared<Function>([](const std::vector<Value>& args){
		if(args.size() != 2){
			ErrorManager("push", 2, args.size());
		}
		if(args[0].getType() != ValueType::kList){
			ErrorManager("push", 0, "list", args[0].getType());
		}

		args[0].asList()->push_back(args[1]);

		return Value(); // nil
	})));

	// pop(list)
	globals.define("pop", Value(std::make_shared<Function>([](const std::vector<Value>& args){
		if(args.size() != 1){
			ErrorManager("pop", 1, args.size());
		}
		if(args[0].getType() != ValueType::kList){
			ErrorManager("pop", 0, "list", args[0].getType());
		}

		auto list = args[0].asList();
		if(list->empty()) return Value(); // nil

		Value last = list->back();
		list->pop_back();

		return last;
	})));

	// insert(list, index, x)
	globals.define("insert", Value(std::make_shared<Function>([](const std::vector<Value>& args){
		if(args.size() != 3){
			ErrorManager("insert", 3, args.size());
		}
		if(args[0].getType() != ValueType::kList){
			ErrorManager("insert", 0, "list", args[0].getType());
		}
		if(args[1].getType() != ValueType::kDouble){
			ErrorManager("insert", 1, "number", args[1].getType());
		}

		auto list = args[0].asList();
		int index = static_cast<int>(args[1].asNumber());
		if(index < 0) index += list->size() + 1;
		index = std::clamp(index, 0, static_cast<int>(list->size()));

		list->insert(list->begin() + index, args[2]);
		return Value(); // nil
	})));

	// remove(list, index)
	globals.define("remove", Value(std::make_shared<Function>([](const std::vector<Value>& args){
		if(args.size() != 2){
			ErrorManager("remove", 2, args.size());
		}
		if(args[0].getType() != ValueType::kList){
			ErrorManager("remove", 0, "list", args[0].getType());
		}
		if(args[1].getType() != ValueType::kDouble){
			ErrorManager("remove", 1, "number", args[1].getType());
		}

		auto list = args[0].asList();
		int index = static_cast<int>(args[1].asNumber());

		if(index < 0) index += list->size();
		if(index < 0 || index >= list->size()) return Value(); // nil

		Value elem =(*list)[index];
		list->erase(list->begin() + index);

		return elem;
	})));

	// sort(list)
	globals.define("sort", Value(std::make_shared<Function>([](const std::vector<Value>& args){
		if(args.size() != 1){
			ErrorManager("sort", 1, args.size());
		}
		if(args[0].getType() != ValueType::kList){
			ErrorManager("sort", 0, "list", args[0].getType());
		}

		auto list = args[0].asList();
		std::sort(list->begin(), list->end(), [](const Value& a, const Value& b){
			if(a.getType() != b.getType()){
				return static_cast<int>(a.getType()) < static_cast<int>(b.getType());
			}

			switch(a.getType()){
				case ValueType::kDouble: return a.asNumber() < b.asNumber();
				case ValueType::kString: return *a.asString() < *b.asString();
				case ValueType::kBool:   return a.asBool() < b.asBool();
				case ValueType::kList:   return a.asList()->size() < b.asList()->size();
				default: return false; // we don't compare the other types
			}
		});

		return Value(); // nil
	})));

	// print(args)
	globals.define("print", Value(std::make_shared<Function>([](const std::vector<Value>& args){
		for(const Value& val : args){
			std::cout<<val.toString();
		}

		return Value();
	})));

	// println(args)
	globals.define("println", Value(std::make_shared<Function>([](const std::vector<Value>& args){
		for(const Value& val : args){
			std::cout<<val.toString();
		}
		std::cout<<'\n';

		return Value();
	})));

	// read(cin)
	globals.define("read", Value(std::make_shared<Function>([](const std::vector<Value>& args){
		for(const Value& val : args){
			std::cout<<val.toString();
		}

		std::string in;
		std::getline(std::cin, in);

		return Value(in);
	})));

	// stacktrace()
	globals.define("stacktrace", Value(std::make_shared<Function>([&](const std::vector<Value>& args){
		if(args.size() != 0){
			ErrorManager("stacktrace", 0, args.size());
		}

		return interpreter.getStackTrace();
	})));

	// show_ast()
	globals.define("show_ast", Value(std::make_shared<Function>([&globals](const std::vector<Value>& args){
		if(args.size() != 0){
			ErrorManager("show_ast", 0, args.size());
		}

		std::cout<<"Abstract Syntax Tree(AST):\n";
		const auto& ast_root = globals.getAstRoot();

		if(ast_root){
			std::cout<<ast_root->toString()<<"\n";
		}
		else{
			std::cout<<"AST parsing resulted in a null root.\n";
		}

		std::cout<<"End of AST\n";

		return Value();
	})));

	// exit()
	globals.define("exit", Value(std::make_shared<Function>([&globals](const std::vector<Value>& args){
		if(args.size() != 0){
			ErrorManager("exit", 0, args.size());
		}

		std::cout<<"\nExiting interactive mode (exit).\n";
		exit(0);

		return Value();
	})));

	// help()
	globals.define("help", Value(std::make_shared<Function>([&globals](const std::vector<Value>& args){
		if(args.size() != 0){
			ErrorManager("help", 0, args.size());
		}

		std::cout<<"Welcome to ITMOScript 1.0.0's help utility!\n\n";
		std::cout<<"Available standard library functions:\n";
		std::cout<<"------------------------------------\n";

		// Number functions
		std::cout<<"Number functions:\n";
		std::cout<<"  abs(x)           - Returns the absolute value of number x\n";
		std::cout<<"  ceil(x)          - Rounds number x up to the nearest integer\n";
		std::cout<<"  floor(x)         - Rounds number x down to the nearest integer\n";
		std::cout<<"  round(x)         - Rounds number x to the nearest integer\n";
		std::cout<<"  sqrt(x)          - Returns the square root of number x (nil for negative x)\n";
		std::cout<<"  rnd(n)           - Returns a random integer from 0 to n-1\n";
		std::cout<<"  parse_num(s)     - Converts string s to a number, returns nil if invalid\n";
		std::cout<<"  to_string(x)     - Converts any value x to its string representation\n";

		// String functions
		std::cout<<"\nString functions:\n";
		std::cout<<"  len(s)           - Returns the length of string or list s\n";
		std::cout<<"  lower(s)         - Converts string s to lowercase\n";
		std::cout<<"  upper(s)         - Converts string s to uppercase\n";
		std::cout<<"  split(s, delim)  - Splits string s by delimiter delim into a list\n";
		std::cout<<"  join(list, delim)- Joins list elements into a string with delimiter delim\n";
		std::cout<<"  replace(s, old, new) - Replaces all occurrences of old with new in string s\n";

		// List functions
		std::cout<<"\nList functions:\n";
		std::cout<<"  range(x, y, step)- Returns a list of numbers from x to y (exclusive) with step\n";
		std::cout<<"  len(list)        - Returns the length of string or list\n";
		std::cout<<"  push(list, x)    - Appends element x to the end of list\n";
		std::cout<<"  pop(list)        - Removes and returns the last element of list (nil if empty)\n";
		std::cout<<"  insert(list, index, x) - Inserts element x at index in list\n";
		std::cout<<"  remove(list, index) - Removes and returns element at index in list (nil if invalid)\n";
		std::cout<<"  sort(list)       - Sorts list in ascending order\n";

		// System functions
		std::cout<<"\nSystem functions:\n";
		std::cout<<"  print(...)       - Prints arguments without a newline\n";
		std::cout<<"  println(...)     - Prints arguments with a newline\n";
		std::cout<<"  read(...)        - Reads a line from input, optionally printing arguments first\n";
		std::cout<<"  stacktrace()     - Returns the current call stack as a list\n";
		std::cout<<"  show_ast()       - Prints the abstract syntax tree of the program\n";
		std::cout<<"  exit()           - Exits the interpreter\n";
		std::cout<<"  help()           - Displays this help message\n";

		std::cout<<"------------------------------------\n";

		return Value();
	})));
}