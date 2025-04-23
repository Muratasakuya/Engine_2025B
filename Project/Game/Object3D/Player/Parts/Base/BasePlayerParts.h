#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Component/TransformComponent.h>
#include <Engine/Core/Component/MaterialComponent.h>

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

	//--------- accessor -----------------------------------------------------

	void SetParent(const Transform3DComponent& parent);

	void SetParam(const PartsParameter& param);

	const Transform3DComponent& GetTransform() const;
protected:
	//========================================================================
	//	protected Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	// components
	Transform3DComponent* transform_;
	MaterialComponent* material_;
};