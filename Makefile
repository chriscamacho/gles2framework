# can be xorg or rpi
PLATFORM=xorg
#PLATFORM=rpi



ifeq ($(PLATFORM),xorg)
	FLAGS=-D__FOR_XORG__ -c -std=gnu99 `pkg-config libpng --cflags` -Iinclude -Ikazmath/kazmath
	LIBS=-lX11 -lEGL -lGLESv2 `pkg-config libpng --libs` -lm
endif

ifeq ($(PLATFORM),rpi)
	FLAGS=-D__FOR_RPi__ -c -std=gnu99 `pkg-config libpng --cflags` -Iinclude -Ikazmath/kazmath
	FLAGS+= -I/opt/vc/include/ -I/opt/vc/include/interface/vcos/pthreads/
	LIBS=-lX11 -lGLESv2 -lEGL -lm -lbcm_host -L/opt/vc/lib `pkg-config libpng --libs`
endif


# ok.... find all src/*.c replace all .c with .o then replace src\ with o\ - and breath
# this is the framework itself without samples
OBJ=$(shell find src/*.c | sed 's/\(.*\.\)c/\1o/g' | sed 's/src\//o\//g')

#kazmath
KAZ=$(shell find kazmath/kazmath/*.c | sed 's/\(.*\.\)c/\1o/g' | sed 's/kazmath\/kazmath\//o\//g')

all: invaders simple sprites

lib/libkazmath.a: $(KAZ)
	ar -cvq lib/libkazmath.a $(KAZ)

simple: $(OBJ) o/simple.o lib/libkazmath.a
	gcc $^ -o simple $(LIBS)

o/simple.o: examples/simple.c
	gcc $(FLAGS) $< -o $@

sprites: $(OBJ) o/sprites.o lib/libkazmath.a
	gcc $^ -o sprites $(LIBS)

o/sprites.o: examples/sprites.c
	gcc $(FLAGS) $< -o $@

invaders: $(OBJ) o/invaders.o lib/libkazmath.a
	gcc $^ -o invaders $(LIBS)

o/invaders.o: examples/invaders.c
	gcc $(FLAGS) $< -o $@

phystest: $(OBJ) o/phystest.o lib/libkazmath.a
	gcc $^ -o phystest $(LIBS) ./lib/libode.a -lstdc++

o/phystest.o: examples/phystest.c
	gcc $(FLAGS) -DdSINGLE -I../ode-0.12/include/ $< -o $@


# used to create object files from all in src directory
o/%.o: src/%.c
	gcc $(FLAGS) $< -o $@

o/%.o: kazmath/kazmath/%.c
	gcc $(FLAGS) $< -o $@

# makes the code look nice!
indent:
	astyle src/*.c include/*.h *.c


# deletes all intermediate object files and all compiled
# executables and automatic source backup files
clean:
	rm -f o/*.o
	rm -f src/*~
	rm -f include/*~
	rm -f examples/*~
	rm -f invaders
	rm -f simple
	rm -f phystest
	rm -f sprites
	rm -f lib/libkazmath.a
