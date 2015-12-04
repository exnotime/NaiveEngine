#new_shader compute
#version 440
#define WORK_GROUP_SIZE 32
layout(local_size_x = WORK_GROUP_SIZE, local_size_y = WORK_GROUP_SIZE) in;
#pragma optionNV(fastmath on)
#pragma optionNV(ifcvt none)
#pragma optionNV(inline all)
#pragma optionNV(strict on)
#pragma optionNV(unroll all)

struct Particle{
	vec4 Position;
	vec4 Velocity;
};
struct IndirectDrawCall {
	uint VertexCount;
	uint InstanceCount;
	uint Offset;
	uint ReservedMustBeZero;
};

uniform vec3 g_Position;
uniform vec3 g_Size;
uniform sampler2D g_RandomTex;
uniform float g_DeltaTime;
uniform float g_Time;
uniform uint g_RandOffset = 0;
shared uint g_RandIndex = 0;
#define RANDTEXSIZE 1024
#define PARTICLE_COUNT 10000

layout(std430, binding = 4) buffer ParticleBuffer{
	Particle g_Particles[PARTICLE_COUNT];
};

layout(std430, binding = 5) buffer ParticleVertexBuffer{
	Particle g_ParticleVertexBuffer[PARTICLE_COUNT];
};

layout(std430, binding = 6) buffer IndirectBuffer{
	IndirectDrawCall g_DrawCall;
};

layout(binding = 7, offset = 0) uniform atomic_uint g_ParticleCounter;

float Randf(){
	float texcoord = float((atomicAdd(g_RandIndex,1) + g_RandOffset) % RANDTEXSIZE) / RANDTEXSIZE * cos(g_Time);
	return texture(g_RandomTex, vec2(texcoord,0)).x;
}

void SpawnParticle(uint i){
	vec3 pos = vec3(Randf(),Randf(),Randf());
	vec3 vel = vec3(Randf(),Randf(),Randf());
	pos = pos * vec3(2,1,2) - vec3(1,0,1);
	pos = g_Position + g_Size * pos;
	vel = vec3(0,-1,0) + (vel * 2 - 1) * vec3(0.3,0.1,0.3);
	g_Particles[i].Position = vec4(pos,1);
	g_Particles[i].Velocity = vec4(vel,0);
}

void main(){
	uint index = gl_GlobalInvocationID.x;
	if(g_Particles[index].Position.y < g_Position.y){
		SpawnParticle(index);
	}
	g_Particles[index].Position += g_Particles[index].Velocity * g_DeltaTime;
	g_ParticleVertexBuffer[index] = g_Particles[index];
	//Check frustum
	//write particle to vertex buffer
	barrier();
	if(index == 0){
		g_DrawCall.InstanceCount = 1;
		g_DrawCall.Offset = 0;
		g_DrawCall.VertexCount = PARTICLE_COUNT;
		g_DrawCall.ReservedMustBeZero = 0;
	}
}
#end_shader
