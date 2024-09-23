# Makefile
CREATORID = pBil
FILENAME = pBill
SRCFILES = $(wildcard sauce/*.c) $(wildcard sauce/*.cc)
HIRES = false

# Palm SDK config
SDK_VERSION = 5
PALMCC = m68k-palmos-gcc
PALMINC = /opt/palmdev/sdk-5r3/include
PILRC = /usr/bin/pilrc
PALMCFLAGS = -O2 -DPALMOS -DSDK_$(SDK_VERSION)=1 \
	-I$(PALMINC) \
	-I$(PALMINC)/Core \
	-I$(PALMINC)/Core/Hardware \
	-I$(PALMINC)/Core/UI \
	-I$(PALMINC)/Core/System \
	-I$(PALMINC)/Dynamic \
	-I$(PALMINC)/Libraries
WARNINGFLAGS = -Wunused

all:
	$(MAKE) EXT="_lowres" HIRES=false build
	$(MAKE) EXT="_hires" HIRES=true PILRCFLAGS="-D PALMHIRES" GCCFLAGS="-DHIRESBUILD" build

build: compile prebin bin gen_grc combine cleanup

compile: 
	$(PALMCC) $(GCCFLAGS) $(PALMCFLAGS) ${WARNINGFLAGS} $(SRCFILES)

ifeq ($(HIRES), true)
prebin:
	./generateBitmaps.sh
	./generateResourceFile.sh --hires
else
prebin:
	./generateBitmaps.sh
	./generateResourceFile.sh
endif

bin:
	$(PILRC) $(PILRCFLAGS) resources/ui.rcp
	$(PILRC) $(PILRCFLAGS) resources/graphicResources.rcp 

gen_grc: 
	m68k-palmos-obj-res a.out

combine:	
	build-prc artifacts/$(FILENAME)$(EXT).prc "$(FILENAME)" $(CREATORID) *.a.out.grc *.bin

cleanup:
	rm *.grc *.out *.bin
	rm -Rf resources/assets
	rm resources/graphicResources.rcp
