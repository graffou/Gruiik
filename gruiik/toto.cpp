#include <iostream>

#include "mbprt.h"
#define WIDTH 480
#define HEIGHT 272

#include "gruik.h"

#define STRINGIFY(a) #a

//§§gruik_obj* gruik_obj::locking_obj = NULL;
//std::vector<gruik_obj> gruik_objs;
gruik_obj gruik_obj::gruik_top;
gruik_obj* gruik_obj::focus_obj = NULL;
gruik_obj* gruik_obj::locking_obj = NULL;
uint8_t gruik_obj::active_layer = 0;


struct gruik_gui:public gruik_obj
{
	GRUIK(tab0);	
};

GKMODULE(tutu)
{
	//MODULE_INST(baba, fuck);
	GKCOMP(tataz, gk_area);

	GKCTOR(tutu)
	{
	}
	
};
struct single_data
{
	uint32_t data = 0;
	uint8_t get_data(uint8_t mode, uint32_t* data_o)
	{
		*data_o = data;
		return 1;
	}
	uint8_t set_data(uint8_t mode, uint32_t* data_i)
	{
		data = *data_i;
		return 1;
	}
};
signal_t<single_data> signal0 = "signal0";
signal_t<single_data> signal1 = "signal1";
signal_t<single_data> signal2 = "signal2";
signal_t<single_data> signal3 = "signal3";


GKMODULE(fuck)
{
	
};

GKMODULE(tuty)
{
	GKCOMP(tata, gk_area);
	GKCOMP(titi, gk_frame);
	GKCOMP(layer1, gk_frame);
	MODULE_INST(bobo, tutu);
	
#if 1
	signal_link link0 = make_combo2_t(signal0, titi); 	 
	signal_link link1 = make_combo2_t(signal0, tata);
	signal_link link40 = make_combo2_t(signal0, bobo);
	signal_link link41 = make_combo2_t(signal1, bobo);
	signal_link link42 = make_combo2_t(signal2, bobo);
	signal_link link2 = make_combo2_t(signal1, titi);
	signal_link link3 = make_combo2_t(signal0, titi);
	signal_link link5 = make_combo2_t(signal2, titi);
	
#else
	signal_link link0 = make_combo2_t(signal0, titi); 	 
	signal_link link1 = make_combo2_t(signal0, tata);
	signal_link link2 = make_combo2_t(signal0, bobo);
	signal_link link3 = make_combo2_t(signal1, tata);
	
	//signal_link link4 = make_combo2_t(signal0, titi);
	//signal_link link5 = make_combo2_t(signal2, titi);
#endif
	GKCTOR(tuty)
	{
		MSG(1)
		tata.info();
		titi.info();
		bobo.info();
		
		MSG(2)		
		split_V(20, bobo, tata);
		tata.info();
		titi.info();
		bobo.info();	
		
		MSG(3)
		tata.split_H(128, tata, titi);
		//gprintf("m %R\n", (tata.x0));

		//gprintf("#Mtata %R %R %R %R %G", tata.x0, tata.x1, tata.y0, tata.y1);
	
		tata.info();
		titi.info();
		bobo.info();
		
		layer1.set_layer(1);

		//event_callback = std::bind(&tuty::input_event, this, std::placeholders::_1, std::placeholders::_2);

		bobo.event_callback = std::bind(&tuty::bobo_input_event, this, std::placeholders::_1, std::placeholders::_2);
		bobo.signal_callback = std::bind(&tuty::bobo_signal_event, this, std::placeholders::_1);
		//bobo.signal_callback = std::bind(&gruik_obj::signal_default, this, std::placeholders::_1);
	

		gruik_top.info();

	}
	
    void bobo_signal_event(uint8_t event)
    {
		uint32_t v[1];
		bobo.signal->psignal->get_data(SINGLE, v);
		gprintf("#VBOBO NEW SIG CALLBACK read data %Y", int(v[0]));
		
	}   
	uint8_t bobo_input_event(xy pos, uint8_t event)
    {
		gprintf("#VBOBO NEW EVT CALLBACK read data %Y", pos);
		
	}
	uint8_t input_event(xy pos, uint8_t event)
    {
		gprintf("#VPOPO NEW EVT CALLBACK read data %Y", pos);
		
	}	
};

//TOP_MODULE(z, tuty);
struct zz
{
	uint8_t a,b,c,d;
	void set(xy x, xy y)
	{
		a = x.x;b=x.y;c=y.x;d=y.y;
	}
};

	tuty popo = make_combo_t("popo", gruik_obj::gruik_top);


#if 0
int main()
{
	tuty popo = make_combo_t("popo", gruik_obj::gruik_top);
	
	//std::function<uint8_t(xy)> f;
#if 0
	std::function<uint8_t(xy, uint8_t)> f;
	f = std::bind(&gruik_obj::event_default, &popo, std::placeholders::_1, std::placeholders::_2);
	xy z(0,4);
	f(z,PRESSED);
	z = {100,100};
	gprintf("#T***************************************************");
	f(z,PRESSED);
	gprintf("#T***************************************************");

	f(z,RELEASED);
#endif	
	gprintf("#T***************************************************");

	gruik_obj::gruik_top.info();
	gruik_obj::input_process(100,100,0); gprintf("popo!!!!!!!!!!!\n\n\n\n\n\n");
	gruik_obj::gruik_top.input_process(100,100,0); gprintf("TOP!!!!!!!!!\n\n\n\n\n\n");
		gruik_obj::gruik_top.info();

	gruik_obj::input_process(100,100,1);gprintf("2\n\n\n\n\n\n");
		gruik_obj::gruik_top.info();

	gruik_obj::input_process(100,100,0);gprintf("3\n\n\n\n\n\n");
		gruik_obj::gruik_top.info();

	gruik_obj::input_process(10,10,1);gprintf("4\n\n\n\n\n\n");
	gruik_obj::input_process(100,100,1);gprintf("5\n\n\n\n\n\n");
	
	//popo.bobo.set_flags(NO_EVENT);
	gruik_obj::input_process(10,10,1);gprintf("4\n\n\n\n\n\n");
	gruik_obj::input_process(10,10,0);gprintf("4\n\n\n\n\n\n");

	gruik_obj::set_active_layer(1);
	popo.layer1.info();
	gruik_obj::input_process(10,10,1);gprintf("4\n\n\n\n\n\n");

	gruik_obj::gruik_top.info();
	
	zz x;
	x.set(xy(0,1),xy(2,3));
	gprintf("x % % % %_n", x.a,x.b,x.c,x.d);
	//gprintf(popo);
	//popo.link0.activate();
	popo.link40.activate();
	popo.link41.activate();
	popo.link42.activate();
	popo.link41.activate();
	popo.link40.activate();
	popo.link42.activate();
	popo.link40.activate();
	popo.link40.activate();
#if 0
	popo.link41.activate();
	popo.link42.activate();
	popo.link40.activate();
	#endif
	uint32_t v[1];
	v[0] = 111;
	signal0.set_data(SINGLE, v);
	signal0.set_event(32); gprintf("\n****\n");
	
	
	signal1.set_event(32);gprintf("\n****\n");
	signal2.set_event(32);gprintf("\n****\n");
	signal3.set_event(32);gprintf("\n****\n");
	
	popo.draw_process(0);
}
#endif
