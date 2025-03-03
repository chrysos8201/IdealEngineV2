#include "Misc/Assimp/AssimpConverter.h"

#include "Misc/Utils/tinyxml2.h"
#include "Misc/Utils/FileUtils.h"
#include "Misc/Utils/StringUtils.h"
#include <filesystem>
#include <limits>
#include <algorithm>
// 유틸리티 함수: UTF-8 문자열을 wstring으로 변환
std::wstring utf8_to_wstring(const std::string& utf8Str) {
	if (utf8Str.empty()) return std::wstring();

	int wideCharLength = MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, NULL, 0);
	std::wstring wideStr(wideCharLength, 0);
	MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, &wideStr[0], wideCharLength);

	return wideStr;
}

// 유틸리티 함수: wstring을 로컬 멀티바이트 문자열로 변환
std::string wstring_to_multibyte(const std::wstring& wideStr) {
	if (wideStr.empty()) return std::string();

	int multiByteLength = WideCharToMultiByte(CP_ACP, 0, wideStr.c_str(), -1, NULL, 0, NULL, NULL);
	std::string multiByteStr(multiByteLength, 0);
	WideCharToMultiByte(CP_ACP, 0, wideStr.c_str(), -1, &multiByteStr[0], multiByteLength, NULL, NULL);

	return multiByteStr;
}


bool IsNegative(const Matrix& m)
{
	Vector3 r0 = Vector3(m._11, m._12, m._13);
	Vector3 r1 = Vector3(m._21, m._22, m._23);
	Vector3 r2 = Vector3(m._31, m._32, m._33);

	auto cross = r0.Cross(r1);
	return cross.Dot(r2) < 0.f;
}

void Decompose(Matrix m, Matrix& rotation, Vector3& scale)
{
	Vector3 r0 = Vector3(m._11, m._12, m._13);
	float lenR0 = r0.Length();

	Vector3 r1 = Vector3(m._21, m._22, m._23);
	float lenR1 = r1.Length();

	Vector3 r2 = Vector3(m._31, m._32, m._33);
	float lenR2 = r2.Length();

	Matrix rotationMatrix = Matrix::Identity;
	Vector3 rotVec0 = Vector3(m._11 / lenR0, m._12 / lenR0, m._13 / lenR0);
	Vector3 rotVec1 = Vector3(m._21 / lenR1, m._22 / lenR1, m._23 / lenR1);
	Vector3 rotVec2 = Vector3(m._31 / lenR2, m._32 / lenR2, m._33 / lenR2);

	scale.x = lenR0;
	scale.y = lenR1;
	scale.z = lenR2;

	rotationMatrix._11 = rotVec0.x; rotationMatrix._12 = rotVec0.y; rotationMatrix._13 = rotVec0.z;
	rotationMatrix._21 = rotVec1.x; rotationMatrix._22 = rotVec1.y; rotationMatrix._23 = rotVec1.z;
	rotationMatrix._31 = rotVec2.x; rotationMatrix._32 = rotVec2.y; rotationMatrix._33 = rotVec2.z;

	if (IsNegative(rotationMatrix))
	{
		rotationMatrix *= -1.f;
		rotVec0 *= -1.f;
		rotVec1 *= -1.f;
		rotVec2 *= -1.f;
		scale *= -1.f;
	}

	rotVec0.Normalize();
	rotVec1.Normalize();
	rotVec2.Normalize();

	rotationMatrix._11 = rotVec0.x; rotationMatrix._12 = rotVec0.y; rotationMatrix._13 = rotVec0.z;
	rotationMatrix._21 = rotVec1.x; rotationMatrix._22 = rotVec1.y; rotationMatrix._23 = rotVec1.z;
	rotationMatrix._31 = rotVec2.x; rotationMatrix._32 = rotVec2.y; rotationMatrix._33 = rotVec2.z;

	rotation = rotationMatrix;
}



AssimpConverter::AssimpConverter()
	: m_scene(nullptr)
{
	m_importer = std::make_shared<Assimp::Importer>();
}

AssimpConverter::~AssimpConverter()
{

}

std::wstring AssimpConverter::ConvertStringToWString(const std::string& str)
{
	return std::wstring(str.begin(), str.end());
}

std::string AssimpConverter::ConvertWStringToString(const std::wstring& wstr)
{
	return std::string(wstr.begin(), wstr.end());
}

void AssimpConverter::Replace(std::string& OutStr, std::string Comp, std::string Rep)
{
	std::string temp = OutStr;
	size_t start_pos = 0;
	while ((start_pos = temp.find(Comp, start_pos)) != std::wstring::npos)
	{
		temp.replace(start_pos, Comp.length(), Rep);
		start_pos += Rep.length();
	}
	OutStr = temp;
}

void AssimpConverter::Replace(std::wstring& OutStr, std::wstring Comp, std::wstring Rep)
{
	std::wstring temp = OutStr;
	size_t start_pos = 0;
	while ((start_pos = temp.find(Comp, start_pos)) != std::wstring::npos)
	{
		temp.replace(start_pos, Comp.length(), Rep);
		start_pos += Rep.length();
	}
	OutStr = temp;
}

void AssimpConverter::ReadAssetFile(const std::wstring& path, bool isSkinnedData, bool onlyPosition)
{
	std::wstring fileStr;
	fileStr = m_assetPath + path;

	uint32 flag = 0;
	flag |= aiProcess_MakeLeftHanded;
	flag |= aiProcess_FlipUVs;
	flag |= aiProcess_FlipWindingOrder;
	flag |= aiProcess_Triangulate;
	flag |= aiProcess_GenUVCoords;
	flag |= aiProcess_GenNormals;
	flag |= aiProcess_CalcTangentSpace;
	flag |= aiProcess_GlobalScale;
	//flag |= aiProcess_SortByPType;
	//flag |= aiProcess_LimitBoneWeights;
	//flag |= aiProcess_JoinIdenticalVertices;

	m_importer->SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);
	m_importer->SetPropertyFloat(AI_CONFIG_GLOBAL_SCALE_FACTOR_KEY, 1.f);
	bool s = m_importer->GetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS);

	m_scene = m_importer->ReadFile(
		ConvertWStringToString(fileStr),
		flag
	);

	if (!m_scene)
	{
		printf(m_importer->GetErrorString());
		assert(false);
		MessageBox(NULL, path.c_str(), L"AssimpConverterReadSsetFile", MB_OK);
	}
}

void AssimpConverter::ExportModelData(std::wstring savePath, bool IsSkinnedData /*= false*/, bool IsConvertCenter /*= false*/)
{
	// skinned data일 경우 
	m_isSkinnedData = IsSkinnedData;


	std::wstring finalPath = m_modelPath + savePath + L".mesh";
	ReadModelData(m_scene->mRootNode, -1, -1);
	WriteModelFile(finalPath);

}

void AssimpConverter::ExportVertexPositionData(const std::wstring& savePath)
{
	std::wstring finalPath = m_modelPath + savePath + L".pos";
	WriteVertexPositionFile(finalPath);
}

std::string AssimpConverter::WriteTexture(std::string SaveFolder, std::string File)
{
	std::wstring fileP = StringUtils::ConvertStringToWString(File);
	std::string fileName = std::filesystem::path(File).filename().string();
	std::string folderName = std::filesystem::path(SaveFolder).filename().string();

	const aiTexture* srcTexture = m_scene->GetEmbeddedTexture(File.c_str());

	// 텍스쳐가 내장되어있을 경우
	if (srcTexture)
	{
		std::string pathStr = (std::filesystem::path(SaveFolder) / fileName).string();

		if (srcTexture->mHeight == 0)
		{
			std::shared_ptr<FileUtils> file = std::make_shared<FileUtils>();
			file->Open(ConvertStringToWString(pathStr), FileMode::Write);
			file->Write(srcTexture->pcData, srcTexture->mWidth);
		}
	}
	// 텍스쳐가 따로 있을 경우
	else
	{
		// 원래있던 파일을 내가 원하는 경로로 옮긴다.
		if (File.size() > 0)
		{

			std::string originStr = (std::filesystem::path(m_assetPath) / folderName / File).string();
			Replace(originStr, "\\", "/");

			std::string pathStr = (std::filesystem::path(SaveFolder) / fileName).string();
			Replace(pathStr, "\\", "/");

			bool isSuccess = ::CopyFileA(originStr.c_str(), pathStr.c_str(), false);
			if (!isSuccess)
			{
				//assert(false);
				//MessageBox(NULL, L"원본 텍스쳐가 없습니다.", L"AssimpConverter", MB_OK);
			}
		}
	}
	return fileName;
}

void AssimpConverter::WriteMaterialData(std::wstring FilePath)
{
	auto path = std::filesystem::path(FilePath);

	std::filesystem::create_directories(path.parent_path());

	std::string folder = path.parent_path().string();
	//folder += "/";
	std::shared_ptr<tinyxml2::XMLDocument> document = std::make_shared<tinyxml2::XMLDocument>();

	tinyxml2::XMLDeclaration* decl = document->NewDeclaration();
	document->LinkEndChild(decl);

	tinyxml2::XMLElement* root = document->NewElement("Materials");
	document->LinkEndChild(root);

	for (std::shared_ptr<AssimpConvert::Material> material : m_materials)
	{
		tinyxml2::XMLElement* node = document->NewElement("Material");
		root->LinkEndChild(node);

		tinyxml2::XMLElement* element = nullptr;

		element = document->NewElement("Name");
		element->SetText(material->name.c_str());
		node->LinkEndChild(element);

		element = document->NewElement("DiffuseFile");
		element->SetText(WriteTexture(folder, material->diffuseTextureFile).c_str());
		node->LinkEndChild(element);
		element = document->NewElement("SpecularFile");
		element->SetText(WriteTexture(folder, material->specularTextureFile).c_str());
		node->LinkEndChild(element);
		element = document->NewElement("NormalFile");
		element->SetText(WriteTexture(folder, material->normalTextureFile).c_str());
		node->LinkEndChild(element);
		element = document->NewElement("MetalicFile");
		element->SetText(WriteTexture(folder, material->metalicTextureFile).c_str());
		node->LinkEndChild(element);
		element = document->NewElement("RoughnessFile");
		element->SetText(WriteTexture(folder, material->roughnessTextureFile).c_str());
		node->LinkEndChild(element);

		element = document->NewElement("Ambient");
		element->SetAttribute("R", material->ambient.x);
		element->SetAttribute("G", material->ambient.y);
		element->SetAttribute("B", material->ambient.z);
		element->SetAttribute("A", material->ambient.w);
		node->LinkEndChild(element);

		element = document->NewElement("Diffuse");
		element->SetAttribute("R", material->diffuse.x);
		element->SetAttribute("G", material->diffuse.y);
		element->SetAttribute("B", material->diffuse.z);
		element->SetAttribute("A", material->diffuse.w);
		node->LinkEndChild(element);

		element = document->NewElement("Specular");
		element->SetAttribute("R", material->specular.x);
		element->SetAttribute("G", material->specular.y);
		element->SetAttribute("B", material->specular.z);
		element->SetAttribute("A", material->specular.w);
		node->LinkEndChild(element);

		element = document->NewElement("Emissive");
		element->SetAttribute("R", material->emissive.x);
		element->SetAttribute("G", material->emissive.y);
		element->SetAttribute("B", material->emissive.z);
		element->SetAttribute("A", material->emissive.w);
		node->LinkEndChild(element);

		element = document->NewElement("Metallic");
		element->SetAttribute("Factor", material->metallicFactor);
		node->LinkEndChild(element);

		element = document->NewElement("Roughness");
		element->SetAttribute("Factor", material->roughnessFactor);
		node->LinkEndChild(element);

		element = document->NewElement("UseTextureInfo");
		element->SetAttribute("Diffuse", material->bUseDiffuseTexture);
		element->SetAttribute("Normal", material->bUseNormalTexture);
		element->SetAttribute("Metallic", material->bUseMetallicTexture);
		element->SetAttribute("Roughness", material->bUseRoughnessTexture);
		node->LinkEndChild(element);

	}

	std::string filePathString = std::string().assign(FilePath.begin(), FilePath.end());
	document->SaveFile(filePathString.c_str());
}

void AssimpConverter::WriteModelFile(const std::wstring& filePath)
{
	auto path = std::filesystem::path(filePath);

	std::filesystem::create_directories(path.parent_path());

	std::shared_ptr<FileUtils> file = std::make_shared<FileUtils>();
	file->Open(filePath, FileMode::Write);

	// Bone Data
	file->Write<uint32>((uint32)m_bones.size());
	for (auto& bone : m_bones)
	{
		file->Write<int32>(bone->index);
		file->Write<std::string>(bone->name);
		file->Write<int32>(bone->parent);
		file->Write<Matrix>(bone->transform);
	}
	file->Write<uint32>((uint32)m_meshes.size());
	for (auto& mesh : m_meshes)
	{
		file->Write<Matrix>(mesh->localTM);

		file->Write<std::string>(mesh->name);
		file->Write<int32>(mesh->boneIndex);
		file->Write<std::string>(mesh->materialName);

		// vertex
		file->Write<uint32>((uint32)mesh->vertices.size());
		file->Write(&mesh->vertices[0], sizeof(BasicVertex) * (uint32)mesh->vertices.size());

		// index
		file->Write<uint32>((uint32)mesh->indices.size());
		file->Write(&mesh->indices[0], sizeof(uint32) * (uint32)mesh->indices.size());

		// bound
		file->Write<Vector3>(mesh->minBounds);
		file->Write<Vector3>(mesh->maxBounds);
	}
}

void AssimpConverter::ReadModelData(aiNode* node, int32 index, int32 parent, bool isNegative /*= false*/)
{
	std::shared_ptr<AssimpConvert::Bone> bone = std::make_shared<AssimpConvert::Bone>();
	bone->index = index;
	bone->parent = parent;
	bone->name = node->mName.C_Str();

	/// Relative Transform
	// float 첫번째 주소 값으로 matrix 복사
	Matrix transform(node->mTransformation[0]);
	bone->transform = transform.Transpose();

	// root (local)
	Matrix matParent = Matrix::Identity;
	if (parent >= 0)
		matParent = m_bones[parent]->transform;

	m_bones.push_back(bone);

	Vector3 scale;
	Quaternion rot;
	Vector3 pos;
	bone->transform.Decompose(scale, rot, pos);
	// rot *= Quaternion::CreateFromYawPitchRoll({ 0.0f, 1.5f, 0.0f });

// 	Quaternion q = Quaternion::CreateFromYawPitchRoll(Vector3(0.0f, 3.14f, 0.0f));
// 	rot *= q;
// 
// 	bone->transform = Matrix::CreateScale(scale);
// 	bone->transform *= Matrix::CreateFromQuaternion(rot);
//  	bone->transform *= Matrix::CreateTranslation(pos);

	if (scale.x < 0)
		bone->isNegative = true;

	if (bone->parent < 0 || m_bones[bone->parent]->parent <= 0)
	{
		matParent = Matrix::Identity;
	}
	bone->transform = bone->transform * matParent;


	isNegative = bone->isNegative != isNegative;

	ReadMeshData(node, index, scale, isNegative);

	for (uint32 i = 0; i < node->mNumChildren; ++i)
	{
		ReadModelData(node->mChildren[i], (uint32)m_bones.size(), index, isNegative);
	}
}

void AssimpConverter::ReadMeshData(aiNode* node, int32 bone, Vector3 scale, bool isNegative /*= false*/)
{
	// 마지막 노드는 정보를 들고 있다.
	if (node->mNumMeshes < 1)
	{
		return;
	}

	// submesh
	for (uint32 i = 0; i < node->mNumMeshes; ++i)
	{
		std::shared_ptr<AssimpConvert::Mesh> mesh = std::make_shared<AssimpConvert::Mesh>();
		mesh->name = node->mName.C_Str();
		mesh->boneIndex = bone;
		mesh->localTM = m_bones[bone]->transform;

		uint32 index = node->mMeshes[i];
		const aiMesh* srcMesh = m_scene->mMeshes[index];

		// Material 이름
		const aiMaterial* material = m_scene->mMaterials[srcMesh->mMaterialIndex];
		mesh->materialName = material->GetName().C_Str();

		// mesh가 여러개일 경우 index가 중복될 수 있다. 
		// 하나로 관리하기 위해 미리 이전 vertex의 size를 가져와서 이번에 추가하는 index에 더해 중복을 피한다.

		const uint32 startVertex = (uint32)mesh->vertices.size();

		// AABB 계산
		AABB aabb = CalculateAABB(srcMesh);

		// AABB 값을 float 타입으로 저장
		mesh->minBounds = Vector3(aabb.minBounds.x, aabb.minBounds.y, aabb.minBounds.z);
		mesh->maxBounds = Vector3(aabb.maxBounds.x, aabb.maxBounds.y, aabb.maxBounds.z);
		Vector3 minBounds(FLT_MAX, FLT_MAX, FLT_MAX);
		Vector3 maxBounds(-FLT_MAX, -FLT_MAX, -FLT_MAX);
		bool isChangeBound = false;
		// Vertex
		for (uint32 v = 0; v < srcMesh->mNumVertices; ++v)
		{
			BasicVertex vertex;
			{
				memcpy(&vertex.Position, &srcMesh->mVertices[v], sizeof(Vector3));
				//2024.08.13 본의 위치를 곱해준다.
				if (m_bones[bone]->parent > 0)
				{
					isChangeBound = true;
					Vector3 result = Vector3::Transform(vertex.Position, m_bones[bone]->transform);
					vertex.Position = result;

					minBounds.x = std::min(minBounds.x, result.x);
					minBounds.y = std::min(minBounds.y, result.y);
					minBounds.z = std::min(minBounds.z, result.z);

					maxBounds.x = max(maxBounds.x, result.x);
					maxBounds.y = max(maxBounds.y, result.y);
					maxBounds.z = max(maxBounds.z, result.z);
				}

			}

			// UV
			if (srcMesh->HasTextureCoords(0))
			{
				memcpy(&vertex.UV, &srcMesh->mTextureCoords[0][v], sizeof(Vector2));
			}

			// Normal
			if (srcMesh->HasNormals())
			{
				memcpy(&vertex.Normal, &srcMesh->mNormals[v], sizeof(Vector3));
				if (m_bones[bone]->parent > 0)
				{
					Vector3 normal = Vector3::TransformNormal(vertex.Normal, m_bones[bone]->transform);
					vertex.Normal = normal;
				}
			}

			// Tangent
			if (srcMesh->HasTangentsAndBitangents())
			{
				memcpy(&vertex.Tangent, &srcMesh->mTangents[v], sizeof(Vector3));
				if (m_bones[bone]->parent > 0)
				{
					Vector3 tangent = Vector3::TransformNormal(vertex.Normal, m_bones[bone]->transform);
					vertex.Tangent = tangent;
				}
			}

			mesh->vertices.push_back(vertex);
		}

		if (isChangeBound)
		{
			mesh->minBounds = minBounds;
			mesh->maxBounds = maxBounds;
		}
		// Index
		for (uint32 f = 0; f < srcMesh->mNumFaces; ++f)
		{
			aiFace& face = srcMesh->mFaces[f];

			for (uint32 k = 0; k < face.mNumIndices; ++k)
			{
				mesh->indices.push_back(face.mIndices[k]);
			}
		}
		if (isNegative)
		{
			for (uint32 k = 0; k < mesh->indices.size(); k += 3)
			{
				auto temp = mesh->indices[k];
				mesh->indices[k] = mesh->indices[k + 2];
				mesh->indices[k + 2] = temp;
			}
		}
		m_meshes.push_back(mesh);
	}
}

uint32 AssimpConverter::GetBoneIndex(const std::string& name)
{
	for (std::shared_ptr<AssimpConvert::Bone>& bone : m_bones)
	{
		if (bone->name == name)
		{
			return bone->index;
		}
	}
	assert(false);
	MessageBox(NULL, L"Cant find Bone Index", L"AssimpConverter::GetBoneIndex", MB_OK);
	return 0;
}

void AssimpConverter::WriteVertexPositionFile(const std::wstring& filePath)
{
	auto path = std::filesystem::path(filePath);

	std::filesystem::create_directories(path.parent_path());

	std::shared_ptr<FileUtils> file = std::make_shared<FileUtils>();
	file->Open(filePath, FileMode::Write);

	// 매쉬 개수
	file->Write<uint32>((uint32)m_meshes.size());

	for (auto& mesh : m_meshes)
	{
		// vertex 개수
		file->Write<uint32>((uint32)mesh->vertices.size());
		//file->Write(&mesh->vertices[0], sizeof(BasicVertex) * (uint32)mesh->vertices.size());

		for (uint32 i = 0; i < (uint32)mesh->vertices.size(); ++i)
		{
			file->Write<float>(mesh->vertices[i].Position.x);
			file->Write<float>(mesh->vertices[i].Position.y);
			file->Write<float>(mesh->vertices[i].Position.z);
		}

		file->Write<uint32>((uint32)mesh->indices.size());
		for (uint32 i = 0; i < (uint32)mesh->indices.size(); ++i)
		{
			file->Write<uint32>(mesh->indices[i]);
		}
	}
}

#undef max

AssimpConverter::AABB AssimpConverter::CalculateAABB(const aiMesh* mesh)
{
	AABB aabb;
	// 초기 최소값, 최대값 설정
	aabb.minBounds = aiVector3D(std::numeric_limits<float>::max());
	aabb.maxBounds = aiVector3D(std::numeric_limits<float>::lowest());

	// 정점들을 순회하며 AABB 경계 계산
	for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
		const aiVector3D& vertex = mesh->mVertices[i];

		aabb.minBounds.x = std::min(aabb.minBounds.x, vertex.x);
		aabb.minBounds.y = std::min(aabb.minBounds.y, vertex.y);
		aabb.minBounds.z = std::min(aabb.minBounds.z, vertex.z);

		aabb.maxBounds.x = std::max(aabb.maxBounds.x, vertex.x);
		aabb.maxBounds.y = std::max(aabb.maxBounds.y, vertex.y);
		aabb.maxBounds.z = std::max(aabb.maxBounds.z, vertex.z);
	}

	return aabb;
}
