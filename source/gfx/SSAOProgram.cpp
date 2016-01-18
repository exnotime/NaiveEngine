#include "SSAOProgram.h"
#include "ShaderBank.h"
using namespace gfx;

float RandfUNORM() {
	return (rand() % RAND_MAX) / float(RAND_MAX);
}
float RandfNORM() {
	return (rand() % RAND_MAX) / float(RAND_MAX) * 2.0f - 1.0f;
}

SSAOProgram::SSAOProgram() {

}
SSAOProgram::~SSAOProgram() {
	delete[] m_Kernel;
	delete m_BlurProgram;
}

void SSAOProgram::Initialize(int width, int height, int kernelSize, int noiseSize) {
	m_Size = glm::vec2(width, height);
	m_SSAOShader = g_ShaderBank.LoadShaderProgram("shader/SSAO.glsl");
	//create kernel
	m_KernelSize = kernelSize;
	m_Kernel = new glm::vec3[kernelSize];
	for (int i = 0; i < kernelSize; ++i) {
		m_Kernel[i] = glm::vec3(RandfNORM(), RandfNORM(), RandfUNORM());
		m_Kernel[i] = glm::normalize(m_Kernel[i]);
		//normalize distrobution
		float scale = float(i) / float(kernelSize);
		scale = glm::mix(0.1f, 1.0f, scale * scale);
		m_Kernel[i] *= scale;
	}
	glm::vec3* noise = new glm::vec3[noiseSize * noiseSize];
	//create noise texture
	for (int i = 0; i < (noiseSize * noiseSize); ++i) {
		noise[i] = glm::vec3(RandfNORM(), RandfNORM(), 0.0f);
		noise[i] = glm::normalize(noise[i]);
	}
	glGenTextures(1, &m_NoiseTexture);
	glBindTexture(GL_TEXTURE_2D, m_NoiseTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, noiseSize, noiseSize, 0, GL_RGB, GL_FLOAT, noise);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_WRAP_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_WRAP_BORDER);
	delete[] noise;
	//create ssao texture
	glGenTextures(1, &m_SSAOTex);
	glBindTexture(GL_TEXTURE_2D, m_SSAOTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width, height, 0, GL_RED, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	m_BlurProgram = new BlurProgram();
	m_BlurProgram->Initlialize();
	m_BlurProgram->SetTargetTexture(m_SSAOTex, 1.0f);
}
void SSAOProgram::Render(GBuffer* gbuffer, RenderQueue* rq) {
	CameraData cam = rq->GetViews()[0].camera;
	ShaderProgram* prog = g_ShaderBank.GetProgramFromHandle(m_SSAOShader);
	prog->Apply();
	const int WORK_GROUP_SIZE = 16;
	GLuint WorkGroupSizeX = GLuint((m_Size.x + WORK_GROUP_SIZE - 1) / float(WORK_GROUP_SIZE));
	GLuint WorkGroupSizeY = GLuint((m_Size.y + WORK_GROUP_SIZE - 1) / float(WORK_GROUP_SIZE));
	prog->SetUniformTextureHandle("gDepthTex", gbuffer->GetTexture(GBUFFER_TEX::DEPTH_SENCIL32), 1);
	prog->SetUniformTextureHandle("gNormalTex", gbuffer->GetTexture(GBUFFER_TEX::NORMAL16), 2);
	prog->SetUniformTextureHandle("gNoiseTex", m_NoiseTexture, 3);
	prog->SetUniformVec2("gScreenSize", m_Size);
	prog->SetUniformFloat("gTanHalfFOV", tan(cam.Fov * 0.5f));
	prog->SetUniformFloat("gAspectRatio", m_Size.x / m_Size.y);
	prog->SetUniformMat4("gProj", cam.Proj);
	prog->SetUniformMat4("gView", cam.ProjView);
	prog->SetUniformUInt("gKernelSize", m_KernelSize);
	glUniform3fv(prog->FetchUniform("gKernel"), m_KernelSize, (float*)m_Kernel);
	glBindImageTexture(0, m_SSAOTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R8);
	glDispatchCompute(WorkGroupSizeX, WorkGroupSizeY, 1);
	m_BlurProgram->Render(2);
}

GLuint SSAOProgram::GetTexture() {
	return m_BlurProgram->GetBluredTexture();
}