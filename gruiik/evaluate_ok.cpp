#include "mbprt.h"
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


struct expression
{
	char* s;
	static int idx;
	double arg;
	int level = 0;
	int par_level;
	static int par_level_close;
	bool in_par;
	bool is_lha;
	
	expression(char* s_i)
	{
		s = s_i;
		arg = 0;
		is_lha = 1;
		level = 0;
		idx = 0;
		in_par = 0;
		par_level_close = 1000;
		par_level = 0;
	}	
	expression(char* s_i, int par_level_i, bool is_lha_i)
	{
		s = s_i;
		arg = 0;
		is_lha = is_lha_i;
		par_level = par_level_i;
		in_par = 0;
		//par_level_close = 1000;
	}
	double evaluate(bool mult_div)
	{
		if (0)//par_level >= par_level_close)
		{
			gprintf("#rPurging paren level % returning %", par_level_close, int(arg));
			return arg;
		} 
		gprintf(">> idx=% / lvl % close %\n", idx, par_level, par_level_close);
		while (1)
		{
			char c = s[idx];
			int char_type = parse_char();
			//gprintf("#V :%s is_num % :", c, int(is_num(c)));
			if (char_type == 128)
			return arg;
			if (is_num(c))
			{
				gprintf("num_eval\n");
				arg = arg*10+(int(c)-48);
				gprintf("#r|%s| ", int(arg));
			}
			else if (is_op(c))
			{	
				gprintf("op evallha % mult_div%\n", int(is_lha), int(mult_div));
				expression exp(s, par_level, 0); // is rha
				int res = exp.evaluate(is_mult_div(c));
				if (not is_mult_div(c))
				{
					double add_res = is_inv_op(c) ? -res : res;
					//gprintf("#marg %", arg);
					//gprintf("#b arg % op % res % \n\n", int(arg), c, int(add_res));
					gprintf("#b  % %  % = % --- lha % mult_div %\n\n", int(arg), c, int(res), int(arg + add_res), int(is_lha), int(mult_div));
					if (is_lha and not mult_div)
							arg += add_res;
					else
							return arg+add_res;		
				}
				else
				{
					double mult_res = arg;
					if (is_inv_op(c))
						mult_res /= res;
					else
						mult_res *= res;
						
					gprintf("#m  % %  % = % \n\n", int(arg), c, int(res), int(mult_res));
						
					if (is_lha) // not the result of bin op, is LHA
						arg = mult_res;
					else 
						return mult_res; 
				}

			}
			else if (is_par_open(c))
			{
				//in_par = 1;
				gprintf("(((((((((parent open mult_div %\n", int(mult_div));
				expression exp(s, par_level+1, 1); // argument after ( is lha
				arg = exp.evaluate(mult_div);
				return arg;
			}
			else if (is_par_close(c))
			{
				gprintf("))))))))))) arg % mult_div % par %\n",int(arg), int(mult_div), par_level); 
				//if (not in_par) idx--;
				//return arg;
				par_level_close = par_level;
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
		gprintf("    idx % type %\n", idx, char_type);return char_type;
	}
};
#define xstr(a) str(a)
#define str(a) #a

int expression::idx;
int expression::par_level_close;
int main()
{
	#define expr 1+(3-2)*(4+6*3)+((3+4)*(5-2)+34)
	#define cexpr "1+2"
	//char calc[255] = "1+(3-2)*(4+6*3)+((3+4)*(5-2)+34)";
	char calc[255] = xstr(expr);
	expression exp(calc);
	double x = exp.evaluate(0);
	double t = expr;
	gprintf("Result of %s = % ref %\n", exp.s, int(x), int(t));
}
