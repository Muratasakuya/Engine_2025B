#pragma once

//============================================================================
//	include
//============================================================================

// c++
#include <string>

namespace SakuEngine {

	/// <summary>
	/// オブジェクトID、識別子、表示名、所属グループ、シーン切替時の破棄可否をまとめて保持するタグ情報。
	/// </summary>
	struct ObjectTag {

		uint32_t objectID;         // objectのID、重複しない
		std::string identifier;    // objectの識別子、重複しない
		std::string name;          // objectの名前、重複したら数字がつく
		std::string groupName;     // objectの所属しているgroupの名前
		bool destroyOnLoad = true; // 全破棄時に破棄しないか
	};

}; // SakuEngine
