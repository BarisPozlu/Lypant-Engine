#include "lypch.h"
#include "Model.h"
#include "VertexArray.h"
#include "Texture.h"
#include "Material.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace lypant
{
	Model::Model(const std::string& path) : m_Path(path)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
		LY_CORE_ASSERT(scene && !(scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) && scene->mRootNode, importer.GetErrorString());

		m_Meshes.reserve(scene->mNumMeshes);

		ProcessNode(scene->mRootNode, scene);
	}

	void Model::ProcessNode(aiNode* node, const aiScene* scene)
	{
		for (int i = 0; i < node->mNumMeshes; i++)
		{
			ProcessMesh(scene->mMeshes[node->mMeshes[i]], scene);
		}

		for (int i = 0; i < node->mNumChildren; i++)
		{
			ProcessNode(node->mChildren[i], scene);
		}
	}

	void Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
	{
		LY_CORE_ASSERT(mesh->mTextureCoords[0], "Mesh loaded does not have texture coordinates.");

		std::shared_ptr<VertexArray> vertexArray = std::make_shared<VertexArray>();

		Vertex* vertices = new Vertex[mesh->mNumVertices];
		
		for (int i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex& vertex = vertices[i];

			vertex.Position.x = mesh->mVertices[i].x;
			vertex.Position.y = mesh->mVertices[i].y;
			vertex.Position.z = mesh->mVertices[i].z;

			vertex.Normal.x = mesh->mNormals[i].x;
			vertex.Normal.y = mesh->mNormals[i].y;
			vertex.Normal.z = mesh->mNormals[i].z;

			vertex.TexCoord.x = mesh->mTextureCoords[0][i].x;
			vertex.TexCoord.y = mesh->mTextureCoords[0][i].y;
		}

		std::shared_ptr<VertexBuffer> vertexBuffer = std::make_shared<VertexBuffer>(vertices, sizeof(Vertex) * mesh->mNumVertices);
		delete[] vertices;

		BufferLayout layout
		{
			{ShaderDataType::Float3, "a_Position"}, {ShaderDataType::Float3, "a_Normal"}, {ShaderDataType::Float2, "a_TexCoord"}
		};

		vertexBuffer->SetLayout(layout);
		vertexArray->AddVertexBuffer(vertexBuffer);
		
		int numberOfIndices = 0;

		for (int i = 0; i < mesh->mNumFaces; i++)
		{
			numberOfIndices += mesh->mFaces[i].mNumIndices;
		}

		unsigned int* indices = new unsigned int[numberOfIndices];
		int index = 0;

		for (int i = 0; i < mesh->mNumFaces; i++)
		{
			int size = sizeof(unsigned int) * mesh->mFaces[i].mNumIndices;
			memcpy(&indices[index], mesh->mFaces[i].mIndices, size);
			index += mesh->mFaces[i].mNumIndices;
		}

		std::shared_ptr<IndexBuffer> indexBuffer = std::make_shared<IndexBuffer>(indices, numberOfIndices);
		delete[] indices;

		vertexArray->SetIndexBuffer(indexBuffer);
		
		std::string directory = m_Path.substr(0, m_Path.find_last_of("/") + 1);

		aiMaterial* aiMaterial = scene->mMaterials[mesh->mMaterialIndex];

		LY_CORE_ASSERT(aiMaterial->GetTextureCount(aiTextureType_DIFFUSE) == 1 && aiMaterial->GetTextureCount(aiTextureType_SPECULAR) == 1, "There are more than 1 texture per map in the mesh");

		aiString localPath;
		aiMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &localPath);

		std::string diffuseMapPath = directory + localPath.C_Str();

		aiMaterial->GetTexture(aiTextureType_SPECULAR, 0, &localPath);

		std::string specularMapPath = directory + localPath.C_Str();

		std::shared_ptr<Material> material = std::make_shared<Material>("shaders/TexturedObject.glsl", diffuseMapPath, specularMapPath);

		m_Meshes.emplace_back(vertexArray, material);
	}
}
