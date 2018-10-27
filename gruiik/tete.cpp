
#include <linux/input.h>

#define PC_GRUIK
#include "mbprt.h"
#include <SDL2/SDL.h>
#include "mouse.h"
#include "monitor.h"
#include <iostream>

#define USE_MONITOR 1
#define USE_MOUSE 1

#define WIDTH 480
#define HEIGHT 272 

#define MAGNIFY 3

#define MONITOR_HOR_RES WIDTH*MAGNIFY
#define MONITOR_VER_RES HEIGHT*MAGNIFY

#include "gruik.h"
#include "gruik_signals.h"
#include "gruik_components.h"
#include "gruik_static.h"

#include "gruik_tp.cpp"
#include<chrono>
#include<thread>

#define SDL_REFR_PERIOD	    50	/*ms*/
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

MODULE_TOP(con, gk_console);
const char* roro = "On y va\nSalut les poteaux\nYo !!!\n Prout\nAllez vous faire\nMangez des pommes\n";
int main()
{
	int i = 0;
	while (roro[i] != '\0')
	{
		con.print_char(roro[i]);
		i++;
	}
	char buf[100];
	for (i = 1;i<22;i++)
	{
		if ((i & 7) == 0)
			sprintf(buf, "la on a i = %d\n", i);
		else
			sprintf(buf, "la on a i = %d", i);
			
		con.print_line(buf);
		con.print_buffer();
		if (i == 4)
		{
			con.buf.cur_char = 36;
			for (int j = 0;j<22;j++)
				con.print_char(7);
		}
		//con.buf.cur_char=0;
		gprintf("\n");
		std::this_thread::sleep_for(std::chrono::microseconds(500000));
	}
}	
