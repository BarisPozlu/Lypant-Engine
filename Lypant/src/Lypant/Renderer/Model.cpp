#include "lypch.h"
#include "Model.h"
#include "Buffer.h"
#include "Image.h"
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
		// TODO:
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

		Vertex* vertices = new Vertex[mesh->mNumVertices];

		for (int i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex& vertex = vertices[i];

			vertex.Position.x = mesh->mVertices[i].x;
			vertex.Position.y = mesh->mVertices[i].y;
			vertex.Position.z = mesh->mVertices[i].z;
			vertex.Position.w = 1;

			vertex.Normal.x = mesh->mNormals[i].x;
			vertex.Normal.y = mesh->mNormals[i].y;
			vertex.Normal.z = mesh->mNormals[i].z;

			vertex.Tangent.x = mesh->mTangents[i].x;
			vertex.Tangent.y = mesh->mTangents[i].y;
			vertex.Tangent.z = mesh->mTangents[i].z;

			vertex.TexCoord.x = mesh->mTextureCoords[0][i].x;
			vertex.TexCoord.y = mesh->mTextureCoords[0][i].y;
		}

		std::shared_ptr<Buffer> vertexBuffer = Buffer::CreateVertexBuffer(vertices, sizeof(Vertex) * mesh->mNumVertices);
		delete[] vertices;

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

		std::shared_ptr<Buffer> indexBuffer = Buffer::CreateIndexBuffer(indices, numberOfIndices);
		delete[] indices;

		if (!loadMaterials)
		{
			m_Meshes.emplace_back(vertexBuffer, indexBuffer, nullptr);
			return;
		}

		MaterialData materialData;
		aiMaterial* aiMaterial = scene->mMaterials[mesh->mMaterialIndex];

		int width;
		int height;
		int channels;

		aiString localPath;

		aiMaterial->Get(AI_MATKEY_BASE_COLOR, materialData.Constants.Albedo);
		aiMaterial->Get(AI_MATKEY_ROUGHNESS_FACTOR, materialData.Constants.Roughness);
		aiMaterial->Get(AI_MATKEY_METALLIC_FACTOR, materialData.Constants.Metallic);

		ImageParams albedoParams;
		albedoParams.LinearSpace = false;
		albedoParams.GenerateMipMap = true;

		if (aiMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &localPath) == aiReturn_SUCCESS)
		{
			if (localPath.C_Str()[0] != '*')
			{
				std::string albedoMapPath = m_Directory + localPath.C_Str();
				materialData.Textures.AlbedoMap = Image::CreateImage2D(albedoMapPath, albedoParams);
			}
		}

		if (aiMaterial->GetTexture(aiTextureType_UNKNOWN, 0, &localPath) == aiReturn_SUCCESS)
		{
			if (localPath.C_Str()[0] != '*')
			{
				std::string aoRoughnessMetallicMapPath = m_Directory + localPath.C_Str();
				materialData.Textures.ORMMap = Image::CreateImage2D(aoRoughnessMetallicMapPath, {});
				materialData.Constants.UseCombinedORM = true;
			}
		}

		else
		{
			if (aiMaterial->GetTexture(aiTextureType_METALNESS, 0, &localPath) == aiReturn_SUCCESS)
			{
				if (localPath.C_Str()[0] != '*')
				{
					std::string metallicMapPath = m_Directory + localPath.C_Str();
					materialData.Textures.MetallicMap = Image::CreateImage2D(metallicMapPath, {});
				}
			}

			if (aiMaterial->GetTexture(aiTextureType_SHININESS, 0, &localPath) == aiReturn_SUCCESS)
			{
				if (localPath.C_Str()[0] != '*')
				{
					std::string roughnessMapPath = m_Directory + localPath.C_Str();
					materialData.Textures.RoughnessMap = Image::CreateImage2D(roughnessMapPath, {});
				}
			}
		}

		if (aiMaterial->GetTexture(aiTextureType_NORMALS, 0, &localPath) == aiReturn_SUCCESS)
		{
			if (localPath.C_Str()[0] != '*')
			{
				std::string normalMapPath = m_Directory + localPath.C_Str();
				materialData.Textures.NormalMap = Image::CreateImage2D(normalMapPath, {});
				materialData.Constants.UseNormalMap = true;
			}
		}

		unsigned char* buffer;

		aiMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &localPath);
		const aiTexture* texture = scene->GetEmbeddedTexture(localPath.C_Str());
		if (texture)
		{
			// TODO: REMOVE, 3 channel srgb is not supported for some reason thus trying 4
			buffer = stbi_load_from_memory(reinterpret_cast<unsigned char*>(texture->pcData), texture->mWidth, &width, &height, &channels, 4);
			materialData.Textures.AlbedoMap = Image::CreateImage2D(width, height, channels + 1, buffer, albedoParams);
			stbi_image_free(buffer);
		}

		aiMaterial->GetTexture(aiTextureType_UNKNOWN, 0, &localPath);
		texture = scene->GetEmbeddedTexture(localPath.C_Str());
		if (texture)
		{
			// TODO: REMOVE, 3 channel unorm is not supported for some reason thus trying 4
			buffer = stbi_load_from_memory(reinterpret_cast<unsigned char*>(texture->pcData), texture->mWidth, &width, &height, &channels, 4);
			materialData.Textures.ORMMap = Image::CreateImage2D(width, height, channels + 1, buffer, {});
			materialData.Constants.UseCombinedORM = true;
			stbi_image_free(buffer);
		}

		aiMaterial->GetTexture(aiTextureType_NORMALS, 0, &localPath);
		texture = scene->GetEmbeddedTexture(localPath.C_Str());
		if (texture)
		{
			// TODO: REMOVE, 3 channel unorm is not supported for some reason thus trying 4
			buffer = stbi_load_from_memory(reinterpret_cast<unsigned char*>(texture->pcData), texture->mWidth, &width, &height, &channels, 4);
			materialData.Textures.NormalMap = Image::CreateImage2D(width, height, channels + 1, buffer, {});
			materialData.Constants.UseNormalMap = true;
			stbi_image_free(buffer);
		}

		std::shared_ptr<Material> material = Material::Create(materialData);
		m_Meshes.emplace_back(vertexBuffer, indexBuffer, material);
	}
}
