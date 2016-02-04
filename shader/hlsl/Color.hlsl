#define MAX_OBJECTS 1000
struct InputData{
	float4x4 wvp;
};

ConstantBuffer<InputData> gShaderData : register(b0);

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

	result.position = mul(position, gShaderData.wvp);
	result.uv = uv.xy;

	return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
	return  float4(1,0,0,1);//g_texture.Sample(g_sampler, input.uv);
}
