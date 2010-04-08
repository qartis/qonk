CXX=g++
CFLAGS=`sdl-config --cflags` -O3 -Wall -Wextra -fmessage-length=0 -funsigned-char -funsigned-bitfields -Werror -Wall -Wextra -Wwrite-strings -Winit-self -Wcast-align -Wcast-qual -Wpointer-arith -Wstrict-aliasing -Wformat=2 -Wmissing-include-dirs -Wno-unused-parameter -Wuninitialized -Wno-error=unused-variable -Wformat-security
#CFLAGS=`sdl-config --cflags` -O3 -Wall -Wextra 
LIBS=`sdl-config --libs` -lSDL_ttf -lSDL_net
OBJECTS=SDL_gfxPrimitives.o extensions.o coordinate.o stars.o planets.o selection.o ships.o main.o actions.o players.o universe.o animations.o fonts.o messages.o timer.o config.o network.o

all: qonk

qonk: $(OBJECTS)
	$(CXX) $(CFLAGS) $(LIBS) $(OBJECTS) -o qonk
	@cp qonk server

.cpp.o:
	$(CXX) $(CFLAGS) -c $<

clean:
	@rm -vf *.o *~ qonk screenshot.bmp DEADJOE
