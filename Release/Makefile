######################################################################################
# GNU GCC ARM Embeded Toolchain base directories and binaries
######################################################################################
#GCC_BASE = E:/Toolchains/GNU_GCC_ARM/4.7/
#GCC_BIN  = $(GCC_BASE)bin/
GCC_BASE = ~/.platformio/packages/toolchain-gccarmnoneeabi/
GCC_BIN  = $(GCC_BASE)bin/
GCC_LIB  = $(GCC_BASE)arm-none-eabi/lib/
GCC_INC  = $(GCC_BASE)arm-none-eabi/include/
AS       = $(GCC_BIN)arm-none-eabi-as
CC       = $(GCC_BIN)arm-none-eabi-gcc
CPP      = $(GCC_BIN)arm-none-eabi-g++
LD       = $(GCC_BIN)arm-none-eabi-gcc
OBJCOPY  = $(GCC_BIN)arm-none-eabi-objcopy


######################################################################################
# Custom options for cortex-m and cortex-r processors
######################################################################################
CORTEX_M0PLUS_CC_FLAGS  = -mthumb -mcpu=cortex-m0plus
CORTEX_M0PLUS_LIB_PATH  = $(GCC_LIB)armv6-m
CORTEX_M0_CC_FLAGS      = -mthumb -mcpu=cortex-m0
CORTEX_M0_LIB_PATH      = $(GCC_LIB)armv6-m
CORTEX_M1_CC_FLAGS      = -mthumb -mcpu=cortex-m1
CORTEX_M1_LIB_PATH      = $(GCC_LIB)armv6-m
CORTEX_M3_CC_FLAGS      = -mthumb -mcpu=cortex-m3
CORTEX_M3_LIB_PATH      = $(GCC_LIB)armv7-m
CORTEX_M4_NOFP_CC_FLAGS = -mthumb -mcpu=cortex-m4
CORTEX_M4_NOFP_LIB_PATH = $(GCC_LIB)armv7e-m
CORTEX_M4_SWFP_CC_FLAGS = -mthumb -mcpu=cortex-m4 -mfloat-abi=softfp -mfpu=fpv4-sp-d16
CORTEX_M4_SWFP_LIB_PATH = $(GCC_LIB)armv7e-m/softfp
CORTEX_M4_HWFP_CC_FLAGS = -mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16
CORTEX_M7_HWFP_LIB_PATH = $(GCC_LIB)armv7e-m/fpu
CORTEX_M7_HWFP_CC_FLAGS = -mthumb -mcpu=cortex-m7 -mfloat-abi=hard -mfpu=fpv4-sp-d16
CORTEX_M4_HWFP_LIB_PATH = $(GCC_LIB)armv7e-m/fpu
CORTEX_R4_NOFP_CC_FLAGS = -mthumb -march=armv7-r
CORTEX_R4_NOFP_LIB_PATH = $(GCC_LIB)armv7-r/thumb
CORTEX_R4_SWFP_CC_FLAGS = -mthumb -march=armv7-r -mfloat-abi=softfp -mfloat-abi=softfp -mfpu=vfpv3-d16
CORTEX_R4_SWFP_LIB_PATH = $(GCC_LIB)armv7-r/thumb/softfp
CORTEX_R4_HWFP_CC_FLAGS = -mthumb -march=armv7-r -mfloat-abi=softfp -mfloat-abi=hard -mfpu=vfpv3-d16
CORTEX_R4_HWFP_LIB_PATH = $(GCC_LIB)armv7-r/thumb/fpu
CORTEX_R5_NOFP_CC_FLAGS = -mthumb -march=armv7-r
CORTEX_R5_NOFP_LIB_PATH = $(GCC_LIB)armv7-r/thumb
CORTEX_R5_SWFP_CC_FLAGS = -mthumb -march=armv7-r -mfloat-abi=softfp -mfloat-abi=softfp -mfpu=vfpv3-d16
CORTEX_R5_SWFP_LIB_PATH = $(GCC_LIB)armv7-r/thumb/softfp
CORTEX_R5_HWFP_CC_FLAGS = -mthumb -march=armv7-r -mfloat-abi=softfp -mfloat-abi=hard -mfpu=vfpv3-d16
CORTEX_R5_HWFP_LIB_PATH = $(GCC_LIB)armv7-r/thumb/fpu


######################################################################################
# Main makefile project configuration
#    PROJECT      = <name of the target to be built>
#    MCU_CC_FLAGS = <one of the CC_FLAGS above>
#    MCU_LIB_PATH = <one of the LIB_PATH above>
#    OPTIMIZE_FOR = < SIZE or nothing >
#    DEBUG_LEVEL  = < -g compiler option or nothing >
#    OPTIM_LEVEL  = < -O compiler option or nothing >
######################################################################################
PROJECT           = gruiik_stm32f7
MCU_CC_FLAGS      = $(CORTEX_M7_HWFP_CC_FLAGS)
MCU_LIB_PATH      = $(CORTEX_M7_HWFP_LIB_PATH)
OPTIMIZE_FOR      =
DEBUG_LEVEL       =
OPTIM_LEVEL       =
LINKER_SCRIPT     = ../LinkerScript.ld
PROJECT_LIB_PATHS = -L.
PROJECT_LIBRARIES =
PROJECT_SYMBOLS   = -DSTM32F746NGx \
-DSTM32746G_DISCO \
-DSTM32F7 \
-DSTM32 \
-DUSE_HAL_DRIVER \
-DSTM32F746xx \
-DTARGET_DISCO_F746NG 

######################################################################################
# Sources
######################################################################################
PROJECT_SRCS      = \
../startup/startup_stm32f746xx.s \
../src/stm32f7xx_it.c \
../src/syscalls.c \
../src/system_stm32f7xx.c

#PROJECT_SRCS     += \
#../hal_stm_lvgl/tft/tft.c \
#../hal_stm_lvgl/touchpad/touchpad.c \

PROJECT_SRCS     += \
../Utilities/STM32746G-Discovery/stm32746g_discovery.c        \
../Utilities/STM32746G-Discovery/stm32746g_discovery_ts.c     \
../Utilities/STM32746G-Discovery/stm32746g_discovery_sdram.c  \
../Utilities/Components/ft5336/ft5336.c \
../MBED/stm32746g_discovery_lcd.c    \
# ../Utilities/STM32746G-Discovery/stm32746g_discovery_eeprom.c \
# ../Utilities/STM32746G-Discovery/stm32746g_discovery_camera.c \
# ../Utilities/STM32746G-Discovery/stm32746g_discovery_sd.c     \
# ../Utilities/STM32746G-Discovery/stm32746g_discovery_qspi.c   \
# ../Utilities/STM32746G-Discovery/stm32746g_discovery_audio.c  \

PROJECT_SRCS     += \
../HAL_Driver/Src/stm32f7xx_hal.c                              \
../HAL_Driver/Src/stm32f7xx_hal_cortex.c                       \
../HAL_Driver/Src/stm32f7xx_hal_dfsdm.c                        \
../HAL_Driver/Src/stm32f7xx_hal_dma2d.c                        \
../HAL_Driver/Src/stm32f7xx_hal_dma.c                          \
../HAL_Driver/Src/stm32f7xx_hal_dma_ex.c                       \
../HAL_Driver/Src/stm32f7xx_hal_flash.c                        \
../HAL_Driver/Src/stm32f7xx_hal_flash_ex.c                     \
../HAL_Driver/Src/stm32f7xx_hal_gpio.c                         \
../HAL_Driver/Src/stm32f7xx_hal_hcd.c                          \
../HAL_Driver/Src/stm32f7xx_hal_i2c.c                          \
../HAL_Driver/Src/stm32f7xx_hal_i2c_ex.c                       \
../HAL_Driver/Src/stm32f7xx_hal_i2s.c                          \
../HAL_Driver/Src/stm32f7xx_hal_jpeg.c                         \
../HAL_Driver/Src/stm32f7xx_hal_ltdc.c                         \
../HAL_Driver/Src/stm32f7xx_hal_ltdc_ex.c                      \
../HAL_Driver/Src/stm32f7xx_hal_mmc.c                          \
../HAL_Driver/Src/stm32f7xx_hal_msp_template.c                 \
../HAL_Driver/Src/stm32f7xx_hal_nand.c                         \
../HAL_Driver/Src/stm32f7xx_hal_nor.c                          \
../HAL_Driver/Src/stm32f7xx_hal_pwr.c                          \
../HAL_Driver/Src/stm32f7xx_hal_pwr_ex.c                       \
../HAL_Driver/Src/stm32f7xx_hal_rcc.c                          \
../HAL_Driver/Src/stm32f7xx_hal_rcc_ex.c                       \
../HAL_Driver/Src/stm32f7xx_hal_sd.c                           \
../HAL_Driver/Src/stm32f7xx_hal_sdram.c                        \
../HAL_Driver/Src/stm32f7xx_hal_sram.c                         \
../HAL_Driver/Src/stm32f7xx_hal_uart.c                         \
../HAL_Driver/Src/stm32f7xx_hal_usart.c                        \
../HAL_Driver/Src/stm32f7xx_ll_dma2d.c                         \
../HAL_Driver/Src/stm32f7xx_ll_dma.c                           \
../HAL_Driver/Src/stm32f7xx_ll_fmc.c                           \
../HAL_Driver/Src/stm32f7xx_ll_gpio.c                          \
../HAL_Driver/Src/stm32f7xx_ll_i2c.c                           \
../HAL_Driver/Src/stm32f7xx_ll_pwr.c                           \
../HAL_Driver/Src/stm32f7xx_ll_rcc.c                           \
../HAL_Driver/Src/stm32f7xx_ll_sdmmc.c                         \
../HAL_Driver/Src/stm32f7xx_ll_tim.c                           \
../HAL_Driver/Src/stm32f7xx_ll_usart.c                         \
../HAL_Driver/Src/stm32f7xx_ll_usb.c                           \
../HAL_Driver/Src/stm32f7xx_ll_utils.c




#GLIBS = ../GLIBS/
#PROJECT_SRCS += $(wildcard $(GLIBS)*.cpp)
PROJECT_SRCS += ../MBED/LCD_DISCO_F746NG.cpp
PROJECT_SRCS += ../MBED/TS_DISCO_F746NG.cpp
PROJECT_SRCS += ../src/main.cpp
######################################################################################
# Includes
######################################################################################
PROJECT_INC_PATHS = \
.. \
../CMSIS/device \
../CMSIS/core \
../inc \
../hal_stm_lvgl/tft \
../hal_stm_lvgl/touchpad \
../Utilities/STM32746G-Discovery \
../Utilities/Components/rk043fn48h \
../Utilities/Components/ft5336 \
../HAL_Driver/Inc \
../gruiik \
../MBED


######################################################################################
# Main makefile system configuration
######################################################################################
SYS_OBJECTS =
SYS_INC_PATHS = -I. -I$(GCC_INC)
SYS_LIB_PATHS = -L$(MCU_LIB_PATH)
ifeq (OPTIMIZE_FOR, SIZE)
SYS_LIBRARIES = -lstdc++_s -lsupc++_s -lm -lc_s -lg_s -lnosys
SYS_LD_FLAGS  = --specs=nano.specs -u _printf_float -u _scanf_float
else
SYS_LIBRARIES = -lstdc++ -lsupc++ -lm -lc -lg -lnosys
SYS_LD_FLAGS  =
endif


###############################################################################
# Command line building
###############################################################################
ifdef DEBUG_LEVEL
CC_DEBUG_FLAGS = -g$(DEBUG_LEVEL)
CC_SYMBOLS = -DDEBUG $(PROJECT_SYMBOLS)
else
CC_DEBUG_FLAGS =
CC_SYMBOLS = -DNODEBUG $(PROJECT_SYMBOLS)
endif

ifdef OPTIM_LEVEL
CC_OPTIM_FLAGS = -O$(OPTIM_LEVEL)
else
CC_OPTIM_FLAGS = -O3
endif

INCLUDE_PATHS = $(PROJECT_INC_LIB) $(SYS_INC_PATHS) $(PROJECT_INC_PATHS_WITH_I)
LIBRARY_PATHS = $(PROJECT_LIB_LIB) $(SYS_LIB_PATHS)
CC_FLAGS = $(MCU_CC_FLAGS) -c $(CC_OPTIM_FLAGS) $(CC_DEBUG_FLAGS) -fno-common -fmessage-length=0 -fno-exceptions -ffunction-sections -fdata-sections
#CC_FLAGS = $(MCU_CC_FLAGS) -c $(CC_OPTIM_FLAGS) $(CC_DEBUG_FLAGS) -fno-common -fmessage-length=0 -Wall -fpermissive -fno-exceptions -ffunction-sections -fdata-sections
LD_FLAGS = $(MCU_CC_FLAGS) -Wl,--gc-sections $(SYS_LD_FLAGS) -Wl,-Map=$(PROJECT).map
LD_SYS_LIBS = $(SYS_LIBRARIES)

BULD_TARGET = $(PROJECT)

###############################################################################
# Filter sources
###############################################################################

PROJECT_OBJECTS_C += $(patsubst %.c,%.o,$(filter %.c,$(PROJECT_SRCS)))
PROJECT_OBJECTS_CPP += $(patsubst %.cpp,%.o,$(filter %.cpp,$(PROJECT_SRCS)))
PROJECT_OBJECTS_S += $(patsubst %.S,%.o,$(filter %.S,$(PROJECT_SRCS)))
PROJECT_OBJECTS_s += $(patsubst %.s,%.o,$(filter %.s,$(PROJECT_SRCS)))
PROJECT_OBJECTS += $(PROJECT_OBJECTS_C)
PROJECT_OBJECTS += $(PROJECT_OBJECTS_CPP)
PROJECT_OBJECTS += $(PROJECT_OBJECTS_S)
PROJECT_OBJECTS += $(PROJECT_OBJECTS_s)

PROJECT_INC_PATHS_WITH_I += $(patsubst %,-I%,$(PROJECT_INC_PATHS))

###############################################################################
# Makefile execution
###############################################################################

all: $(BULD_TARGET).bin

clean:
	rm -f $(BULD_TARGET).bin $(BULD_TARGET).elf $(BULD_TARGET).map $(PROJECT_OBJECTS)

gkclean:
	rm -f $(BULD_TARGET).bin $(BULD_TARGET).elf $(BULD_TARGET).map ../src/main.o

.s.o:
	$(AS) $(MCU_CC_FLAGS) -o $@ $<

.c.o:
#	$(CPP)  $(CC_FLAGS) $(CC_SYMBOLS) -std=gnu++11   $(INCLUDE_PATHS) -o $@ $<
	$(CC)  $(CC_FLAGS) $(CC_SYMBOLS) -std=gnu99   $(INCLUDE_PATHS) -o $@ $<

.cpp.o:
	$(CPP) $(CC_FLAGS) $(CC_SYMBOLS) -std=gnu++11 $(INCLUDE_PATHS) -o $@ $<
#	$(CPP) $(CC_FLAGS) $(CC_SYMBOLS) -std=gnu++98 $(INCLUDE_PATHS) -o $@ $<


$(BULD_TARGET).elf: $(PROJECT_OBJECTS) $(SYS_OBJECTS)
	$(LD) $(LD_FLAGS) -T$(LINKER_SCRIPT) $(LIBRARY_PATHS) -o $@ $^ $(PROJECT_LIBRARIES) $(SYS_LIBRARIES) $(PROJECT_LIBRARIES) $(SYS_LIBRARIES)

$(BULD_TARGET).bin: $(BULD_TARGET).elf
	$(OBJCOPY) -O binary $< $@
