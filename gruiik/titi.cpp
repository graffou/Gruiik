#include "mbprt.h"
#include <functional>


// geometry related classes--------------------------------------------------
class VERT;
class HOR;

class TOP;
class BOTTOM;
class LEFT;
class RIGHT;
template <class T> class DIM
;


template<class T>
bool is_vert() {return 0;}
template<>
bool is_vert<VERT>() {return 1;}
template<>
bool is_vert< DIM<TOP> >() {return 1;}
template<>
bool is_vert< DIM<BOTTOM> >() {return 1;}
template<>
bool is_vert< TOP >() {return 1;}
template<>
bool is_vert< BOTTOM >() {return 1;}

template<class T>
const bool is_left() {return 0;}
template<>
const bool is_left<LEFT>() {return 1;}

template<class T>
gstring pconv(T* x)
{
	return to_hex(int(reinterpret_cast<std::uintptr_t>(x)));
}
struct zA
{
	static zA* last;
	zA() {last = this;}
	virtual void f0(){gprintf("Org 0\n");}
	virtual void f1(){gprintf("Org 1\n");}
	
	//std::function< void(uint16_t)
	
};

struct zB : public zA
{
	static zB* last;
	zB (){last = this;} 

	void f0(){gprintf("New 0\n");}
	void f1(){gprintf("New 0\n");}
	//void f1(){gprintf("New 1\n");}
	//void f1(){}
	void run()
	{
		f0();
		f1();
	}
};

struct zC
{
	zB* p;
	zC(zB x)
	{
		p = &x;
	}
};
zA* zA::last;
zB* zB::last;
int main()
{
	//std::vector<zA*> v;
	zA AA;
	zB BB;
	zC CC(BB);
	AA.f0();
	BB.run();
	zA::last->f0();
	zB::last->f0();
	static_cast<zA*>(zB::last)->f0();
	static_cast<zB*>(zA::last)->f0();

	gprintf(" is vert % %\n", int(is_vert<VERT>()), int(is_vert<HOR>()) );
	gprintf(" is vert % %\n", int(is_vert< DIM<LEFT> >()), int(is_vert< DIM<TOP> >()) );
}
