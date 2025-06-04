#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/ECS/System/Base/ISystem.h>
#include <Engine/Core/ECS/Components/SpriteComponent.h>

// front
class Transform2DComponent;
class SpriteMaterialComponent;

//============================================================================
//	structure
//============================================================================

struct SpriteData {

	Transform2DComponent* transform;
	SpriteMaterialComponent* material;
	SpriteComponent* sprite;
};

//============================================================================
//	SpriteBufferSystem class
//============================================================================
class SpriteBufferSystem :
	public ISystem {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	SpriteBufferSystem() = default;
	~SpriteBufferSystem() = default;

	Archetype Signature() const override;

	void Update(EntityManager& entityManager) override;

	//--------- accessor -----------------------------------------------------

	const std::vector<SpriteData>& GetSpriteData(SpriteLayer layer) { return spriteDataMap_[layer]; }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	std::unordered_map<SpriteLayer, std::vector<SpriteData>> spriteDataMap_;
};