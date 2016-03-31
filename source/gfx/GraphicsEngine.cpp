#include "GraphicsEngine.h"
#include <GL/glew.h>
#include <glm/gtx/transform.hpp>
#include "Camera.h"
#include "RenderQueue.h"
#include "ModelBank.h"
#include "ShaderBank.h"
#include "MaterialBank.h"
#include "Material.h"
#include "GBuffer.h"
#include "LightEngine.h"
#include "TerrainManager.h"
#include "BufferManager.h"
#include "SkyProgram.h"
#include "CascadedShadowMap.h"
#include "ShadowMap.h"
#include "TerrainDeformationProgram.h"
#include "DeferedDecalProgram.h"
#include "BloomProgram.h"
#include "TransparencyProgram.h"
#include "SSAOProgram.h"
#include "CascadedShadowMap.h"
#include "gfxutility.h"
using namespace gfx;

GraphicsEngine::GraphicsEngine() { }

GraphicsEngine::~GraphicsEngine() {
	SAFE_DELETE(m_RenderQueue);
	SAFE_DELETE(m_SkyProgram);
	SAFE_DELETE(m_BloomProgram);
	SAFE_DELETE(m_TransparencyProgram);
}

void GraphicsEngine::Initialize(const GraphicsSettings& settings) {
	m_GraphicsSettings = settings;

	glewInit();
	glEnable( GL_DEPTH_TEST );
	glDepthFunc( GL_LESS );
	glEnable( GL_CULL_FACE );
	glCullFace( GL_BACK );
	glFrontFace( GL_CCW );
	glEnable( GL_TEXTURE_2D );
	glEnable( GL_STENCIL_TEST );
	glEnable(GL_FRAMEBUFFER_SRGB);
	glStencilFunc( GL_ALWAYS, 1, 0xFF );
	glEnable( GL_TEXTURE_CUBE_MAP_SEAMLESS );

	glClearColor( 0.39f, 0.61f, 0.93f, 1.0f ); // cornflower blue
	//load default materials
	g_MaterialBank.Initialize();

	m_RenderQueue = new RenderQueue( );
	m_RenderQueue->CreateBuffer();
	// integrate ibl
	m_PreFilterIblCompute = g_ShaderBank.LoadShaderProgram( "shader/iblprefilter.glsl" );
	
	int iblTexSize = 256;
	m_IntegratedIBL = 0;
	glGenTextures( 1, &m_IntegratedIBL );
	glBindTexture( GL_TEXTURE_2D, m_IntegratedIBL );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RG16, iblTexSize, iblTexSize, 0, GL_RG, GL_FLOAT, nullptr );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glBindTexture( GL_TEXTURE_2D, 0 );

	ShaderProgram* prog = g_ShaderBank.GetProgramFromHandle( m_PreFilterIblCompute );
	prog->Apply();
	glBindImageTexture( 0, m_IntegratedIBL, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RG16 );
	const GLuint x = iblTexSize / 16;
	glDispatchCompute( x, x, 1 );
	glUseProgram( 0 );
	//FrameBuffer
	glGenFramebuffers(1, &m_FrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBuffer);
	glGenTextures(1, &m_ColorBuffer);
	glBindTexture(GL_TEXTURE_2D, m_ColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, m_GraphicsSettings.Width, m_GraphicsSettings.Height, 0, GL_RGB, GL_FLOAT, nullptr);
	glGenTextures(1, &m_NormalBuffer);
	glBindTexture(GL_TEXTURE_2D, m_NormalBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, m_GraphicsSettings.Width, m_GraphicsSettings.Height, 0, GL_RGB, GL_FLOAT, nullptr);
	glGenTextures(1, &m_DepthBuffer);
	glBindTexture(GL_TEXTURE_2D, m_DepthBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, m_GraphicsSettings.Width, m_GraphicsSettings.Height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_ColorBuffer, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, m_NormalBuffer, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, m_DepthBuffer, 0);
	if (!glCheckFramebufferStatus(GL_FRAMEBUFFER)) {
		printf("Error: Invalid framebuffer\n");
	}
	//Buffers
	g_BufferManager.CreateBuffer("IndirectDrawBuffer", GL_DRAW_INDIRECT_BUFFER, sizeof(DrawCall) * 1000, GL_DYNAMIC_COPY);
	g_BufferManager.CreateBuffer("SceneInputBuffer", GL_UNIFORM_BUFFER, sizeof(SceneInput), GL_DYNAMIC_DRAW);
	//Shaders
	m_ForwardGeometryShader = g_ShaderBank.LoadShaderProgram("shader/ForwardGeometry.glsl");
	// LightEngine
	g_LightEngine.Initialize();
	// Bind buffers to shader

	//bloom for post fx
	m_BloomProgram = new BloomProgram();
	m_BloomProgram->Initialize();
	//transparency
	m_TransparencyProgram = new TransparencyProgram();
	m_TransparencyProgram->Initialize(m_GraphicsSettings.Width, m_GraphicsSettings.Height);
	//sky
	m_SkyProgram = new SkyProgram();
	m_SkyProgram->Init();
	m_SkyProgram->SetSkyTexture("asset/sky/ame_iceflats.dds");
}

void GraphicsEngine::Deinitialize() {
	SAFE_DELETE( m_RenderQueue );
	SAFE_DELETE( m_SkyProgram );
	SAFE_DELETE( m_BloomProgram );
	SAFE_DELETE( m_TransparencyProgram );
}

void GraphicsEngine::Draw() {
	DrawGeometry();
	DrawPostFX();
	DrawTransparent();
	g_LightEngine.ClearDirectionalLights();
	g_LightEngine.ClearPointLights();
}

void GraphicsEngine::DrawGeometry() {
	m_RenderQueue->UpdateBuffer();
	//Geometry
	glEnable( GL_CULL_FACE );
	glCullFace( GL_BACK );
	glEnable( GL_DEPTH_TEST );
	glDisable( GL_BLEND );
	glEnable( GL_STENCIL_TEST );
	//build indirectdraw/offset buffer
	unsigned int instanceCount = 0;
	std::vector<unsigned int> offsets;
	std::vector<DrawCall> draws;
	DrawCall dc;
	dc.BaseInstance = 0; 
	dc.BaseVertex = 0;
	for (auto& model : m_RenderQueue->GetModelQueue()) {
		Model m = g_ModelBank.FetchModel(model.Model);
		for (auto& mesh : m.Meshes) {
			dc.Count = mesh.Indices;
			dc.FirstIndex = m.IndexHandle + mesh.IndexBufferOffset;
			dc.InstanceCount = model.InstanceCount;
			draws.push_back(dc);
			offsets.push_back(instanceCount);
			instanceCount += model.InstanceCount -1;
		}
	}
	g_BufferManager.UpdateBuffer("IndirectDrawBuffer", 0, draws.data(), sizeof(DrawCall) * draws.size());
	g_BufferManager.UpdateBuffer("Offsetsbuffer", 0, offsets.data(), sizeof(unsigned int) * offsets.size());
	for ( auto& view : m_RenderQueue->GetViews() ) {
		glViewport( ( GLint )view.viewport.x, ( GLint )view.viewport.y, ( GLsizei )view.viewport.width, ( GLsizei )view.viewport.height );
		// Render Sky
		m_SkyProgram->Render( view.camera );

		g_ModelBank.ApplyBuffers();
		// set per frame variables
		ShaderProgram* prog = g_ShaderBank.GetProgramFromHandle( 0 );
		prog->Apply();
		SceneInput si;
		si.View = view.camera.View;
		si.ViewProj = view.camera.ProjView;
		si.CamPos = view.camera.Position;
		g_BufferManager.UpdateBuffer("SceneInputBuffer", 0, &si, sizeof(SceneInput));

	}
	glViewport( 0, 0, m_GraphicsSettings.Width, m_GraphicsSettings.Height );
}


void GraphicsEngine::DrawPostFX() {
	m_BloomProgram->Render();
	//m_GBuffer->ApplyPostProcessStage();
	//m_GBuffer->ClearScreen();
	glDisable( GL_DEPTH_TEST );
	//ShaderProgram* prog = g_ShaderBank.GetProgramFromHandle(m_FXAAShader);
	//prog->Apply();
	//prog->SetUniformTextureHandle("InputTex", m_GBuffer->GetTexture(GBUFFER_TEX::COLLECTIVE24), 0);
	//prog->SetUniformTextureHandle("InputTex", m_BloomProgram->GetFinalTexture(), 0);
	//prog->SetUniformTextureHandle("InputTex", m_SSAOProgram->GetTexture(), 0);
	//prog->SetUniformVec2("ScreenSize", glm::vec2(m_GraphicsSettings.Width, m_GraphicsSettings.Height));
	static bool useAA = true;
	//prog->SetUniformBool("useAA", useAA);
	glBindVertexArray( 0 );
	glDrawArrays( GL_POINTS, 0, 1 );
	glEnable( GL_DEPTH_TEST );
	glUseProgram( 0 );
}

void GraphicsEngine::DrawTransparent() {
	//m_TransparencyProgram->Render(m_RenderQueue, m_GBuffer);
}