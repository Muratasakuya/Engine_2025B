#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Asset/AssetStructure.h>
#include <Engine/Core/Graphics/Mesh/MeshletStructures.h>

// c++
#include <unordered_map>
#include <thread>
#include <mutex>
#include <deque>
// front
class TextureManager;

//============================================================================
//	ModelManager class
//============================================================================
class ModelLoader {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	ModelLoader() = default;
	~ModelLoader();

	void Init(TextureManager* textureManager);

	void Load(const std::string& modelName);

	void Make(const std::string& modelName,
		const std::vector<MeshVertex>& vertexData,
		const std::vector<uint32_t>& indexData);
	void Export(const std::vector<MeshVertex>& inputVertices,
		const std::vector<uint32_t>& inputIndices,const std::string& filePath);

	bool Search(const std::string& modelName);

	void ReportUsage(bool listAll) const;

	// 非同期処理
	void RequestLoadAsync(const std::string& modelName);
	void WaitAll();

	//--------- accessor -----------------------------------------------------

	const ModelData& GetModelData(const std::string& modelName) const;

	const std::vector<std::string>& GetModelKeys() const;
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	TextureManager* textureManager_;

	std::string baseDirectoryPath_;

	std::unordered_map<std::string, ModelData> models_;

	mutable std::vector<std::string> modelKeysCache_;
	mutable bool isCacheValid_;

	// 非同期処理
	std::thread worker_;
	std::atomic_bool stop_{ false };
	std::mutex jobMutex_;
	std::condition_variable jobCv_;
	std::deque<std::string> jobs_;

	mutable std::mutex modelMutex_;

	//--------- functions ----------------------------------------------------

	ModelData LoadModelFile(const std::string& filePath);
	Node ReadNode(aiNode* node);
	// helper
	bool FindModelPath(const std::string& modelName, std::filesystem::path& outPath);

	// 非同期処理
	void WorkerLoop();
};