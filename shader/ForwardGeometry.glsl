#new_shader vertex
#version 430 core
#extension GL_ARB_shader_draw_parameters : require
layout (location = 0) in vec3 PosL;
layout (location = 1) in vec3 NormalL;
layout (location = 2) in vec2 TexIn;
layout (location = 3) in vec3 TangentL;

layout (location = 0) out vec3 PosW;
layout (location = 1) out vec3 NormalW;
layout (location = 2) out vec2 TexCoord;
layout (location = 3) out vec3 TangentW;
layout (location = 4) out vec4 Color;
layout (location = 5) flat out uint MatIDOut;

struct ShaderInput{
		mat4 World;
		vec4 Color;
		uint MatID;
};
//per frame
layout(std140, binding = 0) uniform SceneInputBuffer{
    mat4 g_View;
    mat4 g_ViewProj;
    vec3 g_CamPos;
};
//per object 
layout(std430, binding = 1) readonly buffer ShaderInputBuffer{
    ShaderInput g_Inputs[];
};
layout(std430, binding = 2) readonly buffer Offsetsbuffer{
    uint g_DrawOffsets[]; //per drawcall offset
};
uniform uint g_DrawCallCount;
void main(){
	uint drawIndex = (gl_DrawIDARB + g_DrawCallCount) + g_DrawOffsets[(gl_DrawIDARB + g_DrawCallCount)] + gl_InstanceID;
	
	ShaderInput input = g_Inputs[drawIndex];
	PosW = (input.World * vec4(PosL, 1)).xyz;
	NormalW = (input.World * vec4(NormalL, 0)).xyz;
	TangentW = (input.World * vec4(TangentL, 0)).xyz;
	Color = input.Color;
	MatIDOut = input.MatID;
	gl_Position = g_ViewProj * vec4(PosW, 1);
}

#end_shader

#new_shader fragment
#version 430 core
#extension GL_ARB_bindless_texture : require
layout (location = 0) in vec3 PosW;
layout (location = 1) in vec3 NormalW;
layout (location = 2) in vec2 TexCoord;
layout (location = 3) in vec3 TangentW;
layout (location = 4) in vec4 Color;
layout (location = 5) flat in uint MatIDOut;

layout(location = 0) out vec3 FragmentColor; //HDR Color
layout(location = 1) out vec3 FragmentViewNormal; //view space normals

struct SurfaceMaterial{
	uint Albedo;
	uint Normal;
	uint Roughnes;
	uint Metallic;
};
//per frame
layout(std140, binding = 0) uniform SceneInputBuffer{
    mat4 g_View;
    mat4 g_ViewProj;
    vec3 g_CamPos;
};
#define MAX_MATERIALS 1000
layout(std430, binding = 1) readonly buffer MaterialBuffer{
	SurfaceMaterial g_Materials[MAX_MATERIALS];
	uvec2 g_Textures[MAX_MATERIALS];
};

#define MAX_POINTLIGHTS 64 //forward rendering is not as nice with number of lights :(
#define MAX_DIRLIGHTS 4
#define MAX_NUM_LIGHTS MAX_POINTLIGHTS + MAX_DIRLIGHTS

#include PBR.txt

layout(std430, binding = 3) readonly buffer LightBuffer {
	Light lights[MAX_NUM_LIGHTS];
};
layout(binding = 0) uniform sampler2D	g_BRDFTex;
layout(binding = 1) uniform samplerCube g_SkyRadianceTex;
layout(binding = 2) uniform samplerCube g_SkyIrradianceTex;

uniform uint g_NumDirLights;
uniform uint g_NumPointLights;
void main(){
	//normals for SSAO/SSR etc
	FragmentViewNormal = (g_View * vec4(NormalW.xyz, 0)).xyz;

	//Get Material
	SurfaceMaterial mat = g_Materials[MatIDOut];
	vec4 albedoColor = texture(sampler2D(g_Textures[mat.Albedo]), TexCoord);
	float roughnes = texture(sampler2D(g_Textures[mat.Roughnes]), TexCoord).r;
	float metallic = texture(sampler2D(g_Textures[mat.Metallic]), TexCoord).r;
	vec4 lightColor = vec4(0);
	uint i;
	for(i = 0; i < g_NumPointLights; ++i){
		Light p = lights[i];
		lightColor += CalcPLight(p, NormalW, PosW, g_CamPos, albedoColor.rgb, roughnes, metallic );
	}
	for(i = 0; i < g_NumDirLights; ++i){
		Light d = lights[MAX_POINTLIGHTS + i];
		lightColor += CalcDLight(d, NormalW, PosW, g_CamPos, albedoColor.rgb, roughnes, metallic );
	}
	//IBL
	lightColor += CalcIBLLight( NormalW, PosW, g_CamPos, albedoColor.rgb, roughnes, metallic, g_SkyRadianceTex, g_SkyIrradianceTex, g_BRDFTex);

	FragmentColor = lightColor.rgb;
}
#end_shader