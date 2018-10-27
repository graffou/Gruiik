
GKMODULE(gk_msg)
{
	text_box msg;
	GKCTOR(gk_msg), msg(this)
		{
			set_invisible();
			set_inactive();
			msg.Vcentered = 0;
			//set_toggle();
		}

	void layout()
	{
		//shrink<HOR>(220);
		//shrink<VERT>(220);
		msg.home();
		msg.clone(*this);
		//msg.remap<BOTTOM>(64);
		//msg.shrink<HOR>(200);
	}

	void draw(uint16_t x)
	{
		layout();
		gruik_obj::lcd->draw_rectangle(org, dim, msg.bg_color, 1, 1);
		gruik_obj::lcd->draw_rectangle(org, dim, 0xffFF0000, 0, 0);
		gprintf("#Ttext box % % div %", msg.org, msg.dim, int(msg.font_div));
		msg.draw_text();
	}

	uint8_t event(xy pos, uint8_t x)
	{
		gprintf("#Vmsg event");

		set_invisible();
		set_inactive();
		gruik_obj::focus_obj = &gruik_obj::gruik_top;
		gruik_obj::locking_obj = &gruik_obj::gruik_top;
		gruik_obj::refresh();
		return x;
	}

	void signal_event(uint8_t x)
	{
		gprintf("#Vmsg signal event");
		if (x == 2)
		{
			gruik_obj::locking_obj = this;
			draw(0);
		}
		set_active();
		set_visible();

	}
	template<class T>
	void set_text(const T* x)
	{
		int len = 0;
		while (x[len] != 0) len++;
		msg.set_buffer(x, len);
	}

	// to be called by a menu
	void callback()
	{
		signal_event(0);
	}

};
