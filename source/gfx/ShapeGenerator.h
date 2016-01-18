#pragma once
#include "GFXLibraryDefine.h"
#include <unordered_map>
/*
This class uses par_shapes.h to generate shapes and puts them into the modelbank
*/
enum BASIC_SHAPE {
	CUBE,
	PLANE,
	SPHERE_SUBDIV,
	SPHERE_PARA,
	CYLINDER,
	DONUT,
	OCTOHEDRON,
	TETRAHEDRON,
	DODECAHEDRON,
	ICOSAHEDRON
};
class par_shapes_mesh_s;
namespace gfx {
#define g_ShapeGenerator ShapeGenerator::GetInstance()
	class ShapeGenerator {
	public:
		GFX_API ~ShapeGenerator();
		GFX_API static ShapeGenerator& GetInstance();

		GFX_API int GenerateModel(BASIC_SHAPE shape); //returns a model handle
	private:
		int CreateModelFromMesh(par_shapes_mesh_s*  mesh);
		ShapeGenerator();
		std::unordered_map<BASIC_SHAPE, int> m_GeneratedShapes;
	};
}