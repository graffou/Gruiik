
struct gk_menu_entry : gk_button
{
	gk_menu_entry* next_entry = NULL;
	std::function<void()> callback;
	gk_menu_entry(combo_t x) : gk_button(x)
	{
		callback = std::bind(&gk_menu_entry::def_callback, this);
		menu_button = 1;
		event_mask = 0;
	}
	void draw(uint16_t x)
	{} // is invisible
//		gk_button::draw(x);
//	}
	uint8_t event(xy pos, uint8_t x)
	{
		gprintf("menu entry test % \n", name);
		if (coord_match(pos))
			callback();
		return x;
	}
	void def_callback(){
		gprintf("#Tmenu entry event %", name);
	};

};

struct gk_menu : gk_menu_entry
{
	bool below = 1;
	gk_menu_entry* last_entry = this;
	xy sz = xy(0,0); // custom menu entry size

	gk_frame menu_frame = make_combo_t("", gruik_obj::gruik_top);
	gk_menu(combo_t x) : gk_menu_entry(x)
		{
			menu_frame.event_mask = 0;
			toggle_obj = 1; // menu is displayed on click, erased on click
			event_mask = 3; // this entry is always active (menu title)
		}

	void add_entry(gk_menu_entry& x)
	{
		last_entry->next_entry = &x;
		last_entry = &x;
	}

	void layout()
	{
		if (below)
		{
			menu_frame.locate_below(*this);
			menu_frame.extend<VERT>(gruik_obj::gruik_top); // allocate all available space below
		}

		gk_menu_entry* ptr = this;
		while(ptr->next_entry)
		{
			ptr->next_entry->clone_below(*ptr);
			ptr->next_entry->check_layout();
			ptr = ptr->next_entry;
		}
	}

	uint8_t event(xy pos, uint8_t x)
	{

		gprintf("#Vmenu event");
		if (x == PRESSED)
		{
			// draw all menu entries
			gk_menu_entry* ptr = next_entry; //
			while(ptr)
			{
				ptr->gk_button::draw(90);
				ptr = ptr->next_entry;
			}
			callback(); // if one is defined...

		}
		if (x == RELEASED)
		{
			gk_menu_entry* ptr = next_entry; //
			while(ptr)
			{
				ptr->event(pos, x);
				ptr = ptr->next_entry;
			}
			if ( (locking_obj == &gruik_top) or (locking_obj == parent) ) // Callback raised another object than top: don't redraw top
			{
				gruik_obj::clear();
				focus_obj = locking_obj; //
				gruik_obj::gruik_top.draw_recurse(0); // redraw all: erases menu
			}
			else gprintf("#TNot redrawing after menu close action because locking_obj = %", locking_obj->name);
		}
		return x;
	}

	void draw(uint16_t x)
	{
		gk_menu_entry::gk_button::draw(16);// menu must be visible and darker
	}

};
