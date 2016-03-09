#new_shader vertex
#version 330
void main()
{}
#end_shader
#new_shader geometry
#version 330 core
layout(points) in;
layout(triangle_strip, max_vertices = 4) out;
out vec2 texcoord;
void main()
{
	gl_Position = vec4( 1.0, 1.0, 0.0, 1.0 );
	texcoord = vec2( 1.0, 1.0 );
	EmitVertex();
	gl_Position = vec4(-1.0, 1.0, 0.0, 1.0 );
	texcoord = vec2( 0.0, 1.0 );
	EmitVertex();
	gl_Position = vec4( 1.0,-1.0, 0.0, 1.0 );
	texcoord = vec2( 1.0, 0.0 );
	EmitVertex();
	gl_Position = vec4(-1.0,-1.0, 0.0, 1.0 );
	texcoord = vec2( 0.0, 0.0 );
	EmitVertex();
	EndPrimitive();
}
#end_shader
#new_shader fragment
#version 430
in vec2 texcoord;
uniform sampler2D   InputTex;
uniform vec2 		ScreenSize;

out vec4 			FinalColor;

vec3 DecodeNormal(vec4 enc){
    vec2 fenc = enc.xy*4-2;
    float f = dot(fenc,fenc);
    float g = sqrt(1-f/4);
    vec3 n;
    n.xy = fenc*g;
    n.z = 1-f/2;
    return n;
}
void main()
{
	//vec4 color = texture(InputTex, texcoord);
	//FinalColor = vec4(Reinhard(color.xyz + Exposure),1);
	//FinalColor = pow(FinalColor, vec4(1.0 / 2.2));

	//if(useAA){
	//	FinalColor = PostFX(InputTex, texcoord);
	//}else {
	FinalColor = vec4(texture(InputTex, texcoord).xyz,1);
    
    //if( FinalColor.x > 0.1){
    //    FinalColor = vec4(1,0,0,1);
    //}

	//FinalColor = vec4(DecodeNormal(texture(InputTex, texcoord)) * 0.5 + 0.5,1);
}
#end_shader
