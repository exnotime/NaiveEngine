#new_shader compute
#version 440
#define WORK_GROUP_SIZE 256
layout(local_size_x = WORK_GROUP_SIZE) in;
#pragma optionNV(fastmath on)
#pragma optionNV(ifcvt none)
#pragma optionNV(inline all)
#pragma optionNV(strict on)
#pragma optionNV(unroll all)

struct Particle{
	vec4 Position;
	vec4 Velocity;
};

uniform vec3 g_Position;
uniform vec3 g_Size;
uniform sampler2D g_RandomTex;
uniform float g_DeltaTime;
uniform float g_Time;
uniform uint g_RandOffset = 0;
uniform uint g_Width;
shared uint g_RandIndex = 0;
#define RANDTEXSIZE 1024
#define PARTICLE_COUNT 16384 * 4

layout(std430, binding = 4) buffer ParticleBuffer{
	Particle g_Particles[PARTICLE_COUNT];
};

layout(std430, binding = 5) buffer ParticleVertexBuffer{
	Particle g_ParticleVertexBuffer[PARTICLE_COUNT];
};

layout(binding = 6, offset = 0) uniform atomic_uint g_ParticleCounter;

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
	uint i = gl_GlobalInvocationID.x;
	if(g_Particles[i].Position.y < g_Position.y){
		SpawnParticle(i);
	}
	g_Particles[i].Position += g_Particles[i].Velocity * g_DeltaTime;
	if(g_Particles[i].Position.z > 0){
		uint index = atomicCounterIncrement(g_ParticleCounter);
		g_ParticleVertexBuffer[index] = g_Particles[i];
	}
}
#end_shader
