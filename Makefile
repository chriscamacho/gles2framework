# only ONE of these three

# rpi		- uses xwindows to provide event handling
# rpi_noX	- get keyboard events from raw input, xwindows not needed

#PLATFORM=rpi
#PLATFORM=rpi_noX
PLATFORM=glfw

CC = gcc

ifeq ($(PLATFORM),rpi)
	FLAGS=-D__FOR_RPi__ -c -std=gnu99 -Iinclude -Ikazmath/kazmath
	FLAGS+= -I/opt/vc/include/ -I/opt/vc/include/interface/vcos/pthreads/
	LIBS=-lX11 -lGLESv2 -lEGL -lm -lbcm_host -L/opt/vc/lib
endif

ifeq ($(PLATFORM),rpi_noX)
	FLAGS=-D__FOR_RPi_noX__ -c -std=gnu99 -Iinclude -Ikazmath/kazmath
	FLAGS+= -I/opt/vc/include/ -I/opt/vc/include/interface/vcos/pthreads/
	LIBS=-lX11 -lGLESv2 -lEGL -lm -lbcm_host -L/opt/vc/lib
endif

ifeq ($(PLATFORM),glfw)
	FLAGS=-D__FOR_GLFW__ -c -std=gnu99 -Iinclude -Ikazmath/kazmath
	LIBS=-lGL -lglfw -lGLEW -lm
endif



# ok.... find all src/*.c replace all .c with .o then replace src\ with o\ - and breath
# this is the framework itself without samples
OBJ=$(shell find src/*.c | sed 's/\(.*\.\)c/\1o/g' | sed 's/src\//o\//g')

#kazmath
KAZ=$(shell find kazmath/kazmath/*.c | sed 's/\(.*\.\)c/\1o/g' | sed 's/kazmath\/kazmath\//o\//g')

all: invaders simple sprites

invaders: $(OBJ) o/invaders.o lib/libkazmath.a
	$(CC) $^ -o invaders $(LIBS)

o/invaders.o: examples/invaders.c
	$(CC) $(FLAGS) $< -o $@

simple: $(OBJ) o/simple.o lib/libkazmath.a
	$(CC) $^ -o simple $(LIBS)

o/simple.o: examples/simple.c
	$(CC) $(FLAGS) $< -o $@

phystest: $(OBJ) o/phystest.o lib/libkazmath.a
#	$(CC) $^ -o phystest $(LIBS) ../opende/ode/src/.libs/libode.a -lstdc++
	$(CC) $^ -o phystest $(LIBS) ../ode-0.12/ode/src/.libs/libode.a -lstdc++

o/phystest.o: examples/phystest.c
#	$(CC) $(FLAGS) -DdSingle -I../opende/include/ $< -o $@
	$(CC) $(FLAGS) -DdSINGLE -I../ode-0.12/include/ $< -o $@

sprites: $(OBJ) o/sprites.o lib/libkazmath.a
	$(CC) $^ -o sprites $(LIBS)

o/sprites.o: examples/sprites.c
	$(CC) $(FLAGS) $< -o $@

chiptest: $(OBJ) o/chiptest.o lib/libkazmath.a
#	$(CC) $^ -o chiptest $(LIBS) ../Chipmunk-6.1.1/src/libchipmunk.a
	$(CC) $^ -o chiptest $(LIBS) ../Chipmunk-Physics/src/libchipmunk.a

o/chiptest.o: examples/chiptest.c
#	$(CC) $(FLAGS) -I../Chipmunk-6.1.1/include/chipmunk/ $< -o $@
	$(CC) $(FLAGS) -I../Chipmunk-Physics/include/chipmunk/ $< -o $@


# used to create object files from all in src directory
o/%.o: src/%.c
	$(CC) $(FLAGS) $< -o $@


lib/libkazmath.a: $(KAZ)
	ar -cvq lib/libkazmath.a $(KAZ) 

o/%.o: kazmath/kazmath/%.c
	$(CC) $(FLAGS) $< -o $@


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

