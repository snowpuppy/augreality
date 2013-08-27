# Universal C Makefile for MCU targets
# Top-level template file to configure build

# Makefile for LeafLabs Maple RET6 (STM32F103RE series)
# To change to LeafLabs Maple R5, use anything other than "MapleRET6" (recommend MapleRBT6)
DEVICE=MapleRET6
# Libraries to include in the link (use -L and -l) e.g. -lm, -lmyLib
LIBRARIES=firmware/libccos-maple.a -lgcc
# Prefix for ARM tools (must be on the path)
MCUPREFIX=arm-none-eabi-
# Flags for the assembler
MCUAFLAGS=-mthumb -mcpu=cortex-m3 -mlittle-endian
# Flags for the compiler
MCUCFLAGS=-mthumb -mcpu=cortex-m3 -mlittle-endian
# Flags for the linker
MCULFLAGS=-nostartfiles -Wl,-static -Wl,-u,VectorTable -Wl,-T,firmware/STM32F10x.ld -Wl,-T,firmware/maple.ld
# Prepares the elf file by converting it to a binary that java can write
MCUPREPARE=$(OBJCOPY) $(OUT) -O binary $(BINDIR)/$(OUTBIN)
# Advanced sizing flags
SIZEFLAGS=
# Uploads program using java
UPLOAD=@java -jar firmware/uniflash.jar maple $(BINDIR)/$(OUTBIN)

# Advanced options
ASMEXT=s
CEXT=c
CPPEXT=cpp
HEXT=h
INCLUDE=-I$(ROOT)/include -I$(ROOT)/include/usb -I$(ROOT)/src
OUTBIN=output.bin
OUTNAME=output.elf

# Flags for programs
AFLAGS:=$(MCUAFLAGS)
ARFLAGS:=$(MCUCFLAGS)
CCFLAGS:=-c -Wall $(MCUCFLAGS) -Os -ffunction-sections -fsigned-char -fomit-frame-pointer
CFLAGS:=$(CCFLAGS) -std=gnu99
CPPFLAGS:=$(CCFLAGS) -fno-exceptions -fno-rtti -felide-constructors
LDFLAGS:=-Wall $(MCUCFLAGS) $(MCULFLAGS) -Wl,--gc-sections

# Tools used in program
AR:=$(MCUPREFIX)ar
AS:=$(MCUPREFIX)as
CC:=$(MCUPREFIX)gcc
CPPCC:=$(MCUPREFIX)g++
OBJCOPY:=$(MCUPREFIX)objcopy
