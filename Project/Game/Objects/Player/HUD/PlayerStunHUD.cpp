#include "PlayerStunHUD.h"

//============================================================================
//	include
//============================================================================

//============================================================================
//	PlayerStunHUD classMethods
//============================================================================

void PlayerStunHUD::Init() {

	// 経過率背景
	progressBarBackground_ = std::make_unique<GameEntity2D>();
	progressBarBackground_->Init("stunProgressBarBackground",
		"stunProgressBarBackground", "PlayerStunHUD");

	// 経過率
	progressBar_ = std::make_unique<GameEntity2D>();
	progressBar_->Init("stunProgressBar", "stunProgressBar", "PlayerStunHUD");

	// 経過率文字
	chainAttackText_ = std::make_unique<GameEntity2D>();
	chainAttackText_->Init("CHAINATTACK", "CHAINATTACK", "PlayerStunHUD");

	// アイコン
	for (uint32_t index = 0; index < stunChainIcon_.size(); ++index) {

		stunChainIcon_[index] = std::make_unique<GameEntity2D>();
		stunChainIcon_[index]->Init("chainPlayerIcon", "chainPlayerIcon", "PlayerStunHUD");
	}

	// 入力
	keyInput_.Init("rightMouseClick", "leftMouseClick", "mouseCancel");
	gamepadInput_.Init("RBButton", "LBButton", "gamepadCancel");

	// タイマー
	restTimerDisplay_ = std::make_unique<GameTimerDisplay>();
	restTimerDisplay_->Init("dd:dd:dd", "timeNumber", "timeSymbol", "restTimer", "PlayerStunHUD");

	// json適応
	ApplyJson();
}

void PlayerStunHUD::Update() {

	// 経過時間表示の更新処理
	restTimerDisplay_->Update(restTimer_);
}

void PlayerStunHUD::UpdateLayout() {

	// ProgressBar
	const Vector2 barPos(centerTranslationX_, barTranslationY_);
	progressBarBackground_->SetTranslation(barPos);
	progressBar_->SetTranslation(barPos);
	chainAttackText_->SetTranslation(barPos);

	// Timer
	restTimerDisplay_->SetOffset(Vector2(timerOffsetX_, 0.0f));
	restTimerDisplay_->SetTranslation(
		Vector2(timerTranslationX_, timerTranslationY_));

	// Cancel
	const Vector2 cancelPos(centerTranslationX_, cancelTranslationY_);
	keyInput_.cancel->SetTranslation(cancelPos);
	gamepadInput_.cancel->SetTranslation(cancelPos);

	// 右入力
	const Vector2 rightPos(centerTranslationX_ + endOffsetX_, cancelTranslationY_);
	keyInput_.rightChain->SetTranslation(rightPos);
	gamepadInput_.rightChain->SetTranslation(rightPos);
	stunChainIcon_[0]->SetTranslation(
		Vector2(centerTranslationX_ + endOffsetX_, barTranslationY_));

	// 左入力
	const Vector2 leftPos(centerTranslationX_ - endOffsetX_, cancelTranslationY_);
	keyInput_.leftChain->SetTranslation(leftPos);
	gamepadInput_.leftChain->SetTranslation(leftPos);
	stunChainIcon_[1]->SetTranslation(
		Vector2(centerTranslationX_ - endOffsetX_, barTranslationY_));
}

void PlayerStunHUD::UpdateSize() {

	stunChainIcon_[0]->SetSize(iconSize_);
	stunChainIcon_[1]->SetSize(iconSize_);

	restTimerDisplay_->SetTimerSize(timerSize_);
	restTimerDisplay_->SetSymbolSize(timerSymbolSize_);

	progressBarBackground_->SetSize(progressBarBackgroundSize_);
	progressBar_->SetSize(progressBarSize_);
	chainAttackText_->SetSize(chainAttackTextSize_);

	// 右入力
	keyInput_.rightChain->SetSize(chainInputSize_);
	gamepadInput_.rightChain->SetSize(chainInputSize_);
	// 左入力
	keyInput_.leftChain->SetSize(chainInputSize_);
	gamepadInput_.leftChain->SetSize(chainInputSize_);
	// キャンセル
	keyInput_.cancel->SetSize(cancelInputSize_);
	gamepadInput_.cancel->SetSize(cancelInputSize_);
}

void PlayerStunHUD::ImGui() {

	if (ImGui::Button("Save Json")) {

		SaveJson();
	}

	ImGui::Checkbox("checkAnimation", &checkAnimation_);

	ImGui::DragFloat("restTime", &restTime_, 0.01f);
	ImGui::DragFloat("restTimer", &restTimer_, 0.01f);
	if (std::numeric_limits<float>::epsilon() < restTime_) {

		ImGui::ProgressBar(restTimer_ / restTime_, ImVec2(256.0f, 0.0f));
	}
	if (ImGui::Button("Reset Timer")) {

		restTimer_ = restTime_;
	}

	bool layoutChanged = false;

	layoutChanged |= ImGui::DragFloat("timerTranslationX", &timerTranslationX_);
	layoutChanged |= ImGui::DragFloat("barTranslationY", &barTranslationY_);
	layoutChanged |= ImGui::DragFloat("timerTranslationY", &timerTranslationY_);
	layoutChanged |= ImGui::DragFloat("cancelTranslationY", &cancelTranslationY_);
	layoutChanged |= ImGui::DragFloat("endOffsetX", &endOffsetX_);
	layoutChanged |= ImGui::DragFloat("timerOffsetX", &timerOffsetX_);
	layoutChanged |= ImGui::DragFloat2("timerSize", &timerSize_.x);
	layoutChanged |= ImGui::DragFloat2("timerSymbolSize", &timerSymbolSize_.x);
	layoutChanged |= ImGui::DragFloat2("progressBarSize", &progressBarSize_.x);
	layoutChanged |= ImGui::DragFloat2("progressBarBackgroundSize", &progressBarBackgroundSize_.x);
	layoutChanged |= ImGui::DragFloat2("chainAttackTextSize", &chainAttackTextSize_.x);
	layoutChanged |= ImGui::DragFloat2("chainInputSize", &chainInputSize_.x);
	layoutChanged |= ImGui::DragFloat2("cancelInputSize", &cancelInputSize_.x);
	layoutChanged |= ImGui::DragFloat2("iconSize", &iconSize_.x);

	if (layoutChanged) {

		UpdateLayout();
		UpdateSize();
	}
}

void PlayerStunHUD::ApplyJson() {

	Json data;
	if (!JsonAdapter::LoadCheck("Player/stunHudParameter.json", data)) {
		return;
	}

	restTime_ = JsonAdapter::GetValue<float>(data, "restTime_");
	timerTranslationX_ = JsonAdapter::GetValue<float>(data, "timerTranslationX_");
	barTranslationY_ = JsonAdapter::GetValue<float>(data, "barTranslationY_");
	timerTranslationY_ = JsonAdapter::GetValue<float>(data, "timerTranslationY_");
	cancelTranslationY_ = JsonAdapter::GetValue<float>(data, "cancelTranslationY_");
	endOffsetX_ = JsonAdapter::GetValue<float>(data, "endOffsetX_");
	timerOffsetX_ = JsonAdapter::GetValue<float>(data, "timerOffsetX_");
	timerSize_ = JsonAdapter::ToObject<Vector2>(data["timerSize_"]);
	timerSymbolSize_ = JsonAdapter::ToObject<Vector2>(data["timerSymbolSize_"]);
	iconSize_ = JsonAdapter::ToObject<Vector2>(data["iconSize_"]);
	progressBarBackgroundSize_ = JsonAdapter::ToObject<Vector2>(data["progressBarBackgroundSize_"]);
	progressBarSize_ = JsonAdapter::ToObject<Vector2>(data["progressBarSize_"]);
	chainAttackTextSize_ = JsonAdapter::ToObject<Vector2>(data["chainAttackTextSize_"]);
	chainInputSize_ = JsonAdapter::ToObject<Vector2>(data["chainInputSize_"]);
	cancelInputSize_ = JsonAdapter::ToObject<Vector2>(data["cancelInputSize_"]);

	// 時間を設定
	restTimer_ = restTime_;

	// 座標を設定
	UpdateLayout();
	// サイズを設定
	UpdateSize();
}

void PlayerStunHUD::SaveJson() {

	Json data;

	data["restTime_"] = restTime_;
	data["timerTranslationX_"] = timerTranslationX_;
	data["barTranslationY_"] = barTranslationY_;
	data["timerTranslationY_"] = timerTranslationY_;
	data["cancelTranslationY_"] = cancelTranslationY_;
	data["endOffsetX_"] = endOffsetX_;
	data["timerOffsetX_"] = timerOffsetX_;
	data["timerSize_"] = JsonAdapter::FromObject<Vector2>(timerSize_);
	data["timerSymbolSize_"] = JsonAdapter::FromObject<Vector2>(timerSymbolSize_);
	data["iconSize_"] = JsonAdapter::FromObject<Vector2>(iconSize_);
	data["progressBarBackgroundSize_"] = JsonAdapter::FromObject<Vector2>(progressBarBackgroundSize_);
	data["progressBarSize_"] = JsonAdapter::FromObject<Vector2>(progressBarSize_);
	data["chainAttackTextSize_"] = JsonAdapter::FromObject<Vector2>(chainAttackTextSize_);
	data["chainInputSize_"] = JsonAdapter::FromObject<Vector2>(chainInputSize_);
	data["cancelInputSize_"] = JsonAdapter::FromObject<Vector2>(cancelInputSize_);

	JsonAdapter::Save("Player/stunHudParameter.json", data);
}

void PlayerStunHUD::ChainInput::Init(const std::string& rightTex, const std::string& leftTex,
	const std::string& cancelTex) {

	rightChain = std::make_unique<GameEntity2D>();
	rightChain->Init(rightTex, rightTex, "PlayerStunHUD");

	leftChain = std::make_unique<GameEntity2D>();
	leftChain->Init(leftTex, leftTex, "PlayerStunHUD");

	cancel = std::make_unique<GameEntity2D>();
	cancel->Init(cancelTex, cancelTex, "PlayerStunHUD");
}