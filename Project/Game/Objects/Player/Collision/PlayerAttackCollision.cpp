#include "PlayerAttackCollision.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/ECS/Components/TransformComponent.h>
#include <Engine/Utility/GameTimer.h>
#include <Lib/Adapter/JsonAdapter.h>
#include <Lib/MathUtils/Algorithm.h>

// imgui
#include <imgui.h>

//============================================================================
//	PlayerAttackCollision classMethods
//============================================================================

namespace {

	// 各状態の名前
	const char* kStateNames[] = {
		"Idle","Walk","Dash","Attack_1st","Attack_2nd","Attack_3rd",
		"SkilAttack","SpecialAttack","Parry",
	};
	const char* kCollisionNames[] = {
		"None","Test","Player","PlayerWeapon","BossEnemy","BossWeapon",
		"CrossMarkWall"
	};
}

void PlayerAttackCollision::Init() {

	// 形状初期化
	weaponBody_ = bodies_.emplace_back(Collider::AddCollider(CollisionShape::OBB().Default()));
	bodyOffsets_.emplace_back(CollisionShape::OBB().Default());

	// タイプ設定
	// 最初は無効状態
	weaponBody_->SetType(ColliderType::Type_None);
	weaponBody_->SetTargetType(ColliderType::Type_BossEnemy);
}

void PlayerAttackCollision::Update(const Transform3DComponent& transform) {

	// 攻撃中かどうか
	const bool isAttack = currentParameter_
		&& currentParameter_->onTime <= currentTimer_
		&& currentTimer_ < currentParameter_->offTime;
	reHitTimer_ = (std::max)(0.0f, reHitTimer_ - GameTimer::GetDeltaTime());

	// 遷移可能な状態の時のみ武器状態にする
	if (isAttack && reHitTimer_ <= 0.0f) {

		weaponBody_->SetType(ColliderType::Type_PlayerWeapon);
		if (currentParameter_) {

			// 状態別で形状の値を設定
			auto& offset = std::get<CollisionShape::OBB>(bodyOffsets_.front());

			const Vector3 offsetWorld =
				transform.GetRight() * currentParameter_->centerOffset.x +
				transform.GetUp() * currentParameter_->centerOffset.y +
				transform.GetForward() * currentParameter_->centerOffset.z;
			offset.center = offsetWorld;
			offset.size = currentParameter_->size;
		}
	} else {

		weaponBody_->SetType(ColliderType::Type_None);

		// 当たらないようにする
		auto& offset = std::get<CollisionShape::OBB>(bodyOffsets_.front());
		offset.center = Vector3(0.0f, -128.0f, 0.0f);
		offset.size = Vector3::AnyInit(0.0f);
	}

	// 時間を進める
	currentTimer_ += GameTimer::GetDeltaTime();

	// 衝突情報更新
	Collider::UpdateAllBodies(transform);
}

void PlayerAttackCollision::SetEnterState(PlayerState state) {

	currentTimer_ = 0.0f;
	if (Algorithm::Find(table_, state)) {

		currentParameter_ = &table_[state];
	}

	// 無効状態を設定
	weaponBody_->SetType(ColliderType::Type_None);
}

void PlayerAttackCollision::OnCollisionEnter(const CollisionBody* collisionBody) {

	if (collisionBody->GetType() == ColliderType::Type_BossEnemy) {

		// 多段ヒットクールタイム設定
		reHitTimer_ = currentParameter_->hitInterval;
	}
}

void PlayerAttackCollision::ImGui() {

	ImGui::Text("currentType: %s", kCollisionNames[static_cast<int>(weaponBody_->GetType())]);

	ImGui::Combo("State", &editingIndex_, kStateNames, IM_ARRAYSIZE(kStateNames));
	PlayerState state = static_cast<PlayerState>(editingIndex_);
	AttackParameter& parameter = table_[state];

	ImGui::Separator();

	ImGui::Text("reHitTimer: %f / %f", reHitTimer_, parameter.hitInterval);

	bool edit = false;
	edit |= ImGui::DragFloat3("centerOffset", &parameter.centerOffset.x, 0.01f);
	edit |= ImGui::DragFloat3("size", &parameter.size.x, 0.01f);
	edit |= ImGui::DragFloat("onTime", &parameter.onTime, 0.01f, 0.0f);
	edit |= ImGui::DragFloat("offTime", &parameter.offTime, 0.01f, parameter.onTime);
	edit |= ImGui::DragFloat("hitInterval", &parameter.hitInterval, 0.01f);

	if (edit) {

		auto& offset = std::get<CollisionShape::OBB>(bodyOffsets_.front());
		offset.center = parameter.centerOffset;
		offset.size = parameter.size;
	}
}

void PlayerAttackCollision::ApplyJson(const Json& data) {

	for (const auto& [key, value] : data.items()) {

		PlayerState state = GetPlayerStateFromName(key);
		AttackParameter parameter;

		parameter.centerOffset = JsonAdapter::ToObject<Vector3>(value["centerOffset"]);
		parameter.size = JsonAdapter::ToObject<Vector3>(value["size"]);
		parameter.onTime = value.value("onTime", 0.0f);
		parameter.offTime = value.value("offTime", 0.0f);
		parameter.hitInterval = value.value("hitInterval", 0.0f);

		table_[state] = parameter;
	}
}

void PlayerAttackCollision::SaveJson(Json& data) {

	for (auto& [state, parameter] : table_) {

		Json& value = data[kStateNames[static_cast<int>(state)]];
		value["centerOffset"] = JsonAdapter::FromObject(parameter.centerOffset);
		value["size"] = JsonAdapter::FromObject(parameter.size);
		value["onTime"] = parameter.onTime;
		value["offTime"] = parameter.offTime;
		value["hitInterval"] = parameter.hitInterval;
	}
}

PlayerState PlayerAttackCollision::GetPlayerStateFromName(const std::string& name) {

	for (int i = 0; i < static_cast<int>(PlayerState::Parry) + 1; ++i) {
		if (name == kStateNames[i]) {

			return static_cast<PlayerState>(i);
		}
	}
	return PlayerState::Idle;
}