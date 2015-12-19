#include "ParticleSystem.h"
#include "BufferManager.h"
#include "OGLTimer.h"
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtx/transform.hpp>
#include <fstream>
using namespace gfx;

ParticleSystem::ParticleSystem() {

}
ParticleSystem::~ParticleSystem() {
	delete m_UpdateShader;
	delete m_RenderShader;
	delete m_Camera;
	delete m_ParticleTexture;
	delete m_UpdateWithoutCullingShader;
}

void ParticleSystem::Init() {
	m_UseCulling = true;
	//compile shaders
	m_UpdateShader = new ShaderProgram();
	m_UpdateShader->LoadCompleteShaderProgramFromFile("shader/ParticleUpdate.glsl", true);
	m_UpdateWithoutCullingShader = new ShaderProgram();
	m_UpdateWithoutCullingShader->LoadCompleteShaderProgramFromFile("shader/ParticleUpdateWithoutCulling.glsl", true);
	m_RenderShader = new ShaderProgram();
	m_RenderShader->LoadCompleteShaderProgramFromFile("shader/ParticleRender.glsl", true);
	//random texture
	glGenTextures(1, &m_RandTex);
	glBindTexture(GL_TEXTURE_2D, m_RandTex);
	unsigned char texture[RANDTEXSIZE];
	for (int i = 0; i < RANDTEXSIZE; i++) {
		texture[i] = rand() % 256;
	}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, RANDTEXSIZE, 1, 0, GL_RED, GL_UNSIGNED_BYTE, texture);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//particle texture
	m_ParticleTexture = new Texture();
	m_ParticleTexture->Init("asset/texture/Snow.png", TEXTURE_COLOR);
	//buffer to update particles
	g_BufferManager.CreateBuffer("ParticleBuffer", GL_SHADER_STORAGE_BUFFER, PARTICLE_COUNT * sizeof(Particle), GL_DYNAMIC_DRAW);
	//set intial vlaues of particles
	Particle* data = (Particle*)malloc(sizeof(Particle) * PARTICLE_COUNT);
	Particle part;
	part.Position = glm::vec4(0, -21, 0, 1);
	part.Velocity = glm::vec4(0, -1, 0, 0);
	for (int i = 0; i < PARTICLE_COUNT; i++) {
		data[i] = part;
	}
	g_BufferManager.UpdateBuffer("ParticleBuffer", 0, data, PARTICLE_COUNT * sizeof(Particle));
	free(data);
	g_BufferManager.BindBufferWithRange("ParticleBuffer", m_UpdateShader, 4, PARTICLE_COUNT * sizeof(Particle));
	g_BufferManager.BindBufferWithRange("ParticleBuffer", m_UpdateWithoutCullingShader, 4, PARTICLE_COUNT * sizeof(Particle));

	g_BufferManager.CreateBuffer("ParticleVertexBuffer", GL_SHADER_STORAGE_BUFFER, PARTICLE_COUNT * sizeof(glm::vec4), GL_DYNAMIC_DRAW);
	g_BufferManager.BindBufferWithRange("ParticleVertexBuffer", m_UpdateShader, 5, PARTICLE_COUNT * sizeof(glm::vec4));
	g_BufferManager.BindBufferWithRange("ParticleVertexBuffer", m_UpdateWithoutCullingShader, 5, PARTICLE_COUNT * sizeof(glm::vec4));
	//set up vertex array
	glGenVertexArrays(1, &m_VertexArray);
	glBindVertexArray(m_VertexArray);
	glBindBuffer(GL_ARRAY_BUFFER, g_BufferManager.GetBuffer("ParticleVertexBuffer"));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), nullptr);
	glGenVertexArrays(1, &m_VertexArrayNoCulling);
	glBindVertexArray(m_VertexArrayNoCulling);
	glBindBuffer(GL_ARRAY_BUFFER, g_BufferManager.GetBuffer("ParticleBuffer"));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), nullptr);

	//combined atomic and indirect buffer
	glGenBuffers(1, &m_AtomicBuffer);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, m_AtomicBuffer);
	glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(IndirectDrawCall), nullptr, GL_DYNAMIC_DRAW);
	glBindBufferRange(GL_ATOMIC_COUNTER_BUFFER, 6, m_AtomicBuffer, 0, sizeof(IndirectDrawCall));
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);

	g_BufferManager.CreateBuffer("ViewPlaneBuffer", GL_UNIFORM_BUFFER, sizeof(glm::vec4) * 6, GL_DYNAMIC_DRAW);
	g_BufferManager.BindBufferToProgram("ViewPlaneBuffer", m_UpdateShader, 7);

	m_Camera = new Camera();
	m_Camera->CalculateViewProjection();
}
void ParticleSystem::Update(float deltatime, RenderQueue* rq) {
	//update atomic
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, m_AtomicBuffer);
	IndirectDrawCall v;
	v.InstanceCount = 1;
	v.Offset = 0;
	v.reservedMustBeZero = 0;
	v.VertexCount = (m_UseCulling == true) ? 0 : PARTICLE_COUNT;
	glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(IndirectDrawCall), &v);
	//View frustum plane extraction
	glm::mat4 vp = rq->GetViews()[0].camera.ProjView;
	//glm::mat4 vp = m_Camera->GetViewProjection();
	glm::vec4 row1 = glm::row(vp, 0);
	glm::vec4 row2 = glm::row(vp, 1);
	glm::vec4 row3 = glm::row(vp, 2);
	glm::vec4 row4 = glm::row(vp, 3);
	glm::vec4 planes[6];
	planes[0] = row4 + row1;
	planes[1] = row4 - row1;
	planes[2] = row4 + row2;
	planes[3] = row4 - row2;
	planes[4] = row4 + row3;
	planes[5] = row4 - row3;
	for (int i = 0; i < 6; ++i) {
		planes[i] = glm::normalize(planes[i]);
	}
	g_BufferManager.UpdateBuffer("ViewPlaneBuffer", 0, planes, sizeof(glm::vec4) * 6);
	TestFrustum(planes, glm::vec4(0, 0, -3, 1), vp);

	m_Time += deltatime;
	ShaderProgram* prog;
	if (m_UseCulling) {
		prog = m_UpdateShader;
	}
	else {
		prog = m_UpdateWithoutCullingShader;
	}
	prog->Apply();
	prog->SetUniformMat4("g_View", vp);
	prog->SetUniformVec3("g_Position", glm::vec3(0, -20, 0));
	prog->SetUniformVec3("g_Size", glm::vec3(100, 100, 100));
	prog->SetUniformFloat("g_DeltaTime", deltatime);
	prog->SetUniformFloat("g_Time", m_Time);
	prog->SetUniformUInt("g_RandOffset", rand() % RANDTEXSIZE);
	glUniform1i(prog->FetchUniform("g_RandomTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_RandTex);
	const int WORK_GROUP_SIZE = 256;
	GLuint WorkGroupSize = GLuint((PARTICLE_COUNT + WORK_GROUP_SIZE - 1) / float(WORK_GROUP_SIZE));
	OGLTimer timer;
	timer.Start();
	glDispatchCompute(WorkGroupSize, 1, 1);
	glMemoryBarrier(GL_ATOMIC_COUNTER_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);
	m_UpdateTime = timer.Stop();
	m_Updatetimes.push_back(m_UpdateTime);
}

void ParticleSystem::Render(RenderQueue* rq,GBuffer* gbuffer) {
	m_RenderShader->Apply();
	CameraData camera = rq->GetViews()[0].camera;
	m_RenderShader->SetUniformMat4("g_ViewProj", camera.ProjView);
	m_RenderShader->SetUniformVec3("g_CamRight", camera.Right);
	m_RenderShader->SetUniformVec3("g_CamUP", camera.Up);
	m_RenderShader->SetUniformFloat("g_ZNear", camera.Near);
	m_RenderShader->SetUniformFloat("g_ZFar", camera.Far);
	m_RenderShader->SetUniformTextureHandle("g_Texture", m_ParticleTexture->GetHandle(), 1);
	m_RenderShader->SetUniformTextureHandle("g_DepthBuffer", gbuffer->GetTexture(GBUFFER_TEX::DEPTH_SENCIL32), 2);

	GLuint buffer = g_BufferManager.GetBuffer((m_UseCulling == true) ? "ParticleVertexBuffer" : "ParticleBuffer");
	GLuint vertexarray = (m_UseCulling == true) ? m_VertexArray : m_VertexArrayNoCulling;
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBindVertexArray(vertexarray);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, m_AtomicBuffer);
	OGLTimer timer;
	timer.Start();
	glDrawArraysIndirect(GL_POINTS, 0);
	m_RenderTime = timer.Stop();
	m_RenderTimes.push_back(m_RenderTime);

	m_TestCounter++;
	if (m_TestCounter >= 20) {
		SaveTest(m_UseCulling);
		m_TestCounter = 0;
	}
}

void ParticleSystem::GetTimes(double& update, double& render) {
	update = m_UpdateTime;
	render = m_RenderTime;
}

void ParticleSystem::ToogleCulling() {
	m_UseCulling = !m_UseCulling;
}

bool ParticleSystem::TestFrustum(glm::vec4* pl, glm::vec4 p, glm::mat4 m) {
	//p = m * p;
	//p /= p.w;
	for (int i = 0; i < 6; ++i) {
		float d = glm::dot(p, pl[i]);
		if (d < 0) {
			return false;
		}
	}
	return true;
}

void ParticleSystem::SaveTest(bool useculling) {
	std::string renderfilename, updatefilename, combinedfilename;
	if (useculling) {
		renderfilename = "TestData/RenderTestData.txt";
		updatefilename = "TestData/UpdateTestData.txt";
		combinedfilename = "TestData/CombinedTestData.txt";
	} else {
		renderfilename = "TestData/RenderTestDataNoCulling.txt";
		updatefilename = "TestData/UpdateTestDataNoCulling.txt";
		combinedfilename = "TestData/CombinedTestDataNoCulling.txt";
	}

	std::ofstream fout;
	fout.open(updatefilename, std::ios_base::app);
	double time = 0;
	for (auto& t : m_Updatetimes) {
		time += t;
	}
	time /= m_Updatetimes.size();
	fout << time << std::endl;
	fout.close();

	time = 0;
	fout.open(renderfilename, std::ios_base::app);
	for (auto& t : m_RenderTimes) {
		time += t;
	}
	time /= m_RenderTimes.size();
	fout << time << std::endl;
	fout.close();

	time = 0;
	fout.open(combinedfilename, std::ios_base::app);
	for (int i = 0; i < m_Updatetimes.size(); i++) {
		time += m_Updatetimes[i];
		time += m_RenderTimes[i];
	}
	time /= m_RenderTimes.size();
	
	fout << time << std::endl;
	fout.close();

	m_Updatetimes.clear();
	m_RenderTimes.clear();
}