#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/GPUObject/CBufferStructures.h>
#include <Lib/MathUtils/Vector2.h>
#include <Lib/MathUtils/Vector3.h>
#include <Lib/MathUtils/Quaternion.h>

// c++
#include <format>

//============================================================================
//	BaseTransform class
//============================================================================
class BaseTransform {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	BaseTransform() = default;
	virtual ~BaseTransform() = default;

	void Init();

	void UpdateMatrix();

	void ImGui(float itemSize);

	void ToJson(Json& data);
	void FromJson(const Json& data);

	//--------- accessor -----------------------------------------------------

	Vector3 GetWorldPos() const;

	Vector3 GetForward() const;

	Vector3 GetBack() const;

	Vector3 GetRight() const;

	Vector3 GetLeft() const;

	Vector3 GetUp() const;

	Vector3 GetDown() const;

	bool IsDirty() const { return isDirty_; }

	//--------- variables ----------------------------------------------------

	Vector3 scale;
	Quaternion rotation;
	Vector3 translation;

	TransformationMatrix matrix;

	const BaseTransform* parent = nullptr;

	Vector3 eulerRotate;
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	Vector3 prevScale;
	Quaternion prevRotation;
	Vector3 prevTranslation;

	// 変更があったかどうかのフラグ
	bool isDirty_;
};

//============================================================================
//	Transform3DComponent class
//============================================================================
class Transform3DComponent :
	public BaseTransform {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	Transform3DComponent() = default;
	~Transform3DComponent() = default;

	//--------- accessor -----------------------------------------------------

	void SetInstancingName(const std::string& name) { meshInstancingName_ = name; }
	const std::string& GetInstancingName() const { return meshInstancingName_; }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	// meshInstancing用の名前
	std::string meshInstancingName_;
};

//============================================================================
//	Transform3DComponent class
//============================================================================
class Transform2DComponent {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	Transform2DComponent() = default;
	~Transform2DComponent() = default;

	void Init();

	void UpdateMatrix();

	void ImGui(float itemSize);

	void SetCenterPos();

	//--------- variables ----------------------------------------------------

	Vector2 translation;
	float rotation;

	Vector2 size;           // 表示サイズ
	Vector2 anchorPoint;    // アンカーポイント

	Vector2 textureLeftTop; // テクスチャ左上座標
	Vector2 textureSize;    // テクスチャ切り出しサイズ

	Matrix4x4 matrix;

	const Transform2DComponent* parent = nullptr;
};