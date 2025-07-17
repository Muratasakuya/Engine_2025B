#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/Mesh/MeshletStructures.h>
#include <Lib/MathUtils/MathUtils.h>

// c++
#include <vector>
#include <cstdint>

//============================================================================
//	UVHelper class
//============================================================================
namespace UVHelper {

	// 連番画像の1枚分でmodelのtexcoordを設定する
	// objectId...    texcoordを設定するobjectのId
	// serialCount... 連番画像の連番数
	// 返り値...       x=1.0fまでのoffset
	std::vector<Vector2> ApplyFrameTexcoord(const ResourceMesh<MeshVertex>& resourceMesh, int serialCount);
}