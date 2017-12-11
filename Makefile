CFLAGS= -Wall -O3 -mwindows
CFLAGS += -Wno-write-strings

CSRC=binclock.cpp bclk_elements.cpp config.cpp wcommon.cpp common_funcs.cpp

OBJS = $(CSRC:.cpp=.o) bcrc.o

BINS=binclock.exe

#**************************************************************
#  generic build rules
#**************************************************************
%.o: %.cpp
	g++ $(CFLAGS) -c $< -o $@

all: $(BINS)

clean:
	rm -f $(BINS) *.o

dist:
	rm -f *.zip
	zip binclock.zip binclock.exe *.bmp copying
																			
lint:
	cmd /C "c:\lint9\lint-nt +v -width(160,4) -ic:\lint9 mingw.lnt -os(_lint.tmp) lintdefs.cpp $(CSRC)"

depend: 
	makedepend $(CSRC)

#**************************************************************
#  build rules for executables                           
#**************************************************************
$(BINS): $(OBJS)
	g++ $(CFLAGS) -s $^ -o $@ -lgdi32 

#**************************************************************
#  build rules for libraries and other components
#**************************************************************
bcrc.o: binclock.rc binclock.h
	windres -O COFF $< -o $@

# DO NOT DELETE

binclock.o: common.h wcommon.h binclock.h bclk_elements.h
bclk_elements.o: common.h binclock.h bclk_elements.h
config.o: common.h binclock.h
wcommon.o: wcommon.h
common_funcs.o: common.h
