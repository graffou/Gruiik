GKMODULE(gk_led)
{
	text_box caption;
	uint32_t col = 0;
	//GKCTOR(gk_led){}
	gk_led(combo_t x) : gruik_obj(x), caption(this)
	{
		caption.set_buffer(&name);
	}

	void draw(uint16_t x)
	{
		gruik_obj::lcd->draw_rectangle(org, dim, 0xff808080, 1, 2);
		gruik_obj::lcd->draw_rectangle(org, dim, col, 1, 4);
		gruik_obj::lcd->draw_rectangle(org, dim, 0xffffffff, 0, 4);
		caption.draw_text();
	}
	uint8_t event(xy pos, uint8_t x){}
	void signal_event(uint8_t x)
	{
		gprintf("LED event data %\n", int(x));
		if (x&TOGGLE)
			col = 0xFFFF0000 * (x&1);
		else
			col = col += 0x00020304;
		draw(0);
	}
	//void layout()
};
