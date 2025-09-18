#include "PlayerSkilAttackState.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/Renderer/LineRenderer.h>
#include <Engine/Utility/GameTimer.h>
#include <Engine/Utility/EnumAdapter.h>
#include <Game/Objects/GameScene/Player/Entity/Player.h>
#include <Game/Objects/GameScene/Enemy/Boss/Entity/BossEnemy.h>

//============================================================================
//	PlayerSkilAttackState classMethods
//============================================================================

PlayerSkilAttackState::PlayerSkilAttackState() {

	rushMoveParam_.name = "RushMove";
	returnMoveParam_.name = "ReturnMove";
	jumpAttackMoveParam_.name = "JumpAttack";

	rushMoveParam_.timer.Reset();
	returnMoveParam_.timer.Reset();
	jumpAttackMoveParam_.timer.Reset();
	canExit_ = false;
}

void PlayerSkilAttackState::Enter(Player& player) {

	// 初期状態を設定
	currentState_ = State::Rush;

	// 敵が攻撃可能範囲にいるかチェック
	const Vector3 playerPos = player.GetTranslation();
	const Vector3 bossEnemyPos = bossEnemy_->GetTranslation();
	Vector3 direction = bossEnemyPos - playerPos;
	assisted_ = CheckInRange(attackPosLerpCircleRange_,
		Vector3(direction).Length());

	// 範囲内にいない場合はスキル攻撃を出来ない
	if (assisted_) {

		// 最初の補間座標を選択
		rushMoveParam_.start = playerPos;
		direction = direction.Normalize();
		rushMoveParam_.target = bossEnemyPos + direction * rushMoveParam_.moveValue;
		// アニメーションを設定
		player.SetNextAnimation("player_skilAttack_1st", false, nextAnimDuration_);
		// 敵の方を向ける
		player.SetRotation(Quaternion::LookRotation(direction, Vector3(0.0f, 1.0f, 0.0f)));
	} else {

		canExit_ = true;
		// 確実に終了するようにする
		exitTimer_ = exitTime_ * 2.0f;
	}
}

void PlayerSkilAttackState::Update(Player& player) {

	if (!assisted_ || canExit_) {
		return;
	}

	// 各状態を更新
	UpdateState(player);
}

void PlayerSkilAttackState::UpdateState(Player& player) {

	switch (currentState_) {
	case PlayerSkilAttackState::State::Rush:

		UpdateRush(player);
		break;
	case PlayerSkilAttackState::State::Return:

		UpdateReturn(player);
		break;
	case PlayerSkilAttackState::State::JumpAttack:

		UpdateJumpAttack(player);
		break;
	}
}

void PlayerSkilAttackState::UpdateRush(Player& player) {

	// 座標を補間する
	rushMoveParam_.timer.Update();
	player.SetTranslation(Vector3::Lerp(rushMoveParam_.start,
		rushMoveParam_.target, rushMoveParam_.timer.easedT_));

	// 終了後次の状態に進める
	if (rushMoveParam_.timer.IsReached()) {

		const Vector3 playerPos = player.GetTranslation();
		const Vector3 bossEnemyPos = bossEnemy_->GetTranslation();
		Vector3 direction = Vector3(bossEnemyPos - playerPos).Normalize();

		// 次の補間座標を設定
		returnMoveParam_.start = rushMoveParam_.target;
		returnMoveParam_.target = bossEnemyPos + direction * returnMoveParam_.moveValue;

		// アニメーションを設定
		player.SetNextAnimation("player_skilAttack_2nd", false, nextAnimDuration_);

		// 敵の方を向ける
		player.SetRotation(Quaternion::LookRotation(direction, Vector3(0.0f, 1.0f, 0.0f)));

		// 次に進める
		currentState_ = State::Return;
	}
}

void PlayerSkilAttackState::UpdateReturn(Player& player) {

	// 座標を補間する
	returnMoveParam_.timer.Update();
	player.SetTranslation(Vector3::Lerp(returnMoveParam_.start,
		returnMoveParam_.target, returnMoveParam_.timer.easedT_));

	// 終了後次の状態に進める
	if (returnMoveParam_.timer.IsReached()) {

		const Vector3 playerPos = player.GetTranslation();
		const Vector3 bossEnemyPos = bossEnemy_->GetTranslation();
		Vector3 direction = Vector3(bossEnemyPos - playerPos).Normalize();

		// 次の補間座標を設定
		jumpAttackMoveParam_.start = returnMoveParam_.target;
		jumpAttackMoveParam_.target = bossEnemyPos + direction * jumpAttackMoveParam_.moveValue;

		// アニメーションを設定
		player.SetNextAnimation("player_skilAttack_3rd", false, nextAnimDuration_);

		// 敵の方を向ける
		player.SetRotation(Quaternion::LookRotation(direction, Vector3(0.0f, 1.0f, 0.0f)));

		// 次に進める
		currentState_ = State::JumpAttack;
	}
}

void PlayerSkilAttackState::UpdateJumpAttack(Player& player) {

	// 座標を補間する
	jumpAttackMoveParam_.timer.Update();
	player.SetTranslation(Vector3::Lerp(jumpAttackMoveParam_.start,
		jumpAttackMoveParam_.target, jumpAttackMoveParam_.timer.easedT_));

	// 補間終了後状態を閉じる
	if (jumpAttackMoveParam_.timer.IsReached()) {

		canExit_ = true;
		// 確実に終了するようにする
		exitTimer_ = exitTime_ * 2.0f;
	}
}

void PlayerSkilAttackState::Exit(Player& player) {

	// リセット
	canExit_ = false;
	exitTimer_ = 0.0f;
	rushMoveParam_.timer.Reset();
	returnMoveParam_.timer.Reset();
	jumpAttackMoveParam_.timer.Reset();
	if (assisted_) {

		player.ResetAnimation();
	}
}

void PlayerSkilAttackState::ImGui(const Player& player) {

	ImGui::Text("currentState: %s", EnumAdapter<State>::ToString(currentState_));
	ImGui::Text(std::format("canExit: {}", canExit_).c_str());
	ImGui::Text(std::format("assisted: {}", assisted_).c_str());

	ImGui::Separator();

	ImGui::DragFloat("nextAnimDuration", &nextAnimDuration_, 0.001f);
	ImGui::DragFloat("rotationLerpRate", &rotationLerpRate_, 0.001f);
	ImGui::DragFloat("exitTime", &exitTime_, 0.01f);

	if (ImGui::CollapsingHeader("Base Attack")) {

		PlayerBaseAttackState::ImGui(player);
	}
	EnumAdapter<State>::Combo("State", &editState_);
	switch (editState_) {
	case PlayerSkilAttackState::State::Rush:

		rushMoveParam_.ImGui(player, *bossEnemy_);
		break;
	case PlayerSkilAttackState::State::Return:

		returnMoveParam_.ImGui(player, *bossEnemy_);
		break;
	case PlayerSkilAttackState::State::JumpAttack:

		jumpAttackMoveParam_.ImGui(player, *bossEnemy_);
		break;
	}
}

void PlayerSkilAttackState::StateMoveParam::ImGui(
	const Player& player, const BossEnemy& bossEnemy) {

	timer.ImGui("MoveTimer");
	ImGui::DragFloat("moveValue", &moveValue, 0.01f);
	ImGui::ProgressBar(timer.t_);

	LineRenderer* renderer = LineRenderer::GetInstance();

	Vector3 startPos = player.GetTranslation();
	const Vector3 bossEnemyPos = bossEnemy.GetTranslation();
	Vector3 direction = Vector3(bossEnemyPos - startPos).Normalize();
	Vector3 targetPos = bossEnemyPos + direction * moveValue;
	startPos.y = 2.0f;
	targetPos.y = 2.0f;
	renderer->DrawLine3D(startPos, targetPos, Color::Cyan());
	renderer->DrawSphere(8, 4.0f, targetPos, Color::Cyan());
}

void PlayerSkilAttackState::StateMoveParam::ApplyJson(const Json& data) {

	if (!data.contains(name)) {
		return;
	}

	timer.FromJson(data[name]);
	moveValue = data[name].value("moveValue", 32.0f);
}

void PlayerSkilAttackState::StateMoveParam::SaveJson(Json& data) {

	timer.ToJson(data[name]);
	data[name]["moveValue"] = moveValue;
}

void PlayerSkilAttackState::ApplyJson(const Json& data) {

	nextAnimDuration_ = JsonAdapter::GetValue<float>(data, "nextAnimDuration_");
	rotationLerpRate_ = JsonAdapter::GetValue<float>(data, "rotationLerpRate_");
	exitTime_ = JsonAdapter::GetValue<float>(data, "exitTime_");

	PlayerBaseAttackState::ApplyJson(data);

	rushMoveParam_.ApplyJson(data);
	returnMoveParam_.ApplyJson(data);
	jumpAttackMoveParam_.ApplyJson(data);
}

void PlayerSkilAttackState::SaveJson(Json& data) {

	data["nextAnimDuration_"] = nextAnimDuration_;
	data["rotationLerpRate_"] = rotationLerpRate_;
	data["exitTime_"] = exitTime_;

	PlayerBaseAttackState::SaveJson(data);

	rushMoveParam_.SaveJson(data);
	returnMoveParam_.SaveJson(data);
	jumpAttackMoveParam_.SaveJson(data);
}

bool PlayerSkilAttackState::GetCanExit() const {

	// 経過時間が過ぎたら
	bool canExit = exitTimer_ > exitTime_;
	return canExit;
}