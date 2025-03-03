#pragma once
#include "Core/Core.h"
#include "Graphics/Resources/VertexInfo.h"
#include "assimp/types.h"

namespace AssimpConvert
{
	struct Mesh
	{
		std::string name;
		int32 boneIndex;	//���� ��� �ε��� ��
		std::string materialName;	// �� mesh�� �׸��� �ʿ��� material�� �̸��� ������ �ְڴ�.

		std::vector<BasicVertex> vertices;
		std::vector<uint32> indices;

		Matrix localTM;

		// AABB ��� ����
		Vector3 minBounds;  // AABB �ּ� ��� (x, y, z)
		Vector3 maxBounds;  // AABB �ִ� ��� (x, y, z)
	};

	struct Bone
	{
		std::string name;
		int32 index = -1;
		int32 parent = -1;
		Matrix transform;
		Matrix offset;

		bool isNegative;
	};

	struct Model
	{

	};
	
	struct Material
	{
		std::string name;
		Color ambient;
		Color specular;
		Color diffuse;
		Color emissive;

		std::string diffuseTextureFile;
		std::string specularTextureFile;
		std::string emissiveTextureFile;
		std::string normalTextureFile;
		std::string metalicTextureFile;
		std::string roughnessTextureFile;

		bool bUseDiffuseTexture;
		bool bUseNormalTexture;
		bool bUseMetallicTexture;
		bool bUseRoughnessTexture;

		float metallicFactor;
		float roughnessFactor;
	};


	// Animation

	struct BlendWeight
	{
		Vector4 indices = Vector4(0, 0, 0, 0);
		Vector4 weights = Vector4(0, 0, 0, 0);

		void Set(uint32 index, uint32 boneIndex, float weight)
		{
			float i = (float)boneIndex;
			
			switch (index)
			{
				case 0:
				{
					indices.x = i;
					weights.x = weight;
				}
				break;
				case 1:
				{
					indices.y = i;
					weights.y = weight;
				}
				break;
				case 2:
				{
					indices.z = i;
					weights.z = weight;
				}
				break;
				case 3:
				{
					indices.w = i;
					weights.w = weight;
				}
				break;
			}
		}
	};

	struct BoneWeights
	{
		void AddWeights(uint32 boneIndex, float weight)
		{
			if (weight <= 0.f)
			{
				return;
			}

			// ����ġ�� �� ũ�� �� �տ� ���� �ִ´�.
			auto findIt = std::find_if(boneWeights.begin(), boneWeights.end(),
				[weight](const std::pair<int, float>& p) {return weight > p.second; });


			boneWeights.insert(findIt, { boneIndex, weight });
		}

		BlendWeight GetBlendWeights()
		{
			BlendWeight ret;
			for (uint32 i = 0; i < boneWeights.size(); ++i)
			{
				if (i >= 4)
					break;

				ret.Set(i, boneWeights[i].first, boneWeights[i].second);
			}

			return ret;
		}

		void Normalize()
		{
			// bone�� 4���� ���� ���
			if (boneWeights.size() >= 4)
			{
				boneWeights.resize(4);
			}

			float totalWeight = 0.f;
			for(const auto& item : boneWeights)
			{
				totalWeight += item.second;
			}

			// ������ ������
			float scale = 1.f / totalWeight;
			for (auto& item : boneWeights)
			{
				item.second *= scale;
			}
		}

		std::vector<std::pair<int32,float>> boneWeights;
	};

	
}

