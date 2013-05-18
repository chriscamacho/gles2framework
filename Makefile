# only ONE of these three

# xorg		- for running in normal xwindows when you can't get to your PI :-o
# rpi		- uses xwindows to provide event handling
# rpi_noX	- get keyboard events from raw input, xwindows not needed

#PLATFORM=xorg
#PLATFORM=rpi
PLATFORM=rpi_noX

####

ifeq ($(PLATFORM),xorg)
	FLAGS= -g -D__FOR_XORG__ -c -std=gnu99 `pkg-config libpng --cflags` -Iinclude -Ikazmath/kazmath
	LIBS=-lX11 -lEGL -lGLESv2 `pkg-config libpng --libs` -lm
endif

ifeq ($(PLATFORM),rpi)
	FLAGS=-D__FOR_RPi__ -c -std=gnu99 `pkg-config libpng --cflags` -Iinclude -Ikazmath/kazmath
	FLAGS+= -I/opt/vc/include/ -I/opt/vc/include/interface/vcos/pthreads/
	LIBS=-lX11 -lGLESv2 -lEGL -lm -lbcm_host -L/opt/vc/lib `pkg-config libpng --libs`
endif

ifeq ($(PLATFORM),rpi_noX)
	FLAGS=-D__FOR_RPi_noX__ -c -std=gnu99 `pkg-config libpng --cflags` -Iinclude -Ikazmath/kazmath
	FLAGS+= -I/opt/vc/include/ -I/opt/vc/include/interface/vcos/pthreads/
	LIBS=-lX11 -lGLESv2 -lEGL -lm -lbcm_host -L/opt/vc/lib `pkg-config libpng --libs`
endif


# ok.... find all src/*.c replace all .c with .o then replace src\ with o\ - and breath
# this is the framework itself without samples
OBJ=$(shell find src/*.c | sed 's/\(.*\.\)c/\1o/g' | sed 's/src\//o\//g')

#kazmath
KAZ=$(shell find kazmath/kazmath/*.c | sed 's/\(.*\.\)c/\1o/g' | sed 's/kazmath\/kazmath\//o\//g')

all: invaders simple sprites

invaders: $(OBJ) o/invaders.o lib/libkazmath.a
	gcc $^ -o invaders $(LIBS)

o/invaders.o: examples/invaders.c
	gcc $(FLAGS) $< -o $@

simple: $(OBJ) o/simple.o lib/libkazmath.a
	gcc $^ -o simple $(LIBS)

o/simple.o: examples/simple.c
	gcc $(FLAGS) $< -o $@

phystest: $(OBJ) o/phystest.o lib/libkazmath.a
	gcc $^ -o phystest $(LIBS) ../ode-0.12/ode/src/.libs/libode.a -lstdc++

o/phystest.o: examples/phystest.c
	gcc $(FLAGS) -DdSINGLE -I../ode-0.12/include/ $< -o $@

sprites: $(OBJ) o/sprites.o lib/libkazmath.a
	gcc $^ -o sprites $(LIBS)

o/sprites.o: examples/sprites.c
	gcc $(FLAGS) $< -o $@

chiptest: $(OBJ) o/chiptest.o lib/libkazmath.a
	gcc $^ -o chiptest $(LIBS) ../Chipmunk-6.1.1/src/libchipmunk.a

o/chiptest.o: examples/chiptest.c
	gcc $(FLAGS) -I../Chipmunk-6.1.1/include/chipmunk/ $< -o $@


# used to create object files from all in src directory
o/%.o: src/%.c
	gcc $(FLAGS) $< -o $@


lib/libkazmath.a: $(KAZ)
	ar -cvq lib/libkazmath.a $(KAZ) 

o/%.o: kazmath/kazmath/%.c
	gcc $(FLAGS) $< -o $@


# makes the code look nice!
indent:
	astyle src/*.c include/*.h example/*.c


# deletes all intermediate object files and all compiled
# executables and automatic source backup files
clean:
	rm -f o/*.o
	rm -f lib/libkazmath.a
	rm -f *~
	rm -f src/*~
	rm -f include/*~
	rm -f examples/*~
	rm -f resources/shaders/*~
	rm -f invaders
	rm -f simple
	rm -f phystest
	rm -f sprites
	rm -f chiptest

