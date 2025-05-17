#include "SystemManager.h"

//============================================================================
//	SystemManager classMethods
//============================================================================

void SystemManager::Update(EntityManager& entityManager) {

	for (const auto& system : std::views::values(systems_)) {

		system->Update(entityManager);
	}
}