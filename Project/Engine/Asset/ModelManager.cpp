#include "ModelManager.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Debug/Assert.h>
#include <Engine/Core/Debug/Logger.h>
#include <Engine/Asset/TextureManager.h>
#include <Engine/Core/Lib/Filesystem.h>

//============================================================================
//	ModelManager classMethods
//============================================================================

void ModelManager::Init(TextureManager* textureManager) {

	textureManager_ = nullptr;
	textureManager_ = textureManager;

	baseDirectoryPath_ = "./Assets/Models/";
	isCacheValid_ = false;
}

void ModelManager::Load(const std::string& modelName) {

	// モデルがすでにあれば読み込みは行わない
	if (models_.find(modelName) != models_.end()) {
		return;
	}

	// model検索
	std::filesystem::path filePath;
	bool found = false;
	for (const auto& entry : std::filesystem::recursive_directory_iterator(baseDirectoryPath_)) {
		if (entry.is_regular_file() &&
			entry.path().stem().string() == modelName) {

			std::string extension = entry.path().extension().string();
			if (extension == ".obj" || extension == ".gltf") {

				filePath = entry.path();
				found = true;
				break;
			}
		}
	}
	ASSERT(found, "model not found in directory or its subdirectories: " + modelName);

	models_[modelName] = LoadModelFile(filePath.string());
	Logger::Log("load model: " + modelName);
}

const ModelData& ModelManager::GetModelData(const std::string& modelName) const {

	ASSERT(models_.find(modelName) != models_.end(), "not found model" + modelName);
	return models_.at(modelName);
}

const std::vector<std::string>& ModelManager::GetModelKeys() const {

	if (!isCacheValid_) {

		modelKeysCache_.clear();
		modelKeysCache_.reserve(models_.size());
		for (const auto& pair : models_) {

			modelKeysCache_.emplace_back(pair.first);
		}
		isCacheValid_ = true;
	}
	return modelKeysCache_;
}

ModelData ModelManager::LoadModelFile(const std::string& filePath) {

	ModelData modelData;            // 構築するModelData
	std::vector<Vector4> positions; // 位置
	std::vector<Vector3> normals;   // 法線
	std::vector<Vector2> texcoords; // テクスチャ座標
	std::string line;               // ファイルから読んだ1行を格納するもの

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filePath.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs);

	// メッシュがないのには対応しない
	assert(scene->HasMeshes());

	// メッシュ解析
	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {

		aiMesh* mesh = scene->mMeshes[meshIndex];

		// 法線がないMeshは今回は非対応
		assert(mesh->HasNormals());
		// TexcoordがないMeshは今回は非対応
		assert(mesh->HasTextureCoords(0));

		MeshModelData meshModelData;
		// 最初に頂点数分のメモリを確保しておく
		meshModelData.vertices.resize(mesh->mNumVertices);

		// vertex解析
		for (uint32_t vertexIndex = 0; vertexIndex < mesh->mNumVertices; ++vertexIndex) {

			aiVector3D pos = mesh->mVertices[vertexIndex];
			aiVector3D normal = mesh->mNormals[vertexIndex];
			aiVector3D texcoord = mesh->mTextureCoords[0][vertexIndex];

			// 座標系の変換
			meshModelData.vertices[vertexIndex].pos = { -pos.x,pos.y,pos.z,1.0f };
			meshModelData.vertices[vertexIndex].normal = { -normal.x,normal.y,normal.z };
			meshModelData.vertices[vertexIndex].texcoord = { texcoord.x,texcoord.y };
		}

		// index解析
		for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {

			aiFace& face = mesh->mFaces[faceIndex];
			assert(face.mNumIndices == 3);

			for (uint32_t element = 0; element < face.mNumIndices; ++element) {

				uint32_t vertexIndex = face.mIndices[element];

				meshModelData.indices.push_back(vertexIndex);
			}
		}

		// bone解析
		for (uint32_t boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) {

			aiBone* bone = mesh->mBones[boneIndex];
			std::string jointName = bone->mName.C_Str();
			JointWeightData& jointWeightData = modelData.skinClusterData[jointName];

			aiMatrix4x4 bindPoseMatrixAssimp = bone->mOffsetMatrix.Inverse();
			aiVector3D scale, translate;
			aiQuaternion rotate;
			bindPoseMatrixAssimp.Decompose(scale, rotate, translate);

			Matrix4x4 bindPoseMatrix =
				Matrix4x4::MakeAxisAffineMatrix({ scale.x,scale.y,scale.z }, { rotate.x,-rotate.y,-rotate.z,rotate.w }, { -translate.x,translate.y,translate.z });
			jointWeightData.inverseBindPoseMatrix = Matrix4x4::Inverse(bindPoseMatrix);

			for (uint32_t weightIndex = 0; weightIndex < bone->mNumWeights; ++weightIndex) {

				jointWeightData.vertexWeights.push_back({ bone->mWeights[weightIndex].mWeight, bone->mWeights[weightIndex].mVertexId });
			}
		}

		// material解析
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0) {

			aiString textureName;
			material->GetTexture(aiTextureType_DIFFUSE, 0, &textureName);
			meshModelData.textureName = textureName.C_Str();

			std::filesystem::path name(meshModelData.textureName.value());
			std::string identifier = name.stem().string();
			meshModelData.textureName = identifier;

			textureManager_->Load(meshModelData.textureName.value());
		}

		modelData.meshes.push_back(meshModelData);
	}

	// 階層構造の作成
	modelData.rootNode = ReadNode(scene->mRootNode);

	return modelData;
}

Node ModelManager::ReadNode(aiNode* node) {

	Node result;

	// nodeのlocalMatrixを取得
	aiMatrix4x4 aiLocalMatrix = node->mTransformation;

	// 列ベクトル形式を行ベクトル形式に転置
	aiLocalMatrix.Transpose();

	aiVector3D scale, translate;
	aiQuaternion rotate;

	// assimpの行列からSRTを抽出する関数
	node->mTransformation.Decompose(scale, rotate, translate);
	result.transform.scale = { scale.x,scale.y ,scale.z };
	// X軸を反転、さらに回転方向が逆なので軸を反転させる
	result.transform.rotate = { rotate.x,-rotate.y ,-rotate.z,rotate.w };
	// X軸を反転
	result.transform.translate = { -translate.x,translate.y ,translate.z };
	result.localMatrix =
		Matrix4x4::MakeAxisAffineMatrix(result.transform.scale, result.transform.rotate, result.transform.translate);

	// Node名を格納
	result.name = node->mName.C_Str();
	// 子どもの数だけ確保
	result.children.resize(node->mNumChildren);

	for (uint32_t childIndex = 0; childIndex < node->mNumChildren; ++childIndex) {

		// 再帰的に読んで階層構造を作っていく
		result.children[childIndex] = ReadNode(node->mChildren[childIndex]);
	}

	return result;
}