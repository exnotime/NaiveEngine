#pragma once
#include <glm/glm.hpp>
#include <string>
#include <vector>
namespace gfx_dx {
struct Mesh {
	unsigned int VertexBufferOffset;
	unsigned int IndexBufferOffset;
	int Indices;
	int Size;
	unsigned int Material = 0;
};

struct Model {
	unsigned int IndexHandle; //place in ModelIndexBuffer
	unsigned int VertexHandle; //place in ModelVertexBuffer
	unsigned int MaterialOffset;
	std::string Name;
	int NumVertices;
	int NumIndices;
	bool Loaded;
	glm::vec3 Min;
	glm::vec3 Max;
	std::vector<Mesh> Meshes;
};
}