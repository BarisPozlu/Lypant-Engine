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
	Model::Model(const std::string& path, bool loadMaterials, bool flipUVs) : m_Path(path)
	{
		m_Directory = path.substr(0, path.find_last_of('/') + 1);

		Assimp::Importer importer;

		unsigned int flags = aiProcess_Triangulate | aiProcess_CalcTangentSpace;
		if (flipUVs)
		{
			flags |= aiProcess_FlipUVs;
		}

		const aiScene* scene = importer.ReadFile(path, flags);
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

		std::shared_ptr<Material> material = std::make_shared<Material>();
		material->Shader = Shader::Load("shaders/Model_PBR.glsl");

		aiMaterial* aiMaterial = scene->mMaterials[mesh->mMaterialIndex];

		int width;
		int height;
		int channels;

		aiString localPath;

		aiMaterial->Get(AI_MATKEY_BASE_COLOR, material->Albedo);
		aiMaterial->Get(AI_MATKEY_ROUGHNESS_FACTOR, material->Roughness);
		aiMaterial->Get(AI_MATKEY_METALLIC_FACTOR, material->Metallic);

		if (aiMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &localPath) == aiReturn_SUCCESS)
		{
			if (localPath.C_Str()[0] != '*')
			{
				std::string albedoMapPath = m_Directory + localPath.C_Str();
				material->AlbedoMap = Texture2D::Load(albedoMapPath, false);
			}
		}

		if (aiMaterial->GetTexture(aiTextureType_UNKNOWN, 0, &localPath) == aiReturn_SUCCESS)
		{
			if (localPath.C_Str()[0] != '*')
			{
				std::string aoRoughnessMetallicMapPath = m_Directory + localPath.C_Str();
				material->ORMMap = Texture2D::Load(aoRoughnessMetallicMapPath);
				material->UseCombinedORM = true;
			}
		}

		else
		{
			if (aiMaterial->GetTexture(aiTextureType_METALNESS, 0, &localPath) == aiReturn_SUCCESS)
			{
				if (localPath.C_Str()[0] != '*')
				{
					std::string metallicMapPath = m_Directory + localPath.C_Str();
					material->MetallicMap = Texture2D::Load(metallicMapPath);
				}
			}

			if (aiMaterial->GetTexture(aiTextureType_SHININESS, 0, &localPath) == aiReturn_SUCCESS)
			{
				if (localPath.C_Str()[0] != '*')
				{
					std::string roughnessMapPath = m_Directory + localPath.C_Str();
					material->RoughnessMap = Texture2D::Load(roughnessMapPath);
				}
			}
		}

		if (aiMaterial->GetTexture(aiTextureType_NORMALS, 0, &localPath) == aiReturn_SUCCESS)
		{
			if (localPath.C_Str()[0] != '*')
			{
				std::string normalMapPath = m_Directory + localPath.C_Str();
				material->NormalMap = Texture2D::Load(normalMapPath);
				material->UseNormalMap = true;
			}
		}
		
		unsigned char* buffer;

		aiMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &localPath);
		const aiTexture* texture = scene->GetEmbeddedTexture(localPath.C_Str());
		if (texture)
		{
			buffer = stbi_load_from_memory(reinterpret_cast<unsigned char*>(texture->pcData), texture->mWidth, &width, &height, &channels, 0);
			material->AlbedoMap = std::make_shared<Texture2D>(width, height, buffer, false);
			stbi_image_free(buffer);
		}

		aiMaterial->GetTexture(aiTextureType_UNKNOWN, 0, &localPath);
		texture = scene->GetEmbeddedTexture(localPath.C_Str());
		if (texture)
		{
			buffer = stbi_load_from_memory(reinterpret_cast<unsigned char*>(texture->pcData), texture->mWidth, &width, &height, &channels, 0);
			material->ORMMap = std::make_shared<Texture2D>(width, height, buffer);
			material->UseCombinedORM = true;
			stbi_image_free(buffer);
		}

		aiMaterial->GetTexture(aiTextureType_NORMALS, 0, &localPath);
		texture = scene->GetEmbeddedTexture(localPath.C_Str());
		if (texture)
		{
			buffer = stbi_load_from_memory(reinterpret_cast<unsigned char*>(texture->pcData), texture->mWidth, &width, &height, &channels, 0);
			material->NormalMap = std::make_shared<Texture2D>(width, height, buffer);
			material->UseNormalMap = true;
			stbi_image_free(buffer);
		}

		m_Meshes.emplace_back(vertexArray, material);

	}
}
