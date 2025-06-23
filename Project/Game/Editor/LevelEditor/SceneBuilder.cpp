#include "SceneBuilder.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Asset/AssetEditor.h>
#include <Lib/Adapter/JsonAdapter.h>

// entity
#include <Game/Objects/Environment/FieldCrossMarkWall.h>

//============================================================================
//	SceneBuilder classMethods
//============================================================================

void SceneBuilder::Init(const std::string& jsonPath) {

	jsonPath_ = jsonPath;

	idDeleteOnSameName_ = true;

	leftChildSize_ = ImVec2(320.0f, 320.0f);
	buttonSize_ = ImVec2(256.0f, 32.0f);
}

void SceneBuilder::ImGui() {

	// layout
	ImGui::BeginGroup();

	// jsonファイル受け取り
	ImGui::BeginChild("RecieveChild##SceneBuilder", leftChildSize_, true);
	ImGui::SeparatorText("Recieve File");

	RecieveFile();
	ImGui::EndChild();

	// 横並びにする
	ImGui::SameLine();
}

void SceneBuilder::CreateEntitiesMap(std::unordered_map<Level::EntityType,
	std::vector<std::unique_ptr<GameEntity3D>>>& entitiesMap) {

	// ファイルを読み込んでjsonデータを取得
	Json data = JsonAdapter::Load(fileName_.value());

	// sceneFileかチェックする
	if (!data.contains("name")) {
		return;
	}
	if (data["name"] != "scene") {
		return;
	}

	// 作成処理
	for (const auto& obj : data["objects"]) {

		BuildEntities(obj, entitiesMap);
	}
}

bool SceneBuilder::IsMeshEntityCreatable(const Json& obj) const {

	return obj.value("type", "") == "MESH" && obj.value("entity_flag", false);
}

void SceneBuilder::BuildEntities(const Json& obj,
	std::unordered_map<Level::EntityType,
	std::vector<std::unique_ptr<GameEntity3D>>>& entitiesMap) {

	// "MESHかどうかチェック"
	if (IsMeshEntityCreatable(obj)) {

		// 種類取得
		const std::string entityTypeName = obj.value("entity_type", "None");
		Level::EntityType entityType = GetEntityType(entityTypeName);

		auto& entities = entitiesMap[entityType];

		// 同名削除処理
		const std::string identifier = obj.value("name", "");
		if (idDeleteOnSameName_) {

			HandleDuplicateEntity(entities, identifier);
		}

		// 生成処理
		auto newEntity = CreateEntity(obj, entityType);

		// transform反映
		ApplyTransform(*newEntity, obj);
		// material反映
		Json materialData = LoadEntityFile(identifier);
		ApplyMaterial(*newEntity, materialData);
		// collision反映
		ApplyCollision(*newEntity, obj);

		// 登録
		entities.emplace_back(std::move(newEntity));
	}

	// 子を作成する
	if (obj.contains("children")) {
		for (const auto& child : obj["children"]) {

			BuildEntities(child, entitiesMap);
		}
	}
}

Json SceneBuilder::LoadEntityFile(const std::string& identifier) {

	Json data;
	if (!JsonAdapter::LoadCheck(jsonPath_ + identifier + ".json", data)) {

		// 読み込めなかった場合空のJsonを返す
		return Json();
	}

	return data;
}

void SceneBuilder::HandleDuplicateEntity(std::vector<std::unique_ptr<GameEntity3D>>& entities,
	const std::string& identifier) {

	entities.erase(std::remove_if(entities.begin(), entities.end(),
		[&](const std::unique_ptr<GameEntity3D>& entity) {
			return entity->GetIdentifier() == identifier; }),
			entities.end());
}

std::unique_ptr<GameEntity3D> SceneBuilder::CreateEntity(
	const Json& obj, Level::EntityType entityType) {

	auto entity = CreateEntityPtr(entityType);

	const std::string modelName = obj.value("modelName", "");

	entity->Init(modelName, modelName, "Scene");
	entity->SetIdentifier(obj.value("name", ""));

	return entity;
}

std::unique_ptr<GameEntity3D> SceneBuilder::CreateEntityPtr(Level::EntityType entityType) {

	switch (entityType) {
	case Level::EntityType::None: {

		return std::make_unique<GameEntity3D>();
	}
	case Level::EntityType::CrossMarkWall: {

		return std::make_unique<FieldCrossMarkWall>();
	}
	}

	return nullptr;
}

void SceneBuilder::ApplyTransform(GameEntity3D& entity, const Json& obj) {

	if (!obj.contains("transform")) {
		return;
	}

	const Json& transform = obj["transform"];

	// 平行移動
	if (transform.contains("translation")) {

		const auto& T = transform["translation"];
		entity.SetTranslation(Vector3(T[0].get<float>(), T[2].get<float>(), T[1].get<float>()));
	}

	// スケール
	if (transform.contains("scaling")) {

		const auto& S = transform["scaling"];
		entity.SetScale(Vector3(S[0].get<float>(), S[2].get<float>(), S[1].get<float>()));
	}

	// 回転
	if (transform.contains("rotation_quaternion")) {

		const auto& R = transform["rotation_quaternion"];
		entity.SetRotation(Quaternion(R[0].get<float>(), -R[2].get<float>(),
			-R[1].get<float>(), R[3].get<float>()).Normalize());
	}
}

void SceneBuilder::ApplyMaterial(GameEntity3D& entity, const Json& data) {

	// 空の場合処理しない
	if (data.empty()) {
		return;
	}

	entity.ApplyMaterial(data);
}

void SceneBuilder::ApplyCollision(GameEntity3D& entity, const Json& data) {

	if (!data.contains("collision")) {
		return;
	}

	// 有効な型かチェックする
	if (CheckCollisionValid<FieldCrossMarkWall>(entity)) {

		// colliderを設定
		entity.BuildBodies(data["collision"]);
	}
}

void SceneBuilder::Reset() {

	fileName_ = std::nullopt;
}

void SceneBuilder::SetFile(const std::string& sceneFile) {

	std::string loadName = "Level/" + sceneFile + ".json";
	// 作成元設定
	fileName_ = loadName;
}

void SceneBuilder::RecieveFile() {

	ImGui::Checkbox("idDeleteOnSameName", &idDeleteOnSameName_);

	ImGui::Button("Load File", buttonSize_);
	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payloadDataId = ImGui::AcceptDragDropPayload(AssetEditor::kDragPayloadId)) {

			auto* payloadJsonData = static_cast<AssetEditor::DragPayload*>(payloadDataId->Data);
			// .json以外は受け付けない
			if (payloadJsonData->type == AssetEditor::PendingType::None) {

				// 読み込み処理
				std::string loadName = "Level/" + std::string(payloadJsonData->name) + ".json";

				// 読み込めたら作成させる
				if (JsonAdapter::LoadAssert(loadName)) {

					// 作成元設定
					fileName_ = loadName;
				}
			}
		}
		ImGui::EndDragDropTarget();
	}
}

Level::EntityType SceneBuilder::GetEntityType(const std::string& entityTypeName) {

	Level::EntityType entityType = Level::EntityType::None;

	if (entityTypeName == "None") {

		entityType = Level::EntityType::None;
	} else if (entityTypeName == "CrossMarkWall") {

		entityType = Level::EntityType::CrossMarkWall;
	}

	return entityType;
}