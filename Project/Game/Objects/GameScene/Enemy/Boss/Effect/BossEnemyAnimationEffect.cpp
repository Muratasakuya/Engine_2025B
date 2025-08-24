#include "BossEnemyAnimationEffect.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Utility/JsonAdapter.h>
#include <Engine/Utility/EnumAdapter.h>
#include <Engine/Effect/Game/Helper/GameEffectCommandHelper.h>
#include <Game/Objects/GameScene/Enemy/Boss/Entity/BossEnemy.h>

//============================================================================
//	BossEnemyAnimationEffect classMethods
//============================================================================

void BossEnemyAnimationEffect::Init(const BossEnemy& bossEnemy) {

	// エフェクト追加
	// 弱攻撃
	lightSlash_.effect = std::make_unique<GameEffect>();
	lightSlash_.effect->CreateParticleSystem("Particle/bossEnemyLightSlash.json");
	// 親を設定
	lightSlash_.effect->SetParent(bossEnemy.GetTransform());

	// 強攻撃
	strongSlash_.effect = std::make_unique<GameEffect>();
	strongSlash_.effect->CreateParticleSystem("Particle/bossEnemyStrongSlash.json");
	// 親を設定
	strongSlash_.effect->SetParent(bossEnemy.GetTransform());

	// チャージ
	// 星
	chargeStar_.effect = std::make_unique<GameEffect>();
	chargeStar_.effect->CreateParticleSystem("Particle/bossEnemyChargeStar.json");
	// 親を設定
	chargeStar_.effect->SetParent(bossEnemy.GetTransform());
	// 集まってくるエフェクト
	chargeCircle_.effect = std::make_unique<GameEffect>();
	chargeCircle_.effect->CreateParticleSystem("Particle/bossEnemyChargeCircle.json");
	// 親を設定
	chargeCircle_.effect->SetParent(bossEnemy.GetTransform());

	// json適応
	ApplyJson();

	// 初期化値
	currentAnimationKey_ = AnimationKey::None;
	editAnimationKey_ = AnimationKey::None;
}

void BossEnemyAnimationEffect::Update(BossEnemy& bossEnemy) {

	// 再生されているアニメーションを取得
	UpdateAnimationKey(bossEnemy);

	// 現在のアニメーションに応じてエフェクトを発生
	UpdateEmit(bossEnemy);

	// 常に更新するエフェクト
	UpdateAllways();
}

void BossEnemyAnimationEffect::UpdateAnimationKey(BossEnemy& bossEnemy) {

	const auto& name = bossEnemy.GetCurrentAnimationName();

	currentAnimationKey_ = AnimationKey::None;
	if (name == "bossEnemy_lightAttack") {

		currentAnimationKey_ = AnimationKey::LightAttack;
	} else if (name == "bossEnemy_strongAttack") {

		currentAnimationKey_ = AnimationKey::StrongAttack;
	}
}

void BossEnemyAnimationEffect::UpdateEmit(BossEnemy& bossEnemy) {

	// 現在のアニメーションに応じてエフェクトを発生させる
	switch (currentAnimationKey_) {
	case BossEnemyAnimationEffect::AnimationKey::None: {

		// エフェクトの発生をリセット
		lightSlash_.effect->ResetEmitFlag();
		strongSlash_.effect->ResetEmitFlag();
		break;
	}
	case BossEnemyAnimationEffect::AnimationKey::LightAttack: {

		if (bossEnemy.IsEventKey("Effect", 0)) {

			// 座標回転、コマンドをセット
			GameEffectCommandHelper::ApplyAndSend(*lightSlash_.effect, bossEnemy.GetRotation(),
				lightSlash_.translation, lightSlash_.rotation);
			lightSlash_.effect->Emit(true);
		}
		break;
	}
	case BossEnemyAnimationEffect::AnimationKey::StrongAttack: {

		if (bossEnemy.IsEventKey("Effect", 0)) {

			// 座標回転、コマンドをセット
			GameEffectCommandHelper::ApplyAndSend(*strongSlash_.effect, bossEnemy.GetRotation(),
				strongSlash_.translation, strongSlash_.rotation);
			strongSlash_.effect->Emit(true);
		}
		break;
	}
	}
}

void BossEnemyAnimationEffect::UpdateAllways() {

	// 集まってくるエフェクト
	chargeCircle_.effect->Emit();
}

void BossEnemyAnimationEffect::ImGui(const BossEnemy& bossEnemy) {

	if (ImGui::Button("Save Json")) {

		SaveJson();
	}

	EnumAdapter<AnimationKey>::Combo("AnimationKey", &editAnimationKey_);

	switch (editAnimationKey_) {
	case BossEnemyAnimationEffect::AnimationKey::LightAttack: {

		if (ImGui::Button("Emit")) {

			// 座標回転、コマンドをセット
			GameEffectCommandHelper::ApplyAndSend(*lightSlash_.effect, bossEnemy.GetRotation(),
				lightSlash_.translation, lightSlash_.rotation);
			lightSlash_.effect->Emit();
		}

		ImGui::DragFloat3("rotation", &lightSlash_.rotation.x, 0.01f);
		ImGui::DragFloat3("translation", &lightSlash_.translation.x, 0.01f);
		break;
	}
	case BossEnemyAnimationEffect::AnimationKey::StrongAttack: {

		if (ImGui::Button("Emit")) {

			// 座標回転、コマンドをセット
			GameEffectCommandHelper::ApplyAndSend(*strongSlash_.effect, bossEnemy.GetRotation(),
				strongSlash_.translation, strongSlash_.rotation);
			strongSlash_.effect->Emit();
		}

		ImGui::DragFloat3("rotation", &strongSlash_.rotation.x, 0.01f);
		ImGui::DragFloat3("translation", &strongSlash_.translation.x, 0.01f);
		break;
	}
	case BossEnemyAnimationEffect::AnimationKey::ChargeAttack: {

		if (ImGui::Button("Emit")) {

			// 座標、コマンドを設定
			// 星
			GameEffectCommandHelper::ApplyAndSend(*chargeStar_.effect, bossEnemy.GetRotation(),
				chargeStar_.translation);
			chargeStar_.effect->Emit();
			// 集まってくるエフェクト
			GameEffectCommandHelper::ApplyAndSend(*chargeCircle_.effect, bossEnemy.GetRotation(),
				chargeCircle_.translation);
			// フラグで発生
			ParticleCommand command{};
			command.target = ParticleCommandTarget::Spawner;
			command.id = ParticleCommandID::SetEmitFlag;
			command.value = true;
			chargeCircle_.effect->SendCommand(command);
		}

		ImGui::DragFloat3("starTranslation", &chargeStar_.translation.x, 0.01f);
		ImGui::DragFloat3("circleTranslation", &chargeCircle_.translation.x, 0.01f);
		break;
	}
	}
}

void BossEnemyAnimationEffect::ApplyJson() {

	Json data;
	if (!JsonAdapter::LoadCheck("Enemy/Boss/animationEffectEmit.json", data)) {
		return;
	}

	auto key = EnumAdapter<AnimationKey>::ToString(AnimationKey::LightAttack);
	lightSlash_.translation = Vector3::FromJson(data[key].value("translation", Json()));
	lightSlash_.rotation = Vector3::FromJson(data[key].value("rotation", Json()));

	key = EnumAdapter<AnimationKey>::ToString(AnimationKey::StrongAttack);
	if (data.contains(key)) {

		strongSlash_.translation = Vector3::FromJson(data[key].value("translation", Json()));
		strongSlash_.rotation = Vector3::FromJson(data[key].value("rotation", Json()));
	}

	key = EnumAdapter<AnimationKey>::ToString(AnimationKey::ChargeAttack);
	if (data.contains(key)) {

		chargeStar_.translation = Vector3::FromJson(data[key].value("starTranslation", Json()));
		chargeCircle_.translation = Vector3::FromJson(data[key].value("circleTranslation", Json()));
	}
}

void BossEnemyAnimationEffect::SaveJson() {

	Json data;

	auto key = EnumAdapter<AnimationKey>::ToString(AnimationKey::LightAttack);
	data[key]["translation"] = lightSlash_.translation.ToJson();
	data[key]["rotation"] = lightSlash_.rotation.ToJson();

	key = EnumAdapter<AnimationKey>::ToString(AnimationKey::StrongAttack);
	data[key]["translation"] = strongSlash_.translation.ToJson();
	data[key]["rotation"] = strongSlash_.rotation.ToJson();

	key = EnumAdapter<AnimationKey>::ToString(AnimationKey::ChargeAttack);
	data[key]["starTranslation"] = chargeStar_.translation.ToJson();
	data[key]["circleTranslation"] = chargeCircle_.translation.ToJson();

	JsonAdapter::Save("Enemy/Boss/animationEffectEmit.json", data);
}