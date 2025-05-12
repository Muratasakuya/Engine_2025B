#include "Skybox.h"

//============================================================================
//	Skybox classMethods
//============================================================================

void Skybox::CreateVertexBuffer(ID3D12Device* device) {

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
		16, 18, 17, 17, 18, 19,

		// 背面
		20, 22, 21, 21, 22, 23
	};

	// buffer作成
	vertexBuffer_.CreateVertexBuffer(device,
		static_cast<UINT>(vertices.size()));
	indexBuffer_.CreateIndexBuffer(device,
		static_cast<UINT>(indices.size()));

	// これ以上頂点の位置は変わらないのでbuffer転送
	vertexBuffer_.TransferVectorData(vertices);
	indexBuffer_.TransferVectorData(indices);
}