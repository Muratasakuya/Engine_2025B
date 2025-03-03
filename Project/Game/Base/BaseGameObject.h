#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Lib/ComponentStructures.h>
#include <Engine/Component/ColliderComponent.h>

// c++
#include <string>
#include <optional>
#include <typeinfo>

//============================================================================
//	BaseGameObject class
//============================================================================
class BaseGameObject {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	BaseGameObject() = default;
	~BaseGameObject() = default;

	void CreateModel(const std::string& modelName,
		const std::optional<std::string>& animationName = std::nullopt);

	// collision
	ColliderComponent* AddCollider(const CollisionShape::Shapes& shape);
	void RemoveCollider(ColliderComponent* collider);

	//--------- accessor -----------------------------------------------------

	// Transform
	void SetScale(const Vector3& scale) { object_.transform->scale = scale; }
	void SetRotate(const Quaternion& rotate) { object_.transform->rotation = rotate; }
	void SetTranslate(const Vector3& translate) { object_.transform->translation = translate; }
protected:
	//========================================================================
	//	protected Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	Object3D object_;
	std::vector<UVTransform> uvTransforms_;

	//--------- functions ----------------------------------------------------

	std::string GetObjectName() const;
};