#ifndef __MBPRT_H__
#define __MBPRT_H__

#include <stdio.h>
#include <string>
   using std::string;

#ifdef PC_GRUIK
#include<iostream>
#endif

#define MSG(a) //std::cerr << a << "\n";

struct color
{
  int v;
  
  color(const int x)
    {
      //v = x > 31 ? 31 : x;
      //v = 31 + (x&7) + 6*((x&4) != 0) + 256*((x&8) != 0) + 16*((x&16) != 0);
      v = x > 63 ? 63 : x;
      v = x > 19 ? (x + 267) : x + 31;
      v = x == 63 ? 0 : v;
      v = x == 62 ? 257 : v;
    }

};

template <class T>
 int color_code(T supto)
    {
        //std::cerr << " ! " << supto << " ! ";
      int code = 1024;
	      if (supto == 'r')
		code = 0;
	      if (supto == 'g')
		code = 1;
	      if (supto == 'y')
		code = 2;
	      if (supto == 'b')
		code = 3;
	      if (supto == 'm')
		code = 4;
	      if (supto == 'c')
		code = 5;
	      if (supto == 'k')
		code = 63;

	      if (supto == 'R')
		code = 20;
	      if (supto == 'G')
		code = 21;
	      if (supto == 'Y')
		code = 22;
	      if (supto == 'B')
		code = 23;
	      if (supto == 'M')
		code = 24;
	      if (supto == 'C')
		code = 25;	      
	      if (supto == 'K')
		code = 62;
	      if (supto == 'T')
		code = (15<<8)+22;
	      if (supto == 'U')
		code = (14<<8)+21;
	      if (supto == 'V')
		code = (13<<8)+20;
	      if (supto == 'W')
		code = (12<<8)+24;
        //std::cerr << " !! " << code << " !! ";
	      return code;
    }


template <class T>
string to_string(const T& x)
{
    //return static_cast<string>(x);
	string s;
	s += x;
	return s;
}


template<>
string to_string(const short int& x)
{
    char buf[12];
    sprintf(buf, "%d", x);
    return string(buf);
}


template<>
string to_string(const int& x)
{
    char buf[12];
    sprintf(buf, "%d", x);
    return string(buf);
}

template<>
string to_string(const color& x)
{
    string os;
    if (x.v & 256)
    {
        os += "\033[1;";
    }
    else
    {
        os += "\033[0;";
    }
    
    os += to_string(x.v&255);
    os += "m";
    return os;
}

struct gstring : public string
{
    //using string::size;
  bool color_change;
  bool no_color_change;
  int base_color;
  int rcnt;

    gstring()
        {
            base_color = 1024;
            rcnt = 0;
        }

    template <class T>
    gstring(T x) : string(x)
        {
            base_color = 1024;
            rcnt = 0;
        }
    
    bool set_color(gstring& os, int code)
        {
            //std::cerr << ":" << code<<":";
            if (code != 1024)
            {
                if (code < 256)
                {
                    os += to_string(color(code));
                }
                else
                {
                    os += to_string(color(code >> 8));
                    os += to_string(color(code & 0xff));
                }
                return true;
            }
      
            return false;
        }

    char get_char(int init = 0)
        {
            //std::cerr << "#" << rcnt << " " << size() << "\n";
            char c;
            if (init == 1)
            {rcnt = 0; return ' ';}
            else
            {
                c = string::operator[](rcnt);
                if (!init)
                    rcnt++;
                return c;
            }
	
        }
    

    // Find a character
    bool upto_char(gstring& os, char x)
        {
            //std::cerr << "&" << rcnt;
            char c;
            int code = 1024;
            while (rcnt < size())
            {
                c = this->get_char();
                //std::cerr << c << rcnt;


                if (rcnt == 1)
                {
                    if (c == '#'||c == '@')
                    {
                        if (c == '@')
                            no_color_change = 1;
                        else
                            no_color_change = 0;
		  
                        c = this->get_char();
                        code = color_code(c);
                        base_color = code;
		  
                        if (code != 1024)
                        {
                            set_color(os, base_color);
                            color_change = 1;
                        }
		      
		  
                        c = this->get_char();
                    }
                    else
                    {
                        base_color = 1024;
                        color_change = 0;
                        no_color_change = 0;		
                    }
                    //std::cerr << " " << base_color << " ";
                }
	  
                if (c == x){//std::cerr << "]";
                    return true;}
                os += c;
            }
            return false;	      
        }

    
    // returns a string from start up to next % sign. next_start is the next pos to scan from
    void till_percent(gstring& os, int start, int& next_start)
        {
            //std::cerr << " " << base_color << " ";
            //res = "";
            //int upto;

            //bool found = this->find_char('%',start, upto);
            if (base_color != 1024)
                set_color(os, base_color);
	
            bool found = this->upto_char(os, '%');


            next_start = start; // PG 0514, not used anymore but avoid warnings
      
      
            if (!found || rcnt == size())
            {
                next_start = size();
            }
            else
            {
                char supto = this->get_char(4);//s[upto+1];
                //std::cerr << ':' << supto << ':';
                if (supto == ' ' || supto == '\n' || supto == '%' ||
                    supto == ',' || supto == '.' ||supto == ';'||supto == ')' ||supto == ']' )
                {
                     MSG("CZ");
                   
                }
                else
                {
                    MSG("CC");
                    color_change |= set_color(os, color_code(this->get_char()));
                }
            }

            //this->clone(res, start, upto - start);
            //os << res;
            // std::cerr << "//TP";
            //return res;
            
        }
    
// display string from position start till ther next % sign. start is updated w/ the next pos to scan from
    void display_till_percent(gstring& os, int& start)
        {
            //std::cerr << "DTP";
            //gstring lala;
            int next_start = 0; //PG 0515, not used trying to avoid warnings

            if (size() > rcnt)
            {
      
                this->till_percent(os, start, next_start);
                //os << lala;
                start = next_start;
            }
            //std::cerr << "//DTE" << rcnt << " " << size();
            //std::cerr << "//DTP";
        }


// Display the string from start til the end
    void display_till_end(gstring& os, int& start)
        {
            //std::cerr << "DTE" << rcnt << " " << size();
            //int next_start; -- !!!!!!
            if (size() > rcnt)
            {
                this->upto_char(os, '%');
                //std::cerr << "(" << rcnt << ")";
                //this->clone(lala, start, size() - start);// - 1);
                //os << lala;
                //start = next_start;-- !!!!!!
            }
  
            if (color_change && !no_color_change)
            {
#ifndef NO_COLOR      
                os += "\033[0m\n";
                //os += std::endl;
#else
                os += "\n";//std::endl;
#endif
      
            }
#ifdef DISCO
            pc.printf(os.c_str());
#else
            printf(os.c_str());

#endif
            //std::cerr << "//DTE";
  
        }

    template <class T>
    void to_stream(gstring& os,  T& x)
        {
            os += to_string(x);
            //std::cerr << "//TS";
        }

    gstring& append_char(char x)
       {
         this->operator+=(x);
       }

    
};




//template <class T, class T0>
template <class T0>
//void gprintf(T& os, T0 x)
void gprintf(T0& x)
{
    gstring os;
    os = to_string(x);
#ifndef PC_GRUIK
    printf(os.c_str());
#else
    std::cerr << os;
#endif
}

#if 0
//template <class T>
//void gprintf(T& os, const char* format)
static void gprintf(const char* format)
{
  int start=0;
    gstring os;
  gstring toto = format; toto.get_char(true);
  toto.display_till_percent(os, start);
  toto.display_till_end(os, start);
}


// gprintf templates for nargs = 1...10
template <class T0>
void gprintf(const char* format, const T0& x0)
{
    gstring os;
  gstring toto = format; toto.get_char(true);
  MSG(1);
  int start=0;

  // before 1st %
  toto.display_till_percent(os, start);
  MSG(2);
  toto.to_stream(os, x0); toto.display_till_percent(os, start);
  MSG(3) 
  // after last %
  toto.display_till_end(os, start);
}

template <class T0, class T1>
void gprintf(const char* format, const T0& x0,const T1& x1)
{
  gstring toto = format; toto.get_char(true);
  
    gstring os;
  int start=0;

  // before 1st %
  toto.display_till_percent(os, start);

  toto.to_stream(os, x0); toto.display_till_percent(os, start);
  toto.to_stream(os, x1); toto.display_till_percent(os, start);
    
  // after last %
  toto.display_till_end(os, start);
}

template <class T0, class T1, class T2, class T3>
void gprintf(const char* format, const T0& x0, const T1& x1, const T2& x2, const T3& x3)
{
  gstring toto = format; toto.get_char(true);
    gstring os;
  
  int start=0;

  // before 1st %
  toto.display_till_percent(os, start);

  toto.to_stream(os, x0); toto.display_till_percent(os, start);
  toto.to_stream(os, x1); toto.display_till_percent(os, start);
  toto.to_stream(os, x2); toto.display_till_percent(os, start);
  toto.to_stream(os, x3); toto.display_till_percent(os, start);
    
  // after last %
  toto.display_till_end(os, start);
}

template <class T0, class T1, class T2>
void gprintf(const char* format, const T0& x0, const T1& x1, const T2& x2)
{
  gstring toto = format; toto.get_char(true);
    gstring os;
  
  int start=0;

  // before 1st %
  toto.display_till_percent(os, start);

  toto.to_stream(os, x0); toto.display_till_percent(os, start);
  toto.to_stream(os, x1); toto.display_till_percent(os, start);
  toto.to_stream(os, x2); toto.display_till_percent(os, start);
    
  // after last %
  toto.display_till_end(os, start);
}


template <class T0, class T1, class T2, class T3, class T4>
void gprintf(const char* format, const T0& x0, const T1& x1, const T2& x2, const T3& x3, const T4& x4)
{
  gstring toto = format; toto.get_char(true);
    gstring os;
  
  int start=0;

  // before 1st %
  toto.display_till_percent(os, start);

  toto.to_stream(os, x0); toto.display_till_percent(os, start);
  toto.to_stream(os, x1); toto.display_till_percent(os, start);
  toto.to_stream(os, x2); toto.display_till_percent(os, start);
  toto.to_stream(os, x3); toto.display_till_percent(os, start);
  toto.to_stream(os, x4); toto.display_till_percent(os, start);
    
  // after last %
  toto.display_till_end(os, start);
}

template <class T0, class T1, class T2, class T3, class T4, class T5>
void gprintf(const char* format, const T0& x0, const T1& x1, const T2& x2, const T3& x3, const T4& x4, const T5& x5)
{
  gstring toto = format; toto.get_char(true);
    gstring os;
  
  int start=0;

  // before 1st %
  toto.display_till_percent(os, start);

  toto.to_stream(os, x0); toto.display_till_percent(os, start);
  toto.to_stream(os, x1); toto.display_till_percent(os, start);
  toto.to_stream(os, x2); toto.display_till_percent(os, start);
  toto.to_stream(os, x3); toto.display_till_percent(os, start);
  toto.to_stream(os, x4); toto.display_till_percent(os, start);
  toto.to_stream(os, x5); toto.display_till_percent(os, start);
    
  // after last %
  toto.display_till_end(os, start);
}

template <class T0, class T1, class T2, class T3, class T4, class T5, class T6>
void gprintf(const char* format, const T0& x0, const T1& x1, const T2& x2, const T3& x3, const T4& x4, const T5& x5, const T6& x6)
{
  gstring toto = format; toto.get_char(true);
    gstring os;
  
  int start=0;

  // before 1st %
  toto.display_till_percent(os, start);

  toto.to_stream(os, x0); toto.display_till_percent(os, start);
  toto.to_stream(os, x1); toto.display_till_percent(os, start);
  toto.to_stream(os, x2); toto.display_till_percent(os, start);
  toto.to_stream(os, x3); toto.display_till_percent(os, start);
  toto.to_stream(os, x4); toto.display_till_percent(os, start);
  toto.to_stream(os, x5); toto.display_till_percent(os, start);
  toto.to_stream(os, x6); toto.display_till_percent(os, start);
    
  // after last %
  toto.display_till_end(os, start);
}

template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7>
void gprintf(const char* format, const T0& x0, const T1& x1, const T2& x2, const T3& x3, const T4& x4, const T5& x5, const T6& x6, const T7& x7)
{
  gstring toto = format; toto.get_char(true);
    gstring os;
  
  int start=0;

  // before 1st %
  toto.display_till_percent(os, start);

  toto.to_stream(os, x0); toto.display_till_percent(os, start);
  toto.to_stream(os, x1); toto.display_till_percent(os, start);
  toto.to_stream(os, x2); toto.display_till_percent(os, start);
  toto.to_stream(os, x3); toto.display_till_percent(os, start);
  toto.to_stream(os, x4); toto.display_till_percent(os, start);
  toto.to_stream(os, x5); toto.display_till_percent(os, start);
  toto.to_stream(os, x6); toto.display_till_percent(os, start);
  toto.to_stream(os, x7); toto.display_till_percent(os, start);
    
  // after last %
  toto.display_till_end(os, start);
}

template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
void gprintf(const char* format, const T0& x0, const T1& x1, const T2& x2, const T3& x3, const T4& x4, const T5& x5, const T6& x6, const T7& x7, const T8& x8)
{
  gstring toto = format; toto.get_char(true);
    gstring os;
  
  int start=0;

  // before 1st %
  toto.display_till_percent(os, start);

  toto.to_stream(os, x0); toto.display_till_percent(os, start);
  toto.to_stream(os, x1); toto.display_till_percent(os, start);
  toto.to_stream(os, x2); toto.display_till_percent(os, start);
  toto.to_stream(os, x3); toto.display_till_percent(os, start);
  toto.to_stream(os, x4); toto.display_till_percent(os, start);
  toto.to_stream(os, x5); toto.display_till_percent(os, start);
  toto.to_stream(os, x6); toto.display_till_percent(os, start);
  toto.to_stream(os, x7); toto.display_till_percent(os, start);
  toto.to_stream(os, x8); toto.display_till_percent(os, start);
    
  // after last %
  toto.display_till_end(os, start);
}
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
void gprintf(const char* format, const T0& x0, const T1& x1, const T2& x2, const T3& x3, const T4& x4, const T5& x5, const T6& x6, const T7& x7, const T8& x8, const T9& x9)
{
  gstring toto = format; toto.get_char(true);
    gstring os;
  
  int start=0;
 
  // before 1st %
  toto.display_till_percent(os, start);

  toto.to_stream(os, x0); toto.display_till_percent(os, start);
  toto.to_stream(os, x1); toto.display_till_percent(os, start);
  toto.to_stream(os, x2); toto.display_till_percent(os, start);
  toto.to_stream(os, x3); toto.display_till_percent(os, start);
  toto.to_stream(os, x4); toto.display_till_percent(os, start);
  toto.to_stream(os, x5); toto.display_till_percent(os, start);
  toto.to_stream(os, x6); toto.display_till_percent(os, start);
  toto.to_stream(os, x7); toto.display_till_percent(os, start);
  toto.to_stream(os, x8); toto.display_till_percent(os, start);
  toto.to_stream(os, x9); toto.display_till_percent(os, start);
    
  // after last %
  toto.display_till_end(os, start);
}
#else
template <class T1, class T2, class T, class ...Args>
void recurse_prt(T1& os, T2& toto, int start, const T& first, const Args&... args)
{
	toto.to_stream(os, first); toto.display_till_percent(os, start);
    recurse_prt(os, toto, start, args...);
}
template<class T1, class T2>
void recurse_prt(T1& os, T2& toto, int start)
{
}
template < class ...Args>
void gprintf(const char* format, const Args&... args)
{
	 gstring toto = format; toto.get_char(true);
	    gstring os;

	  int start=0;

	  // before 1st %
	  toto.display_till_percent(os, start);
	  recurse_prt(os, toto, start, args...);

	  // after last %
	  toto.display_till_end(os, start);

}

static void gprintf(const char* format)
{
  int start=0;
    gstring os;
  gstring toto = format; toto.get_char(true);
  toto.display_till_percent(os, start);
  toto.display_till_end(os, start);
}

#endif
template <class T>
gstring to_hex(T val, int nbins = 0)
{

gstring x;
uint z = val;

if (nbins == 0)
  {
    while (z)
{
  z >>= 4;
  nbins++;
}
  }

z = val;

int nn = 1;
while ( (nn <= nbins) || (nn == 1) )
  {
    int u = (z >> 4*(nbins-1)) & 15;
    //gprintf("%R % - ", u);
    x.append_char(char(u + 48 + 7*(u>9)));
    z = z << 4;
    nn++;
  }
//gprintf("dec2hex % nbins % : % \n", val, nbins, x);
return x;
}

#endif
