#include "PlayerAnimationEffect.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Effect/Game/Helper/GameEffectCommandHelper.h>
#include <Game/Objects/GameScene/Player/Entity/Player.h>

//============================================================================
//	PlayerAnimationEffect classMethods
//============================================================================

void PlayerAnimationEffect::Init(const Player& player) {

	// エフェクト追加
	// 斬撃
	slashEffect_ = std::make_unique<GameEffect>();
	slashEffect_->CreateParticleSystem("Particle/playerSlash.json");
	// 親を設定
	slashEffect_->SetParent(player.GetTransform());
	// 回転する剣の周り
	rotateLeftSwordSlashEffect_ = std::make_unique<GameEffect>();
	rotateLeftSwordSlashEffect_->CreateParticleSystem("Particle/playerLeftSwordSlash.json");
	rotateRightSwordSlashEffect_ = std::make_unique<GameEffect>();
	rotateRightSwordSlashEffect_->CreateParticleSystem("Particle/playerRightSwordSlash.json");
	// 親を設定
	rotateLeftSwordSlashEffect_->SetParent(
		player.GetWeapon(PlayerWeaponType::Left)->GetTransform());
	rotateRightSwordSlashEffect_->SetParent(
		player.GetWeapon(PlayerWeaponType::Right)->GetTransform());

	// json適応
	ApplyJson();

	// 初期化値
	currentAnimationKey_ = AnimationKey::None;
	editAnimationKey_ = AnimationKey::None;
}

void PlayerAnimationEffect::Update(Player& player) {

	// 再生されているアニメーションを取得
	UpdateAnimationKey(player);

	// 現在のアニメーションに応じてエフェクトを発生
	UpdateEmit(player);
}

void PlayerAnimationEffect::UpdateAnimationKey(Player& player) {

	const auto& name = player.GetCurrentAnimationName();
	currentAnimationKey_ = AnimationKey::None;
	if (name == "player_attack_1st") {

		currentAnimationKey_ = AnimationKey::Attack_1st;
	} else if (name == "player_attack_2nd") {

		currentAnimationKey_ = AnimationKey::Attack_2nd;
	} else if (name == "player_attack_3rd") {

		currentAnimationKey_ = AnimationKey::Attack_3rd;
	} else if (name == "player_attack_4th") {

		currentAnimationKey_ = AnimationKey::Attack_4th;
	} else if (name == "player_skilAttack") {

		currentAnimationKey_ = AnimationKey::Skil;
	}
}

void PlayerAnimationEffect::UpdateEmit(Player& player) {

	// 現在のアニメーションに応じてエフェクトを発生させる
	switch (currentAnimationKey_) {
	case PlayerAnimationEffect::AnimationKey::None: {

		// エフェクトの発生をリセット
		secondAttackEventIndex_ = 0;
		break;
	}
	case PlayerAnimationEffect::AnimationKey::Attack_1st: {

		if (player.IsEventKey("Effect", 0)) {

			// スケーリング
			GameEffectCommandHelper::SendScaling(*slashEffect_, firstSlashParam_.scaling);

			// 座標回転、コマンドをセット
			GameEffectCommandHelper::ApplyAndSend(*slashEffect_, player.GetRotation(),
				firstSlashParam_.translation, firstSlashParam_.rotation);
			slashEffect_->Emit();
		}
		break;
	}
	case PlayerAnimationEffect::AnimationKey::Attack_2nd: {

		if (player.IsEventKey("Effect", secondAttackEventIndex_)) {

			auto& param = secondSlashParams_[secondAttackEventIndex_];

			// スケーリング
			GameEffectCommandHelper::SendScaling(*slashEffect_, param.scaling);

			// 座標回転、コマンドをセット
			GameEffectCommandHelper::ApplyAndSend(*slashEffect_, player.GetRotation(),
				param.translation, param.rotation);
			slashEffect_->Emit();
			// インデックスを進める
			++secondAttackEventIndex_;
		}
		break;
	}
	case PlayerAnimationEffect::AnimationKey::Attack_3rd: {

		// 左手
		if (player.IsEventKey("Effect", 0)) {

			// スケーリング
			GameEffectCommandHelper::SendScaling(*rotateLeftSwordSlashEffect_, thirdSlashParam_.scaling);

			// 座標回転、コマンドをセット
			GameEffectCommandHelper::ApplyAndSend(*rotateLeftSwordSlashEffect_, player.GetRotation(),
				thirdSlashParam_.translation, thirdSlashParam_.rotation);
			rotateLeftSwordSlashEffect_->Emit();
		}
		// 右手
		if (player.IsEventKey("Effect", 1)) {

			// スケーリング
			GameEffectCommandHelper::SendScaling(*rotateRightSwordSlashEffect_, thirdSlashParam_.scaling);

			// 座標回転、コマンドをセット
			GameEffectCommandHelper::ApplyAndSend(*rotateRightSwordSlashEffect_, player.GetRotation(),
				thirdSlashParam_.translation, thirdSlashParam_.rotation);
			rotateRightSwordSlashEffect_->Emit();
		}
		break;
	}
	case PlayerAnimationEffect::AnimationKey::Attack_4th: {
		break;
	}
	case PlayerAnimationEffect::AnimationKey::Skil: {
		break;
	}
	}
}

void PlayerAnimationEffect::ImGui(const Player& player) {

	if (ImGui::Button("Save Json")) {

		SaveJson();
	}

	ImGui::Text("currentKey: %s", EnumAdapter<AnimationKey>::ToString(currentAnimationKey_));
	EnumAdapter<AnimationKey>::Combo("AnimationKey", &editAnimationKey_);

	switch (editAnimationKey_) {
	case PlayerAnimationEffect::AnimationKey::Attack_1st: {

		if (ImGui::Button("Emit")) {

			// スケーリング
			GameEffectCommandHelper::SendScaling(*slashEffect_, firstSlashParam_.scaling);

			// 座標回転、コマンドをセット
			GameEffectCommandHelper::ApplyAndSend(*slashEffect_, player.GetRotation(),
				firstSlashParam_.translation, firstSlashParam_.rotation);
			slashEffect_->Emit();
		}

		ImGui::DragFloat("scaling", &firstSlashParam_.scaling, 0.01f);
		ImGui::DragFloat3("rotation", &firstSlashParam_.rotation.x, 0.01f);
		ImGui::DragFloat3("translation", &firstSlashParam_.translation.x, 0.01f);
		break;
	}
	case PlayerAnimationEffect::AnimationKey::Attack_2nd: {

		for (uint32_t index = 0; index < secondSlashCount_; ++index) {

			ImGui::PushID(index);

			ImGui::SeparatorText(("animIndex: " + std::to_string(index)).c_str());

			auto& param = secondSlashParams_[index];
			if (ImGui::Button("Emit")) {

				// スケーリング
				GameEffectCommandHelper::SendScaling(*slashEffect_, param.scaling);

				// 座標回転、コマンドをセット
				GameEffectCommandHelper::ApplyAndSend(*slashEffect_, player.GetRotation(),
					param.translation, param.rotation);
				slashEffect_->Emit();
			}

			ImGui::DragFloat("scaling", &param.scaling, 0.01f);
			ImGui::DragFloat3("rotation", &param.rotation.x, 0.01f);
			ImGui::DragFloat3("translation", &param.translation.x, 0.01f);

			ImGui::PopID();
		}
		break;
	}
	case PlayerAnimationEffect::AnimationKey::Attack_3rd: {

		if (ImGui::Button("Emit")) {

			// スケーリング
			GameEffectCommandHelper::SendScaling(*rotateLeftSwordSlashEffect_, thirdSlashParam_.scaling);

			// 座標回転、コマンドをセット
			GameEffectCommandHelper::ApplyAndSend(*rotateLeftSwordSlashEffect_, player.GetRotation(),
				thirdSlashParam_.translation, thirdSlashParam_.rotation);
			rotateLeftSwordSlashEffect_->Emit();
		}

		ImGui::DragFloat("scaling", &thirdSlashParam_.scaling, 0.01f);
		ImGui::DragFloat3("rotation", &thirdSlashParam_.rotation.x, 0.01f);
		ImGui::DragFloat3("translation", &thirdSlashParam_.translation.x, 0.01f);
		break;
	}
	case PlayerAnimationEffect::AnimationKey::Attack_4th: {
		break;
	}
	case PlayerAnimationEffect::AnimationKey::Skil: {
		break;
	}
	}
}

void PlayerAnimationEffect::ApplyJson() {

	Json data;
	if (!JsonAdapter::LoadCheck("Player/animationEffectEmit.json", data)) {
		return;
	}

	auto key = EnumAdapter<AnimationKey>::ToString(AnimationKey::Attack_1st);
	firstSlashParam_.scaling = data[key].value("scaling", 1.0f);
	firstSlashParam_.translation = Vector3::FromJson(data[key].value("translation", Json()));
	firstSlashParam_.rotation = Vector3::FromJson(data[key].value("rotation", Json()));

	key = EnumAdapter<AnimationKey>::ToString(AnimationKey::Attack_2nd);
	for (uint32_t index = 0; index < secondSlashCount_; ++index) {

		auto& param = secondSlashParams_[index];
		std::string keyIndex = std::to_string(index);

		param.scaling = data[key][keyIndex].value("scaling", 1.0f);
		param.translation = Vector3::FromJson(data[key][keyIndex].value("translation", Json()));
		param.rotation = Vector3::FromJson(data[key][keyIndex].value("rotation", Json()));
	}

	key = EnumAdapter<AnimationKey>::ToString(AnimationKey::Attack_3rd);
	if (data.contains(key)) {

		thirdSlashParam_.scaling = data[key].value("scaling", 1.0f);
		thirdSlashParam_.translation = Vector3::FromJson(data[key].value("translation", Json()));
		thirdSlashParam_.rotation = Vector3::FromJson(data[key].value("rotation", Json()));
	}
}

void PlayerAnimationEffect::SaveJson() {

	Json data;

	auto key = EnumAdapter<AnimationKey>::ToString(AnimationKey::Attack_1st);
	data[key]["scaling"] = firstSlashParam_.scaling;
	data[key]["translation"] = firstSlashParam_.translation.ToJson();
	data[key]["rotation"] = firstSlashParam_.rotation.ToJson();

	key = EnumAdapter<AnimationKey>::ToString(AnimationKey::Attack_2nd);
	for (uint32_t index = 0; index < secondSlashCount_; ++index) {

		auto& param = secondSlashParams_[index];
		std::string keyIndex = std::to_string(index);

		data[key][keyIndex]["scaling"] = param.scaling;
		data[key][keyIndex]["translation"] = param.translation.ToJson();
		data[key][keyIndex]["rotation"] = param.rotation.ToJson();
	}

	key = EnumAdapter<AnimationKey>::ToString(AnimationKey::Attack_3rd);
	data[key]["scaling"] = thirdSlashParam_.scaling;
	data[key]["translation"] = thirdSlashParam_.translation.ToJson();
	data[key]["rotation"] = thirdSlashParam_.rotation.ToJson();

	JsonAdapter::Save("Player/animationEffectEmit.json", data);
}