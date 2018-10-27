
struct gk_curve : gruik_obj
{
	// these sliders are made invisible and are thus tactile zones
	MODULE_INST(zoomx, gk_slider<HOR>);
	MODULE_INST(movex, gk_slider<HOR>);
	MODULE_INST(zoomy, gk_slider<VERT>);
	MODULE_INST(movey, gk_slider<VERT>);
	int buf_len;
	int32_t dc = 0;
	uint32_t bg_color = 0xff0E0E0E;
	uint32_t fg_color = 0xFFFF0000;
	xy posp;
	uint8_t zoomx_fac = 16;
	uint8_t zoomy_fac = 2;
	uint32_t movex_fac = 0;
	int16_t movey_fac = 0;
	uint8_t lock_sld = 0;
	uint32_t fullscale = 0;

	gk_curve(combo_t x) : gruik_obj(x)
	{
		zoomx.invisible = 1;
		zoomy.invisible = 1;

		movex.invisible = 1;
		movey.invisible = 1;
		movex.val = 0; // max zoom, this is 1:1 point / pixel mapping
		movey.val = 0; // max zoom, this is 1:1 point / pixel mapping
		zoomx.val = 128; // max zoom, this is 1:1 point / pixel mapping
		zoomy.val = 128; // max zoom, this is 1:1 point / pixel mapping
		zoomx.set_inactive(); //so that events on these controls are controlled from here
		zoomy.set_inactive();
		movex.set_inactive();
		movey.set_inactive();
		event_mask = MOVED;

	}

	void draw(uint16_t x)
	{
		gruik_obj::lcd->draw_rectangle(org, dim, bg_color, 1, 0);

		int32_t datap = 0;
		int32_t data = 0;
		uint32_t rdata = 0;
		// Get buffer size

		signal->psignal->get_data(RETURN_SIZE, &rdata);
		gprintf("#BBuffer sz %R", int(rdata));
		buf_len = rdata;

		bool unzoomy = (zoomy_fac) < 8;// + MAX(((zoomx.val >> 3)-16),uint16_t(0)) < 0;
		int zoomx_shift = 16 - (zoomx_fac);
		//uint32_t zoomx_osr = ( (zoomx.val >> 3) < 16) ? (1<<(16-(zoomx.val >> 3))): (1<<((zoomx.val >> 3)-16)); //oversampling ratio of buffer data
		uint32_t zoomx_osr = ( (zoomx_fac) < 16) ? (1<<(16-(zoomx_fac))): (1<<((zoomx_fac)-16)); //oversampling ratio of buffer data
		//if ( (zoomx.val >> 3) == 16
		uint32_t zoomy_shift = ABS( (int(zoomy_fac)) - 8 ); //oversampling ratio of buffer data

		// now find dim.x points to display

		int idx_mid = (buf_len * int(movex_fac)) >> 8;
		int win_sz = (uint32_t(dim.x) << (32-(zoomx_fac)))>>16;
		int idx0, idx1;
		int16_t x0, x1, remain;
		idx0 = x0 = x1 = 0;
		x1 =(dim.y>>1);
		uint16_t mask = ( 1 << ((zoomx_fac) - 16) ) - 1;
		//if (0)
		//gprintf("#R zooming % zoomosr % winsz % mid % movex % zoomxshft %", int( not((zoomx.val >> 3) <= 16)), int(zoomx_osr), win_sz, idx_mid, int(movex.val),zoomx_shift );
		for ( int i = -zoomx_osr-1; i < dim.x;i++) // -1-zoomx_dr to have datap initialized when required
		{
			//gprintf("#bi % data % %r  %r zoomosr %B", i, data,int(zoomx.val),int((zoomx.val >> 3) > 16), int(zoomx_osr));

			fullscale = (dim.y << 10);
			fullscale = unzoomy ? fullscale << zoomy_shift : fullscale >> zoomy_shift;
			if ((zoomx_fac) < 16) // deveral samples / pixel
			//if ((zoomx.val >> 3) <= 16) // deveral samples / pixel
			{
				//gprintf(">");
				int16_t x_min, x_max;
				x_min = MIN(dim.y, x1);
				x_max = MAX(0, x1);
				for (int j=0; j < (zoomx_osr); j++)
				{
					//gprintf("#");
					datap = data;
					idx0 = ( ( (i - (dim.x >> 1)) << (zoomx_shift)) + j) + idx_mid;
					if ( (idx0 >= 0) and (idx0 < buf_len) )
					{
						signal->psignal->get_data(ADDRESSED | idx0, &rdata);
					}
					//gprintf("read ok zoomx.val%\n", zoomx.val);

					data = rdata - dc; // DC lvl corection
//					x0 = x1;
//					uint16_t x0 = unzoomy ? data >> zoomy_shift : data << zoomy_shift;
//					x1 = MAX(MIN(x0 + (dim.y>>1), dim.y), 0);//, uint16_t(dim.y>>1)-1);
					x0 = x1;
				    x1 = unzoomy ? (data) >> zoomy_shift :
							data << (zoomy_shift);
					x1 = MAX(MIN(x1 + (dim.y>>1), dim.y),1);//, int16_t(dim.y>>1)-1);


					if ( (idx0>0) and (idx0<buf_len) and (i >= 0) )
					{
						//gprintf("org % len %", xy(i+org.x, x0+org.y + (dim.y>>1)), (x1-x0));
						x_min = MIN(x_min, x1);
						x_max = MAX(x_max, x1);
						//gprintf("org % len \n", line_org, len);
						//gruik_obj::lcd->draw_Vline(line_org,len , fg_color);
					}
					else if (1) //forces no display (line len = -1)
						{
							x_min = 1;//(dim.y>>1);
							x_max = 0;//(dim.y>>1);
							x1 = dim.y>1;
						}
					//else
						//gprintf("#RAddr rejected i % idx0 %", i, idx0);

				}
				xy line_org = xy(org.x+i, org.y + dim.y - x_max);
				int len = MAX((x_max - x_min)+1, 0); // MAX because max/min search does not complete is addr is incorrect
				//gprintf("min % max % org % len % \n", x_min, x_max, line_org, len);

				gruik_obj::lcd->draw_Vline(line_org,len , fg_color);

			}
			else //interpolated buf samples
			{
				//gprintf("ohe");

				remain = i & mask;//( ( ( uint32_t(i) << (25-(zoomx.val>>3)) ) &0xFFFFFF) >> (25-(zoomx.val>>3)) );
				if ( remain == 0 ) // time to fetch another sample
				{
					datap = data;
				//idx0 = ( (i - (win_sz >> 1)) << (zoomx_osr-1)) + idx_mid;
					idx0 = ( ( (i - (dim.x >> 1)) >> (( (zoomx_fac) -16)) )) + idx_mid;

					//idx0 = ( (i ) << (zoomx_osr-1)) + idx_mid;
					if ( (idx0 >= 0) and (idx0 < buf_len) )
					{
						signal->psignal->get_data(ADDRESSED | idx0, &rdata);
						data = rdata - dc; // DC lvl corection
						//gprintf("Int read ok idx0\n", idx0);

					}
					else
					{
						//gprintf("idx0 %\n", idx0);
						data = 0;
					}

				}
//				else
//					gprintf("% % % \n", to_hex(i), to_hex(uint32_t(i) << (32-zoomx_osr)), to_hex( ( uint32_t(i) << (32-zoomx_osr) ) >> (33-zoomx_osr)) );

#if 0
				x0 = x1;
			    x1 = unzoomy ? (datap + remain * (data - datap)) >> zoomy_shift :
						data << (zoomy_shift);
				x1 = MIN(x1, int16_t(dim.y>>1)-1);
				x1 = MAX(x1, int16_t(-dim.y>>1)-1);

				if ( (idx0>0) and (idx0<buf_len) and (i >= 0))
				{
					xy line_org = xy(org.x+i, org.y + (dim.y>>1) - MAX(x0,x1));
					int len = ABS(x1 - x0)+1;
					gprintf("x0 % x1 % Interp org % len % org % rem %\n", int(x0), int(x1), line_org, len, org, remain);
					//exit(0);
					gruik_obj::lcd->draw_Vline(line_org,len , fg_color);
				}
				else
					gprintf("#RRejected i % idx0 % x0 % x1 %", i, idx0, x0, x1);
#else
				x0 = x1;
				int z = datap + ( (data - datap) >> ( (zoomx_fac) - 16) ) * remain;
				//gprintf("> % % % % % %\n", data, datap, data-datap, (data - datap) >> ( (zoomx_fac) - 15), ( (data - datap) >> ( (zoomx_fac) - 15) ) * remain, z);

				x1 = z;//datap + ( (data - datap) >> ( (zoomx.val>>3) - 15) ) * remain;
			    x1 = unzoomy ? x1 >> zoomy_shift :
						x1 << (zoomy_shift);
				x1 = MAX(MIN(x1 + (dim.y>>1), dim.y),1);//, int16_t(dim.y>>1)-1);

				if ( (idx0>0) and (idx0<buf_len) and (i >= 0))
				{
					xy line_org = xy(org.x+i, org.y + dim.y - MAX(x0,x1));
					int len = ABS(x1 - x0)+1;
					//gprintf("x0 % x1 % Interp org % len % org % rem % data % datap % -> %\n", int(x0), int(x1), line_org, len, org, remain,data, datap, z);
					//gprintf("data % datap % rem % z = %\n", data, datap, remain, z);
					//exit(0);
					gruik_obj::lcd->draw_Vline(line_org,len , fg_color);
				}
				//else
					//gprintf("#MAddr rejected i % idx0 % x0 % x1 %", i, idx0, x0, x1);

#endif

			}
			//gprintf("zoomx.val % %\n", int(zoomx.val), int(((zoomx.val >> 3) < 16)));
		}
		gprintf("zoomx % zoom osr% zoom shift % mid %\n", int(zoomx.val>>3),int(zoomx_osr), int(zoomy_shift), idx_mid);
		gprintf(" unz % %   %     %     %\n",int(unzoomy), ABS(( (int(zoomy.val) >> 4) - 16 )), (int(zoomy.val) >> 4) - 16, ABS(-16), ABS(16));


	}

	void layout()
	{
		movey.map<LEFT>(32);
		zoomy.map<RIGHT>(224);
		zoomx.map<TOP>(64);
		movex.map<BOTTOM>(224);
		zoomx.shrink<VERT>(192);
		movex.shrink<VERT>(192);

//		zoomx.remap<RIGHT>(64);
	//	movex.remap<TOP>(192);
	}
	uint8_t event(xy pos, uint8_t x)
	{
#if 1
		uint32_t zoomx_osr = ( (zoomx_fac) < 16) ? (1<<(16-(zoomx_fac))): 1; //oversampling ratio of buffer data
		gprintf("#MCurve evt % % % %r %r %r dc %r %r",
				pos, posp, x, int(zoomx_fac), int(zoomy_fac),int(movex_fac), dc, int(fullscale));
		// track move events with suffucient amplitude
		if ( ( (ABS(pos.x -posp.x) > 8) or (ABS(pos.y-posp.y)>8) ) and (x == MOVED) )//and (x == 2) )
		{
			// zoom Y ----------------
			if (lock_sld == 1)
			{
				if ( (pos.y < posp.y) and (zoomy_fac < 16) )
					zoomy_fac ++;
				else if ( (pos.y > posp.y) and (zoomy_fac > 0))
					zoomy_fac--;
			}

			if (lock_sld == 2)
			{
				if ( (pos.y < posp.y) )//and (dc < 16384) )
					dc += (fullscale>>13);
				else if ( (pos.y > posp.y) )//and (dc > -16384))
					dc-= (fullscale>>13);
			}
			// avoid events in 2 different controls with shared spaces

			if (lock_sld == 3)
			{
				if ( (pos.x > posp.x) and (zoomx_fac < 32) )
					zoomx_fac ++;
				else if ((pos.x < posp.x) and (zoomx_fac > 0))
					zoomx_fac--;
			}

			if (lock_sld == 4)
			{
				if ( (pos.x > posp.x) and (movex_fac <= 255) )
					movex_fac += (8);//zoomx_osr;
				else if ((pos.x < posp.x) and (movex_fac > 0) )
					movex_fac-= (8);//zoomx_osr;
			}

			//if (e != 0)
			//	exit(0);


			draw(0);
			posp = pos;
		}
		else if (x == 1)//click evevnt
		{
			if (zoomy.coord_match(pos))
				lock_sld = 1;
			else if (movey.coord_match(pos))
				lock_sld = 2;
			else if (zoomx.coord_match(pos))
				lock_sld = 3;
			else if (movex.coord_match(pos))
				lock_sld = 4;
			else
				lock_sld = 0; // release lock
			posp = pos;
		}
		return x;
#endif
	}
//	// new data set available
	void signal_event(uint8_t x)
	{
		gprintf("Curve event data %\n", int(x));
		draw(0);
	}


};
