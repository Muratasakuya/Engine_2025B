#include "ComponentHelper.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Component/Manager/ComponentManager.h>

//============================================================================
//	ComponentHelper classMethods
//============================================================================

#define INSTANTIATE_GET_COMPONENT(TYPE) \
template TYPE* Component::GetComponent<TYPE>(EntityID);

#define INSTANTIATE_GET_COMPONENTLIST(TYPE) \
template std::vector<TYPE*> Component::GetComponentList<TYPE>(EntityID);

// 3D
INSTANTIATE_GET_COMPONENT(Transform3DComponent)
INSTANTIATE_GET_COMPONENT(Material)
INSTANTIATE_GET_COMPONENT(ModelComponent)
INSTANTIATE_GET_COMPONENT(AnimationComponent)
// 2D
INSTANTIATE_GET_COMPONENT(Transform2DComponent)
INSTANTIATE_GET_COMPONENT(SpriteComponent)

// 3D
INSTANTIATE_GET_COMPONENTLIST(Material)

template<typename T>
T* Component::GetComponent(EntityID id) {

	return ComponentManager::GetInstance()->GetComponent<T>(id);
}

template <typename T>
std::vector<T*> Component::GetComponentList(EntityID id) {

	return ComponentManager::GetInstance()->GetComponentList<T>(id);
}

//============================================================================
//	GameObjectHelper classMethods
//============================================================================

EntityID GameObjectHelper::CreateObject3D(const std::string& modelName,
	const std::string& objectName, const std::optional<std::string>& groupName,
	const std::optional<std::string>& instancingName, const std::optional<std::string>& animationName) {

	return ComponentManager::GetInstance()->CreateObject3D(
		modelName, objectName, groupName, instancingName, animationName);
}

void GameObjectHelper::RemoveObject3D(EntityID id) {

	ComponentManager::GetInstance()->RemoveObject3D(id);
}

EntityID GameObjectHelper::CreateObject2D(const std::string& textureName,
	const std::string& objectName, const std::optional<std::string>& groupName) {

	return ComponentManager::GetInstance()->CreateObject2D(
		textureName, objectName, groupName);
}

void GameObjectHelper::RemoveObject2D(EntityID id) {

	ComponentManager::GetInstance()->RemoveObject2D(id);
}