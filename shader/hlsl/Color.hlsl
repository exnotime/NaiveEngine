#define MAX_OBJECTS 1000
struct InputData{
	float4x4 World;
	float4 Color;
};

StructuredBuffer<InputData> gShaderData : register(t1);

cbuffer PerFrameBuffer : register(b0){
	float4x4 g_ViewProj;
};

struct PSInput
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
};

Texture2D g_texture : register(t0);
SamplerState g_sampler : register(s0);

PSInput VSMain(float4 position : POSITION, float4 uv : TEXCOORD)
{
	PSInput result;

	result.position = mul(position, gShaderData[0].World);
	result.uv = uv.xy;

	return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
	return gShaderData[0].Color;//g_texture.Sample(g_sampler, input.uv);
}
