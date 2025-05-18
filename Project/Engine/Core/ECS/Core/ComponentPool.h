#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Debug/Assert.h>

// c++
#include <memory>
#include <vector>
#include <bitset>
#include <unordered_map>
#include <typeindex>
// imgui
#include <imgui.h>

// componentBitSize
constexpr const size_t kMaxComponentTypes = 128;
using Archetype = std::bitset<kMaxComponentTypes>;

//============================================================================
//	IComponentPool class
//============================================================================
class IComponentPool {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	IComponentPool() = default;
	virtual ~IComponentPool() = default;

	virtual void Remove(uint32_t entity) = 0;

	virtual void Debug(const char* label) = 0;
};

//============================================================================
//	ComponentPool class
//============================================================================
template<class T, bool kMultiple = false>
class ComponentPool :
	public IComponentPool {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	explicit ComponentPool();
	~ComponentPool() = default;

	//--------- variables ----------------------------------------------------

	// アクセス番地
	// entity -> index
	std::unordered_map<uint32_t, size_t> entityToIndex_;
	// index -> entity
	std::vector<uint32_t> indexToEntity_;

	// componentData
	// kMultiple = true: std::vector<T>
	// kMultiple = false: T
	using Storage = std::conditional_t<kMultiple, std::vector<T>, T>;
	std::vector<Storage> data_;

	//--------- functions ----------------------------------------------------

	// debug
	void Debug(const char* label) override;

	// 追加
	template<class... Args>
	void Add(uint32_t entity, Args&&... args);
	// 削除
	void Remove(uint32_t entity) override;
	// component取得
	Storage* Get(uint32_t entity);
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- functions ----------------------------------------------------

	void RemoveImpl(uint32_t entity);
};

//============================================================================
//	ComponentPool templateMethods
//============================================================================

template<class T, bool kMultiple>
template<class ...Args>
inline void ComponentPool<T, kMultiple>::Add(uint32_t entity, Args && ...args) {

	// すでに持っていれば上書き
	auto it = entityToIndex_.find(entity);
	if (it != entityToIndex_.end()) {

		size_t idx = it->second;
		data_[idx] = Storage{ std::forward<Args>(args)... };
		return;
	}

	// capacityを超えたら
	ASSERT(data_.size() < data_.capacity(), "componentPool capacity exceeded");

	const size_t newIndex = data_.size();
	entityToIndex_[entity] = newIndex;
	indexToEntity_.push_back(entity);

	data_.emplace_back(Storage{ std::forward<Args>(args)... });
}

template<class T, bool kMultiple>
inline ComponentPool<T, kMultiple>::ComponentPool() {

	// 最初に最大数を確保、これ以降は禁止
	const size_t maxEntities = 2048;
	data_.reserve(maxEntities);
}

template<class T, bool kMultiple>
inline void ComponentPool<T, kMultiple>::Debug(const char* label) {

	ImGui::PushItemWidth(224.0f);

	if (!ImGui::CollapsingHeader(label)) {
		ImGui::PopItemWidth();
		return;
	}

	ImGui::Text("size      = %zu", data_.size());
	ImGui::Text("capacity  = %zu", data_.capacity());
	ImGui::Text("element   = %zu bytes", sizeof(Storage));

	// 連続性チェック
	bool contiguous = true;
	for (size_t i = 1; i < data_.size(); ++i) {

		uintptr_t prev = reinterpret_cast<uintptr_t>(&data_[i - 1]);
		uintptr_t curr = reinterpret_cast<uintptr_t>(&data_[i]);
		// 差分をStorageと比較
		if (curr - prev != sizeof(Storage)) {
			contiguous = false;
			break;
		}
	}
	// 連続していれば緑、していなければ赤
	ImGui::TextColored(contiguous ? ImVec4(0.0f, 1.0f, 0.0f, 1.0f)
		: ImVec4(1.0f, 0.0f, 0.0f, 1.0f),
		"contiguous : %s", contiguous ? "YES" : "NO");

	// メモリアドレス一覧
	const std::string labelStr = std::string("MemoryArray##") + label;
	if (ImGui::TreeNode(labelStr.c_str())) {

		ImGui::Text("Index   Address");
		ImGui::Separator();

		uintptr_t prevAddr = 0;
		for (size_t i = 0; i < data_.size(); ++i) {

			uintptr_t addr = reinterpret_cast<uintptr_t>(&data_[i]);
			if (i == 0) {

				ImGui::Text("[%4zu]  0x%016llx      -", i, static_cast<unsigned long long>(addr));
			} else {

				ImGui::Text("[%4zu]  0x%016llx   %+6lld", i,
					static_cast<unsigned long long>(addr),
					static_cast<long long>(addr - prevAddr));
			}

			prevAddr = addr;
		}
		ImGui::TreePop();
	}

	ImGui::PopItemWidth();
}

template<class T, bool kMultiple>
inline void ComponentPool<T, kMultiple>::Remove(uint32_t entity) {

	RemoveImpl(entity);
}

template<class T, bool kMultiple>
inline ComponentPool<T, kMultiple>::Storage* ComponentPool<T, kMultiple>::Get(uint32_t entity) {

	auto it = entityToIndex_.find(entity);
	// entityが存在していれば値を返す
	if (it != entityToIndex_.end()) {

		return  &data_[it->second];
	}
	// 存在していなければnullptrを返す
	return nullptr;
}

template<class T, bool kMultiple>
inline void ComponentPool<T, kMultiple>::RemoveImpl(uint32_t entity) {

	// 存在しないentityの場合処理しない
	auto it = entityToIndex_.find(entity);
	if (it == entityToIndex_.end()) {
		return;
	}

	size_t index = it->second;
	size_t last = data_.size() - 1;
	if (index != last) {

		std::swap(data_[index], data_[last]);
		uint32_t moved = indexToEntity_[last];
		entityToIndex_[moved] = index;
		indexToEntity_[index] = moved;
	}
	data_.pop_back();
	indexToEntity_.pop_back();
	entityToIndex_.erase(it);
}