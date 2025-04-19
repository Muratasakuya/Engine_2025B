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

	void Update();

	void ImGui(float itemWidth);
	//--------- accessor -----------------------------------------------------

	// 頂点情報
	ResourceMesh* GetResourceMesh() { return resourceMesh_.get(); };

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

	// 頂点リソース情報
	std::unique_ptr<ResourceMesh> resourceMesh_;

	std::unique_ptr<PrimitiveMesh> primitiveMesh_;

	// imgui
	int selectBlendModeIndex_;

	//--------- functions ----------------------------------------------------

	// meshletの作成
	std::unique_ptr<ResourceMesh> CreateMeshlet(const std::string& modelName);

	void SelectBlendMode();

	void EditVertex();
};