#include "FieldCollisionCollection.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Utility/JsonAdapter.h>

//============================================================================
//	FieldCollisionCollection classMethods
//============================================================================

void FieldCollisionCollection::Init() {

	// json適応
	ApplyJson();
}

void FieldCollisionCollection::SetPushBackTarget(Player* player, BossEnemy* bossEnemy) {

	player_ = nullptr;
	player_ = player;

	bossEnemy_ = nullptr;
	bossEnemy_ = bossEnemy;

	for (const auto& collision : collisions_) {

		collision->SetPushBackTarget(player_, bossEnemy_);
	}
}

void FieldCollisionCollection::Update() {

	// 全ての衝突を更新
	for (const auto& collision : collisions_) {

		collision->Update();
	}
}

void FieldCollisionCollection::ImGui() {

	if (ImGui::Button("Save")) {
		SaveJson();
	}

	ImGui::SameLine();

	if (ImGui::Button("Add FieldCollision")) {

		auto collision = std::make_unique<FieldCollision>();
		collision->Init();
		collision->SetPushBackTarget(player_, bossEnemy_);
		collisions_.push_back(std::move(collision));
	}

	for (uint32_t i = 0; i < collisions_.size(); ++i) {

		ImGui::PushID(static_cast<int>(i));
		if (ImGui::Button("Remove")) {

			collisions_.erase(collisions_.begin() + i);
			ImGui::PopID();
			--i;
			continue;
		}
		collisions_[i]->ImGui(i);
		collisions_[i]->Update();
		ImGui::PopID();
	}
}

void FieldCollisionCollection::ApplyJson() {

	Json data;
	if (!JsonAdapter::LoadCheck("Level/fieldCollisionCollection.json", data)) {
		return;
	}

	if (data.is_array()) {
		for (const auto& it : data) {

			auto collision = std::make_unique<FieldCollision>();
			collision->Init();
			collision->FromJson(it);
			collision->SetPushBackTarget(player_, bossEnemy_);
			collision->Update();
			collisions_.push_back(std::move(collision));
		}
	}
}

void FieldCollisionCollection::SaveJson() {

	Json data = Json::array();
	for (const auto& collision : collisions_) {

		Json one;
		collision->ToJson(one);
		data.push_back(std::move(one));
	}

	JsonAdapter::Save("Level/fieldCollisionCollection.json", data);
}