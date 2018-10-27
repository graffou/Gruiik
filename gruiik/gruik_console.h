// is a char matrix
struct cline
{
	int16_t start = 0;
	//uint16_t prev_line =
};
typedef int16_t gkc_t;


template <int SZ>
struct circular_buffer
{
    char buffer[SZ];

    gkc_t cur_line = 0;
    gkc_t read_line = 0;
    gkc_t cur_read_char = 0;
    gkc_t first_line = 0;
    gkc_t cur_char = 0;
    gkc_t line_sz = 0;
    gkc_t actual_line_sz = 0;

    bool insert_mode = 1;
    bool in_esc_seq = 0;
    bool line_change = 0;

    // init line with an index that equals 0
    void init()
    {
    	cur_line = 0; cur_char = 0; line_sz = 0; actual_line_sz = 0;
    	//buffer[0] = 0;
    	write_idx(cur_line, -1);
    	write_idx(cur_line+sizeof(gkc_t), 0);

    }

    void write_idx(gkc_t addr, gkc_t idx)
    {
    	//gprintf("#Rwriting idx % at %", idx, addr);
    	gkc_t x = addr;
    	for (int i = 0; i < sizeof(gkc_t); i++)
    	{
    		write_char(x, char( uint8_t( (idx>>(i*8)) & 255) ) );
    		//gprintf("#TWriting % at %", to_hex(uint8_t( (idx>>(i*8)) & 255)), x);
    		x = wrap(x+1);
    	}
    }

    gkc_t read_idx(gkc_t addr)
    {
    	gkc_t x = addr;
    	gkc_t res = 0;
    	for (int i = 0; i < sizeof(gkc_t); i++)
    	{
    		//uint8_t r = buffer[x];
    		//gprintf("#TReading % at %", to_hex(buffer[x]), x);
    		res = res + (gkc_t(uint8_t(buffer[x])) << (i*8));// = char( uint8_t( (idx>>i) & 255) );
    		x = wrap(x+1);
    	}
    	return res;
    }
    circular_buffer(){init();}

	gkc_t wrap(gkc_t addr)
	{
		return addr >= SZ ? addr - SZ : addr;
	}


    void new_line()
    {
    	// write actual size
    	write_idx(wrap(cur_line+sizeof(gkc_t)), actual_line_sz);
    	// link lines

    	gkc_t new_cur_line = wrap(cur_line + line_sz+sizeof(gkc_t)*2);
    	//gprintf("#RNEW LiNE cur sz %M curline % wirintg %", line_sz, new_cur_line, cur_line);

    	write_idx(new_cur_line, cur_line);
    	cur_line = new_cur_line;
    	line_sz = 0;
    	actual_line_sz = 0;
    	cur_char = 0;
    	line_change = 1;
    }


    void write_char(gkc_t addr, char c)
    {
    	//gprintf("#Uwr % at % ", to_hex(c), addr);
    	buffer[addr] = c;
    }

    char read_char(gkc_t addr)
    {
    	//gprintf("// ADDR % rd % //", addr, buffer[addr]);
    	return buffer[addr];
    }

    gkc_t get_line_sz(uint8_t p = 0)
    {
    	if (read_line == cur_line) // not stored yet
    	{
    		if (0)
    		gprintf("XXX % % XXX", read_line, cur_line);
    		return actual_line_sz;
    	}
    	else // has been stored
    	{
    		if(0)
        		gprintf("### % % ###", read_line, cur_line);
    		return read_idx(wrap(read_line + sizeof(gkc_t)));
    	}

    }

    gkc_t init_line()
    {
    	read_line = cur_line; cur_read_char = 0;
    }

    gkc_t set_prev_line()
    {
    	cur_read_char = 0;
    	gkc_t end_of_cur_line = wrap(cur_line + line_sz + 2*sizeof(gkc_t));
    	gkc_t new_read_line = read_idx(read_line);
    	//gprintf("#MReading idx %R at %R", new_read_line, read_line);
    	gkc_t z = wrap(read_line - new_read_line + SZ);
    	gkc_t y = wrap(read_line - end_of_cur_line + SZ);
    	if (z >= y) // this line has been overwritten
    	{
    		read_line = -1;
    	}
    	else
    		read_line = new_read_line;

    	return read_line;

    }

    char get_char()
    {
    	if (cur_read_char < get_line_sz())
    		return read_char(wrap(read_line + 2*sizeof(gkc_t) + (cur_read_char++)));
    	else
    		return char(10);
    }

    void insert_char(char c) //inserts char at cur_char
    {
    	gkc_t line_idx = wrap(cur_line + 2*sizeof(gkc_t));
    	in_esc_seq |= (c == 0x1B);
    	//gprintf("#Uchar % %", to_hex(c), c);
    	//gprintf("&&%s %s&&", line_sz, line_idx+cur_char );
    	if ((line_sz + sizeof(gkc_t)*2 == SZ) and (c!= 10) and (c != 7))// buffer full accept LF and del
    		return;
		gkc_t idx = cur_char;

    	if ((c != 10) and (c!= 8))
    	{
    		if (insert_mode)
    		{
       			for (int i = line_sz-1; i >= cur_char; i--)
    			{
    				write_char(wrap(line_idx+i+1), read_char(wrap(line_idx+i)));
    			}
    		}

    		write_char(wrap(line_idx+cur_char), c);
    		cur_char++;
    		line_sz ++;
    		actual_line_sz += !in_esc_seq;
    	}
    	else if (c == 10)
    	{
    		write_char(wrap(line_idx+cur_char), c);
    		cur_char++;
    		line_sz++;
    		new_line();

    	}
    	else if (cur_char != 0) //DEL
    	{
    			for (int i = idx; i < line_sz; i++)
    			{
    				write_char(wrap(line_idx+i-1), read_char(wrap(line_idx+i)));
    			}
    			cur_char--;
    			line_sz--;
    			actual_line_sz--;
    			gprintf("#RDEL cur % % %", cur_char, line_sz, actual_line_sz);
    	}

    	in_esc_seq &= (c != 0x6D);
    }
};

GKMODULE(gk_console)
{
	circular_buffer<1024> buf;
    gkc_t cur_line_idx; // current idx in buffer
    gkc_t hview_off; // hor buffer view offset
    gkc_t vview_off; // vert buffer view offset
    uint32_t bg_col = 0xffffffff;
    uint32_t fg_col = 0xff000000;
    uint32_t txt_bg_col = 0xffffffff;
    uint32_t txt_fg_col = 0xff000000;

    uint8_t n_cols = 80;
    uint8_t n_rows = 40;
    gkc_t prev_last_line_rows = 0;

    gk_font* font = &default_mono_font;
    //gk_font* font = &default_font;
    xy start;
    xy cur_xy;
    xy last_xy;
    bool abort_next_draw = 0;

    void init()
    {
    	cur_line_idx = 0;
    	hview_off = 0;
    	vview_off = 0;
    }

	gk_console(combo_t x) : gruik_obj(x)
	{
		init();
	}



	void clear()
	{
		init();
	}

	// returns how many console lines are necessary to display line
	uint8_t line_rows(gkc_t line_idx)
	{

	}

	void layout()
	{
		n_cols = dim.x / font->width[0]; //assuming it is a monospaced font
		n_rows = dim.y / font->height;
		//gruik_obj::lcd->draw_rectangle(org, dim, bg_col, 0, 0);
		//exit(0);
	}

	gkc_t get_read_line_rows()
	{
		gkc_t line_rows = 0;
		gkc_t char_idx = 0;
		gkc_t line_sz = buf.get_line_sz(1);
		//gprintf("@c ** % % **", line_sz, n_cols);
		// this is more effective than division
		while (char_idx <= line_sz) // <= so that empty line has at least 1 row
		{
			char_idx += n_cols;
			line_rows++;
		}
		return line_rows;
	}

	void print_char(char c, bool do_draw = 0)
	{
		buf.insert_char(c);
		if (do_draw)
		{
			if (buf.line_change)
				draw(dALL_LINES);
			else
			{
				buf.init_line();
				// compute # of rows of last line: if unchanged, no need to redraw the whole buffer
				gkc_t last_line_rows = 0;
				gkc_t char_idx = 0;

				last_line_rows = get_read_line_rows();

				//gprintf("prev % cur %\n", prev_last_line_rows, last_line_rows);
				if (last_line_rows == prev_last_line_rows)
				{
					draw(dLAST_LINE);
					//cur_xy = last_xy;
					//draw_char(c);
					//last_xy = cur_xy;
				}
				else
				{
					//gprintf("#UALL(((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((");
					draw(dALL_LINES);
					//exit(à)
				}
				prev_last_line_rows = last_line_rows;
			}
		}
	}

	xy init_first_char_pos()
	{
		start = xy(0,0);// org;
		start = start + xy(0, dim.y);
		//gprintf("#T*****************************************1st char pos", start);
		start = start - xy(0, get_read_line_rows() * font->height);
		gprintf("#T*****************************************1st char pos % % % % %",
				start, get_read_line_rows(), int(n_cols), dim.x, int(font->width[0]));
		//exit(0);
		cur_xy = start;
	}
	xy compute_first_char_pos()
	{
		start = start - xy(0, get_read_line_rows() * font->height);
		if (start.y < 0) // next will be out of frame, abort anyway
			abort_next_draw = 1;
		cur_xy = start;
		//gprintf("#V****************************************Next char pos", start);
	}

	char conv_ascii(char c)
	{
		if (c < font->first_ascii)
			c = font->first_ascii;
		else if (c > font->last_ascii)
			c = font->last_ascii;
		c -= font->first_ascii;
		return c;
	}

	void draw_char(char c)
	{
		//if (cur_xy.y < 0) return;
		char c0 = c;
		c = conv_ascii(c);
		const uint8_t* char_addr = (&font->bitmap[font->map[c]]);
		uint8_t addr_step = ((font->width[0]-1)>>3) + 1; // uint8_t* steps to parse next char bitmap line
		xy true_xy;
		const uint8_t* addr;
		uint8_t bit_shift;
		//gprintf("c %", int(c));
		gprintf("|%s %R|", cur_xy, c0);
		for (int j = 0; j < font->height; j++)
		{
			for (int i = 0; i < font->width[0]; i++)
			{
				addr = char_addr + j * addr_step + (i >> 3);
				bit_shift = 7-(i&7);
				true_xy = cur_xy + xy(i,j)+ org;
				//gprintf("xy % %\n", true_xy, int(( ( (*addr) >> bit_shift) & 1) != 0));
				//gprintf("#Ucoo % % % xy %", org, dim, pconv(char_addr), true_xy);

				if (true_xy.y >= org.y)
				if ( ( ( (*addr) >> bit_shift) & 1) != 0)
				{
					gruik_obj::lcd->draw_pixel(true_xy, fg_col);
					//gprintf("f");
				}
				else
				{
					gruik_obj::lcd->draw_pixel(true_xy, bg_col&0xffffffff);
					//gprintf("0");
				}
			}
		}
		cur_xy = cur_xy + xy(font->width[0],0);
	}

	void signal_event(uint8_t event_i)
	{
		print_char(event_i, 1);// 2nd parm is do_draw
	}

	void draw(uint16_t x)
	{
		if ( invisible or parent->invisible ) // secure invisibility, draw is directly called by print and not by parent draw
		{
			gprintf("#V==================REDRAW OF CONSOLE % ABORTED because focus is on %, not % invisible %", name, focus_obj->name, parent->name, int(invisible));
			return;		//exit(0);
		}
		gprintf("Resume console draw name % invisible % % %\n", name, int(invisible), parent->name, parent->invisible);
		buf.init_line();
		init_first_char_pos();
		//gprintf("#Bgot first line %R", buf.read_line);
		abort_next_draw = 0;
		char c;
		if (not(x & dLAST_LINE))
		{
			if (x == 0) // default, will not rdraw when called by print
			{
				gruik_obj::lcd->draw_rectangle(org, dim, bg_col&0xffffffff, 1, 0);
				//for (int k = 0; k < 200; k++)
				gprintf("#UDRAW ALL RECT CONSoLE-----------------------------------% %", org, dim);
				//exit(0);
			}
			//exit(0);
		}

		bool bottom_line = 1;
		last_xy = cur_xy;
		while (1)//buf.get_line_sz() != 0)
		{
			if (1)//buf.get_line_sz() != 0)
			{
				//bool fill_line = 1;
				do
				{
					c = buf.get_char();
					gprintf("@m%c", to_hex(c));
					if (c != 10)
					draw_char(c);

					//gprintf("% : % !! %// \n", buf.cur_read_char, to_hex(c), c);
					if ( (cur_xy.x +font->width [0] ) > dim.x)
					{
						cur_xy = xy(0, cur_xy.y+font->height);
						//fill_line = 0; // would draw a rectaangle to the line below !
					}
					last_xy = cur_xy;
				}
				while (c != 10);

				//if ()
				xy rorg, rdim;
				uint32_t col = bg_col;
				//if (fill_line)
				if (cur_xy.y < dim.y)  // may happen after a line wrap (10l abo
				if ( (cur_xy.y>=0) )//	and (cur_xy.y < dim.y) ) // the second may happen after a line wrap (10l above)
				{
					rorg = xy(org + cur_xy); rdim = xy(dim.x - cur_xy.x, font->height);
				}
				else
				{
					rorg = org + xy(cur_xy.x, 0); rdim = xy(dim.x - cur_xy.x, font->height + cur_xy.y);
					//exit(0);
					//col = 0xFF0000;
				}
				//gruik_obj::lcd->draw_rectangle(org + cur_xy, xy(dim.x - cur_xy.x, font->height), col, 1, 0);
				gprintf("#BRect % %", rorg, rdim);
				gruik_obj::lcd->draw_rectangle(rorg, rdim, col & 0xffffffff, 1, 0);
			}
			else
			{
				gprintf("#VAbort draw on line sz %Y", "0");
			}


			if (x & dLAST_LINE) // no need to redraw everything
			{
				gprintf("#RONLY LAST LINE, ABORT");
				break;
			}
			else
			{
				gprintf("#UDRAW ALL");
			}

			bottom_line = 0;

			if (abort_next_draw)
				break;

			gkc_t nl = buf.set_prev_line();
			//gprintf("#Bgot new line %R", buf.read_line);
			if (nl < 0)
				break;
			gkc_t ls = buf.get_line_sz();

			//gprintf("#MNew line sz %", ls);
			compute_first_char_pos();
			//n++;
			//if (0)//n > 20)
			//	exit(0);
		}
		buf.line_change = 0;

	}

	void print_char_at(char c, uint8_t x, uint8_t y) // assume no more than 256x256 characters
	{

	}

	template<class T>
	void print_line(T& x)
	{
		int i = 0;
		if (1)
		while(x[i] != '\0' )
		{
			//gprintf("PL %\n", to_hex(x[i]));
			//buf.insert_char(x[i]);
			print_char(x[i], (x[i+1] == '\0'));
			i++;
		}
	}

	// this one prints the buffer to stdin and was intended for debug
	void print_buffer()
	{
		gprintf("#RZZZPB");
		//exit(0);
		buf.init_line();
		char c;
		gprintf("#MZZZFirst line sz % cur % read %", buf.get_line_sz(), buf.cur_line, buf.read_line);
		int n = 0;
		while (1)//buf.get_line_sz() != 0)
		{
			if (buf.get_line_sz() != 0)
			do
			{
				c = buf.get_char();
				//gprintf(c);
				//gprintf("% : % !! %// \n", buf.cur_read_char, to_hex(c), c);
			}
			while (c != 10);

			if (not buf.line_change) // no need to redraw everything
				break;

			gkc_t nl = buf.set_prev_line();
			gprintf("#BZZZgot new line %R", buf.read_line);
			if (nl == -1)
				break;
			gkc_t ls = buf.get_line_sz();
			gprintf("#MZZZNew line sz %", ls);
			n++;
			//if (0)//n > 20)
			//	exit(0);
		}
		buf.line_change = 0;
	}


};
