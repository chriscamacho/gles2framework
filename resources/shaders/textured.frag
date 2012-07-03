uniform sampler2D u_texture;

uniform vec3 u_lightDir;
uniform vec3 u_viewDir;

varying vec3 v_Position;
varying vec3 v_Normal;
varying vec2 v_frag_uv; 


void main() {
	vec4 baseColour = texture2D(u_texture,v_frag_uv);
	float nDOTl = dot(v_Normal,u_lightDir);
	vec3 reflect = (2.0 * v_Normal * nDOTl) - u_lightDir;
	float rDOTv = max(0.0, dot(reflect,u_viewDir));
	
// these three vec4's could be material uniforms.	
	vec4 ambi = vec4(.7,.7,.7,0) * baseColour;	
	vec4 diffu = vec4(.8,.8,.8,0) * nDOTl * baseColour;
	
		
// spec strength (could be a material uniform) - TODO seems wrong?	
	vec4 specu = vec4(.9,.9,.9,0) * (rDOTv/99.);//pow(rDOTv,20);

	gl_FragColor = ambi + diffu + specu;
}
