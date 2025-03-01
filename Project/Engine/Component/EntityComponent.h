#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Lib/EntityComponentStructures.h>

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

	//========================================================================
	//	friend class
	//========================================================================

	void Init(ID3D12Device* device, Asset* asset, SRVManager* srvManager);
	void Update();
	friend class Framework;

	const std::unordered_map<BlendMode, std::vector<EntityBufferPtr>>& GetBuffers() const;
	friend class MeshRenderer;
public:
	//========================================================================
	//	public Methods
	//========================================================================

	// entity追加
	EntityData* AddComponent(const std::string& modelName,
		const std::optional<std::string>& animationName = std::nullopt);
	// entity削除
	void RemoveComponent(uint32_t entityID);

	//--------- accessor -----------------------------------------------------

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

	//--------- functions ----------------------------------------------------

	void SortEntitiesByBlendMode() const;

	EntityComponent() = default;
	~EntityComponent() = default;
	EntityComponent(const EntityComponent&) = delete;
	EntityComponent& operator=(const EntityComponent&) = delete;
};