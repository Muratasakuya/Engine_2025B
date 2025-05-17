#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Asset/AssetStructure.h>
#include <Engine/Core/ECS/System/Base/ISystem.h>
#include <Engine/Core/Component/MaterialComponent.h>

//============================================================================
//	MaterialSystem class
//============================================================================
class MaterialSystem :
	public ISystem {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	MaterialSystem() = default;
	~MaterialSystem() = default;

	void Init(std::vector<MaterialComponent>& materials,
		const ModelData& modelData, class Asset* asset);

	Archetype Signature() const override;

	void Update(EntityManager& entityManager) override;
};