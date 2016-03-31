#pragma once

#include "GFXLibraryDefine.h"
#include <GL/glew.h>
#include <glm/glm.hpp>
namespace gfx {
class RenderQueue;
class SkyProgram;
class BloomProgram;
class TransparencyProgram;
typedef int  ShaderProgramHandle;

struct GraphicsSettings {
	int Width = 1280;
	int Height = 720;
	bool UseAA = true;
	int MSAA = 2;
	float BloomStrength = 1.0f;
};

struct DrawCall {
	unsigned int Count;
	unsigned int InstanceCount;
	unsigned int FirstIndex;
	unsigned int BaseVertex;
	unsigned int BaseInstance;
};

struct SceneInput {
	glm::mat4	View;
	glm::mat4	ViewProj;
	glm::vec3	CamPos;
};

#define g_GFXEngine GraphicsEngine::GetInstance()
class GraphicsEngine {
  public:
	GFX_API GraphicsEngine();
	GFX_API ~GraphicsEngine( );

	GFX_API void Initialize(const GraphicsSettings& settings);
	GFX_API void Deinitialize( );
	GFX_API void Draw();

	GFX_API RenderQueue* GetRenderQueue() {
		return m_RenderQueue;
	}
  private:
	
	void DrawGeometry();
	void DrawPostFX();
	void DrawTransparent();

	bool 						m_Initialized = false;
	RenderQueue*				m_RenderQueue = nullptr;
	SkyProgram*					m_SkyProgram = nullptr;
	BloomProgram*				m_BloomProgram = nullptr;
	TransparencyProgram*		m_TransparencyProgram = nullptr;
	GraphicsSettings			m_GraphicsSettings;
	ShaderProgramHandle			m_PreFilterIblCompute;
	ShaderProgramHandle			m_ForwardGeometryShader;
	GLuint						m_IntegratedIBL;

	GLuint						m_FrameBuffer;
	GLuint						m_ColorBuffer;
	GLuint						m_NormalBuffer;
	GLuint						m_DepthBuffer;
};
}
