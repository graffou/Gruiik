#ifdef DEBUG
//#define gprintf(...) gprintf
#else
#define gprintf(...)
#endif

#include "mbprt.h"
#include <vector>
#include <functional>

// Mixed flags (events properties and return values)
typedef enum
{
	NO_EVENT = 0, PRESSED = 1, RELEASED = 2, VISIBLE = 16, MOVED = 32, ABORT = 64, SIGNAL = 128, TOGGLE=128,MATCHED = 254
} event_t;

typedef enum

{
	SINGLE = 0x40000000, ADDRESSED = 0x80000000, SET_POINTER = 0x20000000,
	RETURN_POINTER = 0x20000000, ROLL = 0x10000000, RETURN_SIZE = 0x0800000, SET_SIZE = 0x0800000
}data_mode_t;

typedef enum
{
	dALIGNED_H = 0x8000, dBOTTOM = 0x4000, dALIGNED_V = 0x2000, dRIGHT = 0x1000, dSCALED = 0x0800, dTRANSPARENT = 0x0100,
	dUPPER_CASE = 1, dSYM_CASE = 2, dDRAW = 0, dGET_SIZE = 1, dGET_CURSOR_IDX = 2, dLAST_LINE = 0x100, dALL_LINES = 0x200
}draw_mode_t;

// return codes when drawbox overflow during bitmap draw
typedef enum
{
	OK = 0x00, BOTTOM_OVF = 0x80, TOP_OVF = 0x40, RIGHT_OVF = 0x20, LEFT_OVF = 0x10
}draw_status_t;

#define ENDTEXT 255

struct style_t
{
	uint8_t font_div = 0;

};

// various utilities ----------------------------------------
int cnt = 0;
template<class T>
gstring pconv(T* x)
{
	return to_hex(int(reinterpret_cast<std::uintptr_t>(x)));
}

#define MAX(a, b) ( (a < b) ? b : a )
#define MIN(a, b) ( (a > b) ? b : a )
#define ABS(a) ( (a < 0) ? -(a) : a)

class gruik_obj;

struct combo_t
{
	gstring name;
	gruik_obj* parent;
};


class signal_link;
// signal related classes ---------------------------------------------------
// For now, a single signal can be attached with any gruik_obj.
struct signal_base
{
	signal_base(){}
	//gruik_obj* attached = NULL;
	virtual void attach(signal_link* x){};
	virtual void detach(signal_link* x){};
	virtual void set_event(uint8_t data){};
	virtual void get_data(uint32_t mode, uint32_t* data){};
	virtual void set_data(uint32_t mode, uint32_t* data){};

};
template<class T>
struct combo2_t
{
	gruik_obj* attached = NULL;
	T* psignal = NULL;
};


struct signal_link
{
	gruik_obj* attached = NULL;
	signal_base* psignal = NULL;
	signal_link* prev_entry = NULL; // should link to a link to the same signal
	template<class T>
	signal_link(combo2_t<T> x)
	{
		attached = x.attached;
		psignal = x.psignal;
		//gprintf("#MMMMMMMM call %", pconv(x.psignal));
		x.psignal->attach(this);
	}
	signal_link(signal_base& x, gruik_obj& y)
	{
		attached = &y;
		psignal = &x;
		psignal->attach(this);
	}

	void activate()
	{
		gprintf("#UReactivating link");
		psignal->attach(this);
	}

};

// geometry related classes--------------------------------------------------
class VERT;
class HOR;
class RIGHT;
class LEFT;
class TOP;
class BOTTOM;
template<class T> class DIM;

template<class T>
bool is_vert() {return 0;}
template<>
bool is_vert<VERT>() {return 1;}
template<>
bool is_vert<  DIM<VERT> >() {return 1;}
template<>
bool is_vert<TOP>() {return 1;}
template<>
bool is_vert<BOTTOM>() {return 1;}
template<>
bool is_vert< DIM<TOP> >() {return 1;}
template<>
bool is_vert< DIM<BOTTOM> >() {return 1;}
// returns whether position is located towards obj origin (otherwise is located on the other side wrt direction)
template<class T>
bool at_org() {return 0;}
template<>
bool at_org<LEFT>() {return 1;}
template<>
bool at_org<TOP>() {return 1;}


//#define COORDS_OLD
#if 0
struct xy
{
	int16_t x;
	int16_t y;
	xy(){}
	template <class T1, class T2>
	xy(T1 x_i, T2 y_i) : x(x_i), y(y_i) {}
// default is HOR
	template <class T> xy scale(uint8_t ratio){}
	xy operator-(xy a) {return xy(x-a.x, y-a.y);}
	xy operator+(xy a) {return xy(x+a.x, y+a.y);}
	template <class T> xy offset(xy a, int16_t spacer = 0){}
	template <class T> xy offset(int16_t spacer){}
	// returns coordinate of selcted dimension
	template <class T> xy sel_dim(){}
	template <class T> xy set_dim(int16_t x){}

	template <class T> int16_t get(){}
	template <class T> void set(int16_t a){}



};

template <> xy xy::scale<HOR>(uint8_t ratio){return xy( (( x * uint16_t(ratio) + 127) >> 8), y);}
template <> xy xy::scale<VERT>(uint8_t ratio){return xy( x, (( y * uint16_t(ratio) + 127) >> 8));}
template <> xy xy::offset<HOR>(xy a, int16_t spacer){return xy(x+a.x+spacer,y );}
template <> xy xy::offset<VERT>(xy a, int16_t spacer){return xy(x, y+a.y+spacer);}
template <> xy xy::offset<HOR>(int16_t spacer){return xy(x+spacer,y );}
template <> xy xy::offset<VERT>(int16_t spacer){gprintf("RRR % % % % ", x,y, x, y+spacer);return xy(x, y+spacer);}
template <> xy xy::sel_dim<HOR>(){return xy(x, 0);}
template <> xy xy::sel_dim<VERT>(){return xy(0, y);}
template <> xy xy::set_dim<HOR>(int16_t a){return xy(a, y);}
template <> xy xy::set_dim<VERT>(int16_t a){return(xy(x,a));}
template <> int16_t xy::get<HOR>(){return x;}
template <> int16_t xy::get<VERT>(){return y;}
template <> void xy::set<HOR>(int16_t a){x = a;}
template <> void xy::set<VERT>(int16_t a){y = a;}

#else
//#pragma pack(push,1)
struct xy
{
	int16_t x;
	int16_t y;
	xy(){}
	template <class T1, class T2>
	xy(T1 x_i, T2 y_i) : x(x_i), y(y_i) {}
// default is HOR
	template <class T> xy scale(uint8_t ratio){return xy( (( x * uint16_t(ratio) + 127) >> 8), y);}
	xy operator-(xy a) {return xy(x-a.x, y-a.y);}
	xy operator+(xy a) {return xy(x+a.x, y+a.y);}
	template <class T> xy offset(xy a, int16_t spacer = 0){return xy(x+a.x+spacer,y );}
	template <class T> xy offset(int16_t spacer){return xy(x+spacer,y );}
	// returns coordinate of selcted dimension
	template <class T> xy sel_dim(){return xy(x, 0);}
	template <class T> xy set_dim(int16_t a){return xy(a, y);}

	template <class T> int16_t get(){return x;}
	template <class T> void set(int16_t a){x = a;}



};//__attribute__((packed));
template <> xy xy::scale<VERT>(uint8_t ratio){return xy( x, (( y * uint16_t(ratio) + 127) >> 8));}
template <> xy xy::offset<VERT>(xy a, int16_t spacer){return xy(x, y+a.y+spacer);}
template <> xy xy::offset<VERT>(int16_t spacer){gprintf("RRR % % % % ", x,y, x, y+spacer);return xy(x, y+spacer);}
template <> xy xy::sel_dim<VERT>(){return xy(0, y);}
template <> xy xy::set_dim<VERT>(int16_t a){return(xy(x,a));}
template <> int16_t xy::get<VERT>(){return y;}
template <> void xy::set<VERT>(int16_t a){y = a;}

template <> xy xy::scale< DIM<TOP> >(uint8_t ratio){return xy( x, (( y * uint16_t(ratio) + 127) >> 8));}
template <> xy xy::scale< DIM<VERT> >(uint8_t ratio){return xy( x, (( y * uint16_t(ratio) + 127) >> 8));}
template <> xy xy::offset< DIM<TOP> >(xy a, int16_t spacer){return xy(x, y+a.y+spacer);}
template <> xy xy::offset< DIM<TOP> >(int16_t spacer){gprintf("RRR % % % % ", x,y, x, y+spacer);return xy(x, y+spacer);}
template <> xy xy::sel_dim< DIM<TOP> >(){return xy(0, y);}
template <> xy xy::set_dim< DIM<TOP> >(int16_t a){return(xy(x,a));}
template <> int16_t xy::get< DIM<TOP> >(){return y;}
template <> void xy::set< DIM<TOP> >(int16_t a){y = a;}


template <> xy xy::scale< DIM<BOTTOM> >(uint8_t ratio){return xy( x, (( y * uint16_t(ratio) + 127) >> 8));}
template <> xy xy::offset< DIM<BOTTOM> >(xy a, int16_t spacer){return xy(x, y+a.y+spacer);}
template <> xy xy::offset< DIM<BOTTOM> >(int16_t spacer){gprintf("RRR % % % % ", x,y, x, y+spacer);return xy(x, y+spacer);}
template <> xy xy::sel_dim< DIM<BOTTOM> >(){return xy(0, y);}
template <> xy xy::set_dim< DIM<BOTTOM> >(int16_t a){return(xy(x,a));}
template <> int16_t xy::get< DIM<BOTTOM> >(){return y;}
template <> void xy::set< DIM<BOTTOM> >(int16_t a){y = a;}
#endif

template<>
string to_string(const xy& x)
{
    char buf[12];
    sprintf(buf, "%d %d", int(x.x), int(x.y));
    return string(buf);
}
template <class T> xy split(uint8_t ratio, xy ul, xy lr){}
template <> xy split<HOR>(uint8_t ratio, xy ul, xy lr){}
template <> xy split<VERT>(uint8_t ratio, xy ul, xy lr){}

//include LL raw driversd
//#ifdef PC_GRUIK
#include "gruik_pc.h"
//#endif

struct prout
{
	uint8_t z;
	bool b;
};

template<class T, class T2>
combo_t make_combo_t(const T& name_i, T2& parent_i)
{
	combo_t res;
	res.name = name_i;
	res.parent = static_cast<gruik_obj*>(&parent_i);
	return res;
}

template<class T>
//combo2_t make_combo2_t(signal_t<T>& x, gruik_obj& y)
combo2_t<T> make_combo2_t(T& x, gruik_obj& y)
{
	combo2_t<T> res;
	res.psignal = (&x);
	res.attached = &y;
	return res;
}


struct gk_coords
{
	xy org, dim;
	uint8_t custom_layouted = 0;
	gk_coords(){}

	void set(xy org_i, xy dim_i)
	{
		org = org_i; dim = dim_i;
	}
	void set(const gk_coords& x_i)
	{
		this->operator=(x_i);
	}

	// Locate obj wrt to itselfr
	template<class T>
	void remap(uint8_t ratio)
	{
		xy new_dim = dim.scale< DIM<T> >(ratio); // scale cur area wrt dimension (T = HOR or VERT)
		if (at_org<T>())
		{
			dim = new_dim;
		}
		else
		{
			org = org.offset< DIM<T> >(new_dim);
			dim = dim - new_dim.sel_dim< DIM<T> >();
		}
		//gprintf("#UMAP % % vert % atorg %", org, dim, int(is_vert< DIM<T> >()), int(at_org<T>()));
		custom_layouted = 1; // ?
	}

	template<class T>
	void shrink(uint8_t ratio)
	{
		xy new_dim = dim.scale< DIM<T> >(ratio); // scale cur area wrt dimension (T = HOR or VERT)
		xy diff = dim - new_dim;
		org = org + xy(diff.x>>1, diff.y>>1);
		dim = new_dim;
		//gprintf("#UCROP % % vert % atorg %", org, dim, int(is_vert< DIM<T> >()), int(at_org<T>()));
		custom_layouted = 1; // ?
	}
	//template<class T>
	void clone(gk_coords& x)
	{
		org = x.org;
		dim = x.dim; // Force same width as x box
		custom_layouted = 1;
	}
};//__attribute__((packed));

struct gruik_obj : gk_coords
{
	//static std::vector<gruik_obj*> gruik_objs;
	static gruik_obj* focus_obj; // object that's in focus (the one that processed the latest event)
	static gruik_obj* locking_obj; // an object that locks the UI for itself
	static gruik_obj gruik_top; // an object that locks the UI for itself
	static gruik_obj dummy; // an disposable object that can be used during splitting operations
	static uint8_t active_layer;
	static lcd_drv* lcd;

	static void set_active_layer(uint8_t x) {gprintf("#VActive layer %Y", x); active_layer = x;}
	static void set_lcd_drv(lcd_drv* x) {gprintf("#VSetting LCD %Y", pconv(x)); lcd = x;}

	static style_t style;

	static void clear(uint16_t col = 0)
	{
		lcd->draw_rectangle(gruik_top.org, gruik_top.dim, col);
	}

	static void refresh()
	{
		clear();
		gruik_obj::clear();
		gruik_obj::gruik_top.draw_recurse(0); // redraw all: erases menu
	}

	// Processes inputs from TS (pressed state & coordinates)
	static void input_process(uint16_t x, uint16_t y, uint8_t pressed)
	{
		static xy posp(0,0); // future use (MOVED event)
		static uint8_t focussed = 0;
		static uint8_t locked = 0;
		static uint8_t pressedp = 0;
		xy pos(x, y);
		focussed = int(focus_obj != locking_obj);//gruik_top); // an object is selected
		locked = int(locking_obj != &gruik_top); // an object locks the GUI for itself

		bool click_event = (pressed != pressedp);
		if ( click_event or (focus_obj->event_mask & MOVED) )
		{

			gprintf("#b*********input process********* (%r) %r %r %r %r %r %r pos %",
					int(focussed), focus_obj->name, pconv(focus_obj), pconv(&gruik_top), int(focus_obj != &gruik_top), int(focussed), pos);

			if (pressed)
			{
				//gprintf("#Rpressed");
				if (focussed) // stg already onjoing
				{
					//gprintf("#Rmoved");
					if (focussed)
					{
						if (focus_obj->toggle_obj)
						{
							focus_obj->event_process(pos, RELEASED); // can only have moved. SEND DIFF POS HERE ?
							focus_obj = locking_obj;//&gruik_top; // release control from focussed object
						}
						else
							focus_obj->event_process(pos, MOVED); // can only have moved. SEND DIFF POS HERE ?
					}
				}
				else
				{
					gprintf("#Rsearch...");
					locking_obj->event_process(pos, PRESSED); // touch event; is normally top
					//gruik_top.event_process(pos, PRESSED); // touch event; is normally top
				}
				posp = pos;
			}
			else if (focussed)
			{
				if (not focus_obj->toggle_obj) // toggle objects keep focus until next click
				{
					gprintf("#V %Y losing Focus", focus_obj->name);
					focus_obj->event_process(pos, RELEASED); // release touch event
					focus_obj = locking_obj;
					//focus_obj = &gruik_top;
				}
			}
		}
		pressedp = pressed;
	}
	// default evt callback : propagates event to children

	gstring name;
	gruik_obj* parent;
	gruik_obj* last_child;
	gruik_obj* brother;
	uint8_t event_mask = PRESSED | RELEASED | ABORT; // At least !
	signal_link* signal = NULL;
	prout zob;
	bool valid;
	bool invisible = 0;
	bool inactive = 0;
	uint16_t layer;
	//uint8_t custom_layouted = 0; // not layouted by genric layouter (copy of parent layout)
	bool toggle_obj = 0; // when 1, objects keeps focus until next click
	bool top_layer = 0; // When 1, layer==active_layer is not checked. Set for all top_modules (children of gruik_top)
	//layout all children
	void layout_recurse()
	{
		zob.z = zob.z + 1;
		zob.b = not zob.b;
		gruik_obj* child = last_child;
		//gprintf("#Kin % last is % cur is % ", name, last_child->name, child->name);
		while ((child != this)) // is any child more specifically targeted ?
		{
			gprintf("#CLayouting %b auto layouted %r", child->name, int(child->custom_layouted), zob.z, zob.b);
			//gprintf("#CLayouting %b auto layouted %r", child->name, int(child->custom_layouted));
			if (not child->custom_layouted)
				child->init_layout();// reduce at least to parent size
			child->layout();//_callback();
			child->info();
			child->layout_recurse(); // might be a choice of children not to gain focus
			child = child->brother;
		}
	}

	static void start(uint16_t x=0)
	{
		if (lcd == NULL)
			gprintf("#RERROR! LCD is NOT initialized (use gruik_obj::set_lcd_drv(...)");
		gprintf("#BInitialization %b");
		//gprintf("#Kin % last is % cur is % ", name, last_child->name, child->name);
		gruik_top.layout_recurse();
		gruik_top.draw_recurse(0);

	}

	void set_invisible() {invisible = 1;}
	void set_visible() {invisible = 0;}
	void set_inactive() {inactive=1;}
	void set_active() {inactive=0;}
	void set_toggle(){toggle_obj=1;}

	// default layout: get parent layout
	virtual void layout()
	{
		gprintf("#UDefault layout %Y !! DOES NOTHiNG !!", name);
		//custom_layouted = 1;
		//auto_layouted = 1; // so that children won't break any custom layout
		//gk_coords::set(static_cast<gk_coords>(*parent));
	}

	virtual void init_layout()
	{
		gprintf("#UInit layout %Y", name);
		//auto_layouted = 1; // so that children won't break any custom layout
		gk_coords::set(static_cast<gk_coords>(*parent));
	}

	virtual void draw(uint16_t x)
	{
		draw_recurse(x);
	}

	//default: draw all children
	void draw_recurse(uint16_t x)
	{
		gruik_obj* child = last_child;
		gprintf("#CDrawing %b inv ? %r from parent %R", child->name, int(child->invisible),name);
		gprintf("#Kin % last is % cur is % ", name, last_child->name, child->name);
		while ((child != this)) // is any child more specifically targeted ?
		{
			if (not child->invisible)
			{
				gprintf("#U>>>>Goto to %Y invisible %Y", child->name, int(child->invisible));

				child->draw(x);
				gprintf("#U<<<<Back to %Y", name);

			}
			//child->draw_recurse(x);//_callback(x); // might be a choice of children not to gain focus
			child = child->brother;
		}
	}

	virtual void signal_event(uint8_t x)
	{
		gprintf("#TSignal event def callback in %R code %R", name, int(x));
	}

    virtual uint8_t event(xy pos, uint8_t event_i)
	{

    	//gruik_top.info();


		gprintf(">>>>>>>>>ED %r %r %r %r\n", int(pos.x), int(pos.y), int(event_i), name);
    	uint8_t event_response = NO_EVENT; // default: no child response
		//Loop over children. Break when one responds.
		gruik_obj* child = last_child;

		gprintf("#Kin % last is % cur is % ", name, last_child->name, child->name);
		if(1)
		while ((child != this) and (event_response == NO_EVENT)) // is any child more specifically targeted ?
		{
			gprintf("#mgo %R addr % from %R bro %R", child->name, to_hex(reinterpret_cast<std::uintptr_t>(child)), name, child->brother->name);
			//if (cnt>20) { gprintf("#RLOOP!!");exit(0);} else cnt++;

			event_response = child->event_process(pos, event_i); // might be a choice of children not to gain focus
			if (event_response)
				gprintf("#VResponse from %Y to %Y %Y", child->name, int(pos.x), int(pos.y));
			child = child->brother;
			gprintf("#Gwhile in %R next %G %R %m %m", name, child->name, int(child != this), pconv(this), pconv(child));

		}
		gprintf("#KLeaving % for %", name, child->name);
		return event_i == PRESSED ? MATCHED : NO_EVENT; // anyway at least this level has matched (mandatory, default is used by layout structures)
	}

	//
	uint8_t event_process(xy pos, uint8_t event_i)
	{
		gprintf("#TEP % pos % org % dim %", name, pos, org, dim);
		uint8_t match = coord_match(pos) and not inactive and ((layer == active_layer) or top_layer);
		if ((layer != active_layer) and not (top_layer) )
			gprintf("#Event rejected in %B because of layer %B != %B ", name, layer, active_layer);
		// process event: coords do matter in PRESS event only (for zone selection), ABORT & RELEASED apply to focused object
		if ( (match or (event_i != PRESSED) ) and (event_i and event_mask) )//
		{
			gprintf("#U***********************Process event %Y % % ***********************************", name, int(event_i), int(match));
			if (event_i == PRESSED)
			{
				focus_obj = this; // unless children gain focus
				gprintf("#V %Y gaining Focus", name);
			}
			//uint8_t response = MATCHED;
			return event(pos, event_i);//_callback(pos, event);
			//return event_default(pos, event);
		}
		else
		{
			gprintf("#rProcess event % % ", int(event_i), int(match));
			gprintf("#r % % % %", int(match), int(event_i != PRESSED), int((match or (event_i != PRESSED) )), int((event_i and event_mask)));

			return NO_EVENT;
		}
	}

	// called when this object is linked to a signal (via signal_link* signal)
	// send_event() method of signal_t looks for all linked gruik objects and calls this method
	void signal_process(uint8_t x)
	{
		signal_event(x);
		//gprintf("#TSignal event in %R code %R", name, int(x));
	}

	void draw_process(uint16_t x)
	{
		draw(x);//_callback(x);
	}

	void init()
	{
		gprintf("#Vinit callback of %Y", name);
		focus_obj = &gruik_top;// ready to detect any event
		last_child = this; // no children at first
		locking_obj = &gruik_top;
	}
	// for top level instance
	gruik_obj()
	{

		name = "top";
		init();
		gk_coords::set(xy(0, 0), xy(WIDTH, HEIGHT));
		//init_layout();
		layer = 0;
		brother = this;
		top_layer = 1;
	}

	// for all except of top
	gruik_obj(const combo_t& x)
	{
		name = x.name;
		gprintf("Constructing gruik object % \n", name);

		init();
		parent = x.parent;
		if (parent->last_child != parent) // not first child
			brother = parent->last_child;
		else
			brother = parent;
		parent->last_child = this;
		layer = parent->layer;
		init_layout();
		if (parent == &gruik_top)
			top_layer = 1;
		//gk_coords::set(static_cast<gk_coords>(*parent));
		//std::cerr << "NEW " << name << " parent " << parent->name << " parent last child " << parent->last_child->name << "\n";
		gprintf("#MNEW %R ADDR %R parent %R", name, to_hex(reinterpret_cast<std::uintptr_t>(this)), parent->name);
	}

	void set_flags(uint8_t x) {event_mask = x;}
	void add_flags(uint8_t x) {event_mask |= x;}
	void remove_flags(uint8_t x) {event_mask &= ~x;}
	void toggle_flags(uint8_t x) {event_mask ^= x;}
	void set_layer(uint8_t x) {layer = x;}

	template <class T1, class T2>
	uint8_t coord_match(T1 x_i, T2 y_i)
	{
		return ( (x_i >= org.x) && (x_i <= (org.x+dim.x-1))
				&& (y_i >= org.y) && (y_i <= (org.y+dim.y-1)) );
	}

	uint8_t coord_match(xy pos)
	{
		uint8_t mm = coord_match(pos.x, pos.y);
		//gprintf(">>CHK: % % % %<<", int(mm), org, dim, pos);
		return mm;
	}



	template<class T>
	void split(uint8_t ratio, gruik_obj& x, gruik_obj& y)
	{
		if (x.parent == y.parent) // otherwise splitting op is pointless
		{
			xy y_tmp_org, y_tmp_dim; //tmp vars in case x or y is current object (cutting cur obj to allocate space to another)
			xy new_dim = dim.scale<T>(ratio); // scale cur area wrt dimension (T = HOR or VERT)
			y_tmp_org = org.offset<T>(new_dim); // moves y origin to cur origin + size of x on T dimension
			y_tmp_dim = dim - (y_tmp_org - org); // this gives remaining space for y
			x.org = org; x.dim = new_dim; y.org = y_tmp_org; y.dim = y_tmp_dim;
		}
		else
		{
			gprintf("#RCan't split % % into % and % (whose  parents are % %) % %",
							name, pconv(parent), x.name, y.name, (x.parent->name), y.parent->name, pconv(x.parent), pconv(y.parent));

		}
		x.custom_layouted = 1; y.custom_layouted = 1;
	}


	// Locate obj wrt parent obj.map<VERT
	template<class T>
	void map(uint8_t ratio)
	{
		org = parent->org;
		dim = parent->dim;
		xy new_dim = dim.scale< DIM<T> >(ratio); // scale cur area wrt dimension (T = HOR or VERT)
		if (at_org<T>())
		{
			dim = new_dim;
		}
		else
		{
			org = org.offset< DIM<T> >(new_dim);
			dim = dim - new_dim.sel_dim< DIM<T> >();
		}
		gprintf("#UMAP % % vert % atorg %", org, dim, int(is_vert< DIM<T> >()), int(at_org<T>()));
		custom_layouted = 1; // ?
	}

#if 0
	// Locate obj wrt to itselfr
	template<class T>
	void remap(uint8_t ratio)
	{
		xy new_dim = dim.scale< DIM<T> >(ratio); // scale cur area wrt dimension (T = HOR or VERT)
		if (at_org<T>())
		{
			dim = new_dim;
		}
		else
		{
			org = org.offset< DIM<T> >(new_dim);
			dim = dim - new_dim.sel_dim< DIM<T> >();
		}
		gprintf("#UMAP % % vert % atorg %", org, dim, int(is_vert< DIM<T> >()), int(at_org<T>()));
		custom_layouted = 1; // ?
	}

	template<class T>
	void crop(uint8_t ratio)
	{
		xy new_dim = dim.scale< DIM<T> >(ratio); // scale cur area wrt dimension (T = HOR or VERT)
		if (at_org<T>())
		{
			dim = new_dim;
		}
		else
		{
			org = org.offset< DIM<T> >(new_dim>>1);
			dim = dim - new_dim.sel_dim< DIM<T> >();
		}
		gprintf("#UCROP % % vert % atorg %", org, dim, int(is_vert< DIM<T> >()), int(at_org<T>()));
		custom_layouted = 1; // ?
	}

#endif
	template<class T, class T2, class T3>
	void split_H(T ratio, T2& x, T3& y)
	{
		split<HOR>(ratio, x, y);
	}

	template<class T, class T2, class T3>
	void split_V(T ratio, T2& x, T3& y)
	{
		split<VERT>(ratio, x, y);
	}

	//template<class T>
	void locate_below(gruik_obj& x, uint8_t spacer = 0)
	{
		org = x.org.offset<VERT>(x.dim, spacer);
		//dim.x = x.dim.x; // Force same width as x box
		custom_layouted = 1;
	}

	//template<class T>
	void locate_right_to(gruik_obj& x, uint8_t spacer = 0)
	{
		org = x.org.offset<VERT>(x.dim,spacer);
		//dim.y = x.dim.y; // Force same height as x box
		custom_layouted = 1;
	}
	//template<class T>
	void clone_below(gruik_obj& x, uint8_t spacer = 0)
	{
		org = x.org.offset<VERT>(x.dim, spacer);
		dim.x = x.dim.x; // Force same width as x box
		custom_layouted = 1;
	}

	//template<class T>
	void clone_right_to(gruik_obj& x, uint8_t spacer = 0)
	{
		org = x.org.offset<VERT>(x.dim,spacer);
		dim.y = x.dim.y; // Force same height as x box
		custom_layouted = 1;
	}

	// extend frame: no parameter means all available apace
	template<class T> void extend(uint16_t x = 0)
	{
		if (x != 0)
			dim.set_dim<T>(x);
		else
			dim.set<T>(parent->org.get<T>() + parent->dim.get<T>() - org.get<T>());
	}

	// extend frame: no parameter means all available apace
	template<class T> void extend(gruik_obj& x)
	{
			dim.set<T>(x.org.get<T>() + parent->dim.get<T>() - org.get<T>());
	}

	// checks whether objects lies inside parent frame.
	// replace if not, return error code
	uint8_t check_layout()
	{
		uint8_t status = 0;
		xy br = org + dim;
		if ( (br.x > parent->dim.x) )
		{
			status |= RIGHT_OVF;
			org.x = parent->dim.x - dim.x;
		}
		if ( (br.y > gruik_obj::gruik_top.dim.y) )
		{
			status |= BOTTOM_OVF;
			org.y = parent->dim.y - dim.y;
		}
		return status;
	}
	void info()
	{
		gprintf("#V % : %Y %Y lay % ptr %", name, org, dim, int(layer), pconv(this));
	}

	gruik_obj* get_gruik_ptr(){return this;} // to communicate to all derived objects

};
//#pragma pack(pop)


//#define GRUIK(name, parent) gruik_obj name(#name, parent)
#define GRUIK(name) gruik_obj name = make_combo_t(#name, *this)
//#define GKMODULE(name) // name = make_combo_t(#name, *this)
#define GKMODULE(name) struct name : public gruik_obj// name = make_combo_t(#name, *this)
#define GKCTOR(name) name(combo_t x) : gruik_obj(x)
//#define GKTYPE(name, base_type) typedef gk_type_t<base_type> name
#define GKCOMP(name, type) gk_type_t<type> name = make_combo_t(#name, *this)
#define TOP_MODULE(name, type) type name = make_combo_t(#name, gruik_obj::gruik_top)
#define MODULE_INST(name, type) type name = make_combo_t(#name, *this)
#define SUBMODULE_INST(name, type, parent) type name = make_combo_t(#name, parent)
#define MODULE_TOP(name, type) type name = make_combo_t(#name, gruik_obj::gruik_top)
#define LINK(a,b) signal_link link_##a_##b = make_combo2_t(a,b)
#define LINK_REMOTE(a,c,b) signal_link link_##a_##c_##b = make_combo2_t(a,c.b)
#define SIGNAL(name, type) signal_t<type> name = #name
#define SET_CALLBACK(a,b, c) a.callback = std::bind(&b::c, this)
