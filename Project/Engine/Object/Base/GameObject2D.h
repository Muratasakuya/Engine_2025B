#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Object/Base/Interface/IGameObject.h>

// data
#include <Engine/Object/Data/Sprite.h>
// front
class BaseCamera;

//============================================================================
//	GameObject2D class
//============================================================================
class GameObject2D :
	public IGameObject {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	GameObject2D() = default;
	virtual ~GameObject2D() = default;

	void Init(const std::string& textureName, const std::string& name, const std::string& groupName);

	void DerivedInit() override {};

	// imgui
	void ImGui() override {};
	virtual void DerivedImGui() override {}
	// 各パラメータ
	bool ImGuiSize();

	//--------- accessor -----------------------------------------------------

	/*---------- setter ----------*/

	// transform
	// 座標
	void SetTranslation(const Vector2& translation) { transform_->translation = translation; }
	void SetCenterTranslation();
	void ProjectToScreen(const Vector3& translation, const BaseCamera& camera);
	// サイズ
	void SetSize(const Vector2& size) { transform_->size = size; }
	void SetTextureSize(const Vector2& size) { transform_->textureSize = size; }
	void SetTextureLeftTop(const Vector2& leftTop) { transform_->textureLeftTop = leftTop; }
	// アンカー
	void SetAnchor(const Vector2& anchor) { transform_->anchorPoint = anchor; }
	// 親
	void SetParent(const Transform2D& parent) { transform_->parent = &parent; }
	// 頂点オフセット
	void SetVertexOffset(uint32_t index, const Vector2& offset) { transform_->vertexOffset_[index] = offset; };

	// material
	// 色
	void SetColor(const Color& color) { material_->material.color = color; }
	void SetAlpha(float alpha) { material_->material.color.a = alpha; }
	void SetEmissiveIntensity(float intensity) { material_->material.emissiveIntensity = intensity; }
	void SetEmissionColor(const Vector3& color) { material_->material.emissionColor = color; }
	// UV
	void SetUVTranslationX(float uvX) { material_->uvTransform.translation.x = uvX; }
	void SetUVTranslationY(float uvY) { material_->uvTransform.translation.y = uvY; }
	void SetUVScaleX(float uvX) { material_->uvTransform.scale.x = uvX; }
	void SetUVScaleY(float uvY) { material_->uvTransform.scale.y = uvY; }

	// sprite
	void SetTextureName(const std::string& textureName) { sprite_->SetTextureName(textureName); }
	void SetAlphaTextureName(const std::string& textureName) { sprite_->SetAlphaTextureName(textureName); }
	void SetSpriteLayer(SpriteLayer layer) { sprite_->SetLayer(layer); }
	void SetSpriteLayerIndex(SpriteLayerIndex layerIndex, uint16_t subLayerIndex = 0) { sprite_->SetLayerIndex(layerIndex, subLayerIndex); }
	void SetPostProcessEnable(bool enable) { sprite_->SetPostProcessEnable(enable); }
	void SetBlendMode(BlendMode blendMode) { sprite_->SetBlendMode(blendMode); }

	/*---------- getter ----------*/

	// transform
	const Transform2D& GetTransform() const { return *transform_; }
	const Vector2& GetTranslation() const { return transform_->translation; }
	const Vector2& GetSize() const { return transform_->size; }
	const Vector2& GetTextureSize() const { return transform_->textureSize; }

	// material
	const Color& GetColor() const { return material_->material.color; }
protected:
	//========================================================================
	//	protected Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	// data
	// transform
	Transform2D* transform_;
	// material
	SpriteMaterial* material_;
	// sprite
	Sprite* sprite_;

private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	std::string uniqueName_;
};