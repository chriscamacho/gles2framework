#include  <GLES2/gl2.h>
#include <kazmath.h>
#include <stdio.h>
#include <stdlib.h>
#include "obj.h"
#include "support.h"

/* each OBJ shader is expected to have at least these attribs and uniforms

vertex_attrib		vertex coordinates
uv_attrib			uv coordinates
mvp_uniform			combined model, view and projection matrix
mv_uniform			view, model matrix for lighting
texture_uniform		texture sampler

other custom attribs/uniforms should be handled as a special case in the 
main code

*/

int loadObj(struct obj_t *obj,const char *objFile, char *vert, char *frag) 
{
	FILE *pFile;
	pFile = fopen( objFile , "rb" );
	if (pFile==NULL) {
		printf("Cant find open model - %s\n",objFile);
		return false;
	}
	unsigned int magic;
	int NumVerts;

	fread (&magic,1, sizeof(unsigned int), pFile );
	if (magic!=0x614f4247) {
		printf("Does not appear to be a version 'a' GBO file\n");
		return false;
	}
	fread(&NumVerts,1,sizeof(unsigned int), pFile );
	
	float* Verts = malloc(sizeof(float) * 3 * NumVerts);
	fread(Verts,1,sizeof(float) * 3 * NumVerts, pFile );
	
	float* Norms = malloc(sizeof(float) * 3 * NumVerts);
	fread(Norms,1,sizeof(float) * 3 * NumVerts, pFile );
	
	float* TexCoords = malloc(sizeof(float) * 2 * NumVerts);
	fread(TexCoords,1,sizeof(float) * 2 * NumVerts, pFile );
	
	createObj(obj,NumVerts,Verts,TexCoords,Norms,vert,frag);
	
	free(TexCoords);
	free(Norms);
	free(Verts);
	
	return true;
}

int loadObjCopyShader(struct obj_t *obj,const char *objFile, struct obj_t *sdrobj) 
{
	FILE *pFile;
	pFile = fopen( objFile , "rb" );
	if (pFile==NULL) {
		printf("Cant find open model - %s\n",objFile);
		return false;
	}
	unsigned int magic;
	int NumVerts;

	fread (&magic,1, sizeof(unsigned int), pFile );
	if (magic!=0x614f4247) {
		printf("Does not appear to be a version 'a' GBO file\n");
		return false;
	}
	fread(&NumVerts,1,sizeof(unsigned int), pFile );
	
	float* Verts = malloc(sizeof(float) * 3 * NumVerts);
	fread(Verts,1,sizeof(float) * 3 * NumVerts, pFile );
	
	float* Norms = malloc(sizeof(float) * 3 * NumVerts);
	fread(Norms,1,sizeof(float) * 3 * NumVerts, pFile );
	
	float* TexCoords = malloc(sizeof(float) * 2 * NumVerts);
	fread(TexCoords,1,sizeof(float) * 2 * NumVerts, pFile );
	
	createObjCopyShader(obj,NumVerts, Verts,TexCoords,
		Norms, sdrobj);
	
	free(TexCoords);
	free(Norms);
	free(Verts);
	
	return true;
}


int createObj(struct obj_t *obj, int numVerts, float *verts, float *txVert,
	      float *norms, char *vertShader, char *fragShader)
{
	obj->num_verts = numVerts;

	glGenBuffers(1, &obj->vbo_vert);
	glBindBuffer(GL_ARRAY_BUFFER, obj->vbo_vert);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * numVerts, verts,
		     GL_STATIC_DRAW);

	glGenBuffers(1, &obj->vbo_tex);
	glBindBuffer(GL_ARRAY_BUFFER, obj->vbo_tex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * numVerts, txVert,
		     GL_STATIC_DRAW);

	glGenBuffers(1, &obj->vbo_norm);
	glBindBuffer(GL_ARRAY_BUFFER, obj->vbo_norm);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * numVerts, norms,
		     GL_STATIC_DRAW);

	GLint link_ok = GL_FALSE;

	GLuint vs, fs;
	if ((vs = create_shader(vertShader, GL_VERTEX_SHADER)) == 0)
		return 0;
	if ((fs = create_shader(fragShader, GL_FRAGMENT_SHADER)) == 0)
		return 0;

	obj->program = glCreateProgram();
	glAttachShader(obj->program, vs);
	glAttachShader(obj->program, fs);
	glLinkProgram(obj->program);
	glGetProgramiv(obj->program, GL_LINK_STATUS, &link_ok);
	if (!link_ok) {
		printf("glLinkProgram:");
		print_log(obj->program);
		return 0;
	}

	obj->vert_attrib =
	    getShaderLocation(shaderAttrib, obj->program, "vertex_attrib");
	obj->tex_attrib =
	    getShaderLocation(shaderAttrib, obj->program, "uv_attrib");
	obj->norm_attrib =
	    getShaderLocation(shaderAttrib, obj->program, "norm_attrib");
	obj->mvp_uniform =
	    getShaderLocation(shaderUniform, obj->program, "mvp_uniform");
	obj->mv_uniform =
	    getShaderLocation(shaderUniform, obj->program, "mv_uniform");
	obj->tex_uniform =
	    getShaderLocation(shaderUniform, obj->program, "texture_uniform");

}

/*
 *  create an obj from supplied verts using an existing models shader 
 */
int createObjCopyShader(struct obj_t *obj, int numVerts, float *verts,
			float *txVert, float *norms, struct obj_t *sdrobj)
{
	obj->num_verts = numVerts;
	glGenBuffers(1, &obj->vbo_vert);
	glBindBuffer(GL_ARRAY_BUFFER, obj->vbo_vert);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * numVerts, verts,
		     GL_STATIC_DRAW);

	glGenBuffers(1, &obj->vbo_tex);
	glBindBuffer(GL_ARRAY_BUFFER, obj->vbo_tex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * numVerts, txVert,
		     GL_STATIC_DRAW);

	glGenBuffers(1, &obj->vbo_norm);
	glBindBuffer(GL_ARRAY_BUFFER, obj->vbo_norm);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * numVerts, norms,
		     GL_STATIC_DRAW);

	obj->vert_attrib = sdrobj->vert_attrib;
	obj->tex_attrib = sdrobj->tex_attrib;
	obj->norm_attrib = sdrobj->norm_attrib;
	obj->mvp_uniform = sdrobj->mvp_uniform;
	obj->mv_uniform = sdrobj->mv_uniform;
	obj->tex_uniform = sdrobj->tex_uniform;
	obj->program = sdrobj->program;

}

void drawObj(struct obj_t *obj, kmMat4 * combined, kmMat4 * mv)
{
	glUseProgram(obj->program);

	glUniformMatrix4fv(obj->mvp_uniform, 1, GL_FALSE, (GLfloat *) combined);
	glUniformMatrix4fv(obj->mv_uniform, 1, GL_FALSE, (GLfloat *) mv);

	glUniform1i(obj->tex_uniform, 0);

	glEnableVertexAttribArray(obj->vert_attrib);
	glBindBuffer(GL_ARRAY_BUFFER, obj->vbo_vert);
	glVertexAttribPointer(obj->vert_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(obj->norm_attrib);
	glBindBuffer(GL_ARRAY_BUFFER, obj->vbo_norm);
	glVertexAttribPointer(obj->norm_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(obj->tex_attrib);
	glBindBuffer(GL_ARRAY_BUFFER, obj->vbo_tex);
	glVertexAttribPointer(obj->tex_attrib, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glDrawArrays(GL_TRIANGLES, 0, obj->num_verts);

	glDisableVertexAttribArray(obj->tex_attrib);
	glDisableVertexAttribArray(obj->vert_attrib);
	glDisableVertexAttribArray(obj->norm_attrib);

}
