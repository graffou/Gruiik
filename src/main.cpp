
#include "stm32f7xx.h"
#include "stm32746g_discovery.h"

#if 1
#include "mbed.h" // fake one
#include "LCD_DISCO_F746NG.h"
#include "TS_DISCO_F746NG.h"
#endif


//Serial pc(USBTX, USBRX); //pc.baud(115200);
#include "mbprt.h"

#define DISCO 1

//#define VERTICAL_LAYOUT

#ifndef VERTICAL_LAYOUT
  #define WIDTH 480
  #define HEIGHT 272
#else
  #define WIDTH 272
  #define HEIGHT 480
#endif
static void SystemClock_Config(void);

 
//LCD_DISCO_F746NG lcd;
#if 1
//#define DEBUG
//#define CALC_SOLO
#include "gruik.h"
#include "gruik_signals.h"
#include "gruik_components.h"
#include "gruik_static.h"

#endif

//DigitalOut led1(LED1);



#if 1

GKMODULE(top_bar_t)
{
	MODULE_INST(powered_by_GruiiK, gk_button);
	MODULE_INST(menu0, gk_menu);
	MODULE_INST(menu1, gk_menu);
	MODULE_INST(go_term, gk_menu_entry);
	MODULE_INST(go_curve, gk_menu_entry);
	MODULE_INST(go_nowhere, gk_menu_entry);
	MODULE_INST(quit, gk_menu_entry);
	MODULE_INST(keyboard, gk_menu_entry);
	MODULE_INST(calc, gk_menu_entry);
	MODULE_INST(entry21, gk_menu_entry);
	MODULE_INST(entry31, gk_menu_entry);
	GKCTOR(top_bar_t)
	{
		powered_by_GruiiK.menu_button = 1; // no frame
		powered_by_GruiiK.caption.set_font_div(1);
		menu0.add_entry(go_term);
		menu0.add_entry(go_curve);
		//menu0.add_entry(entry2);
		menu0.add_entry(go_nowhere);
		menu0.add_entry(quit);
		menu1.add_entry(keyboard);
		menu1.add_entry(calc);
		menu1.add_entry(entry21);
		menu1.add_entry(entry31);
	}

	void layout()
	{
		split<HOR>(160, menu0, powered_by_GruiiK);
		menu0.split<HOR>(128, menu0, menu1);
	}
};


//signal_t<single_data> signal0 = "signal0";
SIGNAL(signal0, single_data);
SIGNAL(signal1, single_data);
SIGNAL(signal3, single_data);
SIGNAL(signal2, vector_data<16384>);
SIGNAL(box_signal, single_data);

GKMODULE(top_if_t)
{
	GKCTOR(top_if_t)
	{
		term.set_invisible();
		term.set_inactive();
		kb0.set_inactive();
		kb0.set_invisible();
		calc_app.set_inactive();
		calc_app.set_invisible();
		popup.set_text("\n\nGRaffic User Interface \nIntuitive Kit \n\nGraffou 2018/Make");
		popup.msg.set_font_div(0);
	}

	// frames and modules
	// first components that are supposed to be on top of others
	MODULE_INST(kb0, kb_app_t);
	MODULE_INST(calc_app, calc_app_t);

	//MODULE_INST(term, gk_text_box);
	MODULE_INST(term, gk_console);
	MODULE_INST(curve, gk_curve);
	//MODULE_INST(btn0, gk_button);
	MODULE_INST(btn0, gk_led);
	MODULE_INST(titi, gk_frame);
	MODULE_INST(btn1, gk_button);
	MODULE_INST(btn2, gk_button);
	MODULE_INST(btn3, gk_button);
	MODULE_INST(btn4, gk_button);
	MODULE_INST(led0, gk_led);
	MODULE_INST(sld0, gk_slider<VERT>);
	MODULE_INST(sld1, gk_slider<HOR>);
	MODULE_INST(top_bar, top_bar_t);
	MODULE_INST(dummy, gk_frame);
	MODULE_INST(popup, gk_msg);

	LINK(box_signal, popup);
	LINK_REMOTE(box_signal, top_bar, powered_by_GruiiK);


	// bind modules to signals
	LINK(signal0, btn3);
	LINK(signal0, led0);
	LINK(signal1, sld0);
	LINK(signal1, sld1);
	LINK(signal1, btn0);
	LINK(signal3, btn2);
	LINK(signal2, curve);

	// Custom layout + print a fresh text
	void layout()
	{
		sld1.invisible = 1;
		//split<VERT>(128, top_bar, kb0);
		split<VERT>(25, top_bar, term);
		//split<VERT>(220, tata, sld1);
		term.split<VERT>(240, term, sld1);

		term.split<HOR>(192, term, titi);
		curve.clone(term);
		titi.split<HOR>(200, titi, sld0);
		titi.split<VERT>(128, btn0, btn2);
		btn0.split<VERT>(128, btn0, btn1);
		btn2.split<VERT>(128, dummy, btn2);
		btn2.split<VERT>(128, btn2, btn3);
		btn2.split<HOR>(128, btn2, btn4);
		btn3.split<HOR>(128, btn3, led0);
		btn3.set_toggle(1);
		btn1.set_toggle(1);

		SET_CALLBACK(top_bar.quit, top_if_t, quit);
		SET_CALLBACK(top_bar.go_term, top_if_t, go_term);
		SET_CALLBACK(top_bar.go_curve, top_if_t, go_curve);
		SET_CALLBACK(top_bar.go_nowhere, top_if_t, go_nowhere);
		SET_CALLBACK(top_bar.keyboard, top_if_t, keyboard);
		SET_CALLBACK(top_bar.calc, top_if_t, calc);



		//term.set_font_div(1);
		term.print_line("Gruiik is a simple C++ 11 toolkit for embedded devices!\n");
		term.print_line("Hope you enjoy it!\n");
	}

	void quit()
	{
		gprintf("Adios\n");
		//exit(0);
	}

	 void go_curve()
	{
		gprintf("curve view\n");
		term.set_inactive();
		term.set_invisible();
		curve.set_active();
		curve.set_visible();
	}

    void go_term()
	{
		gprintf("text view\n");
		//kb0.term2.copy_buffer(term);

		curve.set_inactive();
		curve.set_invisible();
		term.set_active();
		term.set_visible();
		term.draw(0);

	}

	void go_nowhere()
	{
		gprintf("text view\n");
		curve.set_inactive();
		curve.set_invisible();
		term.set_inactive();
		term.set_invisible();
		gruik_obj::refresh();
	}

	void keyboard()
	{

			gruik_obj::focus_obj = &kb0;
			gruik_obj::locking_obj = &kb0;
			kb0.set_active();
			kb0.set_visible();
			kb0.draw(0);
	}
	void calc()
	{

			gruik_obj::focus_obj = &calc_app;
			gruik_obj::locking_obj = &calc_app;
			calc_app.set_active();
			calc_app.set_visible();
			calc_app.draw(0);
	}
};




//MODULE_TOP(popo, tuto);
#ifndef CALC_SOLO
MODULE_TOP(gktop, top_if_t);
#else
MODULE_TOP(gktop, calc_app_t);
#endif




 TS_StateTypeDef TS_State;
uint16_t x, y;
uint8_t touched = 0;
uint8_t toggle = 0;
int main()
{

    HAL_Init();

    /* Configure the system clock to 216 MHz */
    SystemClock_Config();
    BSP_LED_Init(LED1);
    for (int i = 0; i < 8; i++) {
        BSP_LED_Toggle(LED1);
        HAL_Delay(50);
    }
    
    LCD_DISCO_F746NG lcd;  
    gruik_obj::lcd->set_lcd(&lcd); 
    TS_DISCO_F746NG ts;   
	//pc.baud(115200*2);
	#if 1
#ifndef VERTICAL_LAYOUT
	lcd.SetXSize(WIDTH);
	lcd.SetYSize(HEIGHT);
#else
	lcd.SetYSize(WIDTH);
	lcd.SetXSize(HEIGHT);
#endif
	lcd.Init();
	lcd.DisplayOn();
	//BSP_LCD_Init();
    //BSP_LCD_LayerDefaultInit(0, LCD_FB_START_ADDRESS);
    //BSP_TS_Init(BSP_LCD_GetXSize(), BSP_LCD_GetYSize());
    //lcd.SetLayerWindow(1, 180, 110, 120, 53);

    lcd.SetLayerWindow(0, 0, 0, 480, 272);
    lcd.SetLayerWindow(1, 0, 0, 1, 1); // ??? nodisplay otherwise

    //lcd.SetLayerWindow(1, 0, 0, 0, 0);


	lcd.SelectLayer(0);

	gruik_obj::start();
	gprintf("#**********************************");

    //led1 = 1;
    int zz = 0;
    BSP_LED_Init(LED1);
    for (int i = 0; i < 8; i++) {
        BSP_LED_Toggle(LED1);
        HAL_Delay(50);
    }
#endif	//gruik_obj::start();

	while(1)
	    {
	      //wait(.05); // that was mbed
	      HAL_Delay(5);
	        ts.GetState(&TS_State);
	        if (TS_State.touchDetected) {
#ifdef VERTICAL_LAYOUT
		        gprintf("#Btouched % % %", int(TS_State.touchX[0]), int(TS_State.touchY[0]), int(touched));

	        	y = TS_State.touchX[0];
	            x = WIDTH-1-TS_State.touchY[0];
#else
	        	x = TS_State.touchX[0];
	            y = TS_State.touchY[0];
#endif
	            touched = 1;
	        	//lcd.SelectLayer(0);

	        }
	        else touched = 0;
	        gprintf("#rtouched % % %", int(x), int(y), int(touched));
			gruik_obj::input_process(x,y,touched);

			char toto[100];
			sprintf(toto, "Counter = %d\n", zz);
			zz++;
#ifndef CALC_SOLO
			if (gktop.btn3.toggle_value)
				if (((zz & 0xf) == 0x0)) gktop.term.print_line(toto);

			if (touched)
	        	toggle = !toggle;
	        //led1 = toggle;
	        BSP_LED_Toggle(LED1);
			if (signal3.is_set())
			{
				gprintf("#UGenerating signal");
				signal3.reset_event();
				for (int i = 0; i < 16384; i++)
					//signal2.set_data(ADDRESSED | i, i&255);
				signal2.set_data(ADDRESSED | i, int32_t((16384-i)*sinf(float(i)*6.28/10*(1-float(i)/20000))*sinf(float(i)/160)));
				signal2.set_event(0); // launch draw
			}
#endif
	    }
}
#else
 int main()
 {
 
     //HAL_Init();
    //SystemClock_Config();

 
     BSP_LED_Init(LED1);
    for (int i = 0; i < 8; i++) {
        BSP_LED_Toggle(LED1);
        HAL_Delay(50);
    }    
    
    HAL_Delay(1500);    
    BSP_LED_Init(LED1);
    for (int i = 0; i < 8; i++) {
        BSP_LED_Toggle(LED1);
        HAL_Delay(50);
    }
 
}
#endif



static void SystemClock_Config(void)
{
    RCC_ClkInitTypeDef RCC_ClkInitStruct;
    RCC_OscInitTypeDef RCC_OscInitStruct;

    /* Enable Power Control clock */
    __HAL_RCC_PWR_CLK_ENABLE();

    /* The voltage scaling allows optimizing the power consumption when the device is
     clocked below the maximum system frequency, to update the voltage scaling value
     regarding system frequency refer to product datasheet.  */
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /*##-1- System Clock Configuration #########################################*/
    /* Enable HSE Oscillator and activate PLL with HSE as source */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 25;
    RCC_OscInitStruct.PLL.PLLN = 400;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 8;
    HAL_RCC_OscConfig(&RCC_OscInitStruct);

    /* Activate the Over-Drive mode */
    HAL_PWREx_EnableOverDrive();

    /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
     clocks dividers */
    RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_6);
}



