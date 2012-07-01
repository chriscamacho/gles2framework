#include <stdio.h>
#include <stdlib.h>

#include "OBJ.h"

int main (int argc, char *argv[])
{

	if (argc!=2) exit(-1);

//OBJNumVerts
//OBJVerts
//OBJNormals
//OBJTexCoords

//	printf("%s\n",argv[1]);

	unsigned int magic;
	magic=0x614F4247;  // GBOa in little endian - initial version "a"

	FILE *pFile;
	pFile = fopen ( argv[1] , "wb" );
	fwrite (&magic , 1 , sizeof(unsigned int) , pFile );
	fwrite (&OBJNumVerts , 1 , sizeof(unsigned int) , pFile );
	fwrite (OBJVerts , 1 , sizeof(float) * 3 * OBJNumVerts, pFile );
	fwrite (OBJNormals , 1 , sizeof(float) * 3 * OBJNumVerts , pFile );
	fwrite (OBJTexCoords , 1 , sizeof(float) * 2 * OBJNumVerts , pFile );
	fclose(pFile);
	
	
}
