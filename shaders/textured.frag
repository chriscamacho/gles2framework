uniform sampler2D		texture_uniform;

vec3 u_LightPos=vec3(0,1,2);       // The position of the light in eye space.
									// TODO should probably be a uniform
varying vec3 v_Position;
varying vec3 v_Normal;
varying vec2 v_frag_uv; 

void main() 
{
    vec3 lightVector = normalize(u_LightPos - v_Position);
    float diffuse = max(dot(v_Normal, lightVector), 0.1);
 
//    float distance = length(u_LightPos - v_Position);
//    diffuse = diffuse * (1.0 / (1.0 + (0.25 * distance * distance)));
 
    gl_FragColor = texture2D(texture_uniform, v_frag_uv) * diffuse;
}
