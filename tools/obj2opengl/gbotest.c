/*
gcc -std=gnu99 gbotest.c -o gbotest
*/

#include <stdio.h>
#include <stdlib.h>

int main (int argc, char *argv[])
{
	if (argc!=2) {
		printf("please specify a GBO file\n");
		return -1;
	}
	FILE *pFile;
	pFile = fopen( argv[1], "rb" );

	unsigned int magic;
	int NumVerts;
	fread (&magic,1, sizeof(unsigned int), pFile );
	if (magic!=0x614f4247) {
		printf("Does not appear to be a version 'a' GBO file\n");
		return -2;
	}
	fread(&NumVerts,1,sizeof(unsigned int), pFile );
	printf("Obj contains %i verts\n",NumVerts);
	
	float* Verts = malloc(sizeof(float) * 3 * NumVerts);
	fread(Verts,1,sizeof(float) * 3 * NumVerts, pFile );
	for (int i=0; i< NumVerts*3; i=i+3) 
		printf("%f\t%f\t%f\n",Verts[i],Verts[i+1],Verts[i+2]);
	free(Verts);
	
	printf("\nNormals\n");
	float* Norms = malloc(sizeof(float) * 3 * NumVerts);
	fread(Norms,1,sizeof(float) * 3 * NumVerts, pFile );
	for (int i=0; i< NumVerts*3; i=i+3) 
		printf("%f\t%f\t%f\n",Norms[i],Norms[i+1],Norms[i+2]);
	free(Norms);
	
	printf("\nTexture coordinates\n");
	float* TexCoords = malloc(sizeof(float) * 2 * NumVerts);
	fread(TexCoords,1,sizeof(float) * 2 * NumVerts, pFile );
	for (int i=0; i< NumVerts*2; i=i+2) 
		printf("%f\t%f\n",TexCoords[i],TexCoords[i+1]);
	free(TexCoords);
	
	
	return 0;
}
