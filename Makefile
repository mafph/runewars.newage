# makefile
# project: RuneWars: New Age
#
# Options:
# DEBUG: build in debug mode
# RELEASE: build with addons extensions
#
# -DRWNA_DATA=path: force RWNA_DATA folder
#

TARGET	:= runewars-na
MAKE	:= make
#CFLAGS := -DSWE_DISABLE_AUDIO

export TARGET CFLAGS

SDL2_EXISTS := $(shell pkg-config SDL2 --exists && echo 1)
SDL1_EXISTS := $(shell pkg-config SDL --exists && echo 1)

ifndef SDL2_EXISTS
ifdef SDL1_EXISTS
export SWE_SDL12 := 1
endif
endif

#export SWE_SDL12=1

all:
#	$(MAKE) -C dist
	$(MAKE) -C dist DEBUG=1
	@cp dist/$(TARGET) .

pot:
	$(MAKE) -C dist pot

clean:
	$(MAKE) -C dist clean
	rm -f $(TARGET)
