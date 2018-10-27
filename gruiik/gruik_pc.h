uint32_t rgb_interp(uint32_t fg, uint32_t bg, uint8_t c, uint8_t div)
{
	uint8_t nc = (1<<div) - c;
	//fg = fg & 0xfcfcfc;
	//bg = bg & 0xfcfcfc;
	//gprintf("c % fg % bg % fg*c % bg*nc % res\n", int(c), to_hex(fg),to_hex(bg),to_hex(fg * c), to_hex(bg * nc),to_hex((fg * c + bg * nc) >> div));
	return (0xFF000000 | ((fg * c + bg * nc) >> div));
}


struct lcd_drv
{

#ifndef DISCO

	// this is for SDL2-based PC simulator
	uint32_t* fb;


	//lcd_drv(){}
	lcd_drv(uint32_t* tb_tft) { fb = tb_tft; }

	void draw_pixel(xy x, uint32_t col)
	{
		//gprintf("#Rxy %", x);
		//if (1)//((x.y>=0) and (x.y*WIDTH+x.x < WIDTH*HEIGHT) )
		if (((x.x) >= 0) and (x.x < WIDTH) and (x.y >= 0) and (x.y < HEIGHT))
		//if ((x.y>=0) and (x.y*WIDTH+x.x < WIDTH*HEIGHT) )
			{
			//gprintf(".%..", x)
			//gprintf("#Bxy %", x);
			//fb[x.y*WIDTH+x.x] = col;
#if 1
			uint32_t base = x.y*WIDTH*MAGNIFY*MAGNIFY + x.x*MAGNIFY;
			for (int i=0; i<MAGNIFY; i++)
			{
				for (int j=0;j<MAGNIFY; j++)
				{
					//fb[base + i*MAGNIFY*WIDTH + j] = col * ((i!=(MAGNIFY-1))&&(j!=(MAGNIFY-1)));
					fb[base + i*MAGNIFY*WIDTH + j] = col ;//* ((i!=(MAGNIFY-1))&&(j!=(MAGNIFY-1)));
				}
			}
#endif
		}
	}
#else
	 //TS_DISCO_F746NG ts;
    LCD_DISCO_F746NG* lcd;
    lcd_drv(){}
	 
	 void set_lcd( LCD_DISCO_F746NG* lcd_i)
	 {
	    lcd = lcd_i;
	 }
	 
	void draw_pixel(xy x, uint32_t col)
	{
		if (((x.x) >= 0) and (x.x < WIDTH) and (x.y >= 0) and (x.y < HEIGHT))
#ifndef VERTICAL_LAYOUT
		lcd->DrawPixel(x.x,x.y,col);

#else
		lcd->DrawPixel(x.y,WIDTH-1-x.x,col);
#endif
		//wait(.00001);
	}
#endif


	void draw_Hline(xy x, int16_t len, uint32_t col)
	{
#if 1
		if ((len > 0) and (x.x >= 0))
		for (int i = x.x; i < x.x+MIN(len, WIDTH); i++)
		{
			draw_pixel(xy(i,x.y), col); //use failsafe method
			//if (x.y*WIDTH+i < WIDTH*HEIGHT)

			//fb[x.y*WIDTH+i] = col;
			//gprintf("x % ", x.y*WIDTH+i);
		}
		//else gprintf("#RLine rejected");
#endif
	}

	void draw_Vline(xy x, uint16_t len, uint32_t col)
	{
		//gprintf("x % len ", x, int(len));
		if (len > 0)
		for (int i = x.y; i < x.y+len; i++)
		{
			draw_pixel(xy(x.x,i),col);
			//		fb[x.x + WIDTH*i] = col;
		}
	}
	void draw_rectangle(xy x, xy sz, uint32_t col, uint8_t fill = 1, uint8_t crop = 0)
	{
		//gprintf ("lims % % (%, %)\n", crop+x.y,x.y + sz.y - crop, x, sz);
		//gprintf("org % len %\n", x+xy(crop,crop), sz.x-2*crop);

		if (fill)
		{
			//gprintf ("lims % %\n", crop+x.y,x.y + sz.y - 2*crop);
			for (int i = crop+x.y; i < x.y + sz.y - crop; i++)
			{
				int16_t  len = sz.x-2*crop;

				if (len > 0)
				{
					draw_Hline(xy(x.x+crop,i), sz.x-2*crop, col);
				}

			}

		}
		else
		{
			gprintf("org % len %\n", x+xy(crop,crop), sz.x-2*crop);
			draw_Hline(x+xy(crop,crop), sz.x-2*crop, col); gprintf("*");
			draw_Hline(x+xy(crop,sz.y-crop-1), sz.x-2*crop, col);gprintf("*");
			draw_Vline(x+xy(crop,crop), sz.y-2*crop, col);gprintf("*");
			draw_Vline(x+xy(sz.x-crop-1,crop), sz.y-2*crop, col);gprintf("*");
		}


	}

	// To draw bitmap fonts, not bitmap image
	// box provides relative coordinates and box origin / dimensions
	template <class T>
	uint8_t draw_bitmap(const uint8_t* map, xy& rel, xy sz, const T& box)
	{
		//gprintf("#UDraw bitmap fg % bg % div %", to_hex(box.fg_color), to_hex(box.bg_color), int(box.font_div));
		uint8_t status = 0;
		//uint16_t i0, i1, j0, j1;
		int i0, i1, j0, j1;
		uint8_t addr_step = ((sz.x-1)>>3) + 1; // uint8_t* steps to parse next char bitmap line
		const uint8_t* char_addr;
		uint8_t bit_shift;
		uint32_t col;
		xy true_xy;

		//gprintf(" %   %  \n", rel, sz);
		i0 = j0 = 0;
		//j0 = ((1 << box.font_div)>>2);
		if (box.font_div == 2) // vertical sampling tricks to obtain acceptable results with 1:4 vertical sampling
			j0 = (box.cur_char > 47) + (box.cur_char >96);
		//else j0 = ((1 << box.font_div)>>2) + (box.cur_char <96);
		//else j0 = (box.cur_char <96) ? 12 : 0;
		i1 = sz.x; j1 = sz.y;
		if (rel.x <0) {i0 = -rel.x; status |= LEFT_OVF;}
		if (rel.y <0) {j0 = -rel.y; status |= TOP_OVF;}
		if (rel.x + (sz.x>>box.font_div) > box.dim.x) {i1 = (box.dim.x - rel.x) << box.font_div; status |= RIGHT_OVF;}
		if (rel.y + (sz.y>>box.font_div) > box.dim.y) {j1 = (box.dim.y - rel.y) << box.font_div; status |= BOTTOM_OVF;}

		//gprintf(" %   box %   % % %\n", rel, box.org, box.dim, int(box.fg_color));
		//gprintf(" %  %  %  %\n", i0,i1,j0,j1);
		//uint8 xR = (boxf.g_color-box.bg_color)
#ifdef CHARDBG
		gprintf("#Corg %R map %R sz %R %R  %R  %R  %R rel %R\n", box.org, pconv(map), sz, i0,i1,j0,j1,rel);
#endif
		if (( (status & RIGHT_OVF) == 0) or (box.abort_draw_on_ovf == 0))
		for (int j = j0; j < j1; j+=(1<<box.font_div))
		{
			int cnt = 1;
			int cnt2 = 0;

			for ( int i = i0; i < i1; i++)
				{
					//char_addr = map + j * addr_step + (i >> 3);
				//gprintf("access %  ptr %\n,",j * addr_step + (i >> 3), pconv(map));
					char_addr = map + j * addr_step + (i >> 3);
					bit_shift = 7-(i&7);
					cnt2 += (((*char_addr) >> bit_shift) & 1) != 0;
					//gprintf("/// %  % fd % %  ///", cnt, cnt2, int(box.font_div), cnt >> box.font_div);
					if ((cnt >> box.font_div) or (i == (i1-1)) )
					{
						cnt = 0;
						//gprintf(" <i : % / % %>", i, int(i>>3), int(bit_shift));
						if (cnt2)
						{
#ifdef CHARDBG
							gprintf(cnt2);
#endif
							true_xy = xy((i>>box.font_div), (j>>box.font_div)) + rel + box.org; // apply box origin to rel coordinates
							//gprintf("col  bg % fg % int %\n",to_hex(box.fg_color),to_hex(box.bg_color), to_hex(( ( (box.fg_color-box.bg_color) * cnt2) >> box.font_div) + box.bg_color) );
//							uint32_t dif = (box.fg_color-box.bg_color);
//							uint8_t Rval = ((dif >> 24) * cnt2) >> box.font_div;
//							uint8_t Gval = ((dif >> 16) * cnt2) >> box.font_div;
//							uint8_t Bval = ((dif >> 8) * cnt2) >> box.font_div;
//							uint32_t col = 0xff;
//							col = (col << 8) | Rval;
//							col = (col << 8) | Gval;
//							col = (col << 8) | Bval;
							uint32_t col = rgb_interp(box.fg_color, box.bg_color, cnt2, box.font_div);
							//gprintf("col % % %/", to_hex(box.fg_color), to_hex(box.bg_color),  to_hex(col));
							//draw_pixel(true_xy, ( ( (box.fg_color-box.bg_color) * cnt2) >> box.font_div) + box.bg_color);
							draw_pixel(true_xy, col);

						}
						else if (not box.no_bg)
						{
							true_xy = xy((i>>box.font_div), (j>>box.font_div)) + rel + box.org; // apply box origin to rel coordinates
							draw_pixel(true_xy, box.bg_color);
							//gprintf(".");
						}
						else
						{
#ifdef CHARDBG
							gprintf(".");
#endif
						}
						cnt2 = 0;
					}
					cnt++;

					//true_xy = xy(i, j) + rel + box.org; // apply box origin to rel coordinates
				}
#ifdef CHARDBG
			gprintf("\n");
#endif

		}
		//rel = xy(rel.x + (sz.x >> box.font_div), rel.y);
		rel = xy(rel.x + (sz.x >> box.font_div) + (box.font_div==2), rel.y);
		return status;
	}

};
