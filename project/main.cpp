#include <Novice.h>
#include <cmath>
#include <iostream>

const char kWindowTitle[] = "LE2B_12_ソネ_タイセイ_タイトル";

struct Vector3 {
	float x, y, z;
};

struct Matrix3x3 {
	float m[3][3];
};

struct Matrix4x4 {
	float m[4][4];
};

float Length(const Vector3 &v) {
	return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

// ベクトルを正規化する関数（長さ1にする）
Vector3 Normalize(const Vector3 &v) {
	float len = Length(v);
	if(len != 0.0f) {
		return { v.x / len, v.y / len, v.z / len };
	}
	return v;
}

Matrix4x4 MakeRotateAxisAngle(const Vector3 &axis, float angle) {
	float c = std::cos(angle);
	float s = std::sin(angle);
	float t = 1.0f - c;

	// 軸成分（正規化されている前提ですが、安全のためローカル変数で扱うのが一般的です）
	float x = axis.x;
	float y = axis.y;
	float z = axis.z;

	Matrix4x4 result = {
		t * x * x + c,      t * x * y + z * s,  t * x * z - y * s,  0.0f,
		t * x * y - z * s,  t * y * y + c,      t * y * z + x * s,  0.0f,
		t * x * z + y * s,  t * y * z - x * s,  t * z * z + c,      0.0f,
		0.0f,               0.0f,               0.0f,               1.0f
	};

	return result;
}

static const int kRowHeight = 20;
void MatrixScreenPrintf(int x, int y, const Matrix4x4 &mat, const char *label) {
	// タイトル
	Novice::ScreenPrintf(x, y, "%s", label);

	// 行列の数値を表示
	for(int i = 0; i < 4; ++i) {
		Novice::ScreenPrintf(
			x,
			y + (i + 1) * kRowHeight, // Y座標を1行ずつずらす
			"%6.3f %6.3f %6.3f %6.3f",
			mat.m[i][0], mat.m[i][1], mat.m[i][2], mat.m[i][3]
		);
	}
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	//=====================
	// 初期化
	//=====================

	Vector3 inputAxis = { 1.0f, 1.0f, 1.0f };
	Vector3 axis = Normalize(inputAxis);

	float angle = 0.44f;

	Matrix4x4 rotateMatrix = MakeRotateAxisAngle(axis, angle);

	// キー入力結果を受け取る箱
	char keys[256] = {0};
	char preKeys[256] = {0};

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		///
		/// ↓更新処理ここから
		///

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///
		MatrixScreenPrintf(0, 0, rotateMatrix, "rotateMatrix");
		///
		/// ↑描画処理ここまで
		///

		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}
