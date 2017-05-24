LIBDIRS = -L/opt/X11/lib
INCDIRS = -I/opt/X11/include -I/opt/X11/include/GL

CC = g++
CFLAGS = -Wall -g $(INCDIRS)
BASELIBS = -lX11  -lGLU -lm
LINUXLIBS = -lGL -lglut
MACLIBS = -framework OpenGL -framework GLUT

mac: gltb.o PixelMap.o
	$(CC) $(CFLAGS) -o $(MAIN) $(LIBDIRS) gltb.o PixelMap.o main.cpp $(BASELIBS) $(MACLIBS)

linux: gltb.o PixelMap.o
	$(CC) $(CFLAGS) -o $(MAIN) $(LIBDIRS) gltb.o PixelMap.o main.cpp $(BASELIBS) $(LINUXLIBS)

gltb.o: gltb.cpp
	$(CC) -c gltb.cpp

PixelMap.o: PixelMap.cpp
	$(CC) -c PixelMap.cpp

clean:
	rm -f *.o
	rm $(MAIN)
