#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Lib/DxStructures.h>
#include <Engine/Core/CBuffer/Transform.h>
#include <Engine/Component/ModelComponent.h>
#include <Engine/Component/TransformComponent.h>

//============================================================================
//	EntityComponentStructures
//============================================================================

// 描画情報
struct RenderingData {

	bool drawEnable;
	BlendMode blendMode;
};

// ユーザーが扱うデータ
struct EntityData {

	uint32_t id;
	Transform3D transform;
	std::vector<Material> materials;
	std::vector<UVTransform> uvTransforms;
	RenderingData renderingData;
};

// GPUに送るデータ
struct EntityBufferData {

	std::unique_ptr<BaseModel> model;
	DxConstBuffer<TransformationMatrix> transform;
	std::vector<DxConstBuffer<Material>> materials;
	RenderingData renderingData;
};

struct EntityAnimationBufferData {

	std::unique_ptr<AnimationModel> model;
	AnimationTransform transform;
	std::vector<DxConstBuffer<Material>> materials;
	RenderingData renderingData;
};