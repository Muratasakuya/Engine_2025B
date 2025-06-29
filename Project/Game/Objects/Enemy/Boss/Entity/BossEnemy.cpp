#include "BossEnemy.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/Renderer/LineRenderer.h>
#include <Lib/Adapter/JsonAdapter.h>

//============================================================================
//	BossEnemy classMethods
//============================================================================

void BossEnemy::InitWeapon() {

	weapon_ = std::make_unique<BossEnemyWeapon>();
	weapon_->Init("bossEnemyWeapon", "bossEnemyWeapon", "Enemy");

	// 武器を右手を親として動かす
	if (const auto& hand = GetJointTransform("rightHand")) {

		weapon_->SetParent(*hand);
	}
}

void BossEnemy::InitAnimations() {

	// 最初は待機状態で初期化
	animation_->SetPlayAnimation("bossEnemy_idle", true);

	// animationのデータを設定
	animation_->SetAnimationData("bossEnemy_teleport");
	animation_->SetAnimationData("bossEnemy_falter");
	animation_->SetAnimationData("bossEnemy_lightAttack");
	animation_->SetAnimationData("bossEnemy_strongAttack");
	animation_->SetAnimationData("bossEnemy_rushAttack");
	animation_->SetAnimationData("bossEnemy_stun");
	animation_->SetAnimationData("bossEnemy_stunUpdate");

	// 右手を親として更新させる
	animation_->SetParentJoint("rightHand");
}

void BossEnemy::InitCollision() {

	CollisionBody* body = bodies_.emplace_back(Collider::AddCollider(CollisionShape::OBB().Default()));
	bodyOffsets_.emplace_back(CollisionShape::OBB().Default());

	// タイプ設定
	body->SetType(ColliderType::Type_BossEnemy);
	body->SetTargetType(ColliderType::Type_Player);
}

void BossEnemy::InitState() {

	// 初期化、ここで初期状態も設定
	stateController_ = std::make_unique<BossEnemyStateController>();
	stateController_->Init(*this);
}

void BossEnemy::InitHUD() {

	// HUDの初期化
	hudSprites_ = std::make_unique<BossEnemyHUD>();
	hudSprites_->Init();
}

void BossEnemy::SetInitTransform() {

	transform_->scale = initTransform_.scale;
	transform_->eulerRotate = initTransform_.eulerRotate;
	transform_->rotation = initTransform_.rotation;
	transform_->translation = initTransform_.translation;
}

void BossEnemy::DerivedInit() {

	// 使用する武器を初期化
	InitWeapon();

	// animation初期化、設定
	InitAnimations();

	// collision初期化、設定
	InitCollision();

	// 状態初期化
	InitState();

	// HUD初期化
	InitHUD();

	// json適応
	ApplyJson();
}

void BossEnemy::SetPlayer(const Player* player) {

	player_ = nullptr;
	player_ = player;

	stateController_->SetPlayer(player);
}

void BossEnemy::SetFollowCamera(const FollowCamera* followCamera) {

	stateController_->SetFollowCamera(followCamera);
}

void BossEnemy::Update() {

	// 閾値のリストの条件に誤りがないかチェック
	// indexNはindexN+1の値より必ず大きい(N=80、N+1=85にはならない)
	if (!stats_.hpThresholds.empty()) {

		std::sort(stats_.hpThresholds.begin(), stats_.hpThresholds.end(), std::greater<int>());
	}

	// 状態の更新
	stateController_->SetStatas(stats_);
	stateController_->Update(*this);

	// 武器の更新
	weapon_->Update();

	// HUDの更新
	hudSprites_->SetStatas(stats_);
	hudSprites_->Update();

	// 衝突情報更新
	Collider::UpdateAllBodies(*transform_);
}

void BossEnemy::OnCollisionEnter([[maybe_unused]] const CollisionBody* collisionBody) {


}

void BossEnemy::DerivedImGui() {

	// 文字サイズを設定
	ImGui::SetWindowFontScale(0.72f);

	ImGui::SeparatorText("HP");

	ImGui::Text("currentHP: %d / %d", stats_.currentHP, stats_.maxHP);
	ImGui::DragInt("maxHP", &stats_.maxHP, 1, 0);
	ImGui::DragInt("currentHP", &stats_.currentHP, 1, 0, stats_.maxHP);
	if (ImGui::Button("ResetHP")) {

		// HPをリセットする
		stats_.currentHP = stats_.maxHP;
	}

	ImGui::SeparatorText("DestroyToughness");

	ImGui::Text("currentDestroyToughness: %d / %d", stats_.currentDestroyToughness, stats_.maxDestroyToughness);
	ImGui::DragInt("maxDestroyToughness", &stats_.maxDestroyToughness, 1, 0);
	ImGui::DragInt("currentDestroyToughness", &stats_.currentDestroyToughness, 1, 0, stats_.maxDestroyToughness);
	if (ImGui::Button("ResetDestroyToughness")) {

		// 靭性値をリセットする
		stats_.currentDestroyToughness = 0;
	}

	ImGui::Separator();

	if (ImGui::BeginTabBar("BossEnemyTab")) {
		if (ImGui::BeginTabItem("Init")) {

			if (ImGui::Button("SaveJson...initParameter.json")) {

				SaveJson();
			}

			// 閾値の追加、設定処理
			if (ImGui::Button("AddHPThreshold")) {

				stats_.hpThresholds.emplace_back(0);
			}
			if (!stats_.hpThresholds.empty()) {

				std::vector<std::string> phaseLabels;
				std::vector<const char*> labelPtrs;

				phaseLabels.reserve(stats_.hpThresholds.size());
				labelPtrs.reserve(stats_.hpThresholds.size());
				for (size_t i = 0; i < stats_.hpThresholds.size(); ++i) {

					phaseLabels.emplace_back("Phase" + std::to_string(i));
					labelPtrs.push_back(phaseLabels.back().c_str());
				}
				ImGui::Combo("Edit Phase", &selectedPhaseIndex_, labelPtrs.data(), static_cast<int>(labelPtrs.size()));
				ImGui::DragInt("Threshold(%)", &stats_.hpThresholds[selectedPhaseIndex_], 1, 0, 100);
			}

			ImGui::Separator();

			if (ImGui::CollapsingHeader("Transform")) {

				initTransform_.ImGui(itemWidth_);
				SetInitTransform();
			}

			if (ImGui::CollapsingHeader("Collision")) {

				Collider::ImGui(itemWidth_);
			}
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("StateParam")) {

			stateController_->ImGui(*this);
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("StateTable")) {

			stateController_->EditStateTable();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("HUD")) {

			hudSprites_->ImGui();
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	// 文字サイズを元に戻す
	ImGui::SetWindowFontScale(1.0f);
}

void BossEnemy::ApplyJson() {

	Json data;
	if (!JsonAdapter::LoadCheck("Enemy/Boss/initParameter.json", data)) {
		return;
	}

	initTransform_.FromJson(data["Transform"]);
	SetInitTransform();

	GameEntity3D::ApplyMaterial(data);
	Collider::ApplyBodyOffset(data);

	stats_.maxHP = JsonAdapter::GetValue<int>(data, "maxHP");
	stats_.maxDestroyToughness = JsonAdapter::GetValue<int>(data, "maxDestroyToughness");
	// 初期化時は最大と同じ値にする
	stats_.currentHP = stats_.maxHP;

	stats_.hpThresholds = JsonAdapter::ToVector<int>(data["hpThresholds"]);
}

void BossEnemy::SaveJson() {

	Json data;

	initTransform_.ToJson(data["Transform"]);
	GameEntity3D::SaveMaterial(data);
	Collider::SaveBodyOffset(data);

	data["maxHP"] = stats_.maxHP;
	data["maxDestroyToughness"] = stats_.maxDestroyToughness;

	data["hpThresholds"] = JsonAdapter::FromVector<int>(stats_.hpThresholds);

	JsonAdapter::Save("Enemy/Boss/initParameter.json", data);
}