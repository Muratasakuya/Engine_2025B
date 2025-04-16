#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/Mesh/PrimitiveMesh.h>

// c++
#include <memory>
// front
class Asset;

//============================================================================
//	PrimitiveMeshComponent class
//============================================================================
class PrimitiveMeshComponent {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	PrimitiveMeshComponent() = default;
	~PrimitiveMeshComponent() = default;

	void Init(ID3D12Device* device, Asset* asset, const std::string& modelName);

	void ImGui(float itemWidth);
	//--------- accessor -----------------------------------------------------

	PrimitiveMesh* GetPrimitiveMesh() const { return primitiveMesh_.get(); }

	//--------- operator -----------------------------------------------------

	// this <-> orther
	PrimitiveMeshComponent(PrimitiveMeshComponent&&) noexcept = default;
	PrimitiveMeshComponent& operator=(PrimitiveMeshComponent&&) noexcept = default;

	// ×copy
	PrimitiveMeshComponent(const PrimitiveMeshComponent&) = delete;
	PrimitiveMeshComponent& operator=(const PrimitiveMeshComponent&) = delete;
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	Asset* asset_;

	std::unique_ptr<PrimitiveMesh> primitiveMesh_;

	// imgui
	int selectBlendModeIndex_;

	//--------- functions ----------------------------------------------------

	// meshletの作成
	ResourceMesh CreateMeshlet(const std::string& modelName);
};