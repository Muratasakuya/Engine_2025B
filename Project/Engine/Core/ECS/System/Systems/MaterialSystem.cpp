#include "MaterialSystem.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Asset/Asset.h>
#include <Engine/Core/ECS/Core/EntityManager.h>

//============================================================================
//	MaterialSystem classMethods
//============================================================================

void MaterialSystem::Init(std::vector<MaterialComponent>& materials,
	const ModelData& modelData, Asset* asset) {

	// mesh数に合わせる
	materials.resize(modelData.meshes.size());
	for (uint32_t meshIndex = 0; meshIndex < modelData.meshes.size(); ++meshIndex) {

		materials[meshIndex].Init();
		materials[meshIndex].material.textureIndex =
			asset->GetTextureGPUIndex(modelData.meshes[meshIndex].textureName.value_or("white"));

		// normalMap用のTextureがあれば設定する
		if (modelData.meshes[meshIndex].normalMapTexture.has_value()) {

			materials[meshIndex].material.normalMapTextureIndex =
				asset->GetTextureGPUIndex(modelData.meshes[meshIndex].normalMapTexture.value());
			materials[meshIndex].material.enableNormalMap = true;
		}

		// baseColorがあれば色を設定する
		if (modelData.meshes[meshIndex].baseColor.has_value()) {

			materials[meshIndex].material.color = modelData.meshes[meshIndex].baseColor.value();
			materials[meshIndex].material.emissionColor = Vector3(
				modelData.meshes[meshIndex].baseColor.value().r,
				modelData.meshes[meshIndex].baseColor.value().g,
				modelData.meshes[meshIndex].baseColor.value().b);
		}
	}
}

Archetype MaterialSystem::Signature() const {

	Archetype arch{};
	arch.set(EntityManager::GetTypeID<MaterialComponent>());
	return arch;
}

void MaterialSystem::Update(EntityManager& entityManager) {

	for (uint32_t entity : entityManager.View(Signature())) {

		auto* materials = entityManager.GetComponent<MaterialComponent, true>(entity);
		for (auto& material : *materials) {

			material.UpdateUVTransform();
		}
	}
}

//============================================================================
//	SpriteMaterialSystem classMethods
//============================================================================

Archetype SpriteMaterialSystem::Signature() const {

	Archetype arch{};
	arch.set(EntityManager::GetTypeID<SpriteMaterialComponent>());
	return arch;
}

void SpriteMaterialSystem::Update(EntityManager& entityManager) {

	for (uint32_t entity : entityManager.View(Signature())) {

		auto* material = entityManager.GetComponent<SpriteMaterialComponent>(entity);
		material->UpdateUVTransform();
	}
}