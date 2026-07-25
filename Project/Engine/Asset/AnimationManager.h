#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Asset/AssetStructure.h>
#include <Engine/Asset/Async/AssetLoadWorker.h>

// assimp
#include <Externals/assimp/include/assimp/Importer.hpp>
#include <Externals/assimp/include/assimp/postprocess.h>
#include <Externals/assimp/include/assimp/scene.h>
// c++
#include <unordered_map>
#include <unordered_set>

namespace SakuEngine {

	// front
	class SRVDescriptor;
	class ModelLoader;

	/// <summary>
	/// モデルに紐づくアニメーション、スケルトン、スキンクラスターを読み込み、再生側へ提供する管理クラス。
	/// </summary>
	class AnimationManager {
	public:
		//========================================================================
		//	public Methods
		//========================================================================

		AnimationManager() = default;
		~AnimationManager() = default;

		/// <summary>
		/// アニメーション読み込みに必要なデバイス、SRV、モデルローダを登録し、非同期ワーカーを開始する。
		/// </summary>
		/// <param name="device">GPUリソース作成に使用するD3D12デバイス。</param>
		/// <param name="srvDescriptor">スキンクラスター用SRVを確保するディスクリプタ管理クラス。</param>
		/// <param name="modelLoader">アニメーションと紐づくモデルデータの取得元。</param>
		void Init(ID3D12Device* device, SRVDescriptor* srvDescriptor, ModelLoader* modelLoader);

		/// <summary>
		/// 指定したアニメーションを読み込み要求し、登録完了まで待機する。
		/// </summary>
		/// <param name="animationName">読み込むアニメーションファイル名またはstem名。</param>
		/// <param name="modelName">骨構造とスキンクラスターの基準にするモデル名。</param>
		void Load(const std::string& animationName, const std::string& modelName);

		/// <summary>
		/// 指定したアニメーションとモデルの組を非同期読み込みキューへ追加する。
		/// </summary>
		/// <param name="animationName">読み込むアニメーションファイル名またはstem名。</param>
		/// <param name="modelName">読み込み完了後に関連付けるモデル名。</param>
		void RequestLoadAsync(const std::string& animationName, const std::string& modelName);
		/// <summary>
		/// 非同期読み込みキューが空になるまで待機する。
		/// </summary>
		void WaitAll();

		//--------- accessor -----------------------------------------------------

		/// <summary>
		/// 再生に必要なアニメーションデータを取得する。未登録の場合はログ出力後にASSERTする。
		/// </summary>
		/// <param name="animationName">取得するアニメーション名。</param>
		/// <returns>登録済みアニメーションデータへの参照。</returns>
		const AnimationData& GetAnimationData(const std::string& animationName) const;
		/// <summary>
		/// 指定モデル用のスケルトンデータを取得する。未登録の場合はログ出力後にASSERTする。
		/// </summary>
		/// <param name="animationName">取得するスケルトン名。</param>
		/// <returns>登録済みスケルトンデータへの参照。</returns>
		const Skeleton& GetSkeletonData(const std::string& animationName) const;
		/// <summary>
		/// 指定モデル用のスキンクラスターデータを取得する。未登録の場合はログ出力後にASSERTする。
		/// </summary>
		/// <param name="animationName">取得するスキンクラスター名。</param>
		/// <returns>登録済みスキンクラスターデータへの参照。</returns>
		const SkinCluster& GetSkinClusterData(const std::string& animationName) const;
	private:
		//========================================================================
		//	private Methods
		//========================================================================

		//--------- structure ----------------------------------------------------

		/// <summary>
		/// 非同期読み込みキューで扱う、アニメーション名とモデル名の組。
		/// </summary>
		struct AnimationAsyncKey {

			std::string animName;
			std::string modelName;
		};

		//--------- variables ----------------------------------------------------

		ID3D12Device* device_;
		SRVDescriptor* srvDescriptor_;
		ModelLoader* modelLoader_;

		std::string baseDirectoryPath_;

		std::unordered_map<std::string, AnimationData> animations_;
		std::unordered_map<std::string, Skeleton> skeletons_;
		std::unordered_map<std::string, SkinCluster> skinClusters_;

		// 非同期処理
		AssetLoadWorker<AnimationAsyncKey> loadWorker_;
		mutable std::mutex animMutex_;

		//--------- functions ----------------------------------------------------

		/// <summary>
		/// モデルのルートノードからジョイント階層と名前検索用マップを構築する。
		/// </summary>
		/// <param name="rootNode">スケルトン生成の起点になるモデルノード。</param>
		/// <returns>再生時に使用するスケルトンデータ。</returns>
		Skeleton CreateSkeleton(const Node& rootNode);
		/// <summary>
		/// ノード階層を再帰的にたどり、親子関係を保持したジョイント配列を作成する。
		/// </summary>
		/// <param name="node">作成対象のモデルノード。</param>
		/// <param name="parent">親ジョイントのインデックス。ルートの場合は空。</param>
		/// <param name="joints">生成したジョイントを追加する配列。</param>
		/// <returns>作成したジョイントのインデックス。</returns>
		int32_t CreateJoint(const Node& node, const std::optional<int32_t> parent, std::vector<Joint>& joints);
		/// <summary>
		/// モデルの頂点ウェイト情報とスケルトンを対応付け、GPUスキニング用のスキンクラスターを作成する。
		/// </summary>
		/// <param name="modelName">参照するモデルデータ名。</param>
		/// <param name="animationName">参照するスケルトンデータ名。</param>
		/// <returns>スキニングで使用するスキンクラスターデータ。</returns>
		SkinCluster CreateSkinCluster(const std::string& modelName, const std::string& animationName);

		/// <summary>
		/// ワーカースレッド上でアニメーションを解析し、共有データへ登録する。
		/// </summary>
		/// <param name="key">アニメーション名と関連モデル名をまとめた読み込みキー。</param>
		void LoadAsync(const AnimationAsyncKey& key);
	};

}; // SakuEngine
