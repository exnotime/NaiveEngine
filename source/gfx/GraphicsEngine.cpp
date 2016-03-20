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
	SAFE_DELETE(m_GBuffer);
	SAFE_DELETE(m_SkyCubeTex);
	SAFE_DELETE(m_IrrCubeTex);
	//SAFE_DELETE(m_ShadowMap);
	SAFE_DELETE(m_SkyProgram);
	SAFE_DELETE(m_TerrainDeform);
	SAFE_DELETE(m_DecalProgram);
	SAFE_DELETE(m_BloomProgram);
	SAFE_DELETE(m_TransparencyProgram);
	//SAFE_DELETE(m_SSAOProgram);
	//SAFE_DELETE(m_CascadedShadowMap);
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
	// Shaders
	m_PreFilterIblCompute	= g_ShaderBank.LoadShaderProgram( "shader/iblprefilter.glsl" );
	m_DeferedLightShader	= g_ShaderBank.LoadShaderProgram( "shader/DeferedLight.glsl" );
	m_DeferedGeometryShader = g_ShaderBank.LoadShaderProgram( "shader/DeferedGeometry.glsl" );
	m_FXAAShader = g_ShaderBank.LoadShaderProgram( "shader/FXAA.glsl" );
	// Gbuffers
	m_GBuffer = new GBuffer( );
	m_GBuffer->Initialize( m_GraphicsSettings.Width, m_GraphicsSettings.Height );
	// integrate ibl
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
	// LightEngine
	g_LightEngine.Initialize();
	g_LightEngine.BindBuffer( m_DeferedLightShader );
	// Bind buffer
	g_BufferManager.BindBufferToProgram( "ShaderInputs", g_ShaderBank.GetProgramFromHandle( m_DeferedGeometryShader ), 0 );
	// ShadowMap
	//m_ShadowMap = new ShadowMap( );
	//m_ShadowMap->Init();
	//terrain deform
	m_TerrainDeform = new TerrainDeformationProgram( );
	m_TerrainDeform->Initialize();
	g_TerrainManager.Init();
	//decals
	m_DecalProgram = new DeferedDecalProgram();
	m_DecalProgram->Initialize();
	//bloom for post fx
	m_BloomProgram = new BloomProgram();
	m_BloomProgram->Initialize();
	m_BloomProgram->SetTargetTexture(m_GBuffer->GetTexture(GBUFFER_TEX::COLLECTIVE24));
	//transparency
	m_TransparencyProgram = new TransparencyProgram();
	m_TransparencyProgram->Initialize(m_GraphicsSettings.Width, m_GraphicsSettings.Height);
	//sky
	m_SkyCubeTex = new Texture();
	m_SkyCubeTex->Init("asset/sky/ame_iceflats_rad.dds", TEXTURE_CUBE);
	m_IrrCubeTex = new Texture();
	m_IrrCubeTex->Init("asset/sky/ame_iceflats_irr.dds", TEXTURE_CUBE);
	m_SkyProgram = new SkyProgram();
	m_SkyProgram->Init();
	m_SkyProgram->SetSkyTexture("asset/sky/ame_iceflats.dds");

	//m_SSAOProgram = new SSAOProgram();
	//m_SSAOProgram->Initialize(m_GraphicsSettings.Width * 0.5f, m_GraphicsSettings.Height * 0.5f, 32, 16);

	//m_CascadedShadowMap = new CascadedShadowMap();
	//m_CascadedShadowMap->Initialize();
}

void GraphicsEngine::Deinitialize() {
	SAFE_DELETE( m_RenderQueue );
	SAFE_DELETE( m_GBuffer );
	SAFE_DELETE( m_SkyCubeTex );
	SAFE_DELETE( m_IrrCubeTex );
	SAFE_DELETE( m_SkyProgram );
	SAFE_DELETE( m_ShadowMap );
	SAFE_DELETE( m_TerrainDeform );
	SAFE_DELETE( m_DecalProgram );
	SAFE_DELETE( m_BloomProgram );
	SAFE_DELETE( m_TransparencyProgram );
}

void GraphicsEngine::Draw() {
	DrawGeometry();
	DrawLight();
	DrawPostFX();
	DrawTransparent();
	g_LightEngine.ClearDirectionalLights();
	g_LightEngine.ClearPointLights();
}

void GraphicsEngine::DrawGeometry() {
	//Deform terrain
	m_TerrainDeform->Render(m_RenderQueue);
	//Shadows
	//temp hack
	//if(g_LightEngine.GetDirLightCount() > 0)
		//m_ShadowMap->SetLight( g_LightEngine.GetDirLightList().at( 0 ) );

	m_RenderQueue->UpdateBuffer();
	//m_ShadowMap->Render( m_RenderQueue );
	//Geometry
	m_GBuffer->ApplyGeometryStage();
	m_GBuffer->ClearScreen();
	glEnable( GL_CULL_FACE );
	glCullFace( GL_BACK );
	glEnable( GL_DEPTH_TEST );
	glDisable( GL_BLEND );
	glEnable( GL_STENCIL_TEST );
	for ( auto& view : m_RenderQueue->GetViews() ) {
		glViewport( ( GLint )view.viewport.x, ( GLint )view.viewport.y, ( GLsizei )view.viewport.width, ( GLsizei )view.viewport.height );
		// Render Sky
		m_SkyProgram->Render( view.camera );
		// Render Terrains
		//g_TerrainManager.RenderTerrains( m_RenderQueue, view.camera );
		g_ModelBank.ApplyBuffers();
		// set per frame variables
		ShaderProgram* prog = g_ShaderBank.GetProgramFromHandle( m_DeferedGeometryShader );
		prog->Apply();
		prog->SetUniformMat4( "g_ViewProj", view.camera.ProjView );
		//prog->SetUniformMat4("g_ViewProj", m_CascadedShadowMap->GetLightMatrices()[0]);
		unsigned int bufferOffset  = 0;
		unsigned int instanceCount = 0;

		// for each model to be rendered
		for ( auto& mo : m_RenderQueue->GetModelQueue() ) {
			const Model& model = g_ModelBank.FetchModel( mo.Model );
			instanceCount = mo.InstanceCount;
			prog->SetUniformUInt( "g_BufferOffset", bufferOffset );
			// for each mesh
			for ( auto& mesh : model.Meshes ) {
				// set textures
				Material* mat		   = g_MaterialBank.GetMaterial( model.MaterialOffset + mesh.Material );
				Texture*  albedoTex	   = g_MaterialBank.GetTexture( mat->GetAlbedoTexture() );
				Texture*  normalTex	   = g_MaterialBank.GetTexture( mat->GetNormalTexture() );
				Texture*  roughnessTex = g_MaterialBank.GetTexture( mat->GetRoughnessTexture() );
				Texture*  metalTex	   = g_MaterialBank.GetTexture( mat->GetMetalTexture() );
				prog->SetUniformTextureHandle( "g_DiffuseTex",	 albedoTex->GetHandle(),	0 );
				prog->SetUniformTextureHandle( "g_NormalTex",	 normalTex->GetHandle(),	1 );
				prog->SetUniformTextureHandle( "g_RoughnessTex", roughnessTex->GetHandle(), 2 );
				prog->SetUniformTextureHandle( "g_MetallicTex",	 metalTex->GetHandle(),		3 );

				glDrawElementsInstanced( GL_TRIANGLES, mesh.Indices, GL_UNSIGNED_INT,
					( GLvoid* )( 0 + ( ( model.IndexHandle + mesh.IndexBufferOffset ) * sizeof( unsigned int ) ) ), instanceCount );
			}
			bufferOffset += instanceCount;
		}
	}
	m_DecalProgram->Render(m_RenderQueue, m_GBuffer);
	glViewport( 0, 0, m_GraphicsSettings.Width, m_GraphicsSettings.Height );
}

void GraphicsEngine::DrawLight() {
	//m_SSAOProgram->Render(m_GBuffer, m_RenderQueue);
	m_GBuffer->ApplyLightingStage();
	g_LightEngine.BuildBuffer();

	ShaderProgram* prog = g_ShaderBank.GetProgramFromHandle( m_DeferedLightShader );
	prog->Apply();
	const int WORK_GROUP_SIZE = 16;

	prog->SetUniformTextureHandle( "gAlbedoBuffer",		m_GBuffer->GetTexture( GBUFFER_TEX::ALBEDO24 ),			 0 );
	prog->SetUniformTextureHandle( "gNormalBuffer",		m_GBuffer->GetTexture( GBUFFER_TEX::NORMAL16 ),			 1 );
	prog->SetUniformTextureHandle( "gRoughMetalBuffer", m_GBuffer->GetTexture( GBUFFER_TEX::ROUGHNESS8_METAL8 ), 2 );
	prog->SetUniformTextureHandle( "gDepthBuffer",		m_GBuffer->GetTexture( GBUFFER_TEX::DEPTH_SENCIL32 ),	 3 );
	prog->SetUniformTextureHandle( "g_BRDFTex",			m_IntegratedIBL,										 4 );

	//prog->SetUniformTextureHandle( "g_ShadowMap",		m_ShadowMap->GetTexture(),								 5 );
	//glActiveTexture(GL_TEXTURE5);
	//glBindTexture(GL_TEXTURE_2D_ARRAY, m_CascadedShadowMap->GetTexture());
	//glUniformMatrix4fv(prog->FetchUniform("gLightMatrices"), 4, GL_FALSE, (float*)(m_CascadedShadowMap->GetLightMatrices()));
	//prog->SetUniformVec4("gNormalizedFarPlanes", m_CascadedShadowMap->GetNormalizedFarPlanes());

	m_SkyCubeTex->Apply(prog->FetchUniform("g_SkyCubeTex"), 6);
	m_IrrCubeTex->Apply(prog->FetchUniform("g_IrradianceCubeTex"), 7);
	//prog->SetUniformTextureHandle("g_SSAOTex", m_SSAOProgram->GetTexture(), 8);

	//prog->SetUniformMat4("gLightMatrix", m_ShadowMap->GetLightMatrix());
	prog->SetUniformUInt( "numDLights",		  g_LightEngine.GetDirLightCount() );
	prog->SetUniformUInt( "gTotalLightCount", g_LightEngine.GetPointLightCount() );

	for ( auto& view : m_RenderQueue->GetViews() ) {
		GLuint WorkGroupSizeX = GLuint( ( view.viewport.width + WORK_GROUP_SIZE - 1 ) / float( WORK_GROUP_SIZE ) );
		GLuint WorkGroupSizeY = GLuint( ( view.viewport.height + WORK_GROUP_SIZE - 1 ) / float( WORK_GROUP_SIZE ) );

		prog->SetUniformVec2( "gScreenSize", glm::vec2( m_GraphicsSettings.Width, m_GraphicsSettings.Height ) );

		prog->SetUniformMat4( "gProj",		  view.camera.Proj );
		prog->SetUniformMat4( "gView",		  view.camera.View );
		glm::mat4 invViewProj = glm::inverse(view.camera.Proj * view.camera.View);
		prog->SetUniformMat4( "gInvProjView", invViewProj );
		prog->SetUniformVec3( "gCamPos", view.camera.Position );
		prog->SetUniformVec2( "gScreenOffset", glm::vec2( view.viewport.x, view.viewport.y ) );
		prog->SetUniformVec2( "gViewportSize", glm::vec2( view.viewport.width, view.viewport.height ) );

		glBindImageTexture( 0, m_GBuffer->GetTexture( GBUFFER_TEX::COLLECTIVE24 ), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8 );
		glDispatchCompute( WorkGroupSizeX, WorkGroupSizeY, 1 );
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glUseProgram( 0 );
}

void GraphicsEngine::DrawPostFX() {
	m_BloomProgram->Render();
	m_GBuffer->ApplyPostProcessStage();
	m_GBuffer->ClearScreen();
	glDisable( GL_DEPTH_TEST );
	ShaderProgram* prog = g_ShaderBank.GetProgramFromHandle(m_FXAAShader);
	prog->Apply();
	//prog->SetUniformTextureHandle("InputTex", m_GBuffer->GetTexture(GBUFFER_TEX::COLLECTIVE24), 0);
	prog->SetUniformTextureHandle("InputTex", m_BloomProgram->GetFinalTexture(), 0);
	//prog->SetUniformTextureHandle("InputTex", m_SSAOProgram->GetTexture(), 0);
	prog->SetUniformVec2("ScreenSize", glm::vec2(m_GraphicsSettings.Width, m_GraphicsSettings.Height));
	static bool useAA = true;
	prog->SetUniformBool("useAA", useAA);
	glBindVertexArray( 0 );
	glDrawArrays( GL_POINTS, 0, 1 );
	glEnable( GL_DEPTH_TEST );
	glUseProgram( 0 );
}

void GraphicsEngine::DrawTransparent() {
	m_TransparencyProgram->Render(m_RenderQueue, m_GBuffer);
}