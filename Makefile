# can be xorg or rpi
#PLATFORM=xorg
PLATFORM=rpi



ifeq ($(PLATFORM),xorg)
	FLAGS=-D__FOR_XORG__ -c -std=gnu99 `pkg-config libpng --cflags` -Iinclude -Isrc-models -Ikazmath/kazmath
	LIBS=-lX11 -lEGL -lGLESv2 `pkg-config libpng --libs` -lm
endif

ifeq ($(PLATFORM),rpi)
	FLAGS=-D__FOR_RPi__ -c -std=gnu99 `pkg-config libpng --cflags` -Iinclude -Isrc-models -Ikazmath/kazmath
	FLAGS+= -I/opt/vc/include/ -I/opt/vc/include/interface/vcos/pthreads/
	FLAGS+= 
	LIBS=-lGLESv2 -lEGL -lm -lbcm_host -L/opt/vc/lib `pkg-config libpng --libs`
endif


# ok.... find all src/*.c replace all .c with .o then replace src\ with o\ - and breath
# this is the framework itself without samples
OBJ=$(shell find src/*.c | sed 's/\(.*\.\)c/\1o/g' | sed 's/src\//o\//g')

# models
OBJ+=$(shell find src-models/*.c | sed 's/\(.*\.\)c/\1o/g' | sed 's/src-models\//o\//g')

#kazmath
OBJ+=$(shell find kazmath/kazmath/*.c | sed 's/\(.*\.\)c/\1o/g' | sed 's/kazmath\/kazmath\//o\//g')

# because the examples main and simple are seperate from the framework
# there are explicit rules for them
main: $(OBJ) o/main.o
	gcc $^ -o main $(LIBS)

simple: $(OBJ) o/simple.o
	gcc $^ -o simple $(LIBS)

o/main.o: main.c
	gcc $(FLAGS) -Isrc $< -o $@

o/simple.o: simple.c
	gcc $(FLAGS) -Isrc $< -o $@


# used to create object files from all in src directory
o/%.o: src/%.c mko
	gcc $(FLAGS) $< -o $@

o/%.o: src-models/%.c mko
	gcc $(FLAGS) $< -o $@

o/%.o: kazmath/kazmath/%.c mko
	gcc $(FLAGS) $< -o $@


# makes the code look nice!
indent:
	indent -linux src/*.c src/*.h *.c

mko:
	mkdir -p o

# deletes all intermediate object files and all compiled
# executables and automatic source backup files
clean:
	rm -f o/*.o
	rm -f src/*~
	rm -f src-models/*~
	rm -f include/*~
	rm -f *~
	rm -f main
	rm -f simple
