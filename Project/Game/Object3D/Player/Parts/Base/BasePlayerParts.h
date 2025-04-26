#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Component/TransformComponent.h>
#include <Engine/Core/Component/MaterialComponent.h>
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

		// jsonのpath、この後にname + .jsonが入る
		const std::string baseFilePath = "Player/PartsParameter/";
		std::string name;          // 名前
		Vector3 offsetTranslation; //座標オフセット、これを動かす

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

	void Init(const std::string& modelName);

	// behavior
	void RegisterBehavior(PlayerBehaviorType type, std::unique_ptr<IPlayerBehavior> behavior);
	void ExecuteBehavior(PlayerBehaviorType type);

	// リセット
	void ResetBehavior(PlayerBehaviorType type);
	//--------- accessor -----------------------------------------------------

	void SetParent(const Transform3DComponent& parent);

	void SetParam(const PartsParameter& param);

	// transform
	void SetScale(const Vector3& scale) { transform_->scale = scale; }
	void SetRotate(const Quaternion& rotate) { transform_->rotation = rotate; }
	void SetTranslate(const Vector3& translate) { transform_->translation = translate; }

	const Transform3DComponent& GetTransform() const;
protected:
	//========================================================================
	//	protected Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	Input* input_;

	// components
	Transform3DComponent* transform_;
	MaterialComponent* material_;

	// behavior
	std::unordered_map<PlayerBehaviorType, std::unique_ptr<IPlayerBehavior>> behaviors_;

	// 共通parameter
	PartsParameter parameter_;

	//--------- functions ----------------------------------------------------

	void InputKey(Vector2& inputValue);
};