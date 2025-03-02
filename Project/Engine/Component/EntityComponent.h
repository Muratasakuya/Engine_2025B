#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Lib/EntityComponentStructures.h>

#include <Externals/DirectXMesh/DirectXMesh.h>

// c++
#include <cstdint>
#include <variant>
#include <unordered_map>
// front
class Asset;
class SRVManager;

//============================================================================
//	EntityComponent class
//============================================================================
class EntityComponent {
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- using --------------------------------------------------------

	using EntityBuffer = std::variant<EntityBufferData, EntityAnimationBufferData>;
	using EntityBufferPtr = std::variant<EntityBufferData*, EntityAnimationBufferData*>;
public:
	//========================================================================
	//	public Methods
	//========================================================================

	// entity追加
	EntityData* AddComponent(const std::string& modelName,
		const std::optional<std::string>& animationName = std::nullopt, const std::string& name = "object");
	// entity削除
	void RemoveComponent(uint32_t entityID);

	void Init(ID3D12Device* device, Asset* asset, SRVManager* srvManager);
	void Update();

	// imgui
	// 操作を行うobjectの選択
	void ImGuiSelectObject();
	// 選択されたobjectの操作
	void ImGuiEdit();

	//--------- accessor -----------------------------------------------------

	const std::unordered_map<BlendMode, std::vector<EntityBufferPtr>>& GetBuffers() const;

	// singleton
	static EntityComponent* GetInstance();
	static void Finalize();
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	static EntityComponent* instance_;

	ID3D12Device* device_;
	Asset* asset_;
	SRVManager* srvManager_;

	uint32_t nextID_ = 0;
	// ユーザーが使うデータ
	std::unordered_map<uint32_t, EntityData> entities_;

	// GPUに転送用
	std::unordered_map<uint32_t, EntityBuffer> buffers_;

	mutable std::unordered_map<BlendMode, std::vector<EntityBufferPtr>> sortedEntities_;
	mutable bool needsSorting_ = true;

	//--------- imgui ---------//

	// 識別名
	std::unordered_map<uint32_t, std::string> entityIdentifiers_;
	// imguiで選択されたidの保持
	std::optional<uint32_t> selectedEntityId_ = std::nullopt;

	mutable std::unordered_map<std::string, std::vector<std::pair<int, uint32_t>>> groupedEntities_;
	mutable size_t lastEntityCount_ = 0;

	std::unordered_map<std::string, int> nameCounts_;

	int selectedMaterialIndex_ = 0;

	//--------- functions ----------------------------------------------------

	void SortEntitiesByBlendMode() const;

	std::string CheckName(const std::string& name);
	std::string SplitBaseNameAndNumber(const std::string& name, int& number);

	// imguiの操作ウィンドウ
	void ImGuiEntityInformation();

	void ImGuiEntityDraw();

	void ImGuiEntityTransform();

	void ImGuiEntityMaterial();

	EntityComponent() = default;
	~EntityComponent() = default;
	EntityComponent(const EntityComponent&) = delete;
	EntityComponent& operator=(const EntityComponent&) = delete;
};