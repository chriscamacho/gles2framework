attribute vec3		vertex_attrib;
attribute vec2		uv_attrib;
attribute vec3		norm_attrib;

uniform mat4		mvp_uniform;
uniform mat4		mv_uniform;

varying vec2		v_frag_uv;
varying vec3		v_Position;
varying vec3		v_Normal;

void main(void) {

	v_frag_uv = uv_attrib;

    v_Position = vec3(mv_uniform * vec4(vertex_attrib,0));
    v_Normal = vec3(mv_uniform * vec4(norm_attrib, 0.0));
    gl_Position = mvp_uniform * vec4(vertex_attrib,1);

}
