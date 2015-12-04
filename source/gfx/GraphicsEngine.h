#pragma once

#include "GFXLibraryDefine.h"
#include <GL/glew.h>
typedef void* SDL_GLContext;

namespace gfx {
class Window;
class GBuffer;
class RenderQueue;
class Texture;
class SkyProgram;
class ShadowMap;
class TerrainDeformationProgram;
class DeferedDecalProgram;
class BloomProgram;
class TransparencyProgram;
class ParticleSystem;
typedef int  ShaderProgramHandle;

struct GraphicsSettings {
	int Width = 1280;
	int Height = 720;
};
#define g_GFXEngine GraphicsEngine::GetInstance()
class GraphicsEngine {
  public:
	GFX_API GraphicsEngine();
	GFX_API ~GraphicsEngine( );

	GFX_API void Initialize(const GraphicsSettings& settings);
	GFX_API void Deinitialize( );
	GFX_API void Draw();
	GFX_API void GetParticleTimes(double& update, double& render); //temp remove soon

	RenderQueue* GetRenderQueue() {
		return m_RenderQueue;
	}
  private:
	
	void DrawGeometry();
	void DrawLight();
	void DrawPostFX();
	void DrawTransparent();

	bool 						m_Initialized = false;
	GBuffer*					m_GBuffer = nullptr;
	RenderQueue*				m_RenderQueue = nullptr;
	Texture*					m_SkyCubeTex = nullptr;
	Texture*					m_IrrCubeTex = nullptr;
	ShadowMap*					m_ShadowMap = nullptr;
	SkyProgram*					m_SkyProgram = nullptr;
	TerrainDeformationProgram*	m_TerrainDeform = nullptr;
	DeferedDecalProgram*		m_DecalProgram = nullptr;
	BloomProgram*				m_BloomProgram = nullptr;
	TransparencyProgram*		m_TransparencyProgram = nullptr;
	ParticleSystem*				m_ParticleSystem = nullptr;
	GraphicsSettings			m_GraphicsSettings;
	int							m_BufferOffset;
	ShaderProgramHandle			m_PreFilterIblCompute;
	ShaderProgramHandle			m_DeferedGeometryShader;
	ShaderProgramHandle			m_DeferedLightShader;
	ShaderProgramHandle			m_FXAAShader;
	GLuint						m_IntegratedIBL;
};
}
