#if (defined(__FOR_RPi_noX__) || defined(__FOR_RPi__))
#include  <GLES2/gl2.h>
#endif
#ifdef __FOR_GLFW__
#include <GL/glfw.h>
#endif

struct obj_t {
    GLuint vbo_vert, vbo_tex, vbo_norm;
    GLint vert_attrib, tex_attrib, norm_attrib;
    GLint mvp_uniform, mv_uniform, tex_uniform;
    GLint lightDir_uniform, viewDir_uniform;
    int num_verts;
    GLuint program;
};

int createObj(struct obj_t *obj, int numVerts, float verts[], float txVert[],
              float norms[], char *vertShader, char *fragShader);
int createObjCopyShader(struct obj_t *obj, int numVerts, float verts[],
                        float txVert[], float norms[], struct obj_t *sdrobj);
void drawObj(struct obj_t *obj, kmMat4 * combined, kmMat4 * mv, kmVec3 lightDir, kmVec3 viewDir);

int loadObj(struct obj_t *obj,const char *objFile, char *vert, char *frag);
int loadObjCopyShader(struct obj_t *obj,const char *objFile, struct obj_t *sdrobj);
