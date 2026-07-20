#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Object/Base/Interface/IGameObject.h>
#include <Engine/Collision/Collider.h>

// data
#include <Engine/Object/Data/Skinned/SkinnedAnimation.h>
#include <Engine/Object/Data/Render/MeshRender.h>

namespace SakuEngine {

	//============================================================================
	//	GameObject3D class
	//	3Dオブジェクトの基底クラス
	//============================================================================
	class GameObject3D :
		public IGameObject,
		public Collider {
	public:
		//========================================================================
		//	public Methods
		//========================================================================

		GameObject3D() = default;
		virtual ~GameObject3D() = default;

		// 初期化
		void Init(const std::string& modelName, const std::string& name,
			const std::string& groupName, const std::optional<std::string>& animationName = std::nullopt);
		virtual void DerivedInit() override {}

		virtual void Update() {}

		// imgui
		void ImGui() override;
		virtual void DerivedImGui() override {}

		// json
		// transform
		void ApplyTransform(const Json& data);
		void SaveTransform(Json& data);
		// material
		void ApplyMaterial(const Json& data);
		void SaveMaterial(Json& data);

		//--------- accessor -----------------------------------------------------

		/*---------- setter ----------*/

		// mode
		void SetUpdateMode(ObjectUpdateMode mode);

		// transform
		// SRT
		void SetScale(const SakuEngine::Vector3& scale) { transform_->SetScale(scale); }
		void SetRotation(const Quaternion& rotation) { transform_->SetRotation(rotation); }
		void SetEulerRotation(const SakuEngine::Vector3& rotation) { transform_->SetEulerRotation(rotation); }
		void SetTranslation(const SakuEngine::Vector3& translation) { transform_->SetTranslation(translation); }
		void SetOffsetTranslation(const SakuEngine::Vector3& translation) { transform_->SetOffsetTranslation(translation); }
		void SetIsDirty(bool isDirty) { transform_->SetIsDirty(isDirty); }
		void SetIgnoreParentScale(bool isIgnore) { transform_->SetIgnoreParentScale(isIgnore); }
		void SetSRT(const SakuEngine::Transform3D& transform);
		// 親
		void SetParent(const SakuEngine::Transform3D& parent, bool isNull = false);
		const SakuEngine::Transform3D* GetJointTransform(const std::string& jointName) const;

		// material
		// 色
		void SetColor(const SakuEngine::Color& color, std::optional<uint32_t> meshIndex = std::nullopt);
		void SetAlpha(float alpha, std::optional<uint32_t> meshIndex = std::nullopt);
		void SetCastShadow(bool cast, std::optional<uint32_t> meshIndex = std::nullopt);
		void SetShadowRate(float rate, std::optional<uint32_t> meshIndex = std::nullopt);
		void SetIsRejection(bool isRejection, std::optional<uint32_t> meshIndex = std::nullopt);
		// texture
		void SetTextureName(const std::string& textureName, std::optional<uint32_t> meshIndex = std::nullopt);

		// meshRender
		void SetMeshRenderView(MeshRenderView renderView) { meshRender_->renderView = renderView; }
		void SetBlendMode(BlendMode blendMode) { meshRender_->blendMode = blendMode; }

		// animation
		void SetNextAnimation(const std::string& nextAnimationName, bool loopAnimation, float transitionDuration);
		void SetDebugViewBone(bool enable) { animation_->SetDebugViewBone(enable); }
		void SetPlaybackSpeed(float playbackSpeed) { animation_->SetPlaybackSpeed(playbackSpeed); }
		void SetCurrentAnimTime(float time) { animation_->SetCurrentAnimTime(time); }
		void ResetAnimation();
		Vector3 GetJointWorldPos(const std::string& jointName) const;
		bool IsAnimationFinished() const { return animation_->IsAnimationFinished(); }
		bool IsAnimationTransition() const { return animation_->IsTransition(); }
		int GetAnimationRepeatCount() const { return animation_->GetRepeatCount(); }
		float GetAnimationDuration(const std::string& animationName) const { return animation_->GetAnimationDuration(animationName); }
		float GetAnimationProgress() const { return animation_->GetProgress(); }
		bool IsEventKey(const std::string& keyEvent, uint32_t frameIndex) { return animation_->IsEventKey(keyEvent, frameIndex); }
		float GetEventTime(const std::string& animName, const std::string& keyEvent, uint32_t frameIndex) const;
		float GetPlaybackSpeed() const { return animation_->GetPlaybackSpeed(); }
		const std::string& GetCurrentAnimationName() const { return animation_->GetCurrentAnimationName(); }

		// postProcess
		void SetPostProcessMask(uint32_t mask, std::optional<uint32_t> meshIndex = std::nullopt);

		/*---------- getter ----------*/

		// transform
		void UpdateMatrix();
		const SakuEngine::Transform3D& GetTransform() const { return *transform_; }
		// SRT
		const SakuEngine::Vector3& GetScale() const { return transform_->GetScale(); }
		const Quaternion& GetRotation() const { return transform_->GetRotation(); }
		const SakuEngine::Vector3& GetTranslation() const { return transform_->GetTranslation(); }
	protected:
		//========================================================================
		//	protected Methods
		//========================================================================

		SakuEngine::Transform3D& TransformData() { return *transform_; }
		const SakuEngine::Transform3D& TransformData() const { return *transform_; }
		std::vector<Material>& MaterialsData() { return *materials_; }
		const std::vector<Material>& MaterialsData() const { return *materials_; }
		MeshRender& MeshRenderData() { return *meshRender_; }
		const MeshRender& MeshRenderData() const { return *meshRender_; }
		SkinnedAnimation* AnimationData() { return animation_; }
		const SkinnedAnimation* AnimationData() const { return animation_; }
	private:
		//========================================================================
		//	private Methods
		//========================================================================

		//--------- variables ----------------------------------------------------

		// material選択インデックス
		int selectedMaterialIndex_;
		SakuEngine::Transform3D* transform_ = nullptr;
		std::vector<Material>* materials_ = nullptr;
		MeshRender* meshRender_ = nullptr;
		SkinnedAnimation* animation_ = nullptr;

		//--------- functions ----------------------------------------------------

		// imgui
		void TransformImGui();
		void MaterialImGui();
		void MeshRenderImGui();
		void AnimationImGui();
	};

}; // SakuEngine
