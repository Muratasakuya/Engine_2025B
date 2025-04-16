#include "PrimitiveMeshData.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Asset/Asset.h>
#include <Lib/MathUtils/Algorithm.h>

//============================================================================
//	PrimitiveMeshData classMethods
//============================================================================

void PrimitiveMeshData::Create(Asset* asset) {

	asset_ = nullptr;
	asset_ = asset;

	// 追加可能なprimitive情報を作成
	CreatePlane();
}

const MeshModelData& PrimitiveMeshData::GetMeshData(const std::string& name) const {

	if (!Algorithm::Find(meshData_, name)) {

		ASSERT(FALSE, name + "is not found createList");
	}

	return meshData_.at(name);
}

void PrimitiveMeshData::CreatePlane() {

	// 平面、billboard+Z向き
	meshNames_.emplace_back("primitivePlane");

	meshData_[meshNames_.back()] = MeshModelData();
	auto& meshData = meshData_[meshNames_.back()];

	// 半分のサイズ
	const float halfSize = 0.5f;
	// 法線
	const Vector3 normal = Vector3(0.0f, 0.0f, 1.0f);

	// 頂点情報
	meshData.vertices = {
		{Vector4(-halfSize,-halfSize,0.0f,1.0f), Vector2(0.0f, 1.0f), normal}, // 左下
		{Vector4(halfSize,-halfSize,0.0f,1.0f), Vector2(1.0f, 1.0f), normal},  // 右下
		{Vector4(-halfSize, halfSize,0.0f,1.0f), Vector2(0.0f, 0.0f), normal}, // 左上
		{Vector4(halfSize, halfSize,0.0f,1.0f), Vector2(1.0f, 0.0f), normal},  // 右上
	};

	// index情報
	meshData.indices = {
		0, 1, 2,
		2, 1, 3
	};

	// modelの登録
	asset_->MakeModel(meshNames_.back(), meshData.vertices, meshData.indices);
}