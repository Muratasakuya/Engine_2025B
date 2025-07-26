## Default Branch

**main**

## Build Status (`main` branch)

- Debug:   [![DebugBuild](https://github.com/Muratasakuya/Engine_2025/actions/workflows/DebugBuild.yml/badge.svg)](https://github.com/Muratasakuya/Engine_2025/actions/workflows/DebugBuild.yml)
- Release: [![ReleaseBuild](https://github.com/Muratasakuya/Engine_2025/actions/workflows/ReleaseBuild.yml/badge.svg)](https://github.com/Muratasakuya/Engine_2025/actions/workflows/ReleaseBuild.yml)

- ## ⚠️ 注意事項

このプロジェクトでは以下のGPU機能を使用しています：

- **Mesh Shader**
- **DXR 1.1（RayQuery）**

そのため、以下の環境では動作しません：

- **DirectX 12 Ultimate** 非対応のGPU
- **NVIDIA GeForce GTXシリーズ（RTXではない）**
- **古いIntel / AMD GPU**

### ✅ 推奨環境

- **GPU**: NVIDIA RTX 20xx 以降 または AMD RDNA2 以降  
- **OS**: Windows 10 バージョン 2004 以降  
- **ドライバ**: DirectX 12 Ultimate 対応ドライバ
