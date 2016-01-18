#pragma once
#include "GBuffer.h"
#include "ShaderProgram.h"
#include "RenderQueue.h"
#include "BlurProgram.h"

namespace gfx {
	class SSAOProgram {
	public:
		SSAOProgram();
		~SSAOProgram();
		void Initialize(int width, int height, int kernelSize, int noiseSize);
		void Render(GBuffer* gbuffer, RenderQueue* rq);
		GLuint GetTexture();
	private:
		GLuint m_NoiseTexture;
		GLuint m_SSAOTex;
		GLuint m_KernelSize;
		int m_SSAOShader;
		BlurProgram* m_BlurProgram;
		glm::vec3* m_Kernel;
		glm::vec2 m_Size;
		
	};
}