uniform sampler2D		texture_uniform;
uniform float cx,cy;
varying vec2 v_frag_uv;
 

void main()
{
	float x=cx+v_frag_uv.x;
	float y=cy+v_frag_uv.y;
    gl_FragColor = texture2D(texture_uniform, vec2(x,y));
}
