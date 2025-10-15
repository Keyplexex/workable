#include "interpreter.h"

Value Interpreter::add(const Value& left, const Value& right){
	if(left.getType() == ValueType::kDouble && right.getType() == ValueType::kDouble){
		return Value(left.asNumber() + right.asNumber());
	}

	if(left.getType() == ValueType::kString && right.getType() == ValueType::kString){
		return Value(*left.asString() + *right.asString());
	}

	if(left.getType() == ValueType::kList && right.getType() == ValueType::kList){
		auto new_list_ptr = std::make_shared<ListType>(*left.asList());
		new_list_ptr->insert(new_list_ptr->end(), right.asList()->begin(), right.asList()->end());
		return Value(new_list_ptr);
	}

	ErrorManager("add (+)", left, right);
	return Value();
}

Value Interpreter::subtract(const Value& left, const Value& right){
	if(left.getType() == ValueType::kDouble && right.getType() == ValueType::kDouble){
		return Value(left.asNumber() - right.asNumber());
	}

	if(left.getType() == ValueType::kString && right.getType() == ValueType::kString){
		std::string result = *left.asString();
		size_t pos = result.find(*right.asString());

		if(pos != std::string::npos){
			result.erase(pos, (right.asString())->length());
		}

		return Value(result);
	}

	ErrorManager("subtract (-)", left, right);
	return Value();
}

Value Interpreter::multiply(const Value& left, const Value& right){
	Value l = left;
	Value r = right;

	if(l.getType() == ValueType::kBool){
		l = Value(l.asBool() ? 1.0 : 0.0);
	}
	if(r.getType() == ValueType::kBool){
		r = Value(r.asBool() ? 1.0 : 0.0);
	}

	if(l.getType() == ValueType::kDouble && r.getType() == ValueType::kDouble){
		return Value(l.asNumber() * r.asNumber());
	}

	if(l.getType() == ValueType::kDouble){
		double n = l.asNumber();

		if(n <= 0){
			if(r.getType() == ValueType::kString){
				return Value("");
			}
			else if(r.getType() == ValueType::kList){
				return Value(std::make_shared<ListType>());
			}
			else{
				ErrorManager("multiply (*)", left, right);
				return Value();
			}
		}
		else{
			if(r.getType() == ValueType::kString || r.getType() == ValueType::kList){
				Value v = r;

				int int_part = static_cast<int>(std::floor(n));
				for(int i=0; i<int_part-1; ++i){
					v = add(v, r);
				}

				double frac_part = n - int_part;

				if(frac_part > 0){
					if(r.getType() == ValueType::kString){
						size_t slice_len = static_cast<size_t>(std::ceil(r.asString()->length() * frac_part));

						if(slice_len > 0){
							std::string slice = r.asString()->substr(0, slice_len);
							v = add(v, Value(slice));
						}
					}

					if(r.getType() == ValueType::kList){
						size_t slice_len = static_cast<size_t>(std::ceil(r.asList()->size() * frac_part));

						if(slice_len > 0){
							auto slice_list = std::make_shared<ListType>();
							slice_list->reserve(slice_len);

							for(size_t i=0; i < slice_len; ++i){
								slice_list->push_back((*r.asList())[i]);
							}

							v = add(v, Value(slice_list));
						}
					}
				}

				return v;
			}
			else{
				ErrorManager("multiply (*)", left, right);
				return Value();
			}
		}
	}

	if(r.getType() == ValueType::kDouble){
		double n = r.asNumber();

		if(n <= 0){
			if(l.getType() == ValueType::kString){
				return Value("");
			}
			else if(l.getType() == ValueType::kList){
				return Value(std::make_shared<ListType>());
			}
			else{
				ErrorManager("multiply (*)", left, right);
				return Value();
			}
		}
		else{
			if(l.getType() == ValueType::kString || l.getType() == ValueType::kList){
				Value v = l;

				int int_part = static_cast<int>(std::floor(n));
				for(int i=0; i<int_part-1; ++i){
					v = add(v, l);
				}

				double frac_part = n - int_part;

				if(frac_part > 0){
					if(l.getType() == ValueType::kString){
						size_t slice_len = static_cast<size_t>(std::ceil(l.asString()->length() * frac_part));

						if(slice_len > 0){
							std::string slice = l.asString()->substr(0, slice_len);
							v = add(v, Value(slice));
						}
					}

					if(l.getType() == ValueType::kList){
						size_t slice_len = static_cast<size_t>(std::ceil(l.asList()->size() * frac_part));

						if(slice_len > 0){
							auto slice_list = std::make_shared<ListType>();
							slice_list->reserve(slice_len);

							for(size_t i=0; i < slice_len; ++i){
								slice_list->push_back((*l.asList())[i]);
							}

							v = add(v, Value(slice_list));
						}
					}
				}

				return v;
			}
			else{
				ErrorManager("multiply (*)", left, right);
				return Value();
			}
		}
	}

	ErrorManager("multiply (*)", left, right);
	return Value();
}

Value Interpreter::divide(const Value& left, const Value& right){
	if(left.getType() == ValueType::kDouble && right.getType() == ValueType::kDouble){
		if(right.asNumber() == 0){
			ErrorManager("divide (/)", "division by zero");
			return Value();
		}

		return Value(left.asNumber() / right.asNumber());
	}

	ErrorManager("divide (/)", left, right);
	return Value();
}

Value Interpreter::modulo(const Value& left, const Value& right){
	if(left.getType() == ValueType::kDouble && right.getType() == ValueType::kDouble){
		return Value(std::fmod(left.asNumber(), right.asNumber()));
	}

	ErrorManager("modulo (%)", left, right);
	return Value();
}

Value Interpreter::power(const Value& left, const Value& right){
	if(left.getType() == ValueType::kDouble && right.getType() == ValueType::kDouble){
		return Value(std::pow(left.asNumber(), right.asNumber()));
	}

	ErrorManager("power (^)", left, right);
	return Value();
}

Value Interpreter::equal(const Value& left, const Value& right, bool isnot){
	if(left.getType() != right.getType()){
		return Value(false);
	}

	switch(left.getType()){
		case ValueType::kDouble: return Value(left.asNumber() == right.asNumber());
		case ValueType::kString: return Value(*(left.asString()) == *(right.asString()));
		case ValueType::kBool: return Value(left.asBool() == right.asBool());
		case ValueType::kNil: return Value(true);

		case ValueType::kList:{
			auto left_list = left.asList();
			auto right_list = right.asList();

			if(left_list->size() != right_list->size()){
				return Value(false);
			}

			for(size_t i=0; i < left_list->size(); ++i){
				Value elem_equal = equal((*left_list)[i], (*right_list)[i]);
				
				if(!elem_equal.asBool()){
					return Value(false);
				}
			}

			return Value(true);
		}

		case ValueType::kFunc:
			return Value(left.asFunction().get() == right.asFunction().get());
	}

	if(isnot) ErrorManager("not equal (!=)", left, right);
	else ErrorManager("equal (==)", left, right);
	return Value();
}

Value Interpreter::notEqual(const Value& left, const Value& right){
	return Value(!equal(left, right, true).asBool());
}

Value Interpreter::lessThan(const Value& left, const Value& right){
	if(left.getType() == ValueType::kDouble && right.getType() == ValueType::kDouble){
		return Value(left.asNumber() < right.asNumber());
	}

	if(left.getType() == ValueType::kString && right.getType() == ValueType::kString){
		return Value(*(left.asString()) < *(right.asString()));
	}
	
	ErrorManager("less than (<)", left, right);
	return Value();
}

Value Interpreter::greaterThan(const Value& left, const Value& right){
	if(left.getType() == ValueType::kDouble && right.getType() == ValueType::kDouble){
		return Value(left.asNumber() > right.asNumber());
	}

	if(left.getType() == ValueType::kString && right.getType() == ValueType::kString){
		return Value(*(left.asString()) > *(right.asString()));
	}

	ErrorManager("greater than (>)", left, right);
	return Value();
}

Value Interpreter::lessThanOrEqual(const Value& left, const Value& right){
	if(left.getType() == ValueType::kDouble && right.getType() == ValueType::kDouble){
		return Value(left.asNumber() <= right.asNumber());
	}

	if(left.getType() == ValueType::kString && right.getType() == ValueType::kString){
		return Value(*(left.asString()) <= *(right.asString()));
	}

	ErrorManager("less than or equal (<=)", left, right);
	return Value();
}

Value Interpreter::greaterThanOrEqual(const Value& left, const Value& right){
	if(left.getType() == ValueType::kDouble && right.getType() == ValueType::kDouble){
		return Value(left.asNumber() >= right.asNumber());
	}

	if(left.getType() == ValueType::kString && right.getType() == ValueType::kString){
		return Value(*(left.asString()) >= *(right.asString()));
	}

	ErrorManager("greater than or equal (>=)", left, right);
	return Value();
}