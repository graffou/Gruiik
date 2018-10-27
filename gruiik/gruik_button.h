
struct gk_button : gruik_obj
{
	text_box caption;
	bool toggle = 0;
	bool toggle_value = 0;
	bool menu_button = 0;
	bool no_frame = 0;
	bool no_caption = 0;
	bool kb_touch = 0;
	uint32_t color = 0xff7f7f7f;
	gk_button()
	{
		caption.set_buffer(&name);
		caption.abort_draw_on_ovf = 0; // never mind if text is a bit out
		caption.replace_us = 1; // replace underscore w/ space
	}
	gk_button(combo_t x) : gruik_obj(x), caption(this)
	{
		caption.set_buffer(&name);
		caption.abort_draw_on_ovf = 0; // never mind if text is a bit out
		caption.replace_us = 1; // replace underscore w/ space
	}

	void set_toggle(uint8_t x, uint8_t val=0){toggle = x;toggle_value = val;}

	void draw(uint16_t x)
	{
		gprintf("#RButton draw name % org %r dim %r x == %", name, org, dim, int(x));
		uint32_t bgcol = color;
		//bgcol = (0x00808080 - x * 0x00010101);
		bgcol = (bgcol - (x==1) * 0x00303030);// and color_mask;
		caption.bg_color = bgcol;// and color_mask;
		gprintf("drawing button frame\n");
		if (not toggle)//pressed
			gruik_obj::lcd->draw_rectangle(org, dim, bgcol, 1, 2*(not menu_button));
		else
			gruik_obj::lcd->draw_rectangle(org, dim, bgcol - toggle_value * 0x00404040, 1, 2*(not menu_button));

		if (not no_frame)
		{
			if (not menu_button)
			{
				if (x != 1) // do not draw outer frame when clicked
				{
					gprintf("drawing border\n");

					gruik_obj::lcd->draw_rectangle(org, dim, 0xffFFFFFF, 0,4);
				}
			}
			else
				gruik_obj::lcd->draw_Hline(org.offset<VERT>(dim,-1), dim.x, 0xffFFFFFF);
		}
		if (not no_caption)
		{
			//caption.fg_color = (x != 1) * 0xffFFFFFF;
			caption.draw_text();
			gprintf("#Busr %", int(caption.replace_us));
		}
		gprintf("#RLeaving draw button !!");
	}

	uint8_t event(xy pos, uint8_t x)
	{
		gprintf("#RButton event");
		draw(x);// draw first in case emitted signal creates a new window(ex:

		if ((x == PRESSED) and toggle)
			toggle_value = !toggle_value;

		if (signal != NULL)
		{
			if (not toggle)
				signal->psignal->set_event(x);
			else
				signal->psignal->set_event(TOGGLE|toggle_value);
			return 0;
		}
		else
		{
			gprintf("ret133");
			return name[0];
		}
	}

	void layout()
	{
		gprintf("#UButton custom layout");
	}


};

struct gk_button_cb : gk_button
{
	std::function<void()> callback;
		gk_button_cb(combo_t x) : gk_button(x)
	{
		callback = std::bind(&gk_button_cb::def_callback, this);
		//event_mask = 0;
	}

	uint8_t event(xy pos, uint8_t x)
	{
		gprintf("menu entry test % \n", name);
		//if (coord_match(pos))
		gk_button::event(pos, x);
		if (x == 2) // release
			callback();
		return x;
	}
	void def_callback(){
		gprintf("#Tmenu entry event %", name);
	};
};
