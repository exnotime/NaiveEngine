#pragma once
#include "RenderQueue.h"
#include "ShaderProgram.h"
#include "Camera.h"
#include "GBuffer.h"
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
	const int PARTICLE_COUNT = 4096 * 64;

	class ParticleSystem {
	public:
		ParticleSystem();
		~ParticleSystem();
		void Init();
		void Update(float deltatime, RenderQueue* rq);
		void Render(RenderQueue* rq, GBuffer* gbuffer);
		void GetTimes(double& update, double& render);
		void ToogleCulling();
		bool TestFrustum(glm::vec4* pl, glm::vec4 p, glm::mat4 m);
		void SaveTest(bool useculling);
	private:
		GLuint m_RandTex;
		ShaderProgram* m_UpdateShader;
		ShaderProgram* m_UpdateWithoutCullingShader;
		ShaderProgram* m_RenderShader;
		GLuint m_VertexArray;
		GLuint m_VertexArrayNoCulling;
		GLuint m_IndirectBuffer;
		GLuint m_AtomicBuffer;
		Texture* m_ParticleTexture;
		Camera* m_Camera;
		float m_Time = 0;

		bool m_UseCulling;
		int m_TestCounter = 0;
		double m_UpdateTime, m_RenderTime;
		std::vector<double> m_Updatetimes, m_RenderTimes;
	};
}