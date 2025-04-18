#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Component/PrimitiveMeshComponent.h>

// c++
#include <optional>

//============================================================================
//	PrimitiveMeshTool class
//============================================================================
class PrimitiveMeshTool {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	PrimitiveMeshTool() = default;
	~PrimitiveMeshTool() = default;

	void EditPrimitiveMesh(const std::optional<int>& selectIndex);

	//--------- accessor -----------------------------------------------------

private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	std::optional<int> selectIndex_;

	const float itemWidth_ = 216.0f;

	// texcoord
	int textureSerialCount_; // 連番画像の枚数の数

	//--------- functions ----------------------------------------------------

	void EditTransform();
	void EditMaterial();

	void EditPrimitive();
	// posを操作する
	void EditPos(PrimitiveMeshComponent* primitiveMesh);
	// texcoordを操作する
	void EditTexcoord(PrimitiveMeshComponent* primitiveMesh);
	// vertexColorを操作する
	void EditVertexColor(PrimitiveMeshComponent* primitiveMesh);
	// drawLineで疑似的にwireFrame描画を行う
	void DrawPrimitiveWireFrame(PrimitiveMeshComponent* primitiveMesh);
	// 頂点を操作しているときにsphereを表示する
	void DrawVertexSphere(PrimitiveMeshComponent* primitiveMesh,
		uint32_t meshIndex, uint32_t vertexIndex);
};