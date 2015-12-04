#include "ParticleSystem.h"
#include "BufferManager.h"
#include "OGLTimer.h"
using namespace gfx;

ParticleSystem::ParticleSystem() {

}
ParticleSystem::~ParticleSystem() {
	delete m_UpdateShader;
	delete m_RenderShader;
}
void ParticleSystem::Init() {
	//compile shader
	m_UpdateShader = new ShaderProgram();
	m_UpdateShader->LoadCompleteShaderProgramFromFile("shader/ParticleUpdate.glsl", true);
	m_RenderShader = new ShaderProgram();
	m_RenderShader->LoadCompleteShaderProgramFromFile("shader/ParticleRender.glsl", true);

	glGenTextures(1, &m_RandTex);
	glBindTexture(GL_TEXTURE_2D, m_RandTex);
	unsigned char texture[RANDTEXSIZE];
	for (int i = 0; i < RANDTEXSIZE; i++) {
		texture[i] = rand() % 256;
	}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, 1024, 1, 0, GL_RED, GL_UNSIGNED_BYTE, texture);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	m_ParticleTexture = new Texture();
	m_ParticleTexture->Init("asset/texture/Snow.png", TEXTURE_COLOR);
	//buffer to update particles
	g_BufferManager.CreateBuffer("ParticleBuffer", GL_SHADER_STORAGE_BUFFER, PARTICLE_COUNT * sizeof(Particle), GL_DYNAMIC_DRAW);
	g_BufferManager.BindBufferToProgram("ParticleBuffer", m_UpdateShader, 4);
	//buffer to be the particles vertex buffer
	g_BufferManager.CreateBuffer("ParticleVertexBuffer", GL_SHADER_STORAGE_BUFFER, PARTICLE_COUNT * sizeof(Particle), GL_DYNAMIC_DRAW);
	g_BufferManager.BindBufferToProgram("ParticleVertexBuffer", m_UpdateShader, 5);
	//buffer to contain the indirect draw call
	g_BufferManager.CreateBuffer("IndirectBuffer", GL_SHADER_STORAGE_BUFFER, PARTICLE_COUNT * sizeof(Particle), GL_DYNAMIC_DRAW);
	g_BufferManager.BindBufferToProgram("IndirectBuffer", m_UpdateShader, 6);
	//set up vertex array
	GLuint buffer = g_BufferManager.GetBuffer("ParticleVertexBuffer");
	glGenVertexArrays(1, &m_VertexArray);

	glBindVertexArray(m_VertexArray);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), nullptr);
}
void ParticleSystem::Update(float deltatime) {
	m_Time += deltatime;
	m_UpdateShader->Apply();
	m_UpdateShader->SetUniformVec3("g_Position", glm::vec3(0, 1, -10));
	m_UpdateShader->SetUniformVec3("g_Size", glm::vec3(100, 100, 100));
	m_UpdateShader->SetUniformFloat("g_DeltaTime", deltatime);
	m_UpdateShader->SetUniformFloat("g_Time", m_Time);
	m_UpdateShader->SetUniformUInt("g_RandOffset", rand() % RANDTEXSIZE);
	glUniform1i(m_UpdateShader->FetchUniform("g_RandomTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_RandTex);
	GLuint WorkGroupSize = GLuint((PARTICLE_COUNT + 32 - 1) / float(32));
	OGLTimer timer;
	timer.Start();
	glDispatchCompute(WorkGroupSize, 1, 1);
	m_UpdateTime = timer.Stop();
}

void ParticleSystem::Render(RenderQueue* rq) {
	m_RenderShader->Apply();
	//glDisable(GL_CULL_FACE);
	CameraData camera = rq->GetViews()[0].camera;
	m_RenderShader->SetUniformMat4("g_ViewProj", camera.ProjView);
	m_RenderShader->SetUniformVec3("g_CamRight", camera.Right);
	m_RenderShader->SetUniformVec3("g_CamUP", camera.Up);
	m_RenderShader->SetUniformFloat("g_ZNear", camera.Near);
	m_RenderShader->SetUniformFloat("g_ZFar", camera.Far);
	m_RenderShader->SetUniformTextureHandle("g_Texture", m_ParticleTexture->GetHandle(), 1);
	glBindBuffer(GL_ARRAY_BUFFER, g_BufferManager.GetBuffer("ParticleBuffer"));
	glBindVertexArray(m_VertexArray);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, g_BufferManager.GetBuffer("IndirectBuffer"));
	OGLTimer timer;
	timer.Start();
	glDrawArraysIndirect(GL_POINTS, 0);
	m_RenderTime = timer.Stop();
}

void ParticleSystem::GetTimes(double& update, double& render) {
	update = m_UpdateTime;
	render = m_RenderTime;
}