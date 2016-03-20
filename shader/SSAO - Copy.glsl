#new_shader compute
#version 430
#define WORK_GROUP_SIZE 16
layout(local_size_x = WORK_GROUP_SIZE, local_size_y = WORK_GROUP_SIZE) in;

const int MAX_KERNEL_SIZE = 128;
uniform sampler2D gDepthTex;
uniform sampler2D gNormalTex;
uniform sampler2D gNoiseTex;
uniform float gTanHalfFOV;
uniform float gAspectRatio = 16.0 / 9.0;
uniform mat4 gProj;
uniform mat4 gViewProj;
uniform vec2 gScreenOffset = vec2(0,0);
uniform vec2 gScreenSize;
uniform uint gKernelSize;
uniform vec3 gKernel[MAX_KERNEL_SIZE];
uniform float gNear;
uniform float gFar;
uniform float gPower = 2.0;
uniform float gRadius = 2.0;
layout(rgba8, binding = 0) writeonly restrict uniform image2D output_img;

float ssao(in mat3 kernelBasis, in vec3 originPos, in float radius) {
	float occlusion = 0.0;
	for (int i = 0; i < gKernelSize; ++i) {
		//get sample position:
		vec3 samplePos = kernelBasis * gKernel[i];
		samplePos = samplePos * radius + originPos;

		//project sample position:
		vec4 offset = gProj * vec4(samplePos, 1.0);
		offset.xy /= offset.w; // only need xy
		offset.xy = offset.xy * 0.5 + 0.5; // scale/bias to texcoords

		//get sample depth:
		float sampleDepth = texture(gDepthTex, offset.xy).r * 2 - 1;
		//sampleDepth = -linearizeDepth(sampleDepth);

		float rangeCheck = smoothstep(0.0, 1.0, radius / abs(originPos.z - sampleDepth));
		occlusion += rangeCheck * step(sampleDepth, samplePos.z);
	}
	occlusion = 1.0 - (occlusion / float(gKernelSize));
	return pow(occlusion, gPower);
}

float GetDepth(vec2 uv){
	float d = texture(gDepthTex, uv).r;
	return gProj[3][2] / (2 * d -1 - gProj[2][2]);
}

void main(){
	ivec2 screenPos = ivec2(gScreenOffset) + ivec2(gl_GlobalInvocationID.xy);
	vec2 uv = vec2(screenPos) / vec2(gScreenSize - 1);

	vec2 csp = uv * 2 - 1;
	vec3 viewRay = vec3(csp.x * gTanHalfFOV * gAspectRatio, csp.y * gTanHalfFOV, 1.0);

	vec3 pos = viewRay * GetDepth(uv);
	float occlusion = 0;
	for(int i = 0; i < gKernelSize; ++i){

		vec3 samplePos = pos + gKernel[i];
		vec4 offset = gProj * vec4(samplePos,1);
		offset.xy / offset.w;
		offset = offset * 0.5 + 0.5;

		float sampleDepth = GetDepth(offset.xy);
		if(abs(pos.z - sampleDepth) < gRadius){
			occlusion += step(sampleDepth, samplePos.z);
		}
	}

	occlusion = 1.0 - (occlusion / float(gKernelSize));
	occlusion = pow(occlusion, gPower);


	//float depth = texture(gDepthTex, uv).r * 2 - 1;
	//depth = linearizeDepth(depth);
//
//	//vec3 originPos = viewRay * depth;
//
//	//vec3 normal = texture(gNormalTex, uv).xyz * 2 - 1;
//	//normal = (gViewProj * vec4(normal, 0.0)).xyz;
//	//normal = normalize(normal);
//
//	//vec2 noiseTexCoords = vec2(textureSize(gDepthTex, 0)) / 16.0f;
//	//noiseTexCoords *= uv;
//
//	//vec3 rvec = texture(gNoiseTex, noiseTexCoords).xyz;
//
//	//vec3 tangent = normalize(rvec - normal * dot(normal, rvec));
	//vec3 bitangent = cross(normal, tangent);
	//mat3 kernelBasis = mat3(tangent, bitangent, normal);
//
	//vec4 ret = vec4(ssao(kernelBasis, originPos, gRadius), 0.0, 0.0, 0.0);
	//vec4 ret = vec4(normal, 1);
	//vec4 ret = vec4(depth,depth,depth,1);
	vec4 ret = vec4(occlusion, 0, 0, 0);
	imageStore(output_img, screenPos, ret);

}
#end_shader
