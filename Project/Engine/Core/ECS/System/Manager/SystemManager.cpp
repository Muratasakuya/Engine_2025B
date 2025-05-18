#include "SystemManager.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/ECS/System/Systems/InstancedMeshSystem.h>

//============================================================================
//	SystemManager classMethods
//============================================================================

void SystemManager::UpdateComponent(EntityManager& entityManager) {

	// 各systemを更新
	for (const auto& [type, system] : systems_) {

		// bufferの転送処理は別で行う
		if (type == std::type_index(typeid(InstancedMeshSystem))) {
			continue;
		}
		system->Update(entityManager);
	}
}

void SystemManager::UpdateBuffer(EntityManager& entityManager) {

	// buffer転送処理
	this->GetSystem<InstancedMeshSystem>()->Update(entityManager);
}