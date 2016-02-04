#pragma once
#include "GFXDXLibraryDefine.h"
#include <utility/BasicShapes.h>
#include <unordered_map>
/*
This class uses par_shapes.h to generate shapes and puts them into the modelbank
*/
class par_shapes_mesh_s;
namespace gfx_dx {
#define g_ShapeGenerator ShapeGenerator::GetInstance()
	class ShapeGenerator {
	public:
		GFX_DX_API ~ShapeGenerator();
		GFX_DX_API static ShapeGenerator& GetInstance();

		GFX_DX_API int GenerateModel(BASIC_SHAPE shape); //returns a model handle
	private:
		int CreateModelFromMesh(par_shapes_mesh_s*  mesh);
		ShapeGenerator();
		std::unordered_map<BASIC_SHAPE, int> m_GeneratedShapes;
	};
}