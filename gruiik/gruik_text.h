struct gk_font
{
    uint32_t first_ascii;
    uint32_t last_ascii;
    uint8_t height;
    const uint8_t* bitmap;
    const uint32_t* map;
    const uint8_t* width;
    struct gk_font * next_page;    /*Pointer to a font extension*/

    template<class T1,class T2, class T3, class T4, class T5, class T6, class T7>
    gk_font(T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6 ,T7 x7)
	{
    	first_ascii = x1; last_ascii = x2; height = x3; bitmap = x4; map = x5; width = x6; next_page = x7;
	}
};

#include "fonts/gruik_font_dejavu_40.h"
#include "fonts/gruik_font_VeraMono_40.h"
#include "fonts/gruik_font_VeraMono_16.h"
#include "fonts/gruik_font_VeraMono_32.h"
#include "fonts/gruik_font_VeraMoBd_32.h"
#include "fonts/gruik_font_VeraMono_20.h"
#include "fonts/gruik_font_dejavu_10.h"
#include "fonts/gruik_font_DejaVuSansMono_16.h"
#include "fonts/gruik_font_DejaVuSansMono_32.h"
#include "fonts/gruik_font_DejaVuSansMono_48.h"
#include "fonts/gruik_font_DejaVuSansMono_Bold_48.h"
#include "fonts/gruik_font_DejaVuSansMono_Bold_32.h"
#include "fonts/gruik_font_DejaVuSans_40.h"
#include "fonts/gruik_font_UbuntuMono_B_40.h"
#include "fonts/gruik_font_LiberationMono_Regular_40.h"
#include "fonts/gruik_font_LiberationMono_Bold_40.h"
#include "fonts/gruik_font_LiberationMono_Regular_16.h"
#include "fonts/gruik_font_FreePixel_16.h"
#include "fonts/gruik_font_FreePixel_32.h"
#include "fonts/gruik_font_ProFontWindows_16.h"
#include "fonts/gruik_font_PixelFJVerdana12pt_16.h"
dejavu_40 default_font;

//VeraMoBd_32 default_font;
//DejaVuSansMono_48 default_font;
//DejaVuSansMono_Bold_32 default_font;
//DejaVuSans_40 default_font;
//UbuntuMono_B_40 default_font;
//LiberationMono_Regular_16 default_font;
//DejaVuSansMono_16 default_font;
//FreePixel_32 default_font; // BOF MADE IT NICE
FreePixel_16 default_mono_font; // MADE IT NICE (-fh = 12)
//ProFontWindows_16 default_font;
//PixelFJVerdana12pt_16 default_font; //NOOOO
//VeraMono_16 default_font; //no, can't do it right
// A frame with no event driven components inside (ex: images, text...)
struct box_base : gk_coords
{
	box_base(){}
	box_base(gruik_obj& x) : gk_coords(x){} // copy parent layout

	void crop(uint8_t crop)
	{
		org = org + xy(crop,crop);
		dim = dim - xy(2*crop,2*crop);
	}
};

//struct

uint8_t convert_upper_case(uint8_t c, uint8_t fmode)
{
	if (fmode & dSYM_CASE)
	{
		bool  keep_num = (c>=48) and (c<58) and (fmode & dUPPER_CASE);
		c -= ( ((c>=48)* 15) + ((c>=96) * 39)) - ((c >= 102)*10) - ((c >= 109)*26) - ((c>= 115)*26)	- (keep_num * 15);
	}
	else if ( (fmode & dUPPER_CASE) and ( (c>= 96) and (c <=122) ) ) // upper case
		c -= 32;

	return c;
}

struct text_box : gk_coords
{
	gruik_obj* pgk;
	xy current;
	uint8_t inter = 0;
	uint8_t crop = 4;//
	uint8_t font_div;
	bool replace_us = 0; //replace underscore by space
	text_box(){}
	text_box(gruik_obj* x){
		pgk = x; current = xy(0,0);
		font_div = 1;
		//font_div = gruik_obj::style.font_div;
//		uint16_t h = dim.y;
//		// Adjust text size to height
//		if (h)
//		while (h < 50)
//		{
//			gprintf(".", int(h));
//			h<<=1;
//			font_div ++;
//		}
	} // copy parent layout

	bool roll = 0; // defines the way lines are written (upward / downward rolling text)
	bool wrap = 1; // defines the way lines are written (wrap line or new line)
	bool multi_line = 1; // defines the way lines are written (wrap line or new line)
	bool Vcentered = 1;
	bool centered = 1;
	bool no_special = 0;
	bool input_gstring = 1; // when data is given by gstrin. else use char*
	gk_font* font = static_cast<gk_font*>(&default_font);
	gstring* buf;
	const char* buf2;
	int16_t buf_len = 0;
	bool end_draw = 0;
	uint32_t bg_color = 0xff7f7f7f;
	uint32_t fg_color = 0xffFFFFFF;
	bool no_bg = 1;
	//ool force_upper = 0; //force upper case
	//bool force_sym = 0; //force symbol case

	uint8_t fmode = 0;
	const uint8_t* char_addr;
	bool abort_draw_on_ovf = 0;
	bool auto_scale = 1;
	int idx = 0;
	char cur_char; // so that draw_bitmap can apply tricks when font_div == 2
	void init_coords()
	{
		org = pgk->org + xy(crop,crop);
		dim = pgk->dim - xy(2*crop,2*crop);
	}

	// divide font size by 2**x
	void set_font_div(uint8_t x)
	{
		font_div = x;

	}

	uint8_t get_char(int idx)
	{
		if (input_gstring)
			return buf->operator[](idx);
		else
			return buf2[idx];
	}

	void home()
	{
		if (1)//up_to_down)
		{
			current = xy(0,0);
		}
		else
		{

		}
	}

	void init_current()
	{
		gprintf("#UINIT %", org);
		// Adjust text size to height
		idx = 0;

		if (auto_scale)
		{
			uint16_t h = dim.y;
			font_div = 0;
			if (h)
				while (h < (font->height - 10))
				{
					gprintf(".", int(h));
					h<<=1;
					font_div ++;
				}

			if (font_div)
				gprintf("#VScaledfont to %Y in %Y %Y", int(font_div), org, dim);
		}
		else
			gprintf("#VKepping font scaling %Y in %Y %Y ", int(font_div), org, dim);

		if (roll)
		{}
		else if (not centered)
		{
			//current = xy(0,0);// begin top left
		}
		else // requires complicated calculations
		{
			gprintf("#UCentered");
			//current = xy(0,0);// begin top left
#if 0
			int idx0 = 0; int w = 0;
			uint8_t c;
			while (idx0 < buf_len)
			{
				c = get_char(idx0);//buf->operator[](idx0);
				int tmp = c - font->first_ascii;
				uint8_t idx = (tmp < 0) ? 0 : (tmp > font->last_ascii-font->first_ascii) ? font->last_ascii-font->first_ascii : tmp;
				w+= (font->width[idx]>>font_div);
				//gprintf("% % \n", c, w);
				idx0++;
			}
			if (Vcentered)
				current = xy((dim.x>>1) - (w>>1), (dim.y>>1)-(font->height>>(1+font_div)));
			else
				current = xy((dim.x>>1) - (w>>1), current.y);
#else
			line_Hcenter();
			if (Vcentered)
				line_Vcenter();
#endif
		}
	}

	void line_Vcenter()
	{
		current = xy(current.x, (dim.y>>1)-(font->height>>(1+font_div)));
	}

	void line_Hcenter()
	{
		gprintf("#WHcenter % idx % buf_len %", org, idx, buf_len);
		int idx0 = idx;
		int w = 0;
		uint8_t c;
		while (idx0 < buf_len)
		{
			c = get_char(idx0);//buf->operator[](idx0);
			int tmp = c - font->first_ascii;
			if ((tmp >= 0) and (tmp <= font->last_ascii-font->first_ascii))// is inside font definition
			{
				w+= (font->width[tmp]>>font_div);
				gprintf("% % \n", int(c), w);
			}
			//uint8_t idx = (tmp < 0) ? 0 : (tmp > font->last_ascii-font->first_ascii) ? font->last_ascii-font->first_ascii : tmp;

			idx0++;
			if ( (c == 10) and not no_special) break;// end line
		}
		current = xy((dim.x>>1) - (w>>1), current.y);
		gprintf("#M org % dim % len % res % idx %", org, dim, w, current, idx);


	}

	void set_roll(bool x){roll = x;}
	void set_font(gk_font& x){font = &x;}
	void set_centered(bool x=1){centered = x;}
	void set_Vcentered(bool x=1){Vcentered = x;}
	void set_buffer(gstring* x){buf = x; buf_len = x->size(); input_gstring = 1;}
	void set_buffer(const char* x, int len){buf2 = x; buf_len = len; input_gstring = 0;idx = 0;}
	void set_crop(uint8_t x){crop = x;}

	void new_line() {current = xy(0, current.y + (font->height>>font_div) + inter);}
	void special(uint8_t c)
	{

		if ((c == 10) or (c == 13) or (c == '\n') ){new_line(); if (centered) line_Hcenter();} //newline
		if (c == 2) {init_current();} // home
	}

	//uint8_t draw_bitmap(uint8_t* map, xy rel, xy sz, const T& box)

	uint8_t draw_char(uint8_t c)
	{
		//gprintf("#VDraw char ");

		uint8_t status = 0;
		if ((c < 32))
		{
			if (not no_special)
				special(c);
			status = 0;
		}
		else
		{
			if (c == '_' and replace_us)
			{
				gprintf("#Mreplacing underscore");
				c = ' '; // So that menus have spaces instead of underscores required to name variables
			}
			else if (c == '_')
				gprintf("#MNOT replacing underscore, so WTF ????");
			if (buf_len < 2)
			c = convert_upper_case(c, fmode);
			if (c == '_')
				gprintf("#RIS UNDERSCORE");
			cur_char = c;
			int tmp = c - font->first_ascii;

			uint8_t idx = (tmp < 0) ? 0 : (tmp > font->last_ascii-font->first_ascii) ? font->last_ascii-font->first_ascii : tmp;
			char_addr = (&font->bitmap[font->map[idx]]);
			status = gruik_obj::lcd->draw_bitmap(char_addr,
					current, xy(font->width[idx], font->height), *this);
			gprintf("#VDraw char % % % % %", current, xy(font->width[idx], font->height), (*this).dim, int((*this).font_div));
		}
		return status;
	}


	uint8_t draw_text()
	{
		gprintf("#VDraw text len % div %", buf_len, int(font_div));
		init_coords(); // make sure that box is properly sized wrt its parent
		//exit(0);
		//int idx = 0;
		uint8_t status = 0;
		if (centered)
			init_current(); // always from the start
		//exit(0);

		gprintf("/");
		uint8_t c;
#if 1
		while( (idx < buf_len) and ( not (end_draw and abort_draw_on_ovf) ))
		{
			c = get_char(idx);//uf->operator[](idx);
			idx++;
			//gprintf("iDX % char % buflen %\n", idx,c,int(buf_len));
			//if (bu) exit(0);

			status = draw_char(c);
			if (status & RIGHT_OVF)
			{
				gprintf("#RROVF");
				if (not multi_line)
					end_draw = 1;
				else
				{
					if (not wrap)
					{
						new_line();
						gprintf("#RNEWLINE");
						status = draw_char(c);
					}
				}
			}
			if ( (status & BOTTOM_OVF) and (status & TOP_OVF))
				end_draw = 1;


		}
#endif
		gprintf("#VEnd Draw text len % ", buf_len);

		return end_draw;
	}

};

struct gk_frame : gruik_obj
{
	gk_frame(combo_t x) : gruik_obj(x)
	{
		event_mask = 0;
	}
};

#define SZ_BUF 255

struct gk_text_box : gruik_obj
{
	text_box caption;
	bool toggle = 0;
	bool toggle_value = 0;
	bool live = 0;
    char buffer[SZ_BUF];

	uint8_t cur_idx = 0;
	uint8_t last_idx = 0;
	uint8_t last_last_idx = 0;

	bool up_to_down = 1;

	gk_text_box(combo_t x) : gruik_obj(x), caption(this)
	{
		caption.set_centered(0);
		caption.fg_color = 0xFF000000;
		caption.bg_color = 0xfffcfcfc; // lsbs oare 0 to allow quick interpolation
		caption.wrap = 0;
		caption.abort_draw_on_ovf = 1;

		//caption.set_buffer(&name);

	}

	void set_font_div(uint8_t x)
	{
		caption.set_font_div(x);
	}
	void init()
	{
		cur_idx = 0;
		last_idx = 0;
		last_last_idx = 0;
		buffer[cur_idx] = 0;//uint8_t(ENDTEXT);
	}

	// for live text
	void signal_event(uint8_t x)
	{
		print_char(x);
	}

	template<class T>
	void copy_buffer(T& target)
	{
		for (int i = 0; i< SZ_BUF; i++)
			target.buffer[i] = buffer[i];
		target.cur_idx = cur_idx;
	}
	template<class T>
	void print_char(T c)
	{
		c = (cur_idx < (SZ_BUF-1) ? c : c==8 ? c : 0);
		gprintf("cur % last % \n", int(cur_idx), int(last_idx));
		if ((last_idx == cur_idx) and ((c >= 32) or (c == 10)) )
			cur_idx++;

		if ( (cur_idx < (SZ_BUF-1)) and (c >= 32))//(c != 0) and (c != 10) )
		{
			buffer[cur_idx] = c;
			cur_idx++;
			buffer[last_idx] = cur_idx; // copy index in string

		}

		if (c == 8)
		{
			if (cur_idx != last_idx)
				cur_idx -= (cur_idx != 1);

			if (cur_idx != last_idx)
			{
				buffer[last_idx] = cur_idx;
			}
			else //if (cur_idx != 0)
			{
				int n = 0;
				while (uint8_t(buffer[n]) != last_idx)
					n = uint8_t(buffer[n]);
				last_idx = n;
				gprintf("***********************************************************");
				buffer[n] = cur_idx;
			}


		}
		else if ((c == 10))//and (cur_idx < (SZ_BUF-1)))
		{
			buffer[last_idx] = cur_idx; // copy index in string
			last_last_idx = last_idx;
			last_idx = cur_idx;
		}
		else if (c == 2)
			init();

		if (not invisible)
			draw(0);
	}

	template <class T>
	void print_line(T x)
	{
		gprintf("#U****************print***************** cur_idx % ");
		//caption.up_to_down = up_to_down;
		uint8_t c = 255;
		last_idx = cur_idx;
		int i = 0;
		if (up_to_down and (cur_idx < 254))
			cur_idx++;
		while ( (cur_idx < (SZ_BUF-1)) and (c != 0) and (c != 10) )
		{
			c = x[i];
			gprintf(">>% \n", c);
			if (c == 2) //HOME
				init();
			else
			{
				buffer[cur_idx] = c;
				cur_idx++;
				i++;
			}
		}

		if (up_to_down)
			buffer[last_idx] = cur_idx; // copy index in string
		else
		{
			buffer[cur_idx] = last_idx; // copy index in string
			cur_idx++;
		}
		if (not invisible)
			draw(0);
	}



	void draw(uint16_t x)
	{
		gprintf("#Vgk_text_box draw up % idx %", int(up_to_down), int(cur_idx));
		gruik_obj::lcd->draw_rectangle(org, dim, caption.bg_color, 1, 0);
		caption.home();
		uint8_t status = 0;
		int idx = up_to_down ? 0 : cur_idx;
		while ( (status == 0) and ((idx != 0) or up_to_down) and ((idx < cur_idx) or not up_to_down) )
		{
			if (up_to_down)
			{
				caption.set_buffer(&buffer[idx+1], uint8_t(buffer[idx]) -idx - 1);
				idx = uint8_t(buffer[idx]);
			}
			else
			{
				caption.set_buffer(&buffer[uint8_t(buffer[idx])], idx - uint8_t(buffer[idx]) - 1);
				idx = uint8_t(buffer[idx]);
			}

			status = caption.draw_text();
			caption.new_line();
			gprintf("#Mstat % idx % cur_idx", int(status), idx, int(cur_idx));
		}
		//while (1){}
	}




};

int get_idx(gk_font* font, uint8_t c)
{
	int res = -1;
	if ((c >= font->first_ascii) and (c<=font->last_ascii))
		res = c - font->first_ascii;
	return res;
}

//int is_special(uint8_t)


struct gk_text_line : gk_coords
{
	//gruik_obj* parent;

	gk_text_line(const gk_coords& x) : gk_coords(x)
	{}

	gstring text;
	xy rel = xy(0,0); // display origin (relative to text box)
	gk_text_line* prev = NULL; // double linked list, so that text can be displayed top to bottom wise or inversely
	gk_text_line* next = NULL;
	//int cur_pos = -1; // position of live char in string, if any
	gk_font* cur_font  = static_cast<gk_font*>(&default_font);
	uint8_t font_div = 1;

	uint32_t bg_color = 0xff808080;
	uint32_t fg_color = 0xffFFFFFF;
	bool abort_draw_on_ovf = 0;
	bool no_bg = 1;

	int dsp_height = 0;//cur_font->height >> cur_div; // display height (corresponds here to default font )
	//gk_text_line(gk_coords)
	char cur_char;

	void set_font( gk_font& x, uint8_t font_div_i)
	{
		cur_font = &x;
		font_div = font_div_i;
	}
	void set_org(xy x) // sets relative position of this text (actually only x.y should matter, x.x=0)
	{
		rel = x;
	}
	xy draw(uint16_t cmd, xy pos=xy(0,0)) /// when cmd = 1, will not draw, just compute the size of this text / when 2 returns char that matches
	{
		if (cmd == dGET_CURSOR_IDX)
		{
			if (pos.y < (org.y + rel.y) )	 // no chance the cursor is here
				return xy(0,0);
			if (text.size() == 0)
			{
				bool match = (pos.y < (org.y + rel.y + (cur_font->height) >> font_div) );
				return xy(0, match);
			}
		}

		xy res = rel;
//		if (cmd == dDRAW)
//			res = rel; // initialization to actual relative text position is required to draw
//		else
//			res = xy(0,0); // otherwise do not init for text dimension

		int idx = 0;
		int line_height = 0;
		bool new_line = 0;
		while (idx < text.size())
		{
			char c = text[idx];
			if (c < 32)
			{
				if (c == 10)
					new_line = 1;
			}
			else
			{
				cur_char = c;
				uint8_t lut_idx = get_idx(cur_font, c);
				new_line = 0;
				int char_width = cur_font->width[lut_idx];
				int char_height = cur_font->width[lut_idx];
				const uint8_t* char_addr;
				uint8_t status = 0;

				line_height = MAX( (cur_font->height >> font_div), line_height); // For future mixed font sizes
				xy new_res = res + xy(char_width>>font_div, res.y);
				if (new_res.x > dim.x) // too large for box width
				{
					new_line = 1;
				}
				else // can draw
				{
					if (cmd == dDRAW) //draw
					{
						char_addr = (&cur_font->bitmap[cur_font->map[lut_idx]]);
						status = gruik_obj::lcd->draw_bitmap(&cur_font->bitmap[cur_font->map[lut_idx]],
									res, xy(cur_font->width[lut_idx], cur_font->height), *this);
					}
					else if(cmd == dGET_SIZE)
					{
						res = new_res;
					}
					else if (cmd == dGET_CURSOR_IDX)
					{
						if (pos.y <= (org.y + res.y)) // on the right line
						{
							// This char
							if ( (pos.x <= (org.x + res.x) ) and (pos.x < (org.x +dim.x) ) )
								return xy(idx,1);
						}
					}
					idx++;
				}
				//res = new_res;

			}
			if (new_line)
			{
				res = xy(0, res.y + line_height);
				line_height = 0;
			}
		}

		if (cmd == dGET_SIZE) // subtract init value for actual size
			res = res - rel;

		return res;
	}


};

struct gk_live_text : gruik_obj
{
//	char_addr = (&font->bitmap[font->map[idx]]);
	//status = gruik_obj::lcd->draw_bitmap(char_addr,
		//	current, xy(font->width[idx], font->height), *this);
	//text_box caption;
	gk_text_line* active_line = NULL;
	gk_text_line* first_line = NULL;
	gk_text_line* last_line = NULL;
	uint8_t font_div = 0;
	int cursor_pos = 0; // cursor position in current line (in chars)
	//ector<gk_text_line*> lines;
	void draw(uint16_t x)
	{

	}
	gk_text_line* insert_line()
	{
		gk_text_line* new_line = new gk_text_line(*this);

		active_line->next = new_line;
		new_line->prev = active_line;
		if (active_line == last_line)
			last_line = new_line;
		// Now split active line content depending on cursor position
		new_line->text.append(active_line->text.substr(cursor_pos, active_line->text.size()-cursor_pos));
		active_line->text.erase(cursor_pos, active_line->text.size()-cursor_pos);

		active_line = new_line;
		cursor_pos = 0;

		draw(0);
		return new_line;
	}

	void delete_line()
	{
		if (first_line == last_line)// only remaining line : can't delete
		{

		}
		else
		{
			// First check whether deleted line is first or last, update last & first accordingly.
			if (active_line == first_line)
				first_line = active_line->next;
			if (active_line == last_line)
				last_line = active_line->prev;

			// Then unlink line
			if (active_line->prev)
				active_line->prev->next = active_line->next;
			if (active_line->next)
				active_line->next->prev = active_line->prev;


			// finally kill it
			delete active_line;
		}
	}
	gk_live_text(combo_t x) : gruik_obj(x)
			{
				active_line = new gk_text_line(*this);
				first_line = active_line;
				last_line = active_line;
				//lines.push_back()
				//caption.set_buffer(&name);

			}
	gstring text;

	void signal_event(uint8_t x)// receive from keyboard
	{
		if (x != 0)
		{
			if (x == 10)
				insert_line();
			else
			{
				active_line->text.insert(cursor_pos, 1, x);
				active_line->draw(dDRAW);
			}
		}
	}

	//uint8_t event()

};
