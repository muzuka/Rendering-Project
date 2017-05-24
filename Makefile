LIBDIRS = -L/opt/X11/lib
INCDIRS = -I/opt/X11/include -I/opt/X11/include/GL

CC = g++
CFLAGS = -Wall -g $(INCDIRS)
BASELIBS = -lX11 -lGLU
LINUXLIBS = -lGL -lglut
MACLIBS = -framework OpenGL -framework GLUT

mac: gltb.o PixelMap.o main.o
	$(CC) $(CFLAGS) -o $(MAIN) $(LIBDIRS) -std=c++0x gltb.o PixelMap.o main.o $(BASELIBS) $(MACLIBS)

linux: gltb.o PixelMap.o main.o
	$(CC) $(CFLAGS) -o $(MAIN) $(LIBDIRS) -std=c++0x gltb.o PixelMap.o main.o $(BASELIBS) $(LINUXLIBS)

gltb.o: gltb.cpp
	$(CC) -c gltb.cpp

PixelMap.o: PixelMap.cpp
	$(CC) -c PixelMap.cpp

main.o: main.cpp
	$(CC) -c main.cpp

clean:
	rm *.o