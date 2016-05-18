
FLAGS= -g -c -std=gnu99 -Iinclude -Ikazmath
LIBS=-lX11 -lEGL -lGLESv2 -lm


# ok.... find all src/*.c replace all .c with .o then replace src\ with o\ - ...and breath
# this is the framework itself without samples
OBJ=$(shell find src/*.c | sed 's/\(.*\.\)c/\1o/g' | sed 's/src\//o\//g')

#kazmath
KAZ=$(shell find kazmath/*.c | sed 's/\(.*\.\)c/\1o/g' | sed 's/kazmath\/kazmath\//o\//g')

all: invaders simple sprites

test:
	echo $(KAZ)

lib/libkazmath.a: $(KAZ)
	ar -cvq lib/libkazmath.a $(KAZ)
	
kazmath/%.o: kazmath/%.c
	gcc $(FLAGS) $< -o $@


invaders: $(OBJ) o/invaders.o lib/libkazmath.a
	gcc $^ -o invaders $(LIBS)

o/invaders.o: examples/invaders.c
	gcc $(FLAGS) $< -o $@

simple: $(OBJ) o/simple.o lib/libkazmath.a
	gcc $^ -o simple $(LIBS)

o/simple.o: examples/simple.c
	gcc $(FLAGS) $< -o $@

phystest: $(OBJ) o/phystest.o lib/libkazmath.a
#	gcc $^ -o phystest $(LIBS) /usr/lib/libode.a -lstdc++
#	gcc $^ -o phystest $(LIBS) ../ode-0.13.1/ode/src/.libs/libode.a -lstdc++ -lpthread
	gcc $^ -o phystest $(LIBS) ../ode-0.14/ode/src/.libs/libode.a -lstdc++ -lpthread

o/phystest.o: examples/phystest.c
#	gcc $(FLAGS) -DdSINGLE $< -o $@
#	gcc $(FLAGS) -I../ode-0.13.1/include $< -o $@
	gcc $(FLAGS) -I../ode-0.14/include $< -o $@

sprites: $(OBJ) o/sprites.o lib/libkazmath.a
	gcc $^ -o sprites $(LIBS)

o/sprites.o: examples/sprites.c
	gcc $(FLAGS) $< -o $@

chiptest: $(OBJ) o/chiptest.o lib/libkazmath.a
#	gcc $^ -o chiptest $(LIBS) ../Chipmunk-6.1.1/src/libchipmunk.a
#	gcc $^ -o chiptest $(LIBS) ../Chipmunk-Physics/src/libchipmunk.a
	gcc $^ -o chiptest $(LIBS) ../Chipmunk-7.0.1/src/libchipmunk.a

o/chiptest.o: examples/chiptest.c
#	gcc $(FLAGS) -I../Chipmunk-6.1.1/include/chipmunk/ $< -o $@
#	gcc $(FLAGS) -I../Chipmunk-Physics/include/chipmunk/ $< -o $@
	gcc $(FLAGS) -I../Chipmunk-7.0.1/include/chipmunk/ $< -o $@


# used to create object files from all in src directory
o/%.o: src/%.c
	gcc $(FLAGS) $< -o $@




# makes the code look nice!
indent:
	astyle src/*.c include/*.h example/*.c


# deletes all intermediate object files and all compiled
# executables and automatic source backup files
clean:
	rm -f o/*.o
	rm -f *~
	rm -f src/*~
	rm -f include/*~
	rm -f examples/*~
	rm -f resources/shaders/*~

clean-examples:
	rm -f invaders
	rm -f simple
	rm -f phystest
	rm -f sprites
	rm -f chiptest

clean-all: clean clean-examples
	rm -f kazmath/*.o
	rm -f lib/libkazmath.a
