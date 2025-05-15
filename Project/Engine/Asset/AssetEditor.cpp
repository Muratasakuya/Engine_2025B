#include "AssetEditor.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Asset/Asset.h>
#include <Lib/Adapter/JsonAdapter.h>

//============================================================================
//	AssetEditor classMethods
//============================================================================

AssetEditor* AssetEditor::instance_ = nullptr;

AssetEditor* AssetEditor::GetInstance() {

	if (instance_ == nullptr) {
		instance_ = new AssetEditor();
	}
	return instance_;
}

void AssetEditor::Finalize() {

	if (instance_ != nullptr) {

		delete instance_;
		instance_ = nullptr;
	}
}

void AssetEditor::BuildDirectoryTree() {

	std::function<void(DirectoryNode&)> recurse = [&](DirectoryNode& node) {

		// フォルダ内走査
		for (const auto& entry : std::filesystem::directory_iterator(node.path)) {

			const auto& p = entry.path();
			const std::string filename = p.filename().string();

			// Engineフォルダは完全に無視
			if (entry.is_directory() && filename == "Engine")
				continue;

			// 子ノード生成
			auto child = std::make_unique<DirectoryNode>();
			child->name = filename;
			child->path = p;
			child->isDirectory = entry.is_directory();

			if (child->isDirectory) {

				// 再帰
				recurse(*child);
				node.children.emplace_back(std::move(child));
			} else {

				// ファイルフィルタリング
				const std::string parent = p.parent_path().filename().string();

				// ファイル種別で振り分け
				if (IsJsonFile(p) || IsModelFile(p) || IsTextureFile(p)) {

					node.children.emplace_back(std::move(child));
				}
				// その他は無視
			}
		}
		};

	recurse(*root_);
}

void AssetEditor::Init(Asset* asset) {

	asset_ = nullptr;
	asset_ = asset;

	// editorに使う画像
	asset_->LoadTexture("folder");
	asset_->LoadTexture("texture"); // texture表示、読み込まれていなければこっち
	asset_->LoadTexture("model");   // model表示
	asset_->LoadTexture("file");    // file表示
	// GPUHandleを予め取得
	folderIcon_ = asset_->GetGPUHandle("folder");
	fileIcon_ = asset_->GetGPUHandle("file");
	modelIcon_ = asset_->GetGPUHandle("model");
	textureIcon_ = asset_->GetGPUHandle("texture");

	// directoryTreeを作成
	root_ = std::make_unique<DirectoryNode>();
	root_->name = "Assets";
	root_->path = std::filesystem::path("./Assets");
	root_->opened = true;
	root_->isDirectory = true;

	BuildDirectoryTree();

	// 最初の設定
	current_ = root_.get();

	// json適応
	ApplyJson();
}

void AssetEditor::EditLayout() {

	ImGui::Begin("AssetEditor Layout");

	if (ImGui::Button("Save Json")) {

		SaveJson();
	}

	ImGui::DragFloat("folderSize", &folderSize_, 0.1f);
	ImGui::DragFloat("folderNameSpancing", &folderNameSpancing_, 0.01f);
	ImGui::DragFloat("folderSpace", &folderSpacing_, 0.1f);
	ImGui::DragFloat("charScale_", &charScale_, 0.01f);
	ImGui::DragFloat("chidNameOffset", &chidNameOffset_, 0.01f);
	ImGui::DragFloat2("folderOffset", &folderOffset_.x, 1.0f);

	ImGui::End();
}

void AssetEditor::ImGui() {

	if (!current_) {
		return;
	}

	// 文字サイズを設定
	ImGui::SetWindowFontScale(charScale_);

	// 各Gui表示
	DrawHeader();
	ImGui::Separator();
	DrawFolderGrid();

	// 読み込み表示
	if (showLoadButton_) {
		DrawLoadOverlay();
	}

	// 元に戻す
	ImGui::SetWindowFontScale(1.0f);
}

void AssetEditor::DrawHeader() {

	//　root表示
	if (ImGui::Button("Root")) {

		current_ = root_.get();
		navStack_.clear();
	}

	// ルートから直前ディレクトリまで
	for (size_t i = 0; i < navStack_.size(); ++i) {

		ImGui::SameLine();
		ImGui::TextUnformatted(">");
		ImGui::SameLine();

		DirectoryNode* node = navStack_[i];
		if (ImGui::Button(node->name.c_str())) {

			// クリックした階層まで戻る
			current_ = node;
			// クリックしたノードより深い階層を削除
			navStack_.resize(i);
			break;
		}
	}

	// 現在ディレクトリ名
	ImGui::SameLine(); ImGui::TextUnformatted(">");
	ImGui::SameLine(); ImGui::TextUnformatted(current_->name.c_str());

	// 元の階層に戻る
	DrawBackButton();
}

void AssetEditor::DrawFolderGrid() {

	const float cellSize = folderSize_ + folderSpacing_;
	const float panelWidth = ImGui::GetContentRegionAvail().x;

	int columnCount = static_cast<int>(panelWidth / cellSize);
	if (columnCount < 1) columnCount = 1;

	ImGui::Columns(columnCount, nullptr, false);

	std::vector<DirectoryNode*> folders;
	std::vector<DirectoryNode*> files;
	for (const auto& childPtr : current_->children) {
		(childPtr->isDirectory ? folders : files).push_back(childPtr.get());
	}

	// folders → files の順に結合して描画
	std::vector<DirectoryNode*> displayList;
	displayList.reserve(folders.size() + files.size());
	displayList.insert(displayList.end(), folders.begin(), folders.end());
	displayList.insert(displayList.end(), files.begin(), files.end());

	// 現在ノードの子を描画
	for (DirectoryNode* child : displayList) {

		ImGui::PushID(child->path.c_str());

		ImVec2 basePos = ImGui::GetCursorScreenPos();
		ImGui::SetCursorScreenPos(ImVec2(
			basePos.x + folderOffset_.x,
			basePos.y + folderOffset_.y));

		// icon表示
		const bool clicked = ImGui::ImageButton(
			child->name.c_str(),
			ImTextureID(GetIconForEntry(*child).ptr),
			ImVec2(folderSize_, folderSize_));

		// 右クリックで「ロード候補」に登録
		if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {

			const std::string stem = child->path.stem().string();
			overlayPos_ = ImGui::GetItemRectMax(); // アイテム右上
			// 少し右上にずらす
			overlayPos_.x += 8.0f;
			overlayPos_.y -= 8.0f;
			if (IsTextureFile(child->path) && !asset_->SearchTexture(stem)) {

				pendingPath_ = child->path;
				pendingType_ = PendingType::Texture;
				showLoadButton_ = true;
			} else if (IsModelFile(child->path) /* && !asset_->SearchModel(stem) */) {

				pendingPath_ = child->path;
				pendingType_ = PendingType::Model;
				showLoadButton_ = true;
			}
		}

		const std::string displayName = child->isDirectory ? child->name : child->path.stem().string();

		ImVec2 textSize = ImGui::CalcTextSize(displayName.c_str());
		float centerOffset = (folderSize_ - textSize.x) * 0.5f;
		if (centerOffset < 0.0f) centerOffset = 0.0f;

		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + centerOffset + chidNameOffset_);
		ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + folderSize_);
		ImGui::TextUnformatted(displayName.c_str());
		ImGui::PopTextWrapPos();

		// クリックでディレクトリへ入る
		if (clicked && child->isDirectory) {

			navStack_.push_back(current_);
			current_ = child;
		}

		ImGui::PopID();
		ImGui::NextColumn();
	}

	ImGui::Columns(1);
}

void AssetEditor::DrawBackButton() {

	ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 80.0f);
	if (ImGui::Button("Back") && !navStack_.empty()) {

		current_ = navStack_.back();
		navStack_.pop_back();
	}
}

void AssetEditor::DrawLoadOverlay() {

	// 右クリック位置にウインドウを出す
	ImGui::SetNextWindowPos(overlayPos_, ImGuiCond_Always);
	ImGui::SetNextWindowBgAlpha(0.90f);

	if (!ImGui::Begin("##LoadOverlay",
		nullptr,
		ImGuiWindowFlags_NoDecoration |
		ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoSavedSettings)) {
		ImGui::End();
		return;
	}

	const char* title =
		(pendingType_ == PendingType::Texture) ? "Load texture" :
		(pendingType_ == PendingType::Model) ? "Load model" : "Load";

	ImGui::TextUnformatted(title);
	ImGui::Separator();
	ImGui::TextWrapped("%s", pendingPath_.filename().string().c_str());

	if (ImGui::Button("Load", ImVec2(64.0f, 0.0f))) {

		std::string id = pendingPath_.stem().string();
		if (pendingType_ == PendingType::Texture) {

			asset_->LoadTexture(id);
		} else if (pendingType_ == PendingType::Model) {

			asset_->LoadModel(id);
		}

		showLoadButton_ = false;
		pendingType_ = PendingType::None;
	}
	ImGui::SameLine();
	if (ImGui::Button("Cancel", ImVec2(64.0f, 0.0f))) {

		showLoadButton_ = false;
		pendingType_ = PendingType::None;
	}

	ImGui::End();
}

bool AssetEditor::IsTextureFile(const std::filesystem::path& path) const {

	const std::string ext = path.extension().string();
	return ext == ".png" || ext == ".jpg" || ext == ".jpeg" ||
		ext == ".tga" || ext == ".bmp" || ext == ".dds";
}

bool AssetEditor::IsModelFile(const std::filesystem::path& path) const {

	const std::string ext = path.extension().string();
	return ext == ".obj" || ext == ".gltf";
}

bool AssetEditor::IsJsonFile(const std::filesystem::path& path) const {

	return path.extension() == ".json";
}

D3D12_GPU_DESCRIPTOR_HANDLE AssetEditor::GetIconForEntry(const DirectoryNode& entry) const {

	if (entry.isDirectory) {
		return folderIcon_;
	}

	const std::string parent = entry.path.parent_path().filename().string();

	// Json
	if (IsJsonFile(entry.path)) {

		return fileIcon_;
	}

	// Model
	if (IsModelFile(entry.path)) {

		return modelIcon_;
	}

	// Texture
	if (IsTextureFile(entry.path)) {

		// 既にロード済みならそのテクスチャ
		const std::string stem = entry.path.stem().string();
		if (asset_->SearchTexture(stem)) {

			D3D12_GPU_DESCRIPTOR_HANDLE handle = asset_->GetGPUHandle(stem);
			return handle;
		}

		// 未ロードの場合はデフォルト
		return textureIcon_;
	}

	// フォールバック
	return fileIcon_;
}

void AssetEditor::ApplyJson() {

	Json data;
	if (!JsonAdapter::LoadCheck(baseJsonPath_ + "editorParameter.json", data)) {
		return;
	}

	folderSize_ = JsonAdapter::GetValue<float>(data, "folderSize_");
	folderSpacing_ = JsonAdapter::GetValue<float>(data, "folderSpacing_");
	folderNameSpancing_ = JsonAdapter::GetValue<float>(data, "folderNameSpancing_");
	charScale_ = JsonAdapter::GetValue<float>(data, "charScale_");
	chidNameOffset_ = JsonAdapter::GetValue<float>(data, "chidNameOffset_");
	folderOffset_ = JsonAdapter::ToObject<Vector2>(data["folderOffset_"]);
}

void AssetEditor::SaveJson() {

	Json data;

	data["folderSize_"] = folderSize_;
	data["folderSpacing_"] = folderSpacing_;
	data["folderNameSpancing_"] = folderNameSpancing_;
	data["charScale_"] = charScale_;
	data["chidNameOffset_"] = chidNameOffset_;
	data["folderOffset_"] = JsonAdapter::FromObject<Vector2>(folderOffset_);

	JsonAdapter::Save(baseJsonPath_ + "editorParameter.json", data);
}