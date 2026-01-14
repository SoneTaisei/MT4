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

float Length (const Vector3 &v) {
	return std::sqrt (v.x * v.x + v.y * v.y + v.z * v.z);
}

// 5. 正規化したQuaternionを返す
Quaternion Normalize (const Quaternion &quaternion) {
	float len = Norm (quaternion);
	if (len != 0.0f) {
		return { quaternion.x / len, quaternion.y / len, quaternion.z / len, quaternion.w / len };
	}
	return quaternion;
}

Vector3 Normalize (const Vector3 &v) {
	float len = Length (v);
	if (len != 0.0f) {
		return { v.x / len, v.y / len, v.z / len };
	}
	return v;
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

Vector3 Transform (const Vector3 &vector, const Matrix4x4 &matrix) {
	Vector3 result;
	// w成分は1として計算 (直積)
	result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + 1.0f * matrix.m[3][0];
	result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + 1.0f * matrix.m[3][1];
	result.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + 1.0f * matrix.m[3][2];
	float w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + 1.0f * matrix.m[3][3];

	// wで割る (パースペクティブ補正が必要な場合。今回は回転だけなので実質w=1ですが念のため)
	if (w != 0.0f) {
		result.x /= w;
		result.y /= w;
		result.z /= w;
	}
	return result;
}

Quaternion MakeRotateAxisAngleQuaternion (const Vector3 &axis, float angle) {
	Quaternion result{};

	float sinHalf = std::sin (angle / 2.0f);
	float cosHalf = std::cos (angle / 2.0f);

	result.x = axis.x * sinHalf;
	result.y = axis.y * sinHalf;
	result.z = axis.z * sinHalf;
	result.w = cosHalf;

	return result;
}

Vector3 RotateVector (const Vector3 &vector, const Quaternion &quaternion) {
	// 1. クォータニオンのベクトル部分 (x, y, z) を取り出す
	Vector3 qVec = { quaternion.x, quaternion.y, quaternion.z };

	// 2. Cross (外積) を計算するための準備
	// もし Vector3 クラスに Cross 関数があればそれを使ってください。
	// ここでは念のため計算式を書いておきます。
	auto Cross = [](const Vector3 &a, const Vector3 &b) -> Vector3 {
		return {
			a.y * b.z - a.z * b.y,
			a.z * b.x - a.x * b.z,
			a.x * b.y - a.y * b.x
		};
		};

		// 3. 回転の計算（スライドの qrq* を効率よく展開した式）
		// t = 2 * cross(qVec, v)
	Vector3 t = Cross (qVec, vector);
	t.x *= 2.0f;
	t.y *= 2.0f;
	t.z *= 2.0f;

	// result = v + q.w * t + cross(qVec, t)
	Vector3 result;
	Vector3 crossQT = Cross (qVec, t);

	result.x = vector.x + (quaternion.w * t.x) + crossQT.x;
	result.y = vector.y + (quaternion.w * t.y) + crossQT.y;
	result.z = vector.z + (quaternion.w * t.z) + crossQT.z;

	return result;
}

Matrix4x4 MakeRotateMatrix (const Quaternion &quaternion) {
	Matrix4x4 result{};

	Quaternion q = quaternion;

	// 正規化
	float norm = std::sqrt (q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
	if (norm > 0.0f) {
		float invNorm = 1.0f / norm;
		q.x *= invNorm; q.y *= invNorm; q.z *= invNorm; q.w *= invNorm;
	}

	result.m[0][0] = q.w * q.w + q.x * q.x - q.y * q.y - q.z * q.z;

	result.m[0][1] = 2 * (q.x * q.y + q.w * q.z);
	result.m[1][0] = 2 * (q.x * q.y - q.w * q.z);

	result.m[0][2] = 2 * (q.x * q.z - q.w * q.y);
	result.m[2][0] = 2 * (q.x * q.z + q.w * q.y);

	result.m[1][1] = q.w * q.w - q.x * q.x + q.y * q.y - q.z * q.z;

	result.m[1][2] = 2 * (q.y * q.z + q.w * q.x);
	result.m[2][1] = 2 * (q.y * q.z - q.w * q.x);

	result.m[2][2] = q.w * q.w - q.x * q.x - q.y * q.y + q.z * q.z;

	result.m[3][0] = 0;
	result.m[3][1] = 0;
	result.m[3][2] = 0;

	result.m[0][3] = 0;
	result.m[1][3] = 0;
	result.m[2][3] = 0;
	result.m[3][3] = 1;

	return result;
}

// 画面表示用のヘルパー関数（課題の出力形式に合わせる）
void QuaternionScreenPrintf (int x, int y, const Quaternion &q, const char *label) {
	// ここは %.2f のまま (画像: 0.20, 0.08...)
	Novice::ScreenPrintf (x, y, "%6.2f  %6.2f  %6.2f  %6.2f  : %s", q.x, q.y, q.z, q.w, label);
}

void VectorScreenPrintf (int x, int y, const Vector3 &v, const char *label) {
	// ここも %.2f のまま (画像: 2.15, -1.44...)
	Novice::ScreenPrintf (x, y, "%6.2f  %6.2f  %6.2f  : %s", v.x, v.y, v.z, label);
}

void MatrixScreenPrintf (int x, int y, const Matrix4x4 &matrix, const char *label) {
	Novice::ScreenPrintf (x, y, "%s", label);
	for (int row = 0; row < 4; ++row) {
		// ここだけ %.3f に変更！ (画像: 0.983, -0.046...)
		Novice::ScreenPrintf (x, y + 20 * (row + 1), "%6.3f  %6.3f  %6.3f  %6.3f",
							  matrix.m[row][0], matrix.m[row][1], matrix.m[row][2], matrix.m[row][3]);
	}
}

// ==========================================
//  Main関数
// ==========================================

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain (HINSTANCE, HINSTANCE, LPSTR, int) {
	Novice::Initialize (kWindowTitle, 1280, 720);

	const int kRowHeight = 20;

	// =====================
	// 初期化と計算
	// =====================

	// スクリーンショット通りの計算処理
	Quaternion rotation = MakeRotateAxisAngleQuaternion (
		Normalize (Vector3{ 1.0f, 0.4f, -0.2f }), 0.45f
	);

	Vector3 pointY = { 2.1f, -0.9f, 1.3f };

	Matrix4x4 rotateMatrix = MakeRotateMatrix (rotation);

	Vector3 rotateByQuaternion = RotateVector (pointY, rotation);
	Vector3 rotateByMatrix = Transform (pointY, rotateMatrix);

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
		QuaternionScreenPrintf (0, kRowHeight * 0, rotation, "rotation");
		MatrixScreenPrintf (0, kRowHeight * 1, rotateMatrix, "rotateMatrix");
		VectorScreenPrintf (0, kRowHeight * 6, rotateByQuaternion, "rotateByQuaternion");
		VectorScreenPrintf (0, kRowHeight * 7, rotateByMatrix, "rotateByMatrix");


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