# Universal C Makefile for MCU targets
# Top-level template file to configure build

# Makefile for STM32F4 series
DEVICE=STM32F4
# Libraries to include in the link (use -L and -l) e.g. -lm, -lmyLib
LIBRARIES=-lm -L$(ROOT) -lSTMUSB -lSTMCore -lSTMMath
# Prefix for ARM tools (must be on the path)
MCUPREFIX=arm-none-eabi-
# Flags for the assembler
MCUAFLAGS=
# Use Cortex-M4 with FPU and Thumb instruction set
MCUCFLAGS=-mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=softfp
# Flags for the linker
MCULFLAGS=-nostartfiles -Wl,-T -Xlinker stm32_flash.ld -static -Wl,-u,Reset_Handler
# Prepares for compiling
MCUPREPARE=$(OBJCOPY) $(OUT) -O binary $(BINDIR)/output.bin
# Advanced sizing flags
SIZEFLAGS=
# Uploads program using flash utility
UPLOAD=cd $(BINDIR) && dfu-util -a 0 -s 0x08000000:leave -D output.bin

# Advanced options
ASMEXT=s
CEXT=c
CPPEXT=cpp
HEXT=h
INCLUDE=-I$(ROOT)/include -I$(ROOT)
OUTNAME=output.elf

# Flags for programs
AFLAGS:=$(MCUCFLAGS) $(MCUAFLAGS)
ARFLAGS:=$(MCUCFLAGS)
CCFLAGS:=-g -c -Wall -Wno-strict-aliasing $(MCUCFLAGS) -Os -ffunction-sections -fdata-sections \
-DARM_MATH_CM4 -DUSE_STDPERIPH_DRIVER -DSTM32F4XX -D__FPU_PRESENT=1 \
-Werror=implicit-function-declaration
CFLAGS:=$(CCFLAGS) -std=gnu99
CPPFLAGS:=$(CCFLAGS) -fno-exceptions -fno-rtti -felide-constructors
LDFLAGS:=-Wall $(MCUCFLAGS) $(MCULFLAGS) -Wl,--gc-sections

# Tools used in program
AR:=$(MCUPREFIX)ar
AS:=$(MCUPREFIX)as
CC:=$(MCUPREFIX)gcc
CPPCC:=$(MCUPREFIX)g++
OBJCOPY:=$(MCUPREFIX)objcopy
