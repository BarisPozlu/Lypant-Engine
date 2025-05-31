#include "lypch.h"
#include "Model.h"
#include "VertexArray.h"
#include "Texture.h"
#include "Material.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <stb_image.h>

namespace lypant
{
	Model::Model(const std::string& path, bool loadMaterials) : m_Path(path)
	{
		m_Directory = path.substr(0, path.find_last_of('/') + 1);

		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
		LY_CORE_ASSERT(scene && !(scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) && scene->mRootNode, importer.GetErrorString());

		m_Meshes.reserve(scene->mNumMeshes);

		ProcessNode(scene->mRootNode, scene, loadMaterials);
	}

	void Model::ProcessNode(aiNode* node, const aiScene* scene, bool loadMaterials)
	{
		for (int i = 0; i < node->mNumMeshes; i++)
		{
			ProcessMesh(scene->mMeshes[node->mMeshes[i]], scene, loadMaterials);
		}

		for (int i = 0; i < node->mNumChildren; i++)
		{
			ProcessNode(node->mChildren[i], scene, loadMaterials);
		}
	}

	void Model::ProcessMesh(aiMesh* mesh, const aiScene* scene, bool loadMaterials)
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

			vertex.Tangent.x = mesh->mTangents[i].x;
			vertex.Tangent.y = mesh->mTangents[i].y;
			vertex.Tangent.z = mesh->mTangents[i].z;

			vertex.TexCoord.x = mesh->mTextureCoords[0][i].x;
			vertex.TexCoord.y = mesh->mTextureCoords[0][i].y;
		}

		std::shared_ptr<VertexBuffer> vertexBuffer = std::make_shared<VertexBuffer>(vertices, sizeof(Vertex) * mesh->mNumVertices);
		delete[] vertices;

		BufferLayout layout
		{
			{ShaderDataType::Float3, "a_Position"}, {ShaderDataType::Float3, "a_Normal"}, {ShaderDataType::Float3, "a_Tangent"}, {ShaderDataType::Float2, "a_TexCoord"}
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

		if (!loadMaterials)
		{
			std::shared_ptr<Material> material = std::make_shared<Material>("shaders/FlatColor.glsl", glm::vec3(1.0, 0.0, 0.0));
			m_Meshes.emplace_back(vertexArray, material);
			return;
		}

		aiMaterial* aiMaterial = scene->mMaterials[mesh->mMaterialIndex];

		LY_CORE_ASSERT(aiMaterial->GetTextureCount(aiTextureType_DIFFUSE) == 1 && aiMaterial->GetTextureCount(aiTextureType_UNKNOWN) == 1 && aiMaterial->GetTextureCount(aiTextureType_NORMALS) == 1, "Model textures are not in the expected format.");
		
		stbi_set_flip_vertically_on_load(0);

		int width;
		int height;
		int channels;

		aiString localPath;

		aiMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &localPath);

		if (localPath.C_Str()[0] != '*')
		{
			std::string diffuseMapPath = m_Directory + localPath.C_Str();
		
			aiMaterial->GetTexture(aiTextureType_UNKNOWN, 0, &localPath);
			std::string aoRoughnessMetallicMapPath = m_Directory + localPath.C_Str();

			aiMaterial->GetTexture(aiTextureType_NORMALS, 0, &localPath);
			std::string normalMapPath = m_Directory + localPath.C_Str();

			std::shared_ptr<Material> material = std::make_shared<Material>("shaders/Model_PBR.glsl", diffuseMapPath, aoRoughnessMetallicMapPath, normalMapPath);
			m_Meshes.emplace_back(vertexArray, material);
		}

		else
		{
			const aiTexture* texture = scene->GetEmbeddedTexture(localPath.C_Str());
			unsigned char* buffer = stbi_load_from_memory(reinterpret_cast<unsigned char*>(texture->pcData), texture->mWidth, &width, &height, &channels, 0);
			std::shared_ptr<Texture2D> albedoMap = std::make_shared<Texture2D>(width, height, buffer, false);
			stbi_image_free(buffer);

			aiMaterial->GetTexture(aiTextureType_UNKNOWN, 0, &localPath);
			texture = scene->GetEmbeddedTexture(localPath.C_Str());
			buffer = stbi_load_from_memory(reinterpret_cast<unsigned char*>(texture->pcData), texture->mWidth, &width, &height, &channels, 0);
			std::shared_ptr<Texture2D> aoRoughnessMetallicMap = std::make_shared<Texture2D>(width, height, buffer);
			stbi_image_free(buffer);

			aiMaterial->GetTexture(aiTextureType_NORMALS, 0, &localPath);
			texture = scene->GetEmbeddedTexture(localPath.C_Str());
			buffer = stbi_load_from_memory(reinterpret_cast<unsigned char*>(texture->pcData), texture->mWidth, &width, &height, &channels, 0);
			std::shared_ptr<Texture2D> normalMap = std::make_shared<Texture2D>(width, height, buffer);
			stbi_image_free(buffer);

			std::shared_ptr<Material> material = std::make_shared<Material>("shaders/Model_PBR.glsl", albedoMap, aoRoughnessMetallicMap, normalMap);

			m_Meshes.emplace_back(vertexArray, material);
		}

	}
}
