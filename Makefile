#*******************************************************************************
# Copyright (C) Martin Laporte.
#*******************************************************************************

.PHONY : all
.PHONY : deps
.PHONY : clean
.PHONY : debug

all :

clean : clean_floppy

all deps clean distclean :
	$(MAKE) $@ -C Kernel
	$(MAKE) $@ -C Bootstrap

floppy : all
	python Tools/MakeFloppy.py

clean_floppy :
	rm -f FloppyImage.img
