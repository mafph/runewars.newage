# makefile
# project: RuneWars New Age
#

ifdef SWE_SDL12
SDLVER := ver12
else
SDLVER := ver20
endif

LIBSWEDIR	:= ../../../../engine
LIBENGINE	:= $(LIBSWEDIR)/libswe.a
CFLAGS		:= $(CFLAGS) -I$(LIBSWEDIR)

include Makefile.$(SDLVER)

CFLAGS		:= $(CFLAGS) $(ENGINE_CFLAGS) -std=c++14 -Wall -Wno-sign-compare -DSWE_WITH_JSON -DSWE_DISABLE_NETWORK -DSWE_DEBUG_MESSAGES
LIBS		:= $(LIBS) $(ENGINE_LIBS) -lz

ifeq ($(CXX),clang++)
CFLAGS		:= $(CFLAGS) -Wno-overloaded-virtual 
endif

ifdef DEBUG
CFLAGS		:= $(CFLAGS) -O0 -g -Werror -DSWE_DEBUG
LIBS		:= $(LIBS)
BOOST_STRACE	:= $(shell pkg-config boost_stacktrace --exists && echo 1)
else
CFLAGS		:= $(CFLAGS) -O2
LIBS		:= $(LIBS)
endif


ifdef BOOST_STRACE
CFLAGS		:= $(CFLAGS) $(shell pkg-config boost_stacktrace --cflags) -DSWE_STACKTRACE
LIBS		:= $(LIBS) $(shell pkg-config boost_stacktrace --libs)
endif

ifndef AR
AR		:= ar
endif

ifndef CXX
CXX		:= g++
endif

ifeq ($(PLATFORM),mingw32)
AR		:= $(CROSS_PREFIX)ar
CXX		:= $(CROSS_PREFIX)g++
WINDRES		:= $(CROSS_PREFIX)windres
CFLAGS		:= $(CFLAGS) -static
LIBS		:= $(LIBS) -lvorbisfile -lvorbis -logg -lmad -lpng -ljpeg -lfreetype -liconv -lz
LIBS		:= -static -Wl,-Bstatic $(LIBS) -lwinmm -limm32 -liphlpapi -lsetupapi -lole32 -loleaut32 -lversion
endif

ifeq ($(PLATFORM),mingw64)
CFLAGS		:= $(CFLAGS) -static
LIBS		:= $(LIBS) -lvorbisfile -lvorbis -logg -lmad lpng -ljpeg -lfreetype -liconv -lz
LIBS		:= -static -Wl,-Bstatic $(LIBS) -lwinmm -limm32 -liphlpapi -lsetupapi -lole32 -loleaut32 -lversion
endif

ifeq ($(PLATFORM),mingw32ce)
AR		:= $(CROSS_PREFIX)ar
CXX		:= $(CROSS_PREFIX)g++
WINDRES		:= $(CROSS_PREFIX)windres
CFLAGS		:= $(CFLAGS) -ffunction-sections -DWITHOUT_MOUSE -static
LIBS		:= -Wl,-Bstatic $(LIBS) -lmmtimer
LDFLAGS		:= -static $(LDFLAGS) -Wl,--gc-sections
endif

ifdef WITH_ICONS
RES		:= $(TARGET).res
endif

SOURCEROOT	:= ../src
SOURCEDIR	:= $(shell ls $(SOURCEROOT))
POT		:= $(TARGET).pot
SRCDIRLIST	:= $(addprefix $(SOURCEROOT)/, $(SOURCEDIR))

SEARCH		:= $(addsuffix /*.cpp, $(SRCDIRLIST))

BUILDDATE	:= $(shell date +%Y%m%d)
#ifndef SRCREVISION
#SRCREVISION	:= $(shell hg parent --template "r{rev}")
#endif
#CFLAGS		:= $(CFLAGS) -DBUILDDATE="\"$(BUILDDATE)\"" -DSRCREVISION="\"$(SRCREVISION)\""
CFLAGS		:= $(CFLAGS) -DBUILDDATE="\"$(BUILDDATE)\""

export CFLAGS AR CXX

all:
	@echo "cflags: $(CFLAGS)"
	@echo "libs: $(LIBS)"
	+make -C $(LIBSWEDIR) SWE_WITH_JSON=1 SWE_DISABLE_NETWORK=1 SWE_DEBUG_MESSAGES=1
	+make $(TARGET)

$(TARGET): $(notdir $(patsubst %.cpp, %.o, $(wildcard $(SEARCH)))) $(LIBENGINE) $(RES)
	@echo "lnk: $@"
	$(CXX) $(LDFLAGS) $^ $(LIBS) -o $@

pot: $(wildcard $(SEARCH))
	@echo "gen: $(POT)"
	@xgettext -d $(TARGET) -C -k_ -o $(POT) $(wildcard $(SEARCH))

$(RES): $(ICOFILE)
	@echo "gen: $(RES)"
	@echo "$(IDICON) ICON \"$(ICOFILE)\"" > $(TARGET).rc
	$(WINDRES) $(TARGET).rc -O coff -o $(TARGET).res

VPATH := $(SRCDIRLIST)

%.o: %.cpp
	@echo "cxx: $@"
	@$(CXX) -c -MD $(addprefix -I, $(SRCDIRLIST)) $< $(CFLAGS)

include $(wildcard *.d)

.PHONY: clean

clean:
	make -C $(LIBSWEDIR) clean
	rm -f *.pot *.o *.d *.rc *.res *.exe $(TARGET)
