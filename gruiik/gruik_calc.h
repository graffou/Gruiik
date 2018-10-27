static auto cl0 = multi("%","(", ")", "\b<-");
static auto cl1 = multi("\1+/-","0", ".", "\n=");
static auto cl2 = multi("C", "R", "E", "-");
static auto cl3 = multi("7","8", "9", "*");
static auto cl4 = multi("4","5", "6", "/");
static auto cl5 = multi("1","2", "3", "+");


struct gk_calc : gruik_obj
{


	uint8_t fcase = 0;
	bool exit_on_return = 0;

	gk_calc(combo_t x) : gruik_obj(x)
	{
		cl0.set_parent(*this);
		cl1.set_parent(*this);
		cl2.set_parent(*this);
		cl3.set_parent(*this);
		cl4.set_parent(*this);
		cl5.set_parent(*this);
		cl2.line[0].color = 0xFFFF3030;

	}

	gk_calc(){}

	template<class T>
	void line_layout(T& x, int n)
	{
		uint16_t line_height = dim.y / 6;
		uint16_t btn_width = dim.x/4;
		uint16_t x_org = 0;
		for (int i = 0; i < x.N; i++)
		{
			x.line[i].dim = xy(btn_width*x.w[i], line_height);
			x.line[i].org = org + xy(x_org, n * line_height);
			gprintf(" Layout i % % %\n", i, x.line[i].org, x.line[i].dim);
			x_org += btn_width*x.w[i];
		}
	}
	void layout()
	{
		// layout is here a fixed matrix of 4x4 buttons
		line_layout(cl0, 0);
		line_layout(cl1, 5);
		line_layout(cl2, 1);
		line_layout(cl3, 2);
		line_layout(cl4, 3);
		line_layout(cl5, 4);
	}

	void draw(uint16_t x)
	{		gruik_obj::lcd->draw_rectangle(org, dim, 0xff808080, 1, 1);

		gruik_obj::lcd->draw_rectangle(org, dim, 0xff404040, 1, 0);
		//while(1){}
		layout();
		cl0.mode(fcase);
		cl1.mode(fcase);
		cl2.mode(fcase);
		cl3.mode(fcase);
		cl4.mode(fcase);
		cl5.mode(fcase);

		cl0.draw(x);
		cl1.draw(x);
		cl2.draw(x);
		cl3.draw(x);
		cl4.draw(x);
		cl5.draw(x);


	}
	uint8_t event(xy pos, uint8_t x)
	{
		gprintf("#Ukb event %", x);
		//uint16_t maj = l3.line[0].toggle_value << 15;
		uint8_t e = 0;
		e |= cl0.event(pos, x);
		e |= cl1.event(pos, x);
		e |= cl2.event(pos, x);
		e |= cl3.event(pos, x);
		e |= cl4.event(pos, x);
		e |= cl5.event(pos, x);

		e = convert_upper_case(e, fcase);

		gprintf("#R--->%",int(e));
		if (signal and (e != 0)) // actual key code can be sent to signal
			signal->psignal->set_event(e);
/*
		if (e == 16) // upper case
		{
			//fcase = fcase xor dUPPER_CASE;
			fcase = (l3.line[0].toggle_value) * dUPPER_CASE | (l4.line[0].toggle_value) * dSYM_CASE;
			draw(0);
		}

		if (e == 17)
		{
			//exit(0);
			//fcase = (l4.line[0].toggle_value) * dSYM_CASE;
			fcase = (l3.line[0].toggle_value) * dUPPER_CASE | (l4.line[0].toggle_value) * dSYM_CASE;
			draw(0);
			gprintf("yo % fcase tog l4.line[0] % \n\n", int(fcase), int(l4.line[0].toggle_value));//, l4.line[0].name);
			//exit(0);
		}

		if ((e == 10) and exit_on_return)// return
		{
			this->set_invisible();
			this->set_inactive();
			gruik_obj::refresh();
			gruik_obj::focus_obj = &gruik_top;
			gruik_obj::locking_obj = &gruik_top;
			active_layer = 0;
		}
*/
	}
};
