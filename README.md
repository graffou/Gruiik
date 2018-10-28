# Gruiik
GRaffic User Interface Intuitive Toolkit

Gruiik is a attempt at building a simple C++11 graphic toolkit for embedded systems.
(initial target is stm32F746discovery kit) 
It surely lacks of optimization and is still a bit messy :)

Thanks to Gábor Kiss-Vámosi for the littlevgl environment.
Thanks to Jardel Weyrich for the calculator software.
I had to modify it for floating point calculation, the original code should be found here:
https://gist.github.com/jweyrich/4bd1d4556069f3d49d73

To build the stm32F746discovery demo:
-------------------------------------
edit makefile to set the right GCC_BASE
GCC_BASE = ~/.platformio/packages/toolchain-gccarmnoneeabi/

cd Release
make

drag and drop gruiik_stm32f7.bin to the disco mass storage device.
To see the curve demo, select menu0/go curve and press btn2 to generate the curve data.
Sliding on top, bottom, left and right edges of the curve select zoom and position.

Go term opens the terminal. Pressing btn3 activates a counter inside the terminal.

Menu1 provides a calculator and a keyboard demo.
When CALC_SOLO and VERTICAL_LAYOUT are defined in main.cpp, the app reduces to the calculator (in portrait mode).

To build the PC demo:
---------------------

cd gruiik

sudo apt-get install libsdl2-2.0-0 (or whatever version)
sudo apt-get install libsdl2-dev

g++ sdl.cpp --std=c++11 -g -o pcgruik -lSDL2 -lm
./pcgruik


