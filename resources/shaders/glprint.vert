attribute vec3		vert_attrib;
attribute vec2		uv_attrib;

uniform mat4		opm_uniform;

varying vec2		v_frag_uv;

void main(void) {

	v_frag_uv = uv_attrib;
    gl_Position = opm_uniform * vec4(vert_attrib,1);

}
