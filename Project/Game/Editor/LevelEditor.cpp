#include "LevelEditor.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Asset/Asset.h>
#include <Engine/Core/ECS/Core/ECSManager.h>
#include <Lib/Adapter/JsonAdapter.h>
#include <Lib/MathUtils/MathUtils.h>

//============================================================================
//	LevelEditor classMethods
//============================================================================

void LevelEditor::LoadFile(Asset* asset) {

	const std::string fullpath = "./Assets/Json/Level/levelEditor.json";

	// ファイルを開く
	std::ifstream file;
	file.open(fullpath);
	// ファイルを開けたかどうか
	if (file.fail()) {
		ASSERT(false, (fullpath + ":failed open").c_str());
	}

	// 解凍処理
	Json deserialized;
	file >> deserialized;

	// 正しいレベルデータファイルかチェックする
	ASSERT(deserialized.is_object(), "");
	ASSERT(deserialized.contains("name"), "");
	ASSERT(deserialized["name"].is_string(), "");

	// 名前を取得
	std::string name = deserialized["name"].get<std::string>();
	// 正しいレベルデータファイルかチェックする
	ASSERT(name.compare("scene") == 0, "");

	// "objects"の全オブジェクトを走査
	for (const auto& object : deserialized["objects"]) {

		ASSERT(object.contains("type"), "");

		// 種別を取得
		std::string type = object["type"].get<std::string>();

		// MESH
		if (type.compare("MESH") == 0) {

			// 要素追加
			LevelData& levelData = lebelDataMap_[type].emplace_back(LevelData());

			// modelの名前を設定
			if (object.contains("name")) {

				levelData.modelName = object["name"];
			}

			// transformの値を設定
			Json transform = object["transform"];
			// SRT
			// S
			levelData.transform.scale.x = static_cast<float>(transform["scaling"][0]);
			levelData.transform.scale.y = static_cast<float>(transform["scaling"][2]);
			levelData.transform.scale.z = static_cast<float>(transform["scaling"][1]);
			// R
			levelData.transform.eulerRotate.x = static_cast<float>(transform["rotation"][0]) * radian;
			levelData.transform.eulerRotate.y = static_cast<float>(transform["rotation"][2]) * radian;
			levelData.transform.eulerRotate.z = static_cast<float>(transform["rotation"][1]) * radian;
			levelData.transform.rotation = Quaternion::EulerToQuaternion(levelData.transform.eulerRotate);
			// T
			levelData.transform.translation.x = static_cast<float>(transform["translation"][0]);
			levelData.transform.translation.y = static_cast<float>(transform["translation"][2]);
			levelData.transform.translation.z = static_cast<float>(transform["translation"][1]);
		}
	}

	// entity作成
	ECSManager* ecsManager = ECSManager::GetInstance();
	for (const auto& dataVector : std::views::values(lebelDataMap_)) {
		for (const auto& data : dataVector) {
			if (!asset->SearchModel(data.modelName)) {

				// 無ければ読み込みを行う
				asset->LoadModel(data.modelName);
			}
			// entity作成
			uint32_t entityId = ecsManager->CreateObject3D(
				data.modelName, data.modelName, "LevelEditor");

			// transformを設定
			Transform3DComponent* transform = ecsManager->GetComponent<Transform3DComponent>(entityId);
			transform->scale = data.transform.scale;
			transform->rotation = data.transform.rotation;
			transform->eulerRotate = data.transform.eulerRotate;
			transform->translation = data.transform.translation;
		}
	}
}