// g++ sdl.cpp --std=c++11 -g -o pcgruik -lSDL2 -lm


#include <linux/input.h>

#define PC_GRUIK
#include "mbprt.h"
#include <SDL2/SDL.h>
#include "mouse.h"
#include "monitor.h"
#include <iostream>
#include <sstream>
#include "unistd.h"

#define USE_MONITOR 1
#define USE_MOUSE 1

#if 1
#define WIDTH 480
#define HEIGHT 272
//#define HEIGHT 250 

#define MAGNIFY 3
#else
#define WIDTH 272
#define HEIGHT 480

#define MAGNIFY 2
#endif
#define MONITOR_HOR_RES WIDTH*MAGNIFY
#define MONITOR_VER_RES HEIGHT*MAGNIFY

#include "gruik.h"
#include "gruik_signals.h"
#include "gruik_components.h"
#include "gruik_static.h"

#include "gruik_tp.cpp"
//#include "gruik_nana.cpp"


#define SDL_REFR_PERIOD	    100	/*ms*/
#define PR(a) std::cerr << a;


static SDL_Window * window;
static SDL_Renderer * renderer;
static SDL_Texture * texture;
static uint32_t tft_fb[MONITOR_HOR_RES * MONITOR_VER_RES];
static volatile bool sdl_inited = false;
static volatile bool sdl_refr_qry = false;
static volatile bool sdl_quit_qry = false;

static bool left_button_down = false;
static int16_t last_x = 0;
static int16_t last_y = 0;


int quit_filter (void *userdata, SDL_Event * event);
void mouse_handler(SDL_Event *event);

static int sdl_refr(void * param)
{
    (void)param;
	gprintf("#GSDL_refr");
	/*Initialize the SDL*/
    SDL_Init(SDL_INIT_VIDEO);

    SDL_SetEventFilter(quit_filter, NULL);

	window = SDL_CreateWindow("GRuiiK toolkit running on PC TFT simulator",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		MONITOR_HOR_RES, MONITOR_VER_RES, 0);       /*last param. SDL_WINDOW_BORDERLESS to hide borders*/

	renderer = SDL_CreateRenderer(window, -1, 0);
	texture = SDL_CreateTexture(renderer,
		SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, MONITOR_HOR_RES, MONITOR_VER_RES);

	/*Initialize the frame buffer to gray (77 is an empirical value) */
	memset(tft_fb, 77, MONITOR_HOR_RES * MONITOR_VER_RES * sizeof(uint32_t));
	SDL_UpdateTexture(texture, NULL, tft_fb, MONITOR_HOR_RES * sizeof(uint32_t));
	sdl_refr_qry = true;
	sdl_inited = true;

	/*Run until quit event not arrives*/
	while(sdl_quit_qry == false) 
		{
		//PR("R")
		/*Refresh handling*/
		if(sdl_refr_qry != false) {
            sdl_refr_qry = false;
            SDL_UpdateTexture(texture, NULL, tft_fb, MONITOR_HOR_RES * sizeof(uint32_t));
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, texture, NULL, NULL);
            SDL_RenderPresent(renderer);
		}

	    SDL_Event event;
	    while(SDL_PollEvent(&event)) {
			PR("!")
#if USE_MOUSE != 0
            mouse_handler(&event);
#endif

#if USE_KEYBOARD
            keyboard_handler(&event);
#endif
	    }
	//gprintf("<zz");

		/*Sleep some time*/
		SDL_Delay(SDL_REFR_PERIOD);
			//gprintf("zz>");

	}

	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	exit(0);

	return 0;
}
int quit_filter (void *userdata, SDL_Event * event)
{
    (void)userdata;

	if(event->type == SDL_QUIT) {
		sdl_quit_qry = true;
	}

	return 1;
}

void monitor_init(void)
{
	SDL_CreateThread(sdl_refr, "sdl_refr", NULL);

	while(sdl_inited == false); /*Wait until 'sdl_refr' initializes the SDL*/
	gprintf("#Mmonitor init");
}

void mouse_handler(SDL_Event *event)
{
	switch (event->type) {
        case SDL_MOUSEBUTTONUP:
            if (event->button.button == SDL_BUTTON_LEFT)
                left_button_down = false;
             //gprintf("@k<%g|%g>", last_x, last_y);

            break;
        case SDL_MOUSEBUTTONDOWN:
            if (event->button.button == SDL_BUTTON_LEFT) {
                left_button_down = true;
                last_x = event->motion.x / MAGNIFY;
                last_y = event->motion.y / MAGNIFY;
              //gprintf("@k<%m|%m>", last_x, last_y);
			//std::cerr << "< BD " << last_x << "," << last_y << ">";

            }
            break;
        case SDL_MOUSEMOTION:
            last_x = event->motion.x / MAGNIFY;
            last_y = event->motion.y / MAGNIFY;
			//td::cerr << "<" << last_x << "," << last_y << ">";
            //gprintf("@k<%r|%r>", last_x, last_y);
            break;
        case SDL_FINGERUP:
            if (1)
                left_button_down = false;
             //gprintf("@k<%g|%g>", last_x, last_y);
			//std::cerr << "< FU " << last_x << "," << last_y << ">";

            break;
            
        case SDL_FINGERDOWN:
            if (1) {
                left_button_down = true;
                last_x = event->tfinger.x / MAGNIFY;
                last_y = event->tfinger.y / MAGNIFY;
              //gprintf("@k<%m|%m>", last_x, last_y);
			//std::cerr << "< FD  " << last_x << "," << last_y << ">";

            }
            break;
        case SDL_FINGERMOTION:
            last_x = event->tfinger.x / MAGNIFY;
            last_y = event->tfinger.y / MAGNIFY;
			//std::cerr << "< FM  " << last_x << "," << last_y << ">";
            //gprintf("@k<%r|%r>", last_x, last_y);
            break;
    }


}

lcd_drv display(tft_fb);
//int SDL_WaitEvent(SDL_Event* event);
SDL_Event* event1;
int main()
{
	std::cerr << "Begin\n";
	//lcd_drv display(tft_f);
	monitor_init();
	//gruik_obj::gruik_top.set(xy(0, 0), xy(HEIGHT, WIDTH));
	//gruik_obj::gruik_top.set(xy(0, 0), xy(HEIGHT, WIDTH));

	gruik_obj::set_lcd_drv(&display);
	gruik_obj::start();
	int zz = 0;
	while (1)
	{
		//SDL_WaitEvent(event1);
		 //gprintf(".");
		//SDL_Delay(.001);
		usleep(100);
		gruik_obj::input_process(last_x,last_y,left_button_down);
		char toto[100];
		sprintf(toto, "Counter = %d\n", zz);
		zz++;
		char titi[2];
		//titi[0] = 65 + (zz&0x1f);titi[1] = '\0';
		//titi[0] = 65 + (gktop.term.get_read_line_rows());titi[1] = '\0';
		//gprintf("#R  %   z %B     ", gktop.term.get_read_line_rows(), zz&0x1FF);
		if (gktop.btn3.toggle_value)
#if 1		
		if (((zz & 0x1ff) == 0x0)) gktop.term.print_line(toto);
			//else if ( (zz&0x3) == 0) gktop.term.print_line(titi);
#else			
		if (((zz & 0x1ff) == 0x0)) gktop.calc_app.term2.print_line(toto);
			else if ( (zz&0x3) == 0) gktop.calc_app.term2.print_line(titi);
#endif			
		sdl_refr_qry = true;
		bool changed = 0;
		
		if (signal3.is_set())
		{
			gprintf("#UGenerating signal");
			signal3.reset_event();
			for (int i = 0; i < 16384; i++)
				//signal2.set_data(ADDRESSED | i, i&255);
				signal2.set_data(ADDRESSED | i, 
				uint32_t(((16384-i)*sin(double(i)*6.28/10*(1-double(i)/20000))*sin(double(i)/160))));
			signal2.set_event(0); // launch draw	
		}
//		for (int i = 0; i < 480*272;i++)
//			changed |= tft_fb[i] != 0x4D4D4D4D;
		
		//gprintf("%",int(changed));
			
	}
}
