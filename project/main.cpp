#include <Novice.h>
#include <cmath>
#include <iostream>

const char kWindowTitle[] = "LE2B_12_ソネ_タイセイ_タイトル";

// ==========================================
//  既存の構造体など
// ==========================================

struct Vector3 {
	float x, y, z;
};

struct Matrix3x3 {
	float m[3][3];
};

struct Matrix4x4 {
	float m[4][4];
};

// ==========================================
//  ここから追加：Quaternion構造体と関数
// ==========================================

struct Quaternion {
	float x;
	float y;
	float z;
	float w;
};

// 1. Quaternionの積
Quaternion Multiply (const Quaternion &lhs, const Quaternion &rhs) {
	Quaternion result;
	// 実部 (w)
	result.w = lhs.w * rhs.w - lhs.x * rhs.x - lhs.y * rhs.y - lhs.z * rhs.z;
	// 虚部 (x, y, z)
	result.x = lhs.w * rhs.x + lhs.x * rhs.w + lhs.y * rhs.z - lhs.z * rhs.y;
	result.y = lhs.w * rhs.y - lhs.x * rhs.z + lhs.y * rhs.w + lhs.z * rhs.x;
	result.z = lhs.w * rhs.z + lhs.x * rhs.y - lhs.y * rhs.x + lhs.z * rhs.w;
	return result;
}

// 2. 単位Quaternionを返す
Quaternion IdentityQuaternion () {
	return { 0.0f, 0.0f, 0.0f, 1.0f };
}

// 3. 共役Quaternionを返す
Quaternion Conjugate (const Quaternion &quaternion) {
	return { -quaternion.x, -quaternion.y, -quaternion.z, quaternion.w };
}

// 4. Quaternionのnormを返す
float Norm (const Quaternion &quaternion) {
	return std::sqrt (
		quaternion.x * quaternion.x +
		quaternion.y * quaternion.y +
		quaternion.z * quaternion.z +
		quaternion.w * quaternion.w
	);
}

// 5. 正規化したQuaternionを返す
Quaternion Normalize (const Quaternion &quaternion) {
	float len = Norm (quaternion);
	if (len != 0.0f) {
		return { quaternion.x / len, quaternion.y / len, quaternion.z / len, quaternion.w / len };
	}
	return quaternion;
}

// 6. 逆Quaternionを返す
Quaternion Inverse (const Quaternion &quaternion) {
	// 逆Q = 共役Q / Norm^2
	Quaternion conj = Conjugate (quaternion);
	float len = Norm (quaternion);
	float lenSq = len * len;

	if (lenSq != 0.0f) {
		return { conj.x / lenSq, conj.y / lenSq, conj.z / lenSq, conj.w / lenSq };
	}
	return quaternion; // 0除算対策
}

// 画面表示用のヘルパー関数（課題の出力形式に合わせる）
void QuaternionScreenPrintf (int x, int y, const Quaternion &q, const char *label) {
	Novice::ScreenPrintf (x, y, "%6.2f  %6.2f  %6.2f  %6.2f  : %s", q.x, q.y, q.z, q.w, label);
}


// ==========================================
//  Main関数
// ==========================================

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain (HINSTANCE, HINSTANCE, LPSTR, int) {
	Novice::Initialize (kWindowTitle, 1280, 720);

	// =====================
	// 初期化と計算
	// =====================

	// 課題で指定された値
	Quaternion q1 = { 2.0f, 3.0f, 4.0f, 1.0f };
	Quaternion q2 = { 1.0f, 3.0f, 5.0f, 2.0f };

	// 各関数の動作確認
	Quaternion identity = IdentityQuaternion ();
	Quaternion conj = Conjugate (q1);
	Quaternion inv = Inverse (q1);
	Quaternion normal = Normalize (q1);
	Quaternion mul1 = Multiply (q1, q2);
	Quaternion mul2 = Multiply (q2, q1);

	float norm = Norm (q1);

	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	while (Novice::ProcessMessage () == 0) {
		Novice::BeginFrame ();
		memcpy (preKeys, keys, 256);
		Novice::GetHitKeyStateAll (keys);

		///
		/// ↓更新処理ここから
		///

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		// 実行結果の表示 (行間を20pxとして表示)
		int startX = 10;
		int startY = 10;
		int stepY = 20;

		QuaternionScreenPrintf (startX, startY + stepY * 0, identity, "Identity");
		QuaternionScreenPrintf (startX, startY + stepY * 1, conj, "Conjugate");
		QuaternionScreenPrintf (startX, startY + stepY * 2, inv, "Inverse");
		QuaternionScreenPrintf (startX, startY + stepY * 3, normal, "Normalize");
		QuaternionScreenPrintf (startX, startY + stepY * 4, mul1, "Multiply(q1, q2)");
		QuaternionScreenPrintf (startX, startY + stepY * 5, mul2, "Multiply(q2, q1)");

		// float値だけ形式が違うので個別に表示
		Novice::ScreenPrintf (startX, startY + stepY * 6, "%6.2f                       : Norm", norm);


		///
		/// ↑描画処理ここまで
		///

		Novice::EndFrame ();

		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	Novice::Finalize ();
	return 0;
}