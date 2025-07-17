#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Asset/AssetStructure.h>
#include <Engine/Object/System/Base/ISystem.h>
#include <Engine/Object/Data/Material.h>

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

	void Init(std::vector<Material>& materials,
		const ModelData& modelData, class Asset* asset);

	Archetype Signature() const override;

	void Update(ObjectPoolManager& ObjectPoolManager) override;
};

//============================================================================
//	SpriteMaterialSystem class
//============================================================================
class SpriteMaterialSystem :
	public ISystem {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	SpriteMaterialSystem() = default;
	~SpriteMaterialSystem() = default;

	void Init();

	Archetype Signature() const override;

	void Update(ObjectPoolManager& ObjectPoolManager) override;
};