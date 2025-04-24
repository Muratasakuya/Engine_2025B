#include "PrimitiveMeshTool.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Component/ComponentHelper.h>
#include <Engine/Renderer/LineRenderer.h>
#include <Engine/Utility/UVHelper.h>

// imgui
#include <imgui.h>

//============================================================================
//	PrimitiveMeshTool classMethods
//============================================================================

void PrimitiveMeshTool::EditPrimitiveMesh(const std::optional<int>& selectIndex) {

	// なにも値が入っていなければ処理しない
	if (!selectIndex.has_value()) {
		return;
	}

	selectIndex_ = selectIndex;

	PrimitiveMeshComponent* primitiveMesh =
		Component::GetComponent<PrimitiveMeshComponent>(selectIndex.value());

	primitiveMesh->ImGui(itemWidth_);

	if (ImGui::BeginTabBar("PrimitiveMeshTabs")) {

		if (ImGui::BeginTabItem("Transform")) {

			EditTransform();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Material")) {

			EditMaterial();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Primitive")) {

			EditPrimitive();
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	// 常に行う処理

	// drawLineで疑似的にwireFrame描画を行う
	DrawPrimitiveWireFrame(primitiveMesh);
}

void PrimitiveMeshTool::EditTransform() {

	// なにも値が入っていなければ処理しない
	if (!selectIndex_.has_value()) {
		return;
	}

	EffectTransformComponent* transform =
		Component::GetComponent<EffectTransformComponent>(selectIndex_.value());

	transform->ImGui(itemWidth_);
}

void PrimitiveMeshTool::EditMaterial() {

	// なにも値が入っていなければ処理しない
	if (!selectIndex_.has_value()) {
		return;
	}

	EffectMaterialComponent* material =
		Component::GetComponent<EffectMaterialComponent>(selectIndex_.value());

	material->ImGui(itemWidth_);
}

void PrimitiveMeshTool::EditPrimitive() {

	// なにも値が入っていなければ処理しない
	if (!selectIndex_.has_value()) {
		return;
	}

	PrimitiveMeshComponent* primitiveMesh =
		Component::GetComponent<PrimitiveMeshComponent>(selectIndex_.value());

	if (ImGui::BeginTabBar("PrimitiveMeshTabs")) {

		if (ImGui::BeginTabItem("Pos")) {

			// posを操作する
			EditPos(primitiveMesh);
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Texcoord")) {

			// texcoordを操作する
			EditTexcoord(primitiveMesh);
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("VertexColor")) {

			// vertexColorを操作する
			EditVertexColor(primitiveMesh);
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}
}

void PrimitiveMeshTool::EditPos(PrimitiveMeshComponent* primitiveMesh) {

	// 頂点情報取得
	ResourceMesh* resourceMesh = primitiveMesh->GetResourceMesh();

	ImGui::SeparatorText("Pos");

	ImGui::PushItemWidth(itemWidth_);

	// meshの数
	for (uint32_t meshIndex = 0; meshIndex < resourceMesh->meshCount_; ++meshIndex) {
		uint32_t vertexIndex = 0;
		for (auto& vertex : resourceMesh->vertices[meshIndex]) {

			Vector4& pos = vertex.pos;

			// posを操作
			if (ImGui::DragFloat3(("pos" + std::to_string(vertexIndex)).c_str(), &pos.x, 0.01f)) {

				// 操作している頂点の場所に球を描画
				DrawVertexSphere(primitiveMesh, meshIndex, vertexIndex);
			}
			++vertexIndex;
		}
	}
	ImGui::PopItemWidth();
}

void PrimitiveMeshTool::EditTexcoord(PrimitiveMeshComponent* primitiveMesh) {

	// 頂点情報取得
	ResourceMesh* resourceMesh = primitiveMesh->GetResourceMesh();

	ImGui::SeparatorText("Texcoord");

	ImGui::PushItemWidth(itemWidth_);

	// meshの数
	for (uint32_t meshIndex = 0; meshIndex < resourceMesh->meshCount_; ++meshIndex) {

		uint32_t vertexIndex = 0;
		for (auto& vertex : resourceMesh->vertices[meshIndex]) {

			Vector2& texcoord = vertex.texcoord;

			// texcoordを操作
			ImGui::DragFloat2(("texcoord" + std::to_string(vertexIndex)).c_str(), &texcoord.x, 0.01f);
			++vertexIndex;
		}
	}

	// 連番画像の1枚にtexcoord座標が合うように計算を行う
	ImGui::InputInt("textureSerialCount", &textureSerialCount_);
	if (ImGui::Button("Apply serialTexcoord")) {

		// 新しいtexcoordを設定
		std::vector<Vector2> newTexcoords = UVHelper::ApplyFrameTexcoord(*resourceMesh, textureSerialCount_);
		// meshの数
		for (uint32_t meshIndex = 0; meshIndex < resourceMesh->meshCount_; ++meshIndex) {
			for (uint32_t vertexIndex = 0; vertexIndex < resourceMesh->vertices[meshIndex].size(); ++vertexIndex) {

				resourceMesh->vertices[meshIndex][vertexIndex].texcoord = newTexcoords[vertexIndex];
			}
		}
	}

	ImGui::PopItemWidth();
}

void PrimitiveMeshTool::EditVertexColor(PrimitiveMeshComponent* primitiveMesh) {

	// 頂点情報取得
	ResourceMesh* resourceMesh = primitiveMesh->GetResourceMesh();

	ImGui::SeparatorText("VertexColor");

	ImGui::PushItemWidth(itemWidth_ * 1.5f);

	// meshの数
	for (uint32_t meshIndex = 0; meshIndex < resourceMesh->meshCount_; ++meshIndex) {
		uint32_t vertexIndex = 0;
		for (auto& vertex : resourceMesh->vertices[meshIndex]) {

			Color& color = vertex.color;

			// vertexColorを操作
			ImGui::ColorEdit4(("color" + std::to_string(vertexIndex)).c_str(), &color.r);
			++vertexIndex;
		}
	}
	ImGui::PopItemWidth();
}

void PrimitiveMeshTool::DrawPrimitiveWireFrame(PrimitiveMeshComponent* primitiveMesh) {

	// 頂点情報取得
	ResourceMesh* resourceMesh = primitiveMesh->GetResourceMesh();
	// transform取得
	EffectTransformComponent* transform =
		Component::GetComponent<EffectTransformComponent>(selectIndex_.value());
	// instance取得
	LineRenderer* lineRenderer = LineRenderer::GetInstance();

	// 太く描画するためのオフセット幅
	const float offsetWidth = 0.004f;

	// meshの数だけline描画を行う
	for (size_t meshIndex = 0; meshIndex < resourceMesh->meshCount_; ++meshIndex) {

		const auto& vertices = resourceMesh->vertices[meshIndex];
		const auto& indices = resourceMesh->indices[meshIndex];

		// 三角形単位で辺を描画
		for (size_t i = 0; i + 2 < indices.size(); i += 3) {

			uint32_t index0 = indices[i];
			uint32_t index1 = indices[i + 1];
			uint32_t index2 = indices[i + 2];

			// worldMatrixで座標変換
			Vector3 p0 = Vector3::Transform(
				Vector3(vertices[index0].pos.x, vertices[index0].pos.y, vertices[index0].pos.z),
				transform->matrix.world);
			Vector3 p1 = Vector3::Transform(
				Vector3(vertices[index1].pos.x, vertices[index1].pos.y, vertices[index1].pos.z),
				transform->matrix.world);
			Vector3 p2 = Vector3::Transform(
				Vector3(vertices[index2].pos.x, vertices[index2].pos.y, vertices[index2].pos.z),
				transform->matrix.world);

			// 線の色（オレンジ）
			Color color = Color(1.0f, 0.647f, 0.0f, 1.0f);

			// オフセットをずらして描画
			auto drawThickLine = [&](const Vector3& a, const Vector3& b) {
				Vector3 dir = Vector3::Normalize(b - a);
				Vector3 right = Vector3::Normalize(Vector3::Cross(dir, Vector3(0.0f, 1.0f, 0.0f))) * offsetWidth;

				lineRenderer->DrawLine3D(a, b, color);                 // 中央
				lineRenderer->DrawLine3D(a + right, b + right, color); // 右側
				lineRenderer->DrawLine3D(a - right, b - right, color); // 左側
				};

			drawThickLine(p0, p1);
			drawThickLine(p1, p2);
			drawThickLine(p2, p0);
		}
	}
}

void PrimitiveMeshTool::DrawVertexSphere(PrimitiveMeshComponent* primitiveMesh,
	uint32_t meshIndex, uint32_t vertexIndex) {

	// 頂点情報取得
	ResourceMesh* resourceMesh = primitiveMesh->GetResourceMesh();
	// transform取得
	EffectTransformComponent* transform =
		Component::GetComponent<EffectTransformComponent>(selectIndex_.value());
	// instance取得
	LineRenderer* lineRenderer = LineRenderer::GetInstance();
	// 頂点情報取得
	const auto& vertices = resourceMesh->vertices[meshIndex];

	// 球の情報
	const int kDivision = 4;
	const float kRadius = 0.1f;

	// worldMatrixで座標変換
	Vector3 vertexPos = Vector3::Transform(
		Vector3(vertices[vertexIndex].pos.x, vertices[vertexIndex].pos.y, vertices[vertexIndex].pos.z),
		transform->matrix.world);

	// 線の色（オレンジ）
	Color color = Color(1.0f, 0.647f, 0.0f, 1.0f);

	// 球を描画
	lineRenderer->DrawSphere(kDivision, kRadius, vertexPos, color);
}