#include "PlayerAttack_2ndState.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/Renderer/LineRenderer.h>
#include <Engine/Utility/GameTimer.h>
#include <Game/Objects/GameScene/Enemy/Boss/Entity/BossEnemy.h>
#include <Game/Objects/GameScene/Player/Entity/Player.h>

//============================================================================
//	PlayerAttack_2ndState classMethods
//============================================================================

PlayerAttack_2ndState::PlayerAttack_2ndState() {

	// effect作成
	slashEffect_ = std::make_unique<GameEffect>();
	slashEffect_->CreateParticleSystem("Particle/playerSlash.json");
}

void PlayerAttack_2ndState::Enter(Player& player) {

	player.SetNextAnimation("player_attack_2nd", false, nextAnimDuration_);
	canExit_ = false;

	// 距離を比較
	const float dist = (bossEnemy_->GetTranslation() - player.GetTranslation()).Length();
	approachPhase_ = (dist > attackPosLerpCircleRange_);

	if (!approachPhase_) {

		CalcWayPoints(player, wayPoints_);
		currentIndex_ = 0;
		segmentTimer_ = 0.0f;
		segmentTime_ = attackPosLerpTime_ / 3.0f;

		// effectを発生させる
		EmitSlashEffectForCurrentIndex(player);
	}

	// 初期化
	currentIndex_ = 0;
	segmentTimer_ = 0.0f;
	segmentTime_ = attackPosLerpTime_ / 3.0f;
	emittedThisSegment_ = false;
}

void PlayerAttack_2ndState::Update(Player& player) {

	// animationが終わったかチェック
	canExit_ = player.IsAnimationFinished();
	// animationが終わったら時間経過を進める
	if (canExit_) {

		exitTimer_ += GameTimer::GetScaledDeltaTime();
	}

	if (approachPhase_) {

		// 回転を設定
		AttackAssist(player);
		const float distance = (bossEnemy_->GetTranslation() - player.GetTranslation()).Length();
		if (distance <= attackPosLerpCircleRange_) {

			// 距離が近くなればば遷移処理を行う
			CalcWayPoints(player, wayPoints_);
			currentIndex_ = 0;
			segmentTimer_ = 0.0f;
			segmentTime_ = attackPosLerpTime_ / 3.0f;
			startTranslation_ = player.GetTranslation();

			approachPhase_ = false;

			// effectを発生させる
			emittedThisSegment_ = false;
			slashEffect_->ResetEmitFlag();
			EmitSlashEffectForCurrentIndex(player);
			emittedThisSegment_ = true;
		}
		return;
	}

	// 遷移が終了したら目標座標を設定する
	if (wayPoints_.size() <= currentIndex_) {

		player.SetTranslation(*targetTranslation_);
		return;
	}

	// 区間開始フレームでまだ発生していなければ発生させる
	if (!emittedThisSegment_ && currentIndex_ < kNumSegments && segmentTimer_ == 0.0f) {

		slashEffect_->ResetEmitFlag();
		EmitSlashEffectForCurrentIndex(player);
		emittedThisSegment_ = true;
	}

	// 各区間を順に補間する
	segmentTimer_ += GameTimer::GetScaledDeltaTime();

	float lerpT = std::clamp(segmentTimer_ / segmentTime_, 0.0f, 1.0f);
	lerpT = EasedValue(attackPosEaseType_, lerpT);

	Vector3 segmentStart = (currentIndex_ == 0) ? startTranslation_ : wayPoints_[currentIndex_ - 1];
	Vector3 segmentEnd = wayPoints_[currentIndex_];

	player.SetTranslation(Vector3::Lerp(segmentStart, segmentEnd, lerpT));

	// 遷移が完了したら次の区間に進む
	if (segmentTime_ <= segmentTimer_) {

		// リセット
		segmentTimer_ = 0.0f;
		++currentIndex_;
		emittedThisSegment_ = false;
	}
}

void PlayerAttack_2ndState::CalcWayPoints(const Player& player, std::array<Vector3, 3>& dstWayPoints) {

	// 目標座標を設定
	startTranslation_ = player.GetTranslation();
	const Vector3 enemyPos = bossEnemy_->GetTranslation();
	Vector3 direction = (enemyPos - startTranslation_).Normalize();
	targetTranslation_ = enemyPos - direction * attackOffsetTranslation_;

	// 目標までの直線距離
	const float distance = (*targetTranslation_ - startTranslation_).Length();
	// 左右振れ幅を計算
	Vector3 right = Vector3::Cross(direction, Vector3(0.0f, 1.0f, 0.0f)).Normalize();
	float swayLength = (std::max)(0.0f, (attackPosLerpCircleRange_ - distance)) * swayRate_;

	// 経由点
	dstWayPoints[0] = Vector3::Lerp(startTranslation_, *targetTranslation_, leftPointAngle_) - right * swayLength;  // 左
	dstWayPoints[1] = Vector3::Lerp(startTranslation_, *targetTranslation_, rightPointAngle_) + right * swayLength; // 右
	dstWayPoints[2] = *targetTranslation_;
}

void PlayerAttack_2ndState::EmitSlashEffectForCurrentIndex(Player& player) {

	const size_t key = (std::min)(currentIndex_, kNumSegments - 1);

	// コマンドに設定
	ParticleCommand command{};
	{
		// 座標設定
		command.target = ParticleCommandTarget::Spawner;
		command.id = ParticleCommandID::SetTranslation;
		command.value = PlayerBaseAttackState::GetPlayerOffsetPos(player, slashEffectTranslatons_[key]);
		slashEffect_->SendCommand(command);
	}
	{
		// 回転設定
		command.target = ParticleCommandTarget::Updater;
		command.id = ParticleCommandID::SetEulerRotation;
		command.filter.updaterId = ParticleUpdateModuleID::Rotation;
		command.value = PlayerBaseAttackState::GetPlayerOffsetRotation(player, slashEffectRotations_[key]);
		slashEffect_->SendCommand(command);
	}
	// 発生させる
	slashEffect_->Emit(true);
}

void PlayerAttack_2ndState::Exit([[maybe_unused]] Player& player) {

	// リセット
	attackPosLerpTimer_ = 0.0f;
	exitTimer_ = 0.0f;

	slashEffect_->ResetEmitFlag();
}

void PlayerAttack_2ndState::ImGui(const Player& player) {

	ImGui::DragFloat("nextAnimDuration", &nextAnimDuration_, 0.001f);
	ImGui::DragFloat("rotationLerpRate", &rotationLerpRate_, 0.001f);
	ImGui::DragFloat("exitTime", &exitTime_, 0.01f);
	ImGui::DragFloat("swayRate", &swayRate_, 0.01f);
	ImGui::DragFloat("leftPointAngle", &leftPointAngle_, 0.01f);
	ImGui::DragFloat("rightPointAngle", &rightPointAngle_, 0.01f);

	PlayerBaseAttackState::ImGui(player);

	CalcWayPoints(player, debugWayPoints_);
	LineRenderer* lineRenderer = LineRenderer::GetInstance();

	Vector3 prev = player.GetTranslation();
	for (uint32_t i = 0; i < debugWayPoints_.size(); ++i) {

		debugWayPoints_[i].y = 2.0f;
		lineRenderer->DrawSphere(8, 2.0f, debugWayPoints_[i], Color::Red());
		lineRenderer->DrawLine3D(debugWayPoints_[i], debugWayPoints_[i] + Vector3(0.0f, 2.0f, 0.0f), Color::White());
		lineRenderer->DrawLine3D(prev, debugWayPoints_[i], Color::White());
		prev = debugWayPoints_[i];
	}

	ImGui::SeparatorText("Effect");
	for (size_t i = 0; i < kNumSegments; ++i) {

		ImGui::PushID(static_cast<int>(i));
		ImGui::Text("Key %zu", i);
		ImGui::DragFloat3("rotation", &slashEffectRotations_[i].x, 0.01f);
		ImGui::DragFloat3("translation", &slashEffectTranslatons_[i].x, 0.01f);
		ImGui::PopID();
	}
}

void PlayerAttack_2ndState::ApplyJson(const Json& data) {

	nextAnimDuration_ = JsonAdapter::GetValue<float>(data, "nextAnimDuration_");
	rotationLerpRate_ = JsonAdapter::GetValue<float>(data, "rotationLerpRate_");
	exitTime_ = JsonAdapter::GetValue<float>(data, "exitTime_");
	swayRate_ = JsonAdapter::GetValue<float>(data, "swayRate_");
	leftPointAngle_ = JsonAdapter::GetValue<float>(data, "leftPointAngle_");
	rightPointAngle_ = JsonAdapter::GetValue<float>(data, "rightPointAngle_");

	if (data.contains("slashEffectRotations_")) {

		const auto& array = data["slashEffectRotations_"];
		const size_t n = (std::min)(array.size(), kNumSegments);
		for (size_t i = 0; i < n; ++i) {

			slashEffectRotations_[i] = slashEffectRotations_[i].FromJson(array[i]);
		}
	}
	if (data.contains("slashEffectTranslatons_")) {
		const auto& array = data["slashEffectTranslatons_"];
		const size_t n = (std::min)(array.size(), kNumSegments);
		for (size_t i = 0; i < n; ++i) {

			slashEffectTranslatons_[i] = slashEffectTranslatons_[i].FromJson(array[i]);
		}
	}

	PlayerBaseAttackState::ApplyJson(data);
}

void PlayerAttack_2ndState::SaveJson(Json& data) {

	data["nextAnimDuration_"] = nextAnimDuration_;
	data["rotationLerpRate_"] = rotationLerpRate_;
	data["exitTime_"] = exitTime_;
	data["swayRate_"] = swayRate_;
	data["leftPointAngle_"] = leftPointAngle_;
	data["rightPointAngle_"] = rightPointAngle_;

	Json rotArray = Json::array();
	Json transArray = Json::array();
	for (size_t i = 0; i < kNumSegments; ++i) {
		
		rotArray.push_back(slashEffectRotations_[i].ToJson());
		transArray.push_back(slashEffectTranslatons_[i].ToJson());
	}
	data["slashEffectRotations_"] = rotArray;
	data["slashEffectTranslatons_"] = transArray;

	PlayerBaseAttackState::SaveJson(data);
}

bool PlayerAttack_2ndState::GetCanExit() const {

	// 経過時間が過ぎたら
	bool canExit = exitTimer_ > exitTime_;
	return canExit;
}