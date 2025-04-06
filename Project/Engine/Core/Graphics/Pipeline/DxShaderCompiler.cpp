#include "DxShaderCompiler.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Debug/Assert.h>
#include <Engine/Asset/Filesystem.h>

//============================================================================
//	DxShaderCompiler classMethods
//============================================================================

void DxShaderCompiler::Init() {

	dxcUtils_ = nullptr;
	dxcCompiler_ = nullptr;
	HRESULT hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils_));
	assert(SUCCEEDED(hr));
	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler_));

	includeHandler_ = nullptr;
	hr = dxcUtils_->CreateDefaultIncludeHandler(&includeHandler_);
	assert(SUCCEEDED(hr));
}

void DxShaderCompiler::Compile(const Json& json, std::vector<ComPtr<IDxcBlob>>& shaderBlobs) {

	// baseShaderPath
	const fs::path basePath = "./Assets/Engine/Shaders/";
	fs::path fullPath;

	for (const auto& shaderPass : json["ShaderPass"]) {
		if (shaderPass.contains("Type")) {

			std::string type = shaderPass["Type"];
			if (type == "Graphics") {

				// vertexShaderのcompile
				if (shaderPass.contains("VertexShader")) {

					std::string vertexShader = shaderPass["VertexShader"];
					if (Filesystem::Found(basePath, vertexShader, fullPath)) {
						ComPtr<IDxcBlob> vertexShaderBlob;
						CompileShader(fullPath.wstring(), L"vs_6_0", vertexShaderBlob);
						shaderBlobs.emplace_back(vertexShaderBlob);
					} else {

						ASSERT(false, "Failed to find HLSL file: " + vertexShader);
					}
				}

				// meshShaderのcompile
				if (shaderPass.contains("MeshShader")) {

					std::string meshShader = shaderPass["MeshShader"];
					if (Filesystem::Found(basePath, meshShader, fullPath)) {

						ComPtr<IDxcBlob> meshShaderBlob;
						CompileShader(fullPath.wstring(), L"ms_6_5", meshShaderBlob);
						shaderBlobs.emplace_back(meshShaderBlob);
					} else {

						ASSERT(false, "Failed to find HLSL file: " + meshShader);
					}
				}

				// pixelShaderのcompile
				if (shaderPass.contains("PixelShader")) {

					std::string pixelShader = shaderPass["PixelShader"];
					if (Filesystem::Found(basePath, pixelShader, fullPath)) {

						ComPtr<IDxcBlob> pixelShaderBlob;
						CompileShader(fullPath.wstring(), L"ps_6_0", pixelShaderBlob);
						shaderBlobs.emplace_back(pixelShaderBlob);
					} else {

						ASSERT(false, "Failed to find HLSL file: " + pixelShader);
					}
				}
			} else if (type == "Compute" && shaderPass.contains("ComputeShader")) {

				// computeShaderのcompile
				std::string computeShader = shaderPass["ComputeShader"];
				if (Filesystem::Found(basePath, computeShader, fullPath)) {

					ComPtr<IDxcBlob> computeShaderBlob;
					CompileShader(fullPath.wstring(), L"cs_6_0", computeShaderBlob);
					shaderBlobs.push_back(computeShaderBlob);
				} else {

					ASSERT(false, "Failed to find HLSL file: " + computeShader);
				}
			}
		}
	}
}

void DxShaderCompiler::CompileShader(
	const std::wstring& filePath, const wchar_t* profile,
	ComPtr<IDxcBlob>& shaderBlob) {

	// hlslファイルを読み込む
	IDxcBlobEncoding* shaderSouce = nullptr;
	HRESULT hr = dxcUtils_->LoadFile(filePath.c_str(), nullptr, &shaderSouce);
	// 読めなかったら止める
	assert(SUCCEEDED(hr));
	// 読み込んだファイルの内容を設定する
	DxcBuffer shaderSourceBuffer;
	shaderSourceBuffer.Ptr = shaderSouce->GetBufferPointer();
	shaderSourceBuffer.Size = shaderSouce->GetBufferSize();
	// UTF8の文字コードであることを通知
	shaderSourceBuffer.Encoding = DXC_CP_UTF8;

	LPCWSTR arguments[] = {
		filePath.c_str(),
		L"-E",L"main",
		L"-T",profile,
		L"-Zi",L"-Qembed_debug",
		L"-Od",
		L"-Zpr",
	};
	IDxcResult* shaderResult = nullptr;
	hr = dxcCompiler_->Compile(
		&shaderSourceBuffer,
		arguments,
		_countof(arguments),
		includeHandler_.Get(),
		IID_PPV_ARGS(&shaderResult));
	// コンパイルエラーではなくdxcが起動できないなど致命的な状況
	assert(SUCCEEDED(hr));

	ComPtr<IDxcBlobUtf8> shaderError = nullptr;

	// エラー情報を取得
	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);

	if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
		// エラーの内容
		const char* errorMessage = reinterpret_cast<const char*>(shaderError->GetBufferPointer());
		errorMessage;
		assert(false);
	}

	hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
	assert(SUCCEEDED(hr));
	// もう使わないリソースを解放
	shaderSouce->Release();
	shaderResult->Release();
	shaderError.Reset();
}