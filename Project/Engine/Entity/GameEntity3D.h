#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Entity/Interface/IGameEntity.h>
#include <Engine/Collision/Collider.h>

// components
#include <Engine/Core/ECS/Components/AnimationComponent.h>

//============================================================================
//	GameEntity3D class
//============================================================================
class GameEntity3D :
	public IGameEntity,
	public Collider {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	GameEntity3D() = default;
	~GameEntity3D() = default;

	void Init(const std::string& modelName, const std::string& name,
		const std::string& groupName, const std::optional<std::string>& animationName = std::nullopt);

	/*-------- collision ----------*/

	// 衝突コールバック関数
	virtual void OnCollisionEnter([[maybe_unused]] const CollisionBody* collisionBody) override {}
	virtual void OnCollisionStay([[maybe_unused]] const CollisionBody* collisionBody) override {}
	virtual void OnCollisionExit([[maybe_unused]] const CollisionBody* collisionBody) override {}

	//--------- accessor -----------------------------------------------------

	/*---------- setter ----------*/

	// transform
	// SRT
	void SetScale(const Vector3& scale) { transform_->scale = scale; }
	void SetRotation(const Quaternion& rotation) { transform_->rotation = rotation; }
	void SetTranslation(const Vector3& translation) { transform_->translation = translation; }
	// 親
	void SetParent(const Transform3DComponent& parent) { transform_->parent = &parent; }

	// material
	// 色
	void SetColor(const Color& color, std::optional<uint32_t> meshIndex = std::nullopt);
	void SetAlpha(float alpha, std::optional<uint32_t> meshIndex = std::nullopt);
	// texture
	void SetTextureName(const std::string& textureName, std::optional<uint32_t> meshIndex = std::nullopt);

	/*---------- getter ----------*/

	// transform
	const Transform3DComponent& GetTransform() const { return *transform_; }
	// SRT
	const Vector3& GetScale() const { return transform_->scale; }
	const Quaternion& GetRotation() const { return transform_->rotation; }
	const Vector3& GetTranslation() const { return transform_->translation; }
protected:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	// components
	// transform
	Transform3DComponent* transform_;
	// material
	std::vector<MaterialComponent>* materials_;
	// animation
	AnimationComponent* animation_;
};