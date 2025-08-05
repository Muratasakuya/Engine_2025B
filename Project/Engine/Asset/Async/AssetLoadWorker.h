#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Asset/Async/AssetAsyncQueue.h>

// c++
#include <functional>

//============================================================================
//	AssetLoadWorker class
//============================================================================
template<class T>
class AssetLoadWorker {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	AssetLoadWorker() = default;
	~AssetLoadWorker();

	void Start(std::function<void(T&&)> process);

	void Stop();

	//--------- accessor -----------------------------------------------------

	AssetAsyncQueue<T>& RefAsyncQueue() { return queue_; }
	const AssetAsyncQueue<T>& GetAsyncQueue() const { return queue_; }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	AssetAsyncQueue<T> queue_;
	std::atomic_bool stop_;
	std::thread thread_;
};

//============================================================================
//	AssetLoadWorker templateMethods
//============================================================================

template<class T>
inline AssetLoadWorker<T>::~AssetLoadWorker() {

	// 起動しているスレッドを終了させる
	Stop();
}

template<class T>
inline void AssetLoadWorker<T>::Start(std::function<void(T&&)> process) {

	stop_ = false;
	thread_ = std::thread([this, process = std::move(process)] {
		while (!stop_) {
			auto job = queue_.PopBlock(stop_);
			if (!job) {

				break;
			}
			process(std::move(*job));
		}
		});
}

template<class T>
inline void AssetLoadWorker<T>::Stop() {

	stop_ = true;
	queue_.AddQueue(T{});
	if (thread_.joinable()) {

		thread_.join();
	}
}