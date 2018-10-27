

#define WIDTH 480
#define HEIGHT 272 

#define MAGNIFY 2

#include "mbprt.h"
#include "gruik.h"
#include "gruik_signals.h"
#include "gruik_components.h"
#include "gruik_static.h"


template <int n>
class R
{
	constexpr int len(){return n;}
	//template<template<int> class T, int P>
	//R(T<P> x){}
};

template < uint N >
struct apply_obj_func
{
  template < typename... ArgsT, typename... Args >
  static R<N-1> applyTuple( /*T* pObj,
                          void (T::*f)( ArgsF... ),*/
                          const std::tuple<ArgsT...>& t,
                          Args... args )
  {
    apply_obj_func<N-1>::applyTuple( t, std::get<N-1>( t ), args... );//sizeof...(ArgsT)
    //std::cerr << N;
    //apply_obj_func<N-1>::applyTuple( pObj, f, t, std::get<sizeof...(ArgsT)-N>( t ), args... );//sizeof...(ArgsT)
    //std::cerr << sizeof...(ArgsT)-N;
    
  }
};

//-----------------------------------------------------------------------------

/**
 * Object Function Tuple Argument Unpacking End Point
 *
 * This recursive template unpacks the tuple parameters into
 * variadic template arguments until we reach the count of 0 where the function
 * is called with the correct parameters
 *
 * @ingroup g_util_tuple
 */
template <>
struct apply_obj_func<0>
{
  template < typename... ArgsT, typename... Args >
  static R<0> applyTuple( /*T* pObj,
                          void (T::*f)( ArgsF... ),*/
                          const std::tuple<ArgsT...>& /* t */,
                          Args... args )
  {
    
  }
};

template < typename... ArgsT >
auto applyTuple(
                 std::tuple<ArgsT...> const& t )->R<sizeof...(ArgsT)>
{
	R<sizeof...(ArgsT)> x;
   return x;//apply_obj_func<sizeof...(ArgsT)>::applyTuple( t );
}


//----------------

struct gk_buttonz
{
	//gstring name;
	gruik_obj* parent;
};

template<int n>
struct kb_row
{
	const int N = n;

    // As many buttons as declared in multi("a","b"...)
    gk_button line[n];
    
    uint8_t w[n];
    
    // default button width is 1 (can be changed later to be a mutiple of that to fill horizontal space)
    kb_row()
    {
		for(int i = 0; i < N; i++)
		{
			w[i] = 1; // default key width
		}
	}
	
    // required because cannot be contructed directly from parent
    void set_parent(gruik_obj& x)
    {
		for(int i = 0; i < N; i++)
			line[i].parent = static_cast<gruik_obj*>(&x);
	}
	
	
	// set button width as a multiple of base button width
	void set_width(int nkey, int p)
	{
		if (nkey < N) // for safety
			w[nkey] = p;
	}
	
	//draw all buttons
	void draw(uint16_t x)
	{
		for(int i = 0; i < N; i++)
			line[i].draw(x);
	}
	
};

template <class T1, class T, class ...Args>
void foo(T1& x, const T& first, const Args&... args)
{
    gprintf("writing % in %\n", first, int(sizeof...(args)));
    x.line[(x.N-1)-sizeof...(args)].name = first;//char(first[0]);
    foo(x, args...);
}

template<class T1>
void foo(T1& x)
{
	
	
}

template < typename... ArgsT >
 auto multi(
                 ArgsT... args)->kb_row<sizeof...(ArgsT)>
{
	//har lst[sizeof...(ArgsT)];
	kb_row<sizeof...(ArgsT)> x;
	foo(x, args...);
	
   return x;//apply_obj_func<sizeof...(ArgsT)>::applyTuple( t );
}

template <typename T, std::size_t N>
constexpr std::size_t arraySize(T (&) [N]) noexcept {
  return N;
}

template <typename T>
class A {
  public:
    const static char* names[];
};



template <int n>
kb_line<n> bobo(R<n> x)
{
	kb_line<n> a;
	return a;
}

// return types are kb_rows. Cannot put those as members of gk_kb*: the compiler does not allow that, but that's ok for global variables (???!!)
	static auto l0 = multi("1","2","3","4","5","6","7","8","9","0");
	static auto l1 = multi("q","w","e","r","t","y","u","i","o","p");
	static auto l2 = multi("a","s","d","f","g","h","j","k","l","*");
	static auto l3 = multi("_","z","x","c","v","b","n","m",".","<");
	static auto l4 = multi("sym", "/", " ", ",","Ret");

struct gk_kb0 : gruik_obj
{

	


	gk_kb0(combo_t x) : gruik_obj(x)
	{
		l0.set_parent(*this);
		l1.set_parent(*this);
		l2.set_parent(*this);
		l3.set_parent(*this);
		l4.set_parent(*this);
		l4.set_width(0, 2);
		l4.set_width(2, 3);
		l4.set_width(4, 3);

	}
	
	gk_kb0(){}
	
	template<class T>
	void line_layout(T& x, int n)
	{
		uint16_t line_height = dim.y / 5;
		uint16_t btn_width = dim.x/10;
		uint16_t x_org = 0;
		for (int i = 0; i < x.N; i++)
		{
			x.line[i].dim = xy(btn_width*x.w[i], line_height);
			x.line[i].org = org + xy(x_org, n * line_height);
			x_org += btn_width*x.w[i];
		}
	}
	void layout()
	{
		// layout is here a fixed matrix of 5x10 buttons
		line_layout(l0, 0);
		line_layout(l1, 1);
		line_layout(l2, 2);
		line_layout(l3, 3);
		line_layout(l4, 4);
		
	}
	
};

template<typename T>
const char* A<T>::names[] = {"foo", "bar"};

template<>
const char* A<bool>::names[] = {"foo","bar", "foobar"};

int main()
{
	gprintf("YOOOOOOOOOOOOOOOOOOO\n");
	R<3> x;
	auto a=bobo(x);
	kb_line<2> b;
	
	auto c = applyTuple( std::make_tuple(1,2,3) );
	auto d = multi("tab", "q", "w", "e","r","t","y","u","i","o","p");
	gprintf("size(d) = %\n", d.N);
	for (int i=0;i<d.N;i++)
		gprintf("line % %\n",i, d.line[i].name);
	//kb_line
	printf("yo\n");
	gstring yu;
	yu = "oh!";
	gprintf("yu % \n", yu);
}
