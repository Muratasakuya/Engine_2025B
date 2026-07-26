#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/DxLib/DxStructures.h>
#include <Engine/Core/Graphics/DxLib/ComPtr.h>

// directX
#include <d3d12.h>
#include <dxgi1_6.h>
// c++
#include <cstdint>
#include <vector>
#include <array>
#include <optional>
#include <chrono>
#include <thread>
#include <future>

namespace SakuEngine {

/// <summary>
/// Direct3D12のコマンドキュー、アロケータ、コマンドリスト、フェンスを管理し、実行と同期を提供する。
/// </summary>
class DxCommand {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	DxCommand() = default;
	~DxCommand() = default;

	/// <summary>
	/// デバイスからコマンドキュー、アロケータ、コマンドリスト、フェンスを生成して初期化する。
	/// </summary>
	/// <param name="device">Direct3D12デバイス。</param>
	void Create(ID3D12Device* device);

	/// <summary>
	/// 記録済みコマンドをキューへ提出し、Present、GPU待機、FPS固定、コマンドリスト再利用準備を行う。
	/// </summary>
	/// <param name="swapChain">Present対象のスワップチェーン。</param>
	void ExecuteCommands(IDXGISwapChain4* swapChain);

	/// <summary>
	/// 現在のコマンドリストを提出し、フェンスを用いてGPUの実行完了までCPU側で待機する。
	/// </summary>
	void WaitForGPU();

	/// <summary>
	/// フェンスイベントを閉じ、関連ウィンドウを閉じる終了処理を行う。
	/// </summary>
	/// <param name="hwnd">閉じる対象のウィンドウハンドル。</param>
	void Finalize(HWND hwnd);

	/// <summary>
	/// 描画/計算コマンドで使用するディスクリプタヒープ配列をコマンドリストへ設定する。
	/// </summary>
	/// <param name="descriptorHeaps">設定するディスクリプタヒープ配列。</param>
	void SetDescriptorHeaps(const std::vector<ID3D12DescriptorHeap*>& descriptorHeaps);

	/// <summary>
	/// 単一レンダーターゲットと任意のDSVをOMステージへ設定し、レンダーターゲットをクリアする。
	/// </summary>
	/// <param name="renderTarget">設定するレンダーターゲット。未指定時はDSVのみを設定する。</param>
	/// <param name="dsvHandle">設定する深度ステンシルビュー。未指定時はDSVを使用しない。</param>
	void SetRenderTargets(const std::optional<RenderTarget>& renderTarget,
		const std::optional<D3D12_CPU_DESCRIPTOR_HANDLE>& dsvHandle = std::nullopt);
	/// <summary>
	/// 複数レンダーターゲットと任意のDSVをOMステージへ設定し、各レンダーターゲットをクリアする。
	/// </summary>
	/// <param name="renderTargets">設定するレンダーターゲット配列。</param>
	/// <param name="dsvHandle">設定する深度ステンシルビュー。未指定時はDSVを使用しない。</param>
	void SetRenderTargets(const std::vector<RenderTarget>& renderTargets,
		const std::optional<D3D12_CPU_DESCRIPTOR_HANDLE>& dsvHandle = std::nullopt);

	/// <summary>
	/// 指定したDSVの深度/ステンシルをクリアする。
	/// </summary>
	/// <param name="dsvHandle">クリア対象のDSV CPUハンドル。</param>
	void ClearDepthStencilView(const D3D12_CPU_DESCRIPTOR_HANDLE& dsvHandle);

	/// <summary>
	/// 指定サイズでビューポートとシザー矩形を設定する。
	/// </summary>
	/// <param name="width">描画領域の幅。</param>
	/// <param name="height">描画領域の高さ。</param>
	void SetViewportAndScissor(uint32_t width, uint32_t height);

	/// <summary>
	/// 複数リソースの状態をまとめて遷移するリソースバリアを発行する。
	/// </summary>
	/// <param name="resources">状態遷移するDirect3D12リソース配列。</param>
	/// <param name="stateBefore">遷移前のリソース状態。</param>
	/// <param name="stateAfter">遷移後のリソース状態。</param>
	void TransitionBarriers(const std::vector<ID3D12Resource*>& resources,
		D3D12_RESOURCE_STATES stateBefore, D3D12_RESOURCE_STATES stateAfter);

	/// <summary>
	/// 指定リソースにUAVバリアを発行し、UAV書き込み順序を保証する。
	/// </summary>
	/// <param name="resource">バリア対象のリソース。</param>
	void UAVBarrier(ID3D12Resource* resource);
	/// <summary>
	/// 全UAV書き込みを対象にしたUAVバリアを発行する。
	/// </summary>
	void UAVBarrierAll();

	/// <summary>
	/// コピー元/コピー先を適切な状態へ遷移し、テクスチャリソースをコピーする。
	/// </summary>
	/// <param name="dstResource">コピー先リソース。</param>
	/// <param name="dstState">コピー前後に維持するコピー先の元状態。</param>
	/// <param name="srcResource">コピー元リソース。</param>
	/// <param name="srcState">コピー前後に維持するコピー元の元状態。</param>
	void CopyTexture(ID3D12Resource* dstResource, D3D12_RESOURCE_STATES dstState,
		ID3D12Resource* srcResource, D3D12_RESOURCE_STATES srcState);

	//--------- accessor -----------------------------------------------------

	/// <summary>
	/// コマンド送信用のDirect3D12コマンドキューを取得する。
	/// </summary>
	/// <returns>内部で保持しているコマンドキュー。</returns>
	ID3D12CommandQueue* GetQueue() const { return commandQueue_.Get(); }
	/// <summary>
	/// 描画コマンドを記録するDirect3D12コマンドリストを取得する。
	/// </summary>
	/// <returns>内部で保持しているコマンドリスト。</returns>
	ID3D12GraphicsCommandList6* GetCommandList() const { return commandList_.Get(); }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	ComPtr<ID3D12GraphicsCommandList6> commandList_;
	ComPtr<ID3D12CommandAllocator> commandAllocator_;

	ComPtr<ID3D12CommandQueue> commandQueue_;

	ComPtr<ID3D12Fence> fence_;
	uint64_t fenceValue_;
	HANDLE fenceEvent_;

	std::chrono::steady_clock::time_point reference_;

	//--------- functions ----------------------------------------------------

	/// <summary>
	/// グラフィックスコマンドリストを閉じてキューへ提出し、スワップチェーンをPresentする。
	/// </summary>
	/// <param name="swapChain">Present対象のスワップチェーン。</param>
	void ExecuteGraphicsCommands(IDXGISwapChain4* swapChain);
	/// <summary>
	/// フェンス値をシグナルし、GPUが指定値に到達するまでイベントで待機する。
	/// </summary>
	void FenceEvent();
	/// <summary>
	/// 固定FPS向けにCPU側の待機と基準時刻の更新を行う。
	/// </summary>
	void UpdateFixFPS();
};

}; // SakuEngine
