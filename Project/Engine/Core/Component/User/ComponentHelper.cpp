#include "ComponentHelper.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Component/Manager/ComponentManager.h>

//============================================================================
//	ComponentHelper classMethods
//============================================================================

#define INSTANTIATE_GET_COMPONENT(TYPE) \
template TYPE* Component::GetComponent<TYPE>(uint32_t);

#define INSTANTIATE_GET_COMPONENTLIST(TYPE) \
template std::vector<TYPE*> Component::GetComponentList<TYPE>(uint32_t);

// 3D
INSTANTIATE_GET_COMPONENT(Transform3DComponent)
INSTANTIATE_GET_COMPONENT(MaterialComponent)
INSTANTIATE_GET_COMPONENT(AnimationComponent)
// 2D
INSTANTIATE_GET_COMPONENT(Transform2DComponent)
INSTANTIATE_GET_COMPONENT(SpriteMaterial)
INSTANTIATE_GET_COMPONENT(SpriteComponent)

// 3D
INSTANTIATE_GET_COMPONENTLIST(MaterialComponent)

template<typename T>
T* Component::GetComponent(uint32_t id) {

	return ComponentManager::GetInstance()->GetComponent<T>(id);
}

template <typename T>
std::vector<T*> Component::GetComponentList(uint32_t id) {

	return ComponentManager::GetInstance()->GetComponentList<T>(id);
}

//============================================================================
//	GameObjectHelper classMethods
//============================================================================

void GameObjectHelper::SetImGuiFunc(uint32_t entityId, std::function<void()> func) {

	ComponentManager::GetInstance()->SetImGuiFunc(entityId, func);
}

uint32_t GameObjectHelper::CreateObject3D(const std::string& modelName,
	const std::string& objectName, const std::optional<std::string>& groupName,
	const std::optional<std::string>& animationName) {

	return ComponentManager::GetInstance()->CreateObject3D(
		modelName, objectName, groupName, animationName);
}

void GameObjectHelper::RemoveObject3D(uint32_t id) {

	ComponentManager::GetInstance()->RemoveObject3D(id);
}

uint32_t GameObjectHelper::CreateObject2D(const std::string& textureName,
	const std::string& objectName, const std::optional<std::string>& groupName) {

	return ComponentManager::GetInstance()->CreateObject2D(
		textureName, objectName, groupName);
}

void GameObjectHelper::RemoveObject2D(uint32_t id) {

	ComponentManager::GetInstance()->RemoveObject2D(id);
}