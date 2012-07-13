attribute vec3		vertex_attrib;

uniform mat4		mvp_uniform;

uniform float		u_point_size;

void main(void) {

	gl_PointSize = u_point_size;
	gl_Position = mvp_uniform * vec4(vertex_attrib,1);

}
