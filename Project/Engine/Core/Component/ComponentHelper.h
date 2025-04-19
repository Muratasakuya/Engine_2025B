#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Component/Base/IComponent.h>
#include <Engine/Core/Component/TransformComponent.h>
#include <Engine/Core/Component/MaterialComponent.h>
#include <Engine/Core/Component/PrimitiveMeshComponent.h>
#include <Engine/Core/Component/AnimationComponent.h>
#include <Engine/Core/Component/SpriteComponent.h>

// imgui
#include <imgui.h>
// c++
#include <string>
#include <optional>

//============================================================================
//	ComponentHelper class
//============================================================================
class Component {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	template <typename T>
	static T* GetComponent(uint32_t id);

	template <typename T>
	static std::vector<T*> GetComponentList(uint32_t id);
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- functions ----------------------------------------------------

	Component() = delete;
	~Component() = delete;
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

	static uint32_t CreateObject3D(const std::string& modelName, const std::string& objectName,
		const std::optional<std::string>& groupName = std::nullopt,
		const std::optional<std::string>& animationName = std::nullopt);

	static void RemoveObject3D(uint32_t entityId);

	//---------- effect ------------------------------------------------------

	static uint32_t CreateEffect(const std::string& modelName, const std::string& textureName,
		const std::string& objectName, const std::optional<std::string>& groupName = std::nullopt);

	static void RemoveEffect(uint32_t entityId);

	//--------- object2D -----------------------------------------------------

	static uint32_t CreateObject2D(const std::string& textureName, const std::string& objectName,
		const std::optional<std::string>& groupName = std::nullopt);

	static void RemoveObject2D(uint32_t entityId);
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- functions ----------------------------------------------------

	GameObjectHelper() = delete;
	~GameObjectHelper() = delete;
};