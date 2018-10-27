
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

    void mode(uint8_t kb_mode)
    {
		for(int i = 0; i < N; i++)
		{
			//ine[i].caption.force_upper = (kb_mode == dUPPER_CASE);
			line[i].caption.fmode   = (kb_mode);
		}
    }
    // required because cannot be contructed directly from parent
    void set_parent(gruik_obj& x)
    {
		for(int i = 0; i < N; i++)
		{
			line[i].parent = static_cast<gruik_obj*>(&x);
			line[i].caption.pgk = &line[i];
			gprintf("Setting parent name of % : %\n", line[i].name, x.name);
			line[i].no_frame = 1;
			line[i].caption.replace_us = 0;
		}
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
		gprintf("#Ubutton line draw");
		for(int i = 0; i < N; i++)
			line[i].draw(x);
	}

	uint8_t event(xy pos, uint8_t x)
	{
		uint8_t e = 0;
		for (int i = 0; i < N; i++)
			if (line[i].coord_match(pos))
			{
				e = line[i].event(pos, x);
				if (x==PRESSED)
					gruik_obj::focus_obj = &line[i]; // PRESSED
				else
					gruik_obj::focus_obj = line[i].parent;//RELEASED give back control to keyboard
			}
		return e;
	}
};

template <class T1, class T, class ...Args>
void foo(T1& x, const T& first, const Args&... args)
{
    //gprintf("writing % in %\n", first, int(sizeof...(args)));
    //x.line[(x.N-1)-sizeof...(args)].name = "";//char(first[0]);
    //gprintf("name sz %\n",int(x.line[(x.N-1)-sizeof...(args)].name.size()));
    x.line[(x.N-1)-sizeof...(args)].name = first;//char(first[0]);
    x.line[(x.N-1)-sizeof...(args)].caption.no_special = 1; // ignore special chars (code < 32)
    //gprintf("name sz %\n",int(x.line[(x.N-1)-sizeof...(args)].name.size()));
    x.line[(x.N-1)-sizeof...(args)].caption.set_buffer(&x.line[(x.N-1)-sizeof...(args)].name);

    foo(x, args...);
}

template<class T1>
void foo(T1& x)
{


}

template < typename... ArgsT >
 auto multi(
                 ArgsT&... args)->kb_row<sizeof...(ArgsT)>
{
	//har lst[sizeof...(ArgsT)];
	kb_row<sizeof...(ArgsT)> x;
	foo(x, args...);

   return x;//apply_obj_func<sizeof...(ArgsT)>::applyTuple( t );
}


// return types are kb_rows. Cannot put those as members of gk_kb*: the compiler does not allow that, but that's ok for global variables (???!!)
static auto l0 = multi("1","2","3","4","5","6","7","8","9","0");
static auto l1 = multi("q","w","e","r","t","y","u","i","o","p");
static auto l2 = multi("a","s","d","f","g","h","j","k","l","*");
static auto l3 = multi("\x10_","z","x","c","v","b","n","m",".","\b<-");
static auto l4 = multi("\x11sym", "/", " ", ",","\nRet");
//
//static auto sym_l0 = multi("&","#","'","\",","{","}","(",")","[","]");
//static auto sym_l1 = multi("|","\\","=","+","-","$",",",";",":","%");
//static auto sym_l2 = multi("<",">","d","f","g","h","j","k","l","*");
//static auto sym_l3 = multi("\x10_","z","x","c","v","b","n","m",".","\b<-");
//static auto sym_l4 = multi("\x11sym", "/", " ", ",","\nRet");

struct gk_kb0 : gruik_obj
{


	uint8_t fcase = 0;
	bool exit_on_return = 0;

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
		l3.line[0].set_toggle(1);
		l4.line[0].set_toggle(1);
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
			gprintf(" Layout i % % %\n", i, x.line[i].org, x.line[i].dim);
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

	void draw(uint16_t x)
	{		gruik_obj::lcd->draw_rectangle(org, dim, 0xff808080, 1, 1);

		gruik_obj::lcd->draw_rectangle(org, dim, 0xff404040, 1, 0);
		//while(1){}
		layout();
		l0.mode(fcase);
		l1.mode(fcase);
		l2.mode(fcase);
		l3.mode(fcase);
		l4.mode(fcase);

		l0.draw(x);
		l1.draw(x);
		l2.draw(x);
		l3.draw(x);
		l4.draw(x);


	}
	uint8_t event(xy pos, uint8_t x)
	{
		gprintf("#Ukb event %", x);
		uint16_t maj = l3.line[0].toggle_value << 15;
		uint8_t e = 0;
		e |= l0.event(pos, x);
		e |= l1.event(pos, x);
		e |= l2.event(pos, x);
		e |= l3.event(pos, x);
		e |= l4.event(pos, x);

		e = convert_upper_case(e, fcase);

		gprintf("#R--->%",int(e));
		if (signal and (e != 0)) // actual key code can be sent to signal
			signal->psignal->set_event(e);

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

	}
};
