#include <cassert>
#include <iostream>
#include <typeinfo>
//https://gist.github.com/jweyrich/4bd1d4556069f3d49d73
// la premiere version, les dernieres en c++11 compilent pas !
 template <typename ExpressionT>
struct expression
{
	virtual ~expression() {};
	virtual ExpressionT operator()() const = 0;
};
 template <typename ValueT>
struct value_token : public expression<ValueT>
{
	value_token(const ValueT value)
		: _value(value)
	{}
 	ValueT operator()() const {
		return _value;
	}
 public:
	ValueT _value;
};
 template <typename NumberT>
struct number_token : public value_token<NumberT>
{
	number_token(const NumberT value = {})
		: value_token<NumberT>(value)
	{}
};
 template <typename LeftT, typename RightT, typename ResultT>
struct binary_expression : public expression<ResultT>
{
	binary_expression(expression<LeftT> * const left = nullptr, expression<RightT> * const right = nullptr)
		: _left(left), _right(right)
	{}
 protected:
	expression<LeftT> * _left;
	expression<RightT> * _right;
};
 template <typename LeftT, typename RightT, typename ResultT>
struct add : public binary_expression<LeftT, RightT, ResultT>
{
	add(expression<LeftT> * const left, expression<RightT> * const right)
		: binary_expression<LeftT, RightT, ResultT>(left, right)
	{}
 	ResultT operator()() const {
		printf("ResultT = %s\n", typeid(ResultT).name());
		return (*this->_left)() + (*this->_right)();
	}
};
 template <typename LeftT, typename RightT, typename ResultT>
struct subtract : public binary_expression<LeftT, RightT, ResultT>
{
	subtract(expression<LeftT> * const left, expression<RightT> * const right)
		: binary_expression<LeftT, RightT, ResultT>(left, right)
	{}
 	ResultT operator()() const {
		printf("ResultT = %s\n", typeid(ResultT).name());
		return (*this->_left)() - (*this->_right)();
	}
};
 template <typename LeftT, typename RightT, typename ResultT>
struct multiply : public binary_expression<LeftT, RightT, ResultT>
{
	multiply(expression<LeftT> * const left, expression<RightT> * const right)
		: binary_expression<LeftT, RightT, ResultT>(left, right)
	{}
 	ResultT operator()() const {
		printf("ResultT = %s\n", typeid(ResultT).name());
		return (*this->_left)() * (*this->_right)();
	}
};
 template <typename LeftT, typename RightT, typename ResultT>
struct divide : public binary_expression<LeftT, RightT, ResultT>
{
	divide(expression<LeftT> * const left, expression<RightT> * const right)
		: binary_expression<LeftT, RightT, ResultT>(left, right)
	{}
 	ResultT operator()() const {
		printf("ResultT = %s\n", typeid(ResultT).name());
		return (*this->_left)() / (*this->_right)();
	}
};
 template <typename T>
class evaluator
{
public:
	const expression<T> * parse(const char *);			// Parse an expression
 private:
	expression<T> * parse_number(const char *&);		// Parse numeric constants
	expression<T> * parse_atom(const char *&);			// Parse nested expression
	expression<T> * parse_summands(const char *&);		// Parse '+' and '-' operations
	expression<T> * parse_factors(const char *&);		// Parse '*' and '/' operations
};
 template <typename T>
expression<T> * evaluator<T>::parse_number(const char *& s)
{
	assert("Sanity check" && s && std::isdigit(*s));
	number_token<T> * nt = new number_token<T>(0);
	// Convert number...
	double post_comma = 0;
	while (*s && (std::isdigit(*s) or (*s == '.') ) )
	{
		if (*s != '.')
		{
			if (not post_comma)	
				nt->_value = nt->_value * 10 + *s++ - '0';
			else
			{
				nt->_value = nt->_value + (*s++ - '0') * post_comma;
				post_comma /= 10;
			}
				
		}
		else
		{
			post_comma = .1;
			s++;
		}
	}
	return nt;
}
 template <typename T>
expression<T>* evaluator<T>::parse_atom(const char *& s)
{
	assert("Sanity check" && s);
	if (*s == 0)
	{
		throw std::runtime_error("Atom parse error: unexpected EOS");
	}
	else if (*s == '(')
	{
		s++;
		expression<T> * atom = parse_summands(s);
		if (*s == ')')
		{
			s++;
			return atom;
		}
		throw std::runtime_error("Atom parse error: unbalanced brackets");
 	}
	else if (std::isdigit(*s))
	{
		expression<T> * atom = parse_number(s);
		return atom;
	}
	throw std::runtime_error("Atom parse error: unexpected char");
}
 template <typename T>
expression<T>* evaluator<T>::parse_factors(const char *& s)
{
	assert("Sanity check" && s);
	expression<T> * left = parse_atom(s);
	while (*s)
	{
		if (*s == '*')
		{
			s++;
			expression<T> * right = parse_atom(s);
			left = new multiply<T,T,T>(left, right);
			continue;
		}
		else if (*s == '/')
		{
			s++;
			expression<T> * right = parse_atom(s);
			left = new divide<T,T,T>(left, right);
			continue;
		}
		return left;
	}
	return left;
}
 template <typename T>
expression<T> * evaluator<T>::parse_summands(const char *& s)
{
	assert("Sanity check" && s);
	expression<T> * left = parse_factors(s);
	while (*s)
	{
		if (*s == '+')
		{
			s++;
			expression<T> * right = parse_factors(s);
			left = new add<T,T,T>(left, right);
			continue;
		}
		else if (*s == '-')
		{
			s++;
			expression<T> * right = parse_factors(s);
			left = new subtract<T,T,T>(left, right);
			continue;
		}
		return left;
	}
	return left;
}
 template <typename T>
const expression<T> * evaluator<T>::parse(const char * s)
{
	return parse_summands(s);
}
 template <typename T>
T evaluate(const char* const e)
{
	evaluator<T> ev;
	const expression<T> * const ex = ev.parse(e);
	assert("Sanity check" && ex);
	return (*ex)();
}
 // s = evaluate(“(4+3)*2/5” );
// s = evaluate(“((12+9)/2)*5”);
 int main(int argc, char *argv[])
{
	// {
	// 	int a = evaluate<int>("(4+3)*2/5");
	// 	assert("Unexpected result" && a == 2);
	// 	std::cout << "\"(4+3)*2/5\" = " << a << std::endl;
	// }
	// {
	// 	int a = evaluate<int>("((12+9)/2)*5");
	// 	assert("Unexpected result" && a == 50);
	// 	std::cout << "\"((12+9)/2)*5\" = " << a << std::endl;
	// }
	
	std::istream& input_stream = std::cin;
	std::string input_expression;
	
	while (true)
	{	
		std::cout << "> ";
		if (!std::getline(input_stream, input_expression))
			break;
 		double result = evaluate<double>(input_expression.c_str());
		std::cout << "(" << input_expression << ") = " << result << std::endl;
	}
	return EXIT_SUCCESS;
} 
