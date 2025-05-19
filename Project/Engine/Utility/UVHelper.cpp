#include "UVHelper.h"

//============================================================================
//	UVHelper classMethods
//============================================================================

std::vector<Vector2> UVHelper::ApplyFrameTexcoord(const ResourceMesh<MeshVertex>& resourceMesh, int serialCount) {

	// 設定するtexture座標
	std::vector<Vector2> texcoords{};

	const size_t vertexSize = resourceMesh.vertices.front().size();
	texcoords.resize(vertexSize);

	for (uint32_t i = 0; i < vertexSize; ++i) {

		// texcoordXを連番画像分で割る
		texcoords[i].x = resourceMesh.vertices.front()[i].texcoord.x /
			static_cast<float>(serialCount);

		// texcoordが1.0fのときは1.0fにする
		if (resourceMesh.vertices.front()[i].texcoord.y == 1.0f) {

			texcoords[i].y = 1.0f;
		}
	}

	return texcoords;
}