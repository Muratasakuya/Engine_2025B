#include "Skybox.h"

//============================================================================
//	Skybox classMethods
//============================================================================

Skybox* Skybox::instance_ = nullptr;

Skybox* Skybox::GetInstance() {

	if (instance_ == nullptr) {
		instance_ = new Skybox();
	}
	return instance_;
}

void Skybox::Finalize() {

	if (instance_ != nullptr) {

		delete instance_;
		instance_ = nullptr;
	}
}

void Skybox::CreateVertexBuffer() {

	// 頂点データ設定
	std::vector<Vector4> vertices = {

		// 右面
		{1.0f,  1.0f, -1.0f, 1.0f},
		{1.0f,  1.0f,  1.0f, 1.0f},
		{1.0f, -1.0f, -1.0f, 1.0f},
		{1.0f, -1.0f,  1.0f, 1.0f},

		// 左面
		{-1.0f,  1.0f,  1.0f, 1.0f},
		{-1.0f,  1.0f, -1.0f, 1.0f},
		{-1.0f, -1.0f,  1.0f, 1.0f},
		{-1.0f, -1.0f, -1.0f, 1.0f},

		// 上面
		{-1.0f, 1.0f,  1.0f, 1.0f},
		{ 1.0f, 1.0f,  1.0f, 1.0f},
		{-1.0f, 1.0f, -1.0f, 1.0f},
		{ 1.0f, 1.0f, -1.0f, 1.0f},

		// 下面
		{-1.0f, -1.0f, -1.0f, 1.0f},
		{ 1.0f, -1.0f, -1.0f, 1.0f},
		{-1.0f, -1.0f,  1.0f, 1.0f},
		{ 1.0f, -1.0f,  1.0f, 1.0f},

		// 前面
		{-1.0f,  1.0f, 1.0f, 1.0f},
		{ 1.0f,  1.0f, 1.0f, 1.0f},
		{-1.0f, -1.0f, 1.0f, 1.0f},
		{ 1.0f, -1.0f, 1.0f, 1.0f},

		// 背面
		{ 1.0f,  1.0f, -1.0f, 1.0f},
		{-1.0f,  1.0f, -1.0f, 1.0f},
		{ 1.0f, -1.0f, -1.0f, 1.0f},
		{-1.0f, -1.0f, -1.0f, 1.0f},
	};

	// 頂点インデックスデータ設定
	std::vector<uint32_t> indices = {

		// 右面
		0, 2, 1,  1, 2, 3,

		// 左面
		4, 6, 5,  5, 6, 7,

		// 上面
		8, 10, 9, 9, 10, 11,

		// 下面
		12, 14, 13, 13, 14, 15,

		// 前面
		16, 17, 18, 18, 17, 19,

		// 背面
		20, 21, 22, 22, 21, 23
	};

	// buffer作成
	vertexBuffer_.CreateVertexBuffer(device_,
		static_cast<UINT>(vertices.size()));
	indexBuffer_.CreateIndexBuffer(device_,
		static_cast<UINT>(indices.size()));
	// index数設定
	indexCount_ = static_cast<UINT>(indices.size());

	// これ以上頂点の位置は変わらないのでbuffer転送
	vertexBuffer_.TransferVectorData(vertices);
	indexBuffer_.TransferVectorData(indices);
}

void Skybox::CreateCBuffer(uint32_t textureIndex) {

	// cBufferに渡す値の初期化
	transform_.Init();
	// 初期化値で320.0fにスケーリング
	const float scale = 320.0f;
	transform_.scale = Vector3::AnyInit(scale);
	// Y座標は64.0f
	transform_.translation.y = 64.0f;
	transform_.UpdateMatrix();

	material_.color = Color::White();
	material_.textureIndex = textureIndex;

	// buffer作成
	matrixBuffer_.CreateConstBuffer(device_);
	materialBuffer_.CreateConstBuffer(device_);

	// 1度bufferを転送する
	matrixBuffer_.TransferData(transform_.matrix.world);
	materialBuffer_.TransferData(material_);
}

void Skybox::Create(uint32_t textureIndex) {

	// 頂点buffer作成
	CreateVertexBuffer();

	// cBuffer作成
	CreateCBuffer(textureIndex);

	isCreated_ = true;
}

void Skybox::Update() {

	if (!isCreated_) {
		return;
	}

	// 行列更新
	transform_.UpdateMatrix();

	// buffer転送
	matrixBuffer_.TransferData(transform_.matrix.world);
	materialBuffer_.TransferData(material_);
}

void Skybox::ImGui() {

	if (!isCreated_) {
		return;
	}

	ImGui::SeparatorText("Transform");

	transform_.ImGui(itemWidth_);

	ImGui::SeparatorText("Color");

	ImGui::ColorEdit4("color", &material_.color.r);
}