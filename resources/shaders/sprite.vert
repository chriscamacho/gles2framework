attribute vec3		vert_attrib;
attribute vec2		uv_attrib;

uniform mat4		opm_uniform;
uniform vec2		u_size;

varying vec2		v_frag_uv;

void main(void) {

	v_frag_uv = uv_attrib;
	vec4 vert_pos;
	vert_pos = vec4(vert_attrib,1);
	vert_pos.x = vert_pos.x * u_size.x;
	vert_pos.y = vert_pos.y * u_size.y;
	
    gl_Position = opm_uniform * vert_pos;

}
