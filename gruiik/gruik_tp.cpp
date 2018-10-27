#include <iostream>
#include "mbprt.h"
GKMODULE(top_bar_t)
{
	MODULE_INST(powered_by_GruiiK, gk_button);
	MODULE_INST(menu0, gk_menu);
	MODULE_INST(menu1, gk_menu);
	MODULE_INST(go_term, gk_menu_entry);
	MODULE_INST(go_curve, gk_menu_entry);
	MODULE_INST(go_fuck_yourself, gk_menu_entry);
	MODULE_INST(quit, gk_menu_entry);
	MODULE_INST(keyboard, gk_menu_entry);
	MODULE_INST(calc, gk_menu_entry);
	MODULE_INST(entry21, gk_menu_entry);
	MODULE_INST(entry31, gk_menu_entry);	
	GKCTOR(top_bar_t) 
	{
		powered_by_GruiiK.menu_button = 1; // no frame
		powered_by_GruiiK.caption.set_font_div(1);
		menu0.add_entry(go_term);
		menu0.add_entry(go_curve);
		menu0.add_entry(go_fuck_yourself);
		menu0.add_entry(quit);	
		menu1.add_entry(keyboard);
		menu1.add_entry(calc);
		menu1.add_entry(entry21);
		menu1.add_entry(entry31);
	}
	
	void layout()
	{
		split<HOR>(160, menu0, powered_by_GruiiK);
		menu0.split<HOR>(128, menu0, menu1);
	}
};
/*
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
		term2.caption.font_div = 2;
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


GKMODULE(calc_app_t)
{
	MODULE_INST(EXIT_, gk_menu);
	MODULE_INST(kb0, gk_calc);
	MODULE_INST(term2, gk_console);//text_box);
	SIGNAL(signal0, single_data);
	LINK(signal0, kb0);
	LINK(signal0, term2);
	//FreePixel_32 big_font;
	VeraMoBd_32 big_font;
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
		map<RIGHT>(128);
		split<VERT>(15, EXIT_, term2);
	
		term2.split<VERT>(100, term2, kb0);
		SET_CALLBACK(EXIT_, calc_app_t, quit);
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
*/
//signal_t<single_data> signal0 = "signal0";
SIGNAL(signal0, single_data);
SIGNAL(signal1, single_data);
SIGNAL(signal3, single_data);
SIGNAL(signal2, vector_data<16384>);
SIGNAL(box_signal, single_data);

GKMODULE(top_if_t)
{
	GKCTOR(top_if_t) 
	{
		term.set_invisible();
		term.set_inactive();
		kb_app.set_inactive();
		kb_app.set_invisible();
		calc_app.set_inactive();
		calc_app.set_invisible();
		kb1.set_inactive();
		kb1.set_invisible();
		popup.set_text("\n\nGRaffic User Interface \nIntuitive Kit \n\nGraffou 2018");
		popup.msg.set_font_div(0);
	}

	// frames and modules
	// first components that are supposed to be on top of others
	MODULE_INST(kb_app, kb_app_t);
	MODULE_INST(calc_app, calc_app_t);
	MODULE_INST(kb1, gk_kb0);
	
	//MODULE_INST(term, gk_text_box);
	MODULE_INST(term, gk_console);
	MODULE_INST(curve, gk_curve);
	MODULE_INST(titi, gk_frame);
	MODULE_INST(L0, gk_led);
	MODULE_INST(btn1, gk_button);
	MODULE_INST(btn2, gk_button);
	MODULE_INST(btn3, gk_button);
	MODULE_INST(btn4, gk_button);
	MODULE_INST(mini_btn, gk_button);
	MODULE_INST(led0, gk_led);
	MODULE_INST(sld0, gk_slider<VERT>);
	MODULE_INST(sld1, gk_slider<HOR>);
	MODULE_INST(top_bar, top_bar_t);
	MODULE_INST(dummy, gk_frame);
	MODULE_INST(popup, gk_msg);
	LINK(box_signal, popup);
	LINK_REMOTE(box_signal, top_bar, powered_by_GruiiK);

	
	// bind modules to signals	
	LINK(signal0, btn3);	 
	LINK(signal0, led0);
	LINK(signal1, sld0);	 
	LINK(signal1, sld1);	 
	LINK(signal1, L0);	 
	LINK(signal3, btn2);	 
	LINK(signal2, curve);	 
	
	// Custom layout + print a fresh text
	void layout()
	{
		sld1.invisible = 1;
		
		kb1.map<BOTTOM>(128);
		kb1.layer = 1;
		kb1.exit_on_return = 1;
		//kb1.shrink<HOR>(192);
		
		split<VERT>(25, top_bar, term);	
		//split<VERT>(220, tata, sld1);	
		term.split<VERT>(230, term, sld1);			
		term.split<HOR>(192, term, titi);
		
		gprintf("#VTerm coords % %", term.org, term.dim);
		gprintf("#Vbar coords % %", top_bar.org, top_bar.dim);
		//exit(0);
		//term.shrink<HOR>(240);
		//term.shrink<VERT>(240);
		
		
		curve.clone(term);
		titi.split<HOR>(230, titi, sld0);
		titi.split<VERT>(128, L0, btn2);
		L0.split<VERT>(48, mini_btn, L0);
		L0.split<VERT>(128, L0, btn1);
		btn2.split<VERT>(128, dummy, btn2);
		btn2.split<VERT>(128, btn2, btn3);
		btn2.split<HOR>(128, btn2, btn4);
		btn3.split<HOR>(128, btn3, led0);
		btn3.set_toggle(1);	
		btn1.set_toggle(1);	

		SET_CALLBACK(top_bar.quit, top_if_t, quit);
		SET_CALLBACK(top_bar.go_term, top_if_t, go_term);
		SET_CALLBACK(top_bar.go_curve, top_if_t, go_curve);
		SET_CALLBACK(top_bar.go_fuck_yourself, top_if_t, go_fuck_yourself);
		SET_CALLBACK(top_bar.keyboard, top_if_t, keyboard);
		SET_CALLBACK(top_bar.calc, top_if_t, calc);
		//erm.caption.force_upper = 1;
		//term.set_font_div(1);
		term.print_line("Allez tous vous faire enculer ! Bande de gros bataaaaaaaaaards \n");
		term.print_line("Bande de gros batards !\n");
		calc_app.term2.print_line("GRUIK CALC v0.0\n");
	}

	void quit()
	{
		gprintf("Adios\n");
		exit(0);
	}
	
	 void go_curve()
	{
		gprintf("curve view\n");
		term.set_inactive();
		term.set_invisible();
		curve.set_active();
		curve.set_visible();		
	}
	
    void go_term()
	{
		gprintf("text view\n");
		//kb_app.term2.copy_buffer(term);
		curve.set_inactive();
		curve.set_invisible();
		//term.draw();
		term.set_active();
		term.set_visible();	
		term.draw(0);	
	}   
	
	void go_fuck_yourself()
	{
		gprintf("text view\n");
		curve.set_inactive();
		curve.set_invisible();
		term.set_inactive();
		term.set_invisible();	
		gruik_obj::refresh();	
	}
	
	void keyboard2()
	{
		kb1.set_active();
		kb1.set_visible();
		active_layer = 1;
		
	}
	void keyboard()
	{
		static bool k = 0;
		
		if (1)//k==0)
		{
			gruik_obj::focus_obj = &kb_app;
			gruik_obj::locking_obj = &kb_app;
			kb_app.set_active();
			kb_app.set_visible();
			kb_app.draw(0);
		}
		else
		{
			kb_app.set_inactive();
			kb_app.set_invisible();
			gruik_obj::refresh();
		}
		k = !k;		
	}
	
	void calc()
	{
		gruik_obj::focus_obj = &calc_app;
		gruik_obj::locking_obj = &calc_app;
		calc_app.set_active();
		calc_app.set_visible();
		calc_app.draw(0);
	}
		
};



GKMODULE(tuto)
{
	//GKCOMP(tata, gk_area);
	MODULE_INST(zob, top_bar_t);
	MODULE_INST(s0, gk_frame);
	MODULE_INST(s1, gk_frame);
	

	//GKCOMP(layer1, gk_frame);
	//MODULE_INST(bobo, tutu);
	
	GKCTOR(tuto) {}
	
	void layout()
	{
		s0.split_H(100, s0, s1);
		s0.info();
		
		s0.split_V(100, s0, zob);
		s0.info();
		
	}
	
};
	

//MODULE_TOP(popo, tuto);
MODULE_TOP(gktop, top_if_t);

	void quit()
	{
		gprintf("Callback 0\n");
		exit(0);
	}

#if 0
int main()
{
	tuty popo = make_combo_t("popo", gruik_obj::gruik_top);
	
	//std::function<uint8_t(xy)> f;
	std::function<uint8_t(xy, uint8_t)> f;
	f = std::bind(&gruik_obj::event_default, &popo, std::placeholders::_1, std::placeholders::_2);

#if 0
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
