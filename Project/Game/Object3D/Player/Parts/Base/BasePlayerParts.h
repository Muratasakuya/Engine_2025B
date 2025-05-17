#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/ECS/Components/TransformComponent.h>
#include <Engine/Core/ECS/Components/MaterialComponent.h>
#include <Game/Object3D/Player/Behavior/Parts/Base/IPlayerBehavior.h>

// c++
#include <unordered_map>
// front
class Input;

//============================================================================
//	BasePlayerParts class
//============================================================================
class BasePlayerParts {
	//========================================================================
	//	public Methods
	//========================================================================
public:

	//--------- structure ----------------------------------------------------

	// 各parts共通項目
	struct PartsParameter {

		std::string name;          // 名前
		Vector3 offsetTranslation; // 座標オフセット、これを動かす
		Vector3 initRotationAngle; // 初期回転角

		BasePlayerParts* owner;

		// imgui
		const float itemWidth = 224.0f;
		void ImGui();

		// json
		void ApplyJson();
		void SaveJson();
	};
public:
	//========================================================================
	//	public Methods
	//========================================================================

	BasePlayerParts() = default;
	virtual ~BasePlayerParts() = default;

	void Init(const std::string& modelName,
		const std::optional<std::string>& objectName = std::nullopt);

	// behavior
	void RegisterBehavior(PlayerBehaviorType type, std::unique_ptr<IPlayerBehavior> behavior);
	void ExecuteBehavior(PlayerBehaviorType type);

	// リセット
	void ResetBehavior(PlayerBehaviorType type);

	// material
	void ImGuiMaterial();
	void ApplyJson();
	void SaveJson();
	//--------- accessor -----------------------------------------------------

	void SetParent(const Transform3DComponent& parent);

	// transform
	void SetScale(const Vector3& scale) { transform_->scale = scale; }
	void SetRotate(const Quaternion& rotate) { transform_->rotation = rotate; }
	void SetTranslate(const Vector3& translate) { transform_->translation = translate; }

	const Transform3DComponent& GetTransform() const;

	template <typename T>
	T* GetBehavior(PlayerBehaviorType type) const;
protected:
	//========================================================================
	//	protected Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	Input* input_;

	// jsonのpath、この後にname + .jsonが入る
	const std::string baseInitJsonFilePath_ = "Player/PartsParameter/Init/";
	const std::string baseBehaviorJsonFilePath_ = "Player/PartsParameter/Behavior/";

	// components
	Transform3DComponent* transform_;
	MaterialComponent* material_;

	// behavior
	std::unordered_map<PlayerBehaviorType, std::unique_ptr<IPlayerBehavior>> behaviors_;

	// 共通parameter
	PartsParameter parameter_;

	//--------- functions ----------------------------------------------------

	// init
	void SetParam();

	Quaternion CalRotationAxisAngle(const Vector3& rotationAngle);

	void InputKey(Vector2& inputValue);
};

//============================================================================
//	BasePlayerParts templateMethods
//============================================================================

template<typename T>
inline T* BasePlayerParts::GetBehavior(PlayerBehaviorType type) const {

	auto it = behaviors_.find(type);
	if (it != behaviors_.end()) {

		return static_cast<T*>(it->second.get());
	}
	return nullptr;
}