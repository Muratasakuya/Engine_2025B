#pragma once

//============================================================================
//	include
//============================================================================

// 設計図
// emitterが複数のparticleを所持する
// emitterはtransformを持つ
// emitterは存在しているemitterを親に設定できるようにする
// 各particleに必要な情報
// mesh...       各particleにつき1つ持たせる
// texture...    particle内で別々のtextureを持たせられる、とりあえず同じでいい
// material...   PSの構造体と同じものを持たせる -> structuredBuffer
// transform...  euler角のtransformを持たせる -> structuredBuffer
// numInstance...各particleごとのinstance数、これをinstance数としてmeshに渡して描画する
// 上記がbufferに必要なデータ、各particleデータごとに必要
// 上記のbufferの値を決めるのがparameter
// parameterは各particleにつき1つ持たせる
// このparameterはeditorで操作可能にする
// parameterの値でparticleをcreaterクラスで作成、
// 作成されたparticleをupdaterクラスで更新
// 更新処理にも種類がある、ここからは学校行きながら考える
// 更新処理はsystemに渡して行う
// emitterはparticleを所持するだけのクラスにする
// systemはemittersを所持するクラスにする

//============================================================================
//	ParticleEmitter class
//============================================================================
class ParticleEmitter {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	ParticleEmitter() = default;
	~ParticleEmitter() = default;

	//--------- accessor -----------------------------------------------------

private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------



	//--------- functions ----------------------------------------------------

};