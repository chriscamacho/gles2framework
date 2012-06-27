#!/bin/sh

./obj2opengl.pl -noverbose -noScale -noMove -o OBJ.h $1.obj
gcc builder.c -o builder
./builder $1.gbo

