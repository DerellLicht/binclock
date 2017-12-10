CFLAGS= -Wall -O3 -mwindows
CFLAGS += -Wno-write-strings

CSRC=binclock.cpp bclk_elements.cpp regif.cpp wcommon.cpp common_funcs.cpp

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
	zip binclock.src.zip makefile *.cpp *.h *.hpp *.ini *.rc *.ico *.bmp copying
																			
lint:
	cmd /C "c:\lint9\lint-nt +v -width(160,4) -ic:\lint9 mingw.lnt -os(_lint.tmp) lintdefs.cpp $(CSRC)"

depend: 
	makedepend *.cpp

#**************************************************************
#  build rules for executables                           
#**************************************************************
binclock.exe: $(OBJS)
	g++ $(CFLAGS) -s $^ -o $@ -lgdi32 -lcomctl32

#**************************************************************
#  build rules for libraries and other components
#**************************************************************
bcrc.o: binclock.rc binclock.h
	windres -O COFF $< -o $@

# DO NOT DELETE

bclk_elements.o: bclk_elements.h common.h regif.hpp
binclock.o: wcommon.h binclock.h bclk_elements.h regif.hpp
common_funcs.o: common.h
regif.o: common.h regif.hpp
wcommon.o: wcommon.h
