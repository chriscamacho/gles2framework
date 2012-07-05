uniform sampler2D		texture_uniform;
varying vec2 v_frag_uv;
 

void main()
{
    gl_FragColor = texture2D(texture_uniform, v_frag_uv);

}
