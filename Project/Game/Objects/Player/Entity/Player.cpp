#include "Player.h"

//============================================================================
//	include
//============================================================================

//============================================================================
//	Player classMethods
//============================================================================

void Player::InitWeapon() {

	// 右手
	rightWeapon_ = std::make_unique<PlayerWeapon>();
	rightWeapon_->Init("playerRightWeapon", "playerRightWeapon", "Player");

	// 左手
	leftWeapon_ = std::make_unique<PlayerWeapon>();
	leftWeapon_->Init("playerLeftWeapon", "playerLeftWeapon", "Player");

	// 武器を手を親として動かす
	if (const auto& hand = GetJointTransform("rightHand")) {

		rightWeapon_->SetParent(*hand);
	}
	if (const auto& hand = GetJointTransform("leftHand")) {

		leftWeapon_->SetParent(*hand);
	}
}

void Player::InitAnimations() {

	// 最初は待機状態で初期化
	animation_->SetPlayAnimation("player_idle", true);

	// animationのデータを設定
	animation_->SetAnimationData("player_walk");
	animation_->SetAnimationData("player_dash");
	animation_->SetAnimationData("player_attack_1st");
	animation_->SetAnimationData("player_attack_2nd");
	animation_->SetAnimationData("player_attack_3rd");
	animation_->SetAnimationData("player_skilAttack");

	// 両手を親として更新させる
	animation_->SetParentJoint("rightHand");
	animation_->SetParentJoint("leftHand");
}

void Player::InitCollision() {

	CollisionBody* body = bodies_.emplace_back(Collider::AddCollider(CollisionShape::OBB().Default()));
	bodyOffsets_.emplace_back(CollisionShape::OBB().Default());

	// タイプ設定
	body->SetType(ColliderType::Type_Player);
	body->SetTargetType(ColliderType::Type_BossEnemy);
}

void Player::InitState() {

	// 初期化、ここで初期状態も設定
	stateController_ = std::make_unique<PlayerStateController>();
	stateController_->Init(*this);
}

void Player::InitHUD() {

	// HUDの初期化
	hudSprites_ = std::make_unique<PlayerHUD>();
	hudSprites_->Init();
}

void Player::SetInitTransform() {

	transform_->scale = initTransform_.scale;
	transform_->eulerRotate = initTransform_.eulerRotate;
	transform_->rotation = initTransform_.rotation;
	transform_->translation = initTransform_.translation;
}

void Player::DerivedInit() {

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

void Player::SetBossEnemy(const BossEnemy* bossEnemy) {

	stateController_->SetBossEnemy(bossEnemy);
}

void Player::SetFollowCamera(const FollowCamera* followCamera) {

	stateController_->SetFollowCamera(followCamera);
}

void Player::Update() {

	// 状態の更新
	stateController_->SetStatas(stats_);
	stateController_->Update(*this);

	// HUDの更新
	hudSprites_->SetStatas(stats_);
	hudSprites_->Update();

	// 衝突情報更新
	Collider::UpdateAllBodies(*transform_);
}

void Player::OnCollisionEnter([[maybe_unused]] const CollisionBody* collisionBody) {
}

void Player::DerivedImGui() {

	ImGui::PushItemWidth(itemWidth_);
	ImGui::SetWindowFontScale(0.8f);

	if (ImGui::BeginTabBar("PlayerTabs")) {

		// ---- Stats ---------------------------------------------------
		if (ImGui::BeginTabItem("Stats")) {

			ImGui::Text("HP : %d / %d", stats_.currentHP, stats_.maxHP);
			ImGui::Text("SP : %d / %d", stats_.currentSkilPoint, stats_.maxSkilPoint);

			ImGui::DragInt("Max HP", &stats_.maxHP, 1, 0);
			ImGui::DragInt("Cur HP", &stats_.currentHP, 1, 0, stats_.maxHP);
			ImGui::DragInt("Max SP", &stats_.maxSkilPoint, 1, 0);
			ImGui::DragInt("Cur SP", &stats_.currentSkilPoint, 1, 0, stats_.maxSkilPoint);

			if (ImGui::Button("Reset HP")) {
				stats_.currentHP = stats_.maxHP;
			}
			ImGui::SameLine();
			if (ImGui::Button("Reset SP")) {
				stats_.currentSkilPoint = 0;
			}
			ImGui::EndTabItem();
		}

		// ---- Init ----------------------------------------------------
		if (ImGui::BeginTabItem("Init")) {

			if (ImGui::Button("Save##InitJson")) {
				SaveJson();
			}

			initTransform_.ImGui(itemWidth_);
			Collider::ImGui(itemWidth_);
			ImGui::EndTabItem();
		}

		// ---- State ---------------------------------------------------
		if (ImGui::BeginTabItem("State")) {
			stateController_->ImGui(*this);
			ImGui::EndTabItem();
		}

		// ---- HUD -----------------------------------------------------
		if (ImGui::BeginTabItem("HUD")) {
			hudSprites_->ImGui();
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}

	ImGui::PopItemWidth();
	ImGui::SetWindowFontScale(1.0f);
}

void Player::ApplyJson() {

	Json data;
	if (!JsonAdapter::LoadCheck("Player/initParameter.json", data)) {
		return;
	}

	initTransform_.FromJson(data["Transform"]);
	SetInitTransform();

	GameEntity3D::ApplyMaterial(data);
	Collider::ApplyBodyOffset(data);

	stats_.maxHP = JsonAdapter::GetValue<int>(data, "maxHP");
	stats_.maxSkilPoint = JsonAdapter::GetValue<int>(data, "maxSkilPoint");
	// 初期化時は最大と同じ値にする
	stats_.currentHP = stats_.maxHP;
	stats_.currentSkilPoint = stats_.maxSkilPoint;
}

void Player::SaveJson() {

	Json data;

	initTransform_.ToJson(data["Transform"]);
	GameEntity3D::SaveMaterial(data);
	Collider::SaveBodyOffset(data);

	data["maxHP"] = stats_.maxHP;
	data["maxSkilPoint"] = stats_.maxSkilPoint;

	JsonAdapter::Save("Player/initParameter.json", data);
}