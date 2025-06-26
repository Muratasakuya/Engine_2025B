#include "BossEnemyStateController.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Utility/GameTimer.h>
#include <Lib/Adapter/RandomGenerator.h>
#include <Lib/Adapter/JsonAdapter.h>

// state
#include <Game/Objects/Enemy/Boss/State/States/BossEnemyIdleState.h>
#include <Game/Objects/Enemy/Boss/State/States/BossEnemyTeleportationState.h>
#include <Game/Objects/Enemy/Boss/State/States/BossEnemyStunState.h>
#include <Game/Objects/Enemy/Boss/State/States/BossEnemyFalterState.h>
#include <Game/Objects/Enemy/Boss/State/States/BossEnemyLightAttackState.h>
#include <Game/Objects/Enemy/Boss/State/States/BossEnemyStrongAttackState.h>
#include <Game/Objects/Enemy/Boss/State/States/BossEnemyChargeAttackState.h>
#include <Game/Objects/Enemy/Boss/State/States/BossEnemyRushAttackState.h>

//============================================================================
//	BossEnemyStateController classMethods
//============================================================================

namespace {

	// 各状態の名前
	const char* kStatetateNames[] = {
		"Idle","Teleport","Stun","Falter","LightAttack","StrongAttack",
		"ChargeAttack","RushAttack",
	};

	// jsonを保存するパス
	const std::string kStateJsonPath = "Enemy/Boss/stateParameter.json";
	const std::string kStateTableJsonPath = "Enemy/Boss/stateTable.json";
}

void BossEnemyStateController::Init(BossEnemy& owner) {

	// 各状態を初期化
	states_.emplace(BossEnemyState::Idle, std::make_unique<BossEnemyIdleState>());
	states_.emplace(BossEnemyState::Teleport, std::make_unique<BossEnemyTeleportationState>());
	states_.emplace(BossEnemyState::Stun, std::make_unique<BossEnemyStunState>());
	states_.emplace(BossEnemyState::Falter, std::make_unique<BossEnemyFalterState>());
	states_.emplace(BossEnemyState::LightAttack, std::make_unique<BossEnemyLightAttackState>());
	states_.emplace(BossEnemyState::StrongAttack, std::make_unique<BossEnemyStrongAttackState>());
	states_.emplace(BossEnemyState::ChargeAttack, std::make_unique<BossEnemyChargeAttackState>());
	states_.emplace(BossEnemyState::RushAttack, std::make_unique<BossEnemyRushAttackState>());

	// json適応
	ApplyJson();

	// 初期状態を設定
	requested_ = BossEnemyState::Idle;
	ChangeState(owner);
}

void BossEnemyStateController::SetPlayer(const Player* player) {

	// 各状態にplayerをセット
	for (const auto& state : std::views::values(states_)) {

		state->SetPlayer(player);
	}
}

void BossEnemyStateController::Update(BossEnemy& owner) {

	// 現在のフェーズの更新処理
	UpdatePhase();
	UpdateStateTimer();

	// 何か設定されて入れば状態遷移させる
	if (requested_.has_value()) {

		ChangeState(owner);
	}

	// 現在の状態を更新
	if (BossEnemyIState* currentState = states_[current_].get()) {

		currentState->Update(owner);
	}
}

void BossEnemyStateController::UpdatePhase() {

	// 現在のHP割合
	uint32_t hpRate = (stats_.currentHP * 100) / stats_.maxHP;

	// HP割合に応じて現在のフェーズを計算して設定
	currentPhase_ = 0;
	for (uint32_t threshold : stats_.hpThresholds) {
		if (hpRate < threshold) {

			// 閾値以下ならフェーズを進める
			++currentPhase_;
		}
	}

	prevPhase_ = UINT32_MAX;
	// phaseが切り替わったらリセットする
	if (prevPhase_ != currentPhase_) {

		prevPhase_ = currentPhase_;
		currentComboIndex_ = 0;
		currentSequenceIndex_ = 0;
		stateTimer_ = 0.0f;
	}
}

void BossEnemyStateController::UpdateStateTimer() {

	// 時間経過を進める
	stateTimer_ += GameTimer::GetDeltaTime();

	const auto& phase = stateTable_.phases[currentPhase_];
	if (phase.nextStateDuration <= stateTimer_) {

		// 次のステートを決定する
		ChooseNextState(phase);
		stateTimer_ = 0.0f;
	}
}

void BossEnemyStateController::ChangeState(BossEnemy& owner) {

	// 同じなら遷移させない
	if (requested_.value() == current_) {
		requested_ = std::nullopt;
		return;
	}

	// 現在の状態の終了処理
	if (auto* currentState = states_[current_].get()) {

		currentState->Exit(owner);
	}

	// 次の状態を設定する
	current_ = requested_.value();

	// 次の状態を初期化する
	if (auto* currentState = states_[current_].get()) {

		currentState->Enter(owner);
	}
}

void BossEnemyStateController::ChooseNextState(const BossEnemyPhase& phase) {

	if (phase.comboIndices.size() > 1) {

		// 複数のコンボがある場合はランダムに選ぶ
		currentComboSlot_ = RandomGenerator::Generate(0, static_cast<int>(phase.comboIndices.size() - 1));
	}

	int comboID = phase.comboIndices[currentComboSlot_];
	comboID = std::clamp(comboID, 0, static_cast<int>(stateTable_.combos.size() - 1));
	const BossEnemyCombo& combo = stateTable_.combos[comboID];

	// コンボのインデックスを更新
	currentSequenceIndex_ = (currentSequenceIndex_ + 1) % combo.sequence.size();
	BossEnemyState next = combo.sequence[currentSequenceIndex_];

	// 現在の状態と同じなら次の状態を選ぶ
	if (!combo.allowRepeat && next == current_) {

		currentSequenceIndex_ = (currentSequenceIndex_ + 1) % combo.sequence.size();
		next = combo.sequence[currentSequenceIndex_];
	}
	requested_ = next;
}

void BossEnemyStateController::SyncPhaseCount() {

	const size_t required = stats_.hpThresholds.size() + 1;
	// 足りなければ追加
	while (stateTable_.phases.size() < required) {

		stateTable_.phases.emplace_back(BossEnemyPhase{});
	}
	// 余っていれば削る
	if (stateTable_.phases.size() > required) {

		stateTable_.phases.resize(required);
	}
}

void BossEnemyStateController::DrawHighlighted(bool highlight, const ImVec4& col, const std::function<void()>& draw) {

	if (highlight) {

		ImGui::PushStyleColor(ImGuiCol_Text, col);
	}

	draw();
	if (highlight) {

		ImGui::PopStyleColor();
	}
}

void BossEnemyStateController::ImGui() {

	if (ImGui::Button("SaveJson...stateParameter.json")) {

		SaveJson();
	}

	// 各stateの値を調整
	editingStateIndex_ = 0;
	ImGui::Combo("EditState", &editingStateIndex_, kStatetateNames, IM_ARRAYSIZE(kStatetateNames));
	ImGui::SeparatorText(kStatetateNames[editingStateIndex_]);

	if (const auto& state = states_[static_cast<BossEnemyState>(editingStateIndex_)].get()) {

		state->ImGui();
	}
}

void BossEnemyStateController::EditStateTable() {

	// editorParameters
	const ImVec2 buttonSize = ImVec2(136.0f, 30.0f * 0.72f);

	int currentComboID = -1;
	const auto& curPhaseCombos = stateTable_.phases[currentPhase_].comboIndices;
	if (!curPhaseCombos.empty() && currentComboSlot_ < curPhaseCombos.size()) {
		currentComboID = curPhaseCombos[currentComboSlot_];
	}

	//--------------------------------------------------------------------
	// 概要表示
	//--------------------------------------------------------------------

	if (ImGui::Button("SaveJson...stateParameter.json")) {

		SaveJson();
	}
	ImGui::Separator();

	//--------------------------------------------------------------------
	// ComboListテーブル
	//--------------------------------------------------------------------

	ImGui::SeparatorText("Edit Combo");

	// settings
	const ImVec4 headerColor = ImGui::GetStyleColorVec4(ImGuiCol_Header);
	ImGui::PushStyleColor(ImGuiCol_HeaderHovered, headerColor);
	ImGui::PushStyleColor(ImGuiCol_HeaderActive, headerColor);

	if (ImGui::Button("CreateCombo")) {
		stateTable_.combos.emplace_back(BossEnemyCombo{});
	}

	if (ImGui::BeginTable("##ComboList", 4, ImGuiTableFlags_BordersInner)) {

		ImGui::TableSetupColumn("Combo");     // 0
		ImGui::TableSetupColumn("Sequence");  // 1
		ImGui::TableSetupColumn("AddState");  // 2
		ImGui::TableHeadersRow();

		for (int comboIdx = 0; comboIdx < static_cast<int>(stateTable_.combos.size()); ++comboIdx) {

			auto& combo = stateTable_.combos[comboIdx];
			ImGui::PushID(comboIdx);
			ImGui::TableNextRow();

			//----------------------------------------------------------------
			//  列0: Combo名
			//----------------------------------------------------------------

			ImGui::TableNextColumn();
			const std::string comboLabel = "Combo" + std::to_string(comboIdx);
			bool isCurrentCombo = (comboIdx == currentComboID);

			ImGui::AlignTextToFramePadding();
			DrawHighlighted(isCurrentCombo, kHighlight, [&] {
				ImGui::Selectable(("Combo" + std::to_string(comboIdx)).c_str()); });

			if (ImGui::BeginDragDropSource()) {

				ImGui::SetDragDropPayload("ComboIdx", &comboIdx, sizeof(int));
				ImGui::Text("%s", comboLabel.c_str());
				ImGui::EndDragDropSource();
			}

			//----------------------------------------------------------------
			// 列1: Sequence
			//----------------------------------------------------------------
			ImGui::TableNextColumn();
			for (size_t seqIdx = 0; seqIdx < combo.sequence.size();) {

				const int stateId = static_cast<int>(combo.sequence[seqIdx]);
				bool isCurrentState = isCurrentCombo && (seqIdx == currentSequenceIndex_);

				ImGui::PushID(static_cast<int>(seqIdx));

				bool clicked = false;
				DrawHighlighted(isCurrentState, kHighlight, [&] {
					clicked = ImGui::Button(kStatetateNames[stateId], buttonSize); });
				if (clicked) {

					combo.sequence.erase(combo.sequence.begin() + seqIdx);
					currentSequenceIndex_ = std::min<uint32_t>(currentSequenceIndex_,
						combo.sequence.empty() ? 0u : uint32_t(combo.sequence.size() - 1));

					ImGui::PopID();
					continue;
				} else {
					if (ImGui::BeginDragDropSource()) {

						const int payload = static_cast<int>(seqIdx);
						ImGui::SetDragDropPayload("SeqReorder", &payload, sizeof(int));
						ImGui::Text("%s", kStatetateNames[stateId]);
						ImGui::EndDragDropSource();
					}
					if (ImGui::BeginDragDropTarget()) {
						if (auto* payload = ImGui::AcceptDragDropPayload("SeqReorder")) {

							const int fromIdx = *static_cast<const int*>(payload->Data);
							std::swap(combo.sequence[fromIdx], combo.sequence[seqIdx]);
						}
						ImGui::EndDragDropTarget();
					}

					// → 区切り矢印
					ImGui::SameLine();
					if (seqIdx < combo.sequence.size() - 1) {

						ImGui::TextUnformatted("-");
						ImGui::SameLine();
					}

					// 削除しなかったときのみ進める
					++seqIdx;
				}

				ImGui::PopID();
			}

			//----------------------------------------------------------------
			// 列2: AddStateドロップダウン
			//----------------------------------------------------------------
			ImGui::TableNextColumn();
			{
				ImGui::PushItemWidth(buttonSize.x);

				static int selectedState = 0;
				const std::string addLabel = "##" + std::to_string(comboIdx);
				if (ImGui::Combo(addLabel.c_str(), &selectedState, kStatetateNames, IM_ARRAYSIZE(kStatetateNames))) {

					const BossEnemyState newState = static_cast<BossEnemyState>(selectedState);
					if (std::ranges::find(combo.sequence, newState) == combo.sequence.end()) {

						combo.sequence.push_back(newState);
					}
				}

				ImGui::PopItemWidth();
			}
			ImGui::PopID();
		}
		ImGui::EndTable();
	}
	ImGui::PopStyleColor(2);

	ImGui::SeparatorText("Edit Phase");

	//--------------------------------------------------------------------
	// Phasesテーブル
	//--------------------------------------------------------------------

	float duration = stateTable_.phases[currentPhase_].nextStateDuration;
	float progress = std::clamp(stateTimer_ / duration, 0.0f, 1.0f);
	ImGui::ProgressBar(progress, ImVec2(200.0f, 0.0f));

	// 数値表示
	ImGui::SameLine();
	ImGui::Text("%.3f / %.3f", stateTimer_, duration);

	// settings
	ImGui::PushStyleColor(ImGuiCol_HeaderHovered, headerColor);
	ImGui::PushStyleColor(ImGuiCol_HeaderActive, headerColor);

	// 必要フェーズ数をHP閾値に合わせて調整
	SyncPhaseCount();

	if (ImGui::BeginTable("##Phases", 4, ImGuiTableFlags_BordersInner)) {

		ImGui::TableSetupColumn("Phase");
		ImGui::TableSetupColumn("Duration");
		ImGui::TableSetupColumn("Combos");
		ImGui::TableSetupColumn("AddCombo");
		ImGui::TableHeadersRow();

		for (uint32_t phaseIdx = 0; phaseIdx < static_cast<int>(stateTable_.phases.size()); ++phaseIdx) {

			auto& phase = stateTable_.phases[phaseIdx];
			ImGui::PushID(phaseIdx);
			ImGui::TableNextRow();

			//----------------------------------------------------------------
			// 列0: Phase 名
			//----------------------------------------------------------------

			ImGui::TableNextColumn();
			ImGui::AlignTextToFramePadding();
			DrawHighlighted(phaseIdx == currentPhase_, kHighlight, [&] {
				ImGui::Text("Phase%d", phaseIdx); });

			//----------------------------------------------------------------
			// 列1: Duration
			//----------------------------------------------------------------

			ImGui::TableNextColumn();
			ImGui::DragFloat("##value", &phase.nextStateDuration, 0.01f);

			//----------------------------------------------------------------
			// 列2: Phaseが保持するCombo
			//----------------------------------------------------------------

			ImGui::TableNextColumn();
			for (size_t slotIdx = 0; slotIdx < phase.comboIndices.size();) {

				int comboID = phase.comboIndices[slotIdx];
				bool isCurrentCombo = (phaseIdx == currentPhase_) && (comboID == currentComboID);
				const std::string name = "Combo" + std::to_string(comboID);

				ImGui::PushID(static_cast<int>(slotIdx));

				bool clicked = false;
				DrawHighlighted(isCurrentCombo, kHighlight, [&] {
					clicked = ImGui::SmallButton(name.c_str()); });
				if (clicked) {

					phase.comboIndices.erase(phase.comboIndices.begin() + slotIdx);
					if (phaseIdx == currentPhase_) {
						currentComboSlot_ = std::min<uint32_t>(currentComboSlot_,
							phase.comboIndices.empty() ? 0u : uint32_t(phase.comboIndices.size() - 1));
					}
				} else {
					if (ImGui::BeginDragDropSource()) {

						const int payload = static_cast<int>(slotIdx);
						ImGui::SetDragDropPayload("PhaseReorder", &payload, sizeof(int));
						ImGui::Text("%s", name.c_str());
						ImGui::EndDragDropSource();
					}
					if (ImGui::BeginDragDropTarget()) {
						if (auto* payload = ImGui::AcceptDragDropPayload("PhaseReorder")) {

							const int fromIdx = *static_cast<const int*>(payload->Data);
							std::swap(phase.comboIndices[fromIdx],
								phase.comboIndices[slotIdx]);
						}
						ImGui::EndDragDropTarget();
					}

					ImGui::SameLine();
					++slotIdx;
				}

				ImGui::PopID();
			}

			//----------------------------------------------------------------
			// 列3: AddComboドロップターゲット
			//----------------------------------------------------------------
			ImGui::TableNextColumn();
			ImGui::Dummy(ImVec2(70.0f, 20.0f));

			if (ImGui::BeginDragDropTarget()) {
				if (const ImGuiPayload* pl = ImGui::AcceptDragDropPayload("ComboIdx")) {
					const int comboID = *static_cast<const int*>(pl->Data);
					phase.comboIndices.push_back(comboID);
				}
				ImGui::EndDragDropTarget();
			}

			ImGui::PopID();
		}
		ImGui::EndTable();
	}
	ImGui::PopStyleColor(2);
}

void BossEnemyStateController::ApplyJson() {

	// state
	{
		Json data;
		if (!JsonAdapter::LoadCheck(kStateJsonPath, data)) {
			return;
		}

		for (auto& [state, ptr] : states_) {

			ptr->ApplyJson(data[kStatetateNames[static_cast<int>(state)]]);
		}
	}

	// tabel
	{
		Json data;
		if (JsonAdapter::LoadCheck(kStateTableJsonPath, data)) {

			stateTable_.FromJson(data);
		} else {

			stateTable_.combos.clear(); stateTable_.phases.clear();
			// Combo0: Idle
			BossEnemyCombo combo;
			combo.sequence.emplace_back(BossEnemyState::Idle);
			stateTable_.combos.push_back(combo);
			// Phase0がCombo0 を参照
			BossEnemyPhase phase;
			phase.comboIndices.emplace_back(0);
			stateTable_.phases.push_back(phase);
		}
	}
}

void BossEnemyStateController::SaveJson() {

	// state
	{
		Json data;
		for (auto& [state, ptr] : states_) {

			ptr->SaveJson(data[kStatetateNames[static_cast<int>(state)]]);
		}

		JsonAdapter::Save(kStateJsonPath, data);
	}

	// table
	{
		Json data;
		stateTable_.ToJson(data);

		JsonAdapter::Save(kStateTableJsonPath, data);
	}
}
