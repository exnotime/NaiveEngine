#pragma once
#include "RenderQueue.h"
#include "ShaderProgram.h"
#include "Camera.h"
#define PARTICLE_COUNT 16384
#define RANDTEXSIZE 4096
namespace gfx {
	struct IndirectDrawCall {
		GLuint VertexCount;
		GLuint InstanceCount;
		GLuint Offset;
		GLuint reservedMustBeZero = 0;
	};

	struct Particle {
		glm::vec4 Position;
		glm::vec4 Velocity;
	};

	class ParticleSystem {
	public:
		ParticleSystem();
		~ParticleSystem();
		void Init();
		void Update(float deltatime, RenderQueue* rq);
		void Render(RenderQueue* rq);
		void GetTimes(double& update, double& render);
	private:
		GLuint m_RandTex;
		ShaderProgram* m_UpdateShader;
		ShaderProgram* m_RenderShader;
		GLuint m_VertexArray;
		GLuint m_IndirectBuffer;
		GLuint m_AtomicBuffer;
		Texture* m_ParticleTexture;
		Camera* m_Camera;
		float m_Time = 0;
		double m_UpdateTime, m_RenderTime;
	};
}