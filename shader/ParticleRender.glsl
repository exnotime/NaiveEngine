#new_shader vertex
#version 430
in vec4 inPosition;
out vec4 Position;
void main(){
	Position = inPosition;
}
#end_shader

#new_shader geometry
#version 430
layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

in vec4 Position[];
out vec2 TexCoord;
uniform mat4 g_ViewProj;
uniform vec3 g_CamRight;
uniform vec3 g_CamUP;
#define PARTICLE_SIZE 0.5

void main(){
	vec4 pos = vec4(Position[0].xyz - (g_CamRight + g_CamUP) * PARTICLE_SIZE,1);
	gl_Position = g_ViewProj * vec4(pos.xyz,1);
	TexCoord = vec2(1, 1);
	EmitVertex();
	pos = vec4(Position[0].xyz - (g_CamRight - g_CamUP) * PARTICLE_SIZE,1);
	gl_Position = g_ViewProj * vec4(pos.xyz,1);
	TexCoord = vec2(0, 1);
	EmitVertex();
	pos = vec4(Position[0].xyz + (g_CamRight - g_CamUP) * PARTICLE_SIZE,1);
	gl_Position = g_ViewProj * vec4(pos.xyz,1);
	TexCoord = vec2(1, 0);
	EmitVertex();
	pos = vec4(Position[0].xyz + (g_CamRight + g_CamUP) * PARTICLE_SIZE,1);
	gl_Position = g_ViewProj * vec4(pos.xyz,1);
	TexCoord = vec2(0, 0);
	EmitVertex();
	EndPrimitive();
}
#end_shader

#new_shader fragment
#version 430
layout(location = 0) out vec4 acum;
layout(location = 1) out vec4 revealage;

in vec2 TexCoord;

uniform float g_ZNear;
uniform float g_ZFar;
uniform sampler2D g_Texture;
uniform sampler2D g_DepthBuffer;

float d(float z){
	return ((g_ZNear * g_ZFar) / (z - g_ZFar)) / (g_ZNear - g_ZFar);
}
float w(float z, float alpha){
	return alpha * max(0.01, 3 * 1000 * pow((1.0 - d(z)), 3) );
}
void main(){
	if(gl_FragCoord.z > texelFetch(g_DepthBuffer, ivec2(gl_FragCoord.xy), 0).r){
		discard;
	}
	vec4 color = texture(g_Texture, TexCoord);
	float alpha = 0.5f * color.a;

	acum = vec4(color.xyz, alpha) * w(gl_FragCoord.z, alpha);
	revealage = vec4(alpha);
}
#end_shader
