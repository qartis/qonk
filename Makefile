CXX=g++
CFLAGS=`sdl-config --cflags` -Os -Wall -Wextra -fmessage-length=0 -Winit-self -Wcast-align -Wcast-qual -Wpointer-arith -Wstrict-aliasing -Wformat=2 -Wmissing-include-dirs -Wno-unused-parameter -Wuninitialized -Wno-error=unused-variable -Wformat-security -Wno-write-strings -g
LIBS=`sdl-config --libs` -lSDL_ttf -lSDL_net

C_SRC = $(shell ls -t *.c)
CPP_SRC = $(shell ls -t *.cpp)
OBJ = $(C_SRC:%.c=%.o) $(CPP_SRC:%.cpp=%.o)

all: qonk

qonk: $(OBJ)
	$(CXX) $(CFLAGS) $(LIBS) $(OBJ) -o qonk

%.o : %.cpp %.h
	$(CXX) $(CFLAGS) -c $< -o $@

%.o : %.cpp
	$(CXX) $(CFLAGS) -c $< -o $@


%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -vf $(OBJ) screenshot.bmp /tmp/.obj/qonk
