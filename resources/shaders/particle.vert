attribute vec3		vertex_attrib;

uniform mat4		mvp_uniform;

void main(void) {

	gl_PointSize = 32.0;
	gl_Position = mvp_uniform * vec4(vertex_attrib,1);

}
