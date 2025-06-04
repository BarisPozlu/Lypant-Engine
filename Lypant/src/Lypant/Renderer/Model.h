#pragma once

#include "lypch.h"
#include "Mesh.h"
#include <glm/glm.hpp>

struct aiScene;
struct aiNode;
struct aiMesh;

namespace lypant
{
	class Model
	{
	public:
		Model(const std::string& path, bool loadMaterials = true, bool flipUVs = true);

		inline const std::vector<Mesh>& GetMeshes() const { return m_Meshes; }
	private:
		void ProcessNode(aiNode* node, const aiScene* scene, bool loadMaterials);
		void ProcessMesh(aiMesh* mesh, const aiScene* scene, bool loadMaterials);
	private:
		struct Vertex
		{
			glm::vec3 Position;
			glm::vec3 Normal;
			glm::vec3 Tangent;
			glm::vec2 TexCoord;
		};
	private:
		std::vector<Mesh> m_Meshes;
		std::string m_Path;
		std::string m_Directory;
	};
}
