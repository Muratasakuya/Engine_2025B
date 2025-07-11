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
	virtual ~GameEntity3D() = default;

	void Init(const std::string& modelName, const std::string& name,
		const std::string& groupName, const std::optional<std::string>& animationName = std::nullopt);
	virtual void DerivedInit() override {}

	virtual void Update() {}

	// imgui
	void ImGui() override;
	virtual void DerivedImGui() override {}

	// json
	// material
	void ApplyMaterial(const Json& data);
	void SaveMaterial(Json& data);

	//--------- accessor -----------------------------------------------------

	/*---------- setter ----------*/

	// transform
	// SRT
	void SetScale(const Vector3& scale) { transform_->scale = scale; }
	void SetRotation(const Quaternion& rotation) { transform_->rotation = rotation; }
	void SetTranslation(const Vector3& translation) { transform_->translation = translation; }
	// 親
	void SetParent(const Transform3DComponent& parent) { transform_->parent = &parent; }
	const Transform3DComponent* GetJointTransform(const std::string& jointName) const;

	// material
	// 色
	void SetColor(const Color& color, std::optional<uint32_t> meshIndex = std::nullopt);
	void SetAlpha(float alpha, std::optional<uint32_t> meshIndex = std::nullopt);
	void SetCastShadow(bool cast, std::optional<uint32_t> meshIndex = std::nullopt);
	// texture
	void SetTextureName(const std::string& textureName, std::optional<uint32_t> meshIndex = std::nullopt);

	// animation
	void SetNextAnimation(const std::string& nextAnimationName, bool loopAnimation, float transitionDuration);
	void SetDebugViewBone(bool enable) { animation_->SetDebugViewBone(enable); }

	void ResetAnimation();
	Vector3 GetJointWorldPos(const std::string& jointName) const;
	bool IsAnimationFinished() const { return animation_->IsAnimationFinished(); }
	int GetAnimationRepeatCount() const { return animation_->GetRepeatCount(); }
	float GetAnimationDuration(const std::string& animationName) const { return animation_->GetAnimationDuration(animationName); }
	bool IsEventKey(uint32_t frameIndex) const { return animation_->IsEventKey(frameIndex); }

	/*---------- getter ----------*/

	// transform
	const Transform3DComponent& GetTransform() const { return *transform_; }
	// SRT
	const Vector3& GetScale() const { return transform_->scale; }
	const Quaternion& GetRotation() const { return transform_->rotation; }
	const Vector3& GetTranslation() const { return transform_->translation; }
protected:
	//========================================================================
	//	protected Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	// components
	// transform
	Transform3DComponent* transform_;
	// material
	std::vector<MaterialComponent>* materials_;
	// animation
	AnimationComponent* animation_;
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	// material選択インデックス
	int selectedMaterialIndex_;

	//--------- functions ----------------------------------------------------

	// imgui
	void TransformImGui();
	void MaterialImGui();
	void AnimationImGui();
};