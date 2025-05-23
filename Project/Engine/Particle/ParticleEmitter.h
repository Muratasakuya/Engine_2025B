#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Particle/ParticleParameter.h>
#include <Engine/Asset/AssetEditor.h>

// emitter
#include <Engine/Core/Graphics/Mesh/Mesh.h>
#include <Engine/Core/Graphics/GPUObject/DxConstBuffer.h>
#include <Engine/Core/ECS/Components/TransformComponent.h>

// imgui
#include <imgui.h>
// c++
#include <cstdint>
#include <list>

// 設計図
// emitterが複数のparticleを所持する
// emitterはtransformを持つ
// emitterは存在しているemitterを親に設定できるようにする
// 各particleに必要な情報
// mesh...       各particleにつき1つ持たせる
// texture...    particle内で別々のtextureを持たせられる、とりあえず同じでいい
// material...   PSの構造体と同じものを持たせる -> structuredBuffer
// transform...  euler角のtransformを持たせる -> structuredBuffer
// numInstance...各particleごとのinstance数、これをinstance数としてmeshに渡して描画する
// 上記がbufferに必要なデータ、各particleデータごとに必要
// 上記のbufferの値を決めるのがparameter
// parameterは各particleにつき1つ持たせる
// このparameterはeditorで操作可能にする
// parameterとは別でemitterがそれぞれのparticleをどのように制御するかのcontrollerクラスも作成する
// parameterの値でparticleをcreaterクラスで作成、
// 作成されたparticleをupdaterクラスで更新
// 更新処理にも種類がある
// 更新処理はsystemに渡して行う
// emitterはparticleを所持するだけのクラスにする
// systemはemittersを所持するクラスにする

//============================================================================
//	structures
//============================================================================

// bufferデータ
struct EffectMaterial {

	//色
	Color color;

	// texure
	uint32_t textureIndex;
	uint32_t noiseTextureIndex;

	// flags
	int32_t useNoiseTexture; // noiseTextureを使うか
	int32_t useVertexColor;  // 頂点色を使うか

	// alphaReference
	float textureAlphaReference;      // 貼るtexture
	float noiseTextureAlphaReference; // noiseTexture

	// emission
	float emissiveIntensity; // 発光強度
	Vector3 emissionColor;   // 発光色

	// uv
	Matrix4x4 uvTransform;
};

// 各particleの情報
struct ParticleData {

	// 動かす元の値
	// groupのparameterから値を取得してくる
	ParticleParameter parameter;

	// timer(T)
	float currentTime;    // 現在の経過時間
	float easedLifeT;     // currentTのeasing計算後の寿命値1.0f -> 0.0f
	float easedProgressT; // currentTのeasing計算後の値0.0f -> 1.0f

	// 移動方向(速度)
	Vector3 velocity;      // 設定速度、groupのparameterから値を取得してくる
	Vector3 easedVelocity; // easedProgressTを掛けた後の値

	// bufferに渡すデータ
	EffectMaterial material; // material
	BaseTransform transform; // transform(matrix)
};

// particleの集まり
struct ParticleGroup {

	// mesh情報
	std::unique_ptr<EffectMesh> mesh;
	// structuredBuffer
	DxConstBuffer<EffectMaterial> materialBuffer; // material
	DxConstBuffer<Matrix4x4> worldMatrixBuffer;   // matrix
	// instance数
	uint32_t numInstance;

	// particleを構築するparameter
	ParticleParameter parameter;
	// particle情報
	std::list<ParticleData> particles;
};

//============================================================================
//	ParticleEmitter class
//============================================================================
class ParticleEmitter {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	ParticleEmitter() = default;
	~ParticleEmitter() = default;

	void Init(const std::string& name, Asset* asset, ID3D12Device* device);

	void Update();

	void ImGui();
	//--------- accessor -----------------------------------------------------

	void SetParent(const BaseTransform& parent) { transform_.parent = &parent; }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- structrue ----------------------------------------------------

	struct InputTextValue {

		char nameBuffer[128]; // imgui入力用
		std::string name;     // 入力した文字を取得する用

		void Reset();
	};

	//--------- variables ----------------------------------------------------

	Asset* asset_;
	ID3D12Device* device_;

	// emitterの情報
	// 名前
	std::string name_;

	// 各particleの親となる
	BaseTransform transform_;

	// 所持しているparticle
	std::vector<ParticleGroup> particleGroups_;

	// particle追加入力処理
	InputTextValue addParticleNameInputText_;
	std::string addModelName_;
	std::optional<int> currentSelectIndex_;

	// editor
	ImVec2 addButtonSize_;  // 追加ボタンサイズ
	ImVec2 dropSize_;       // ドロップ処理受け取りサイズ
	ImVec2 leftChildSize_;  // 左側
	ImVec2 rightChildSize_; // 右側

	//--------- functions ----------------------------------------------------

	void EditLayout();

	// 作成処理
	void CreateParticle();
	// meshletの作成
	ResourceMesh<EffectMeshVertex> CreateMeshlet(const std::string& modelName);

	// emitter描画処理
	void DrawParticleEmitters();

	// 追加処理
	void AddParticle();
	// 選択処理
	void SelectParticle();
	// 値操作
	void EditParticle();
};