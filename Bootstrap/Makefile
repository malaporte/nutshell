#*******************************************************************************
# Copyright (C) Martin Laporte.
#*******************************************************************************

SOURCES := BootSector.S \
           KernelLoader.S
OUTPUTS := Bootstrap.bin

-include ../Templates/Global.mak

Bootstrap.exe Bootstrap.bin : $(OBJECTS)
	$(LD) $(OBJECTS) -o Bootstrap.exe -Ttext=0x0 -e 0x0
	$(OBJCOPY) Bootstrap.exe Bootstrap.bin -O binary