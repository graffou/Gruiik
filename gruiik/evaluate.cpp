#include "mbprt.h"
#include<iostream>
enum {num, op, par_open, par_close, end};
uint8_t is_num(char c)
{
	//gprintf(" char % % is num % \n", c, int(c), int((c > 47) and (c < 59)));
	return (c > 47) and (c < 59);
}
uint8_t is_op(char c)
{
	return (c == '+') or (c == '-') or (c == '*') or (c == '/');
}
uint8_t is_mult_div(char c)
{
	return (c == '*') or (c == '/');
}
uint8_t is_inv_op(char c)
{
	return (c == '-') or (c == '/');
}
uint8_t is_par_open(char c) 
{
	return c == '(';
}
uint8_t is_par_close(char c) 
{
	return c == ')';
}
uint8_t is_string_end(char c)
{
	return c == 0;
}

void tab(int n)
{
	for (int i = 0; i < n; i++)
	std::cerr << " ";
}

struct expression
{
	char* s;
	static int idx;
	double arg;
	int level = 0;
	int par_level;
	bool is_lha;
	bool in_mult_div;
	
	expression(char* s_i)
	{
		s = s_i;
		arg = 0;
		is_lha = 1;
		level = 0;
		idx = 0;
		par_level = 0;
		in_mult_div = 0;
	}	
	expression(char* s_i, int par_level_i, bool is_lha_i)
	{
		s = s_i;
		arg = 0;
		is_lha = is_lha_i;
		par_level = par_level_i;
		in_mult_div = 0;
	}
	double evaluate(bool mult_div)
	{

		tab(level);gprintf(">> idx=% / lvl % par lvl % \n", idx, level, par_level);
		while (1)
		{
			char c = s[idx];
			int char_type = parse_char();
			tab(level);gprintf("#V :%s is_num % :", c, int(is_num(c)));
			if (char_type == 128)
			return arg;
			if (is_num(c))
			{
				tab(level);gprintf("num_eval\n");
				arg = arg*10+(int(c)-48);
				tab(level);gprintf("#r|%s| ", int(arg));
			}
			else if (is_op(c))
			{	
				tab(level);gprintf("#Mop eval %R lha % mult_div % level %B", c, int(is_lha), int(in_mult_div), level); 
				if (in_mult_div and not is_mult_div(c))
				{
					tab(level);gprintf("purging * \n");
					idx--;
					return arg;
				}	
				expression exp(s, par_level, 0); // is rha
				exp.level = level+1;
				tab(level);gprintf("#T>>>>>>> rha op");
				int res = exp.evaluate(is_mult_div(c));
				if (not is_mult_div(c))
				{
					double add_res = is_inv_op(c) ? -res : res;
					//gprintf("#marg %", arg);
					//gprintf("#b arg % op % res % \n\n", int(arg), c, int(add_res));
					tab(level);gprintf("#b% %  % = % --- lha % mult_div %", int(arg), c, int(res), int(arg + add_res), int(is_lha), int(mult_div));
					in_mult_div = 0;
					if (is_lha)
						arg+=add_res;
					else
					{
						tab(level);gprintf("#U<<<< add res %", add_res);
						return arg + add_res;
					}
					//if (is_lha and not mult_div)
					//arg += add_res;
					//else
	
				}
				else
				{
					in_mult_div = 1;
					double mult_res = arg;
					if (is_inv_op(c))
						mult_res /= res;
					else
						mult_res *= res;
						
					tab(level);gprintf("#m% %  % = % ", int(arg), c, int(res), int(mult_res));
						
					if (is_lha) // not the result of bin op, is LHA
						arg = mult_res;
					else 
					{
						tab(level);gprintf("#U<<<< mult res %", mult_res);

						return mult_res; 
					}
				}

			}
			else if (is_par_open(c))
			{
				//in_par = 1;
				tab(level);gprintf("(((((((((parent open mult_div %\n", int(mult_div));
				tab(level);gprintf("#T>>>>>> paren open");
				expression exp(s, par_level+1, 1); // argument after ( is lha
				exp.level = level+1;
				arg = exp.evaluate(0);
				tab(level);gprintf("#U<<<< par open");
				//if (not is_mult_div(s[idx+1]))
				return arg;
			}
			else if (is_par_close(c))
			{
				tab(level);gprintf("))))))))))) arg % mult_div % par %\n",int(arg), int(mult_div), par_level); 
				tab(level);gprintf("#U<<<< par close");
				return arg;
			}	
			else if (is_string_end(c))
			{
				return arg;
			}
		}			
	}
	
	int parse_char()
	{
		char c = s[idx];
	//gprintf(" % : ", int (c));
		int char_type = 
		is_string_end(c) << 7 | 
		is_num(c) | 
		is_op(c) << 1 | 
		is_par_open(c) << 2 |
		is_par_close(c) << 3; 
		idx+= (char_type != 128);
		tab(level);gprintf("idx % type %\n", idx, char_type);return char_type;
	}
};
#define xstr(a) str(a)
#define str(a) #a

int expression::idx;
int main()
{
	#define expr 1+(3-2)*(4+6*3)+((3+4)*(5-2)+34)
	//#define expr 1+(3-2)*(4+6*3)+((3+4)*(5-2)+34)-((56+((((5-(4*5)-4*2)*4+3-4))*2)-4*5+3))
	//#define expr ((56+((((5-(4*5)-4*2)*4+3-4))*2)-4*5+3))
	//#define expr (((((3-4))*2)+3))
	//#define expr (3-4)*2+3
	//char calc[255] = "1+(3-2)*(4+6*3)+((3+4)*(5-2)+34)";
	char calc[255] = xstr(expr);
	expression exp(calc);
	double x = exp.evaluate(0);
	double t = expr;
	gprintf("Result of %s = %R ref %R %R % % %\n", exp.s, int(x), int(t), (3-4)*2+3, (3-4),(3-4)*2,(3-4)*2+3);
}
