#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Component/Base/IComponent.h>
#include <Engine/Component/TransformComponent.h>
#include <Engine/Component/ModelComponent.h>
#include <Engine/Component/AnimationComponent.h>
#include <Engine/Component/SpriteComponent.h>
#include <Engine/Core/Lib/CBufferStructures.h>

// c++
#include <string>
#include <optional>

//============================================================================
//	ComponentHelper class
//============================================================================
class ComponentHelper {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	template <typename T>
	static T* GetComponent(EntityID id);

	template <typename T>
	static std::vector<T*> GetComponentList(EntityID id);
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- functions ----------------------------------------------------

	ComponentHelper() = delete;
	~ComponentHelper() = delete;
};

//============================================================================
//	GameObjectHelper class
//============================================================================
class GameObjectHelper {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	//--------- object3D -----------------------------------------------------

	static EntityID CreateObject3D(const std::string& modelName, const std::string& objectName,
		const std::optional<std::string>& groupName = std::nullopt,
		const std::optional<std::string>& instancingName = std::nullopt,
		const std::optional<std::string>& animationName = std::nullopt);

	static void RemoveObject3D(EntityID id);

	//--------- object2D -----------------------------------------------------

	static EntityID CreateObject2D(const std::string& textureName, const std::string& objectName,
		const std::optional<std::string>& groupName = std::nullopt);

	static void RemoveObject2D(EntityID id);
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- functions ----------------------------------------------------

	GameObjectHelper() = delete;
	~GameObjectHelper() = delete;
};