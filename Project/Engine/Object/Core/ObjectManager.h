#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Object/Core/ObjectPoolManager.h>
#include <Engine/Object/System/Manager/SystemManager.h>

// directX
#include <d3d12.h>
// c++
#include <optional>
#include <string>
#include <unordered_set>
namespace SakuEngine {

	// front
	class Asset;
	class DxCommand;

	/// <summary>
	/// オブジェクト生成/破棄、各データプール管理、各System連携を統括する管理クラス。
	/// </summary>
	class ObjectManager {
	public:
		//========================================================================
		//	public Methods
		//========================================================================

		ObjectManager() = default;
		~ObjectManager() = default;

		/// <summary>
		/// デバイス、アセット、コマンドを受け取り、各Systemとオブジェクトプールを初期化する。
		/// </summary>
		/// <param name="device">GPUリソース作成に使用するDirect3D12デバイス。</param>
		/// <param name="asset">モデル、テクスチャ、フォントなどを取得するアセット管理。</param>
		/// <param name="dxCommand">描画バッファ構築に使用するコマンド制御。</param>
		void Init(ID3D12Device* device, Asset* asset, DxCommand* dxCommand);

		/// <summary>
		/// 登録済みSystemのデータ更新処理を実行する。
		/// </summary>
		void UpdateData();
		/// <summary>
		/// 登録済みSystemのGPUバッファ更新処理を実行する。
		/// </summary>
		void UpdateBuffer();

		//---------- objects -----------------------------------------------------

		/// <summary>
		/// モデル名をもとにTransform、Material、MeshRender、必要ならAnimationを持つ3Dオブジェクトを作成する。
		/// </summary>
		/// <param name="modelName">描画に使用するモデル名。</param>
		/// <param name="name">作成するオブジェクト名。</param>
		/// <param name="groupName">所属グループ名。</param>
		/// <param name="animationName">スキニング再生に使用するアニメーション名。</param>
		/// <returns>作成したオブジェクトID。</returns>
		uint32_t CreateObjects(const std::string& modelName, const std::string& name,
			const std::string& groupName, const std::optional<std::string>& animationName = std::nullopt);
		/// <summary>
		/// 指定テクスチャを使用するスカイボックスオブジェクトを作成する。
		/// </summary>
		/// <param name="textureName">スカイボックス描画に使用するキューブマップテクスチャ名。</param>
		/// <returns>作成したオブジェクトID。</returns>
		uint32_t CreateSkybox(const std::string& textureName);
		/// <summary>
		/// エフェクト用Transformを持つ空のエフェクトオブジェクトを作成する。
		/// </summary>
		/// <param name="name">作成するエフェクトオブジェクトの識別名。</param>
		/// <param name="groupName">所属グループ名。</param>
		/// <returns>作成したオブジェクトID。</returns>
		uint32_t CreateEffect(const std::string& name, const std::string& groupName);

		/// <summary>
		/// 指定テクスチャを使用する2Dスプライトオブジェクトを作成する。
		/// </summary>
		/// <param name="textureName">スプライト描画に使用するテクスチャ名。</param>
		/// <param name="name">作成するオブジェクト名。</param>
		/// <param name="groupName">所属グループ名。</param>
		/// <returns>作成したオブジェクトID。</returns>
		uint32_t CreateObject2D(const std::string& textureName, const std::string& name,
			const std::string& groupName);
		/// <summary>
		/// MSDFフォントとアトラステクスチャを使用するテキストオブジェクトを作成する。
		/// </summary>
		/// <param name="atlasTextureName">文字描画に使用するMSDFアトラステクスチャ名。</param>
		/// <param name="fontJsonPath">フォントメトリクスを読み込むJSONファイルパス。</param>
		/// <param name="name">作成するオブジェクト名。</param>
		/// <param name="groupName">所属グループ名。</param>
		/// <returns>作成したオブジェクトID。</returns>
		uint32_t CreateTextObject(const std::string& atlasTextureName, const std::string& fontJsonPath,
			const std::string& name, const std::string& groupName);

		/// <summary>
		/// ObjectTagだけを持つ空オブジェクトを作成し、後続処理で必要なデータを追加できる状態にする。
		/// </summary>
		/// <param name="name">作成するオブジェクト名。</param>
		/// <param name="groupName">所属グループ名。</param>
		/// <returns>作成したオブジェクトID。</returns>
		uint32_t BuildEmptyObject(const std::string& name, const std::string& groupName);

		/// <summary>
		/// 指定オブジェクトIDに紐づく全データを破棄対象にする。
		/// </summary>
		/// <param name="object">破棄対象のオブジェクトID。</param>
		void Destroy(uint32_t object);
		/// <summary>
		/// destroyOnLoadが有効な全オブジェクトを一括破棄する。
		/// </summary>
		void DestroyAll();

		//--------- accessor -----------------------------------------------------

		/// <summary>
		/// 指定オブジェクトに結びつくデータTを取得する。
		/// </summary>
		/// <param name="object">取得対象のオブジェクトID。</param>
		/// <returns>指定オブジェクトが持つデータストレージ。</returns>
		template<class T, bool Flag = false>
		typename ObjectPool<T, Flag>::Storage* GetData(uint32_t object);

		/// <summary>
		/// 登録済みSystemから型TのSystemを取得する。
		/// </summary>
		/// <returns>指定型のSystemポインタ。</returns>
		template<class T>
		T* GetSystem() const;

		/// <summary>
		/// オブジェクトプール管理を取得する。
		/// </summary>
		/// <returns>内部で保持しているObjectPoolManager。</returns>
		ObjectPoolManager* GetObjectPoolManager() const { return objectPoolManager_.get(); }

		/// <summary>
		/// ObjectManagerのシングルトンインスタンスを取得する。
		/// </summary>
		/// <returns>ObjectManagerの共有インスタンス。</returns>
		static ObjectManager* GetInstance();
		/// <summary>
		/// ObjectManagerのシングルトン参照を無効化する。
		/// </summary>
		static void Finalize();
	private:
		//========================================================================
		//	private Methods
		//========================================================================

		//--------- variables ----------------------------------------------------

		Asset* asset_;
		ID3D12Device* device_;

		static ObjectManager* instance_;

		std::unique_ptr<ObjectPoolManager> objectPoolManager_;

		std::unique_ptr<SystemManager> systemManager_;
	};

	//============================================================================
	//	ObjectManager templateMethods
	//============================================================================

	/// <summary>
	/// 指定オブジェクトに結びつくデータTを取得する。
	/// </summary>
	/// <param name="object">取得対象のオブジェクトID。</param>
	/// <returns>指定オブジェクトが持つデータストレージ。</returns>
	template<class T, bool Flag>
	inline typename ObjectPool<T, Flag>::Storage* ObjectManager::GetData(uint32_t object) {

		return objectPoolManager_->GetData<T, Flag>(object);
	}

	/// <summary>
	/// 登録済みSystemから型TのSystemを取得する。
	/// </summary>
	/// <returns>指定型のSystemポインタ。</returns>
	template<class T>
	inline T* ObjectManager::GetSystem() const {

		return systemManager_->GetSystem<T>();
	}
}; // SakuEngine
