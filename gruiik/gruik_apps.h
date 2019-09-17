#include "math.h" // for double calc operators

GKMODULE(kb_app_t)
{
	MODULE_INST(EXIT_, gk_menu);
	MODULE_INST(kb0, gk_kb0);
	MODULE_INST(term2, gk_text_box);
	SIGNAL(signal0, single_data);
	LINK(signal0, kb0);
	LINK(signal0, term2);
	GKCTOR(kb_app_t)
	{
		//exit_.caption.auto_scale = 0;
		//exit_.caption.font_div = 1;
		term2.caption.crop = 0;
	}

	void layout()
	{
		term2.caption.font_div = 1;
		split<VERT>(15, EXIT_, term2);

		term2.split<VERT>(128, term2, kb0);
		SET_CALLBACK(EXIT_, kb_app_t, quit);
	}


	void quit()
	{
			set_invisible();
			set_inactive();
			focus_obj = &gruik_top;
			locking_obj = &gruik_top;
			refresh();
	}

};

#define msg(a) //std::cerr << a
//#include <cassert>
//#include <iostream>
//#include <typeinfo>
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
		//printf("ResultT = %s\n", typeid(ResultT).name());
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
		//printf("ResultT = %s\n", typeid(ResultT).name());
 		msg("BESUB/");
 		gprintf("% ", uint64_t(&(*this->_right)));
 		msg("BESUB/");

 		gprintf("% ", uint64_t(&(*this->_left)));
 		msg("BESUB/");


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
		//printf("ResultT = %s\n", typeid(ResultT).name());
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
		//printf("ResultT = %s\n", typeid(ResultT).name());
		return (*this->_left)() / (*this->_right)();
	}
};

 bool isdigit(char c)
 {
	 return ( (c > 47) && (c < 58) ) or (c == 'E') or (c == 'e') or (c == '.');
 }

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
	 msg("PN/");
	//assert("Sanity check" && s && std::isdigit(*s));
	number_token<T> * nt = new number_token<T>(0);
	// Convert number...

	if (*s == 0)
	{
		gprintf("#REMPTY EXP");
		nt->_value = 0;
		return nt;
	}
	double post_comma = 0;
	bool neg = 0;
	if (*s == '-')
	{
		neg = 1;
		s++;
	}
	while (*s && isdigit(*s))
	{

		if (*s == '.')
		{
			post_comma = .1;
			s++;
		}
		else if ( (*s == 'e') or (*s == 'E') )
		{
			s++;
			bool neg_pow = 0;
			if (*s == '+')
			{
				s++;
			}
			else if (*s == '-')
			{
				neg_pow = 1; s++;
			}

			expression<T> * atom = parse_number(s);
			if (not neg_pow)
				nt->_value *= pow(10, (*atom)());
			else
				nt->_value *= pow(10, -(*atom)());

		}
		else
		{
			if (not post_comma)	
				nt->_value = nt->_value * 10 + *s++ - '0';
			else
			{
				nt->_value = nt->_value + (*s++ - '0') * post_comma;
				post_comma /= 10;
			}
				
		}
	}
	if (neg)
		nt->_value = - nt->_value;
	return nt;
}
 template <typename T>
expression<T>* evaluator<T>::parse_atom(const char *& s)
{
	//assert("Sanity check" && s);
	 msg("PA/");
	if (*s == 0)
	{
		expression<T> * atom = parse_number(s);
		//throw std::runtime_error("Atom parse error: unexpected EOS");
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
		//throw std::runtime_error("Atom parse error: unbalanced brackets");
 	}
	else if (isdigit(*s))
	{
		expression<T> * atom = parse_number(s);
		return atom;
	}
	else // none
	{

		number_token<T> * nt = new number_token<T>(0);
		expression<T> * atom =nt;
		return atom;
	}
	//throw std::runtime_error("Atom parse error: unexpected char");
	 msg("!PA/");

}
 template <typename T>
expression<T>* evaluator<T>::parse_factors(const char *& s)
{
	 msg("PF/");

	//assert("Sanity check" && s);
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
	//assert("Sanity check" && s);
	 msg("PS/");

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
	 msg("P/");

	evaluator<T> ev;
	const expression<T> * const ex = ev.parse(e);
	//assert("Sanity check" && ex);
	return (*ex)();
}

GKMODULE(calc_app_t)
{
	MODULE_INST(EXIT_, gk_menu);
	MODULE_INST(kb0, gk_calc);
	MODULE_INST(term2, gk_console);//text_box);
	SIGNAL(signal0, single_data);
	//SIGNAL(signal1, single_data);
	LINK(signal0, kb0);
	signal_link link0=make_combo2_t(signal0, *this); // LINK macro defines link name with *this inside -> don't use macro
	//LINK(signal0, term2);
	//FreePixel_32 big_font;
	VeraMoBd_32 big_font;
	//int start = 0;
	//int startp = 0;
	GKCTOR(calc_app_t)
	{
		//exit_.caption.auto_scale = 0;
		//exit_.caption.font_div = 1;
		//term2.caption.crop = 0;
		term2.font = &big_font;
	}

	void layout()
	{
		//term2.caption.font_div = 1;
#ifndef CALC_SOLO
		map<RIGHT>(128);
#endif
		split<VERT>(15, EXIT_, term2);

		term2.split<VERT>(100, term2, kb0);
		SET_CALLBACK(EXIT_, calc_app_t, quit);
	}

	void signal_event(uint8_t x)
	{

		double result;
		if (x == '\n')
		{
			std::string s;
			char c = ' ';
			term2.buf.init_line();
			do
			{
				c = term2.buf.get_char();
				gprintf("#Cc = %m %m", int(c), c);
				s += c;
			}
			while (c != 10);
			gprintf("#VExpression %Y", s);
		    result = evaluate<double>(s.c_str());
			//std::cerr << "(" << s << ") = " << result << std::endl;
			char res[100];
			sprintf(res, "\n%.6g", result);
			term2.print_line(res);	
			//startp = start;
			//start = term2.buf.cur_char;
		}
		else if (x == 'C')
		{
			term2.signal_event(10);
		}
		else if (x == 'R')
		{
			/*
			term2.buf.cur_set_prev_line();
			char c = 0;
			do
			{
				c = term2.buf.get_char();
				signal_event(c);
			}
			while(c != 10)
			*/
		}
		else if (x == 1)
		{
			term2.print_line("*(-1)");
		}
		else
			term2.signal_event(x);


	}

	void quit()
	{
			set_invisible();
			set_inactive();
			focus_obj = &gruik_top;
			locking_obj = &gruik_top;
			refresh();
	}

};
