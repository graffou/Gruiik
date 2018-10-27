

template<class T>
GKMODULE(gk_slider)
{
	text_box caption;
	uint32_t col = 0;
	uint8_t val = 0;
	//bool invisible = 0;
	//bool no_caption = 1;



	gk_slider(combo_t x) : gruik_obj(x), caption(this)
	{
		//caption.set_buffer(&name);
		event_mask |= MOVED;
	}

	void draw(uint16_t x)
	{
		if (x != MOVED)
		gruik_obj::lcd->draw_rectangle(org, dim, 0xff808080, 1, 2);
		int16_t a = ( (dim.get<T>() * (is_vert<T>() ? val : 255 - val) ) ) >> 8;
		int16_t b = dim.get<T>() - a;

		uint32_t col1, col2;
		col1 = is_vert<T>() ? 0 : 0xffFF0000;
		col2 = not is_vert<T>() ? 0 : 0xffFF0000;
		if (1)//not invisible)
		{
			gruik_obj::lcd->draw_rectangle(org, dim.offset<T>(-a), col1, 1, 4);
			gruik_obj::lcd->draw_rectangle(org.offset<T>(b), dim.set_dim<T>(a), col2, 1, 4);
			gruik_obj::lcd->draw_rectangle(org, dim, 0xffffffff, 0, 4);
		}
		gprintf("Draw % val %\n", name, val);
		//else
			//caption.draw_text();
	}

	uint8_t event(xy pos, uint8_t x)
	{
		if (coord_match(pos))
		{
			if (is_vert<T>())
				val = (org.get<T>() + dim.get<T>() - pos.get<T>()) * 255 / dim.get<T>();
			else
				val = (pos.get<T>() - org.get<T>()) * 255 / dim.get<T>();

			//val = (org.y + dim.y - pos.y) * 255 / dim.y;
		gprintf("#cslider % %m xy % box % dim %", name,int(val), pos, org, dim);
		if (not invisible)
			draw(x);
		if (signal)
			signal->psignal->set_event(val);

		}
		return x;
	}

	//
	void signal_event(uint8_t x)
	{
		gprintf("slider event data %\n", int(x));
		//uint32_t v;
		//signal->psignal->get_data(&v);
		val = x;
		if (not invisible)
			draw(0);
	}
	//void layout()
};
