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

// 内積（Dot Product）
float Dot (const Vector3 &v1, const Vector3 &v2) {
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

// 外積（Cross Product）
Vector3 Cross (const Vector3 &v1, const Vector3 &v2) {
	return {
		v1.y * v2.z - v1.z * v2.y,
		v1.z * v2.x - v1.x * v2.z,
		v1.x * v2.y - v1.y * v2.x
	};
}

// 単位行列を作成する関数
Matrix4x4 MakeIdentity4x4 () {
	return {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
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

// ある方向からある方向への回転行列
Matrix4x4 DirectionToDirection (const Vector3 &from, const Vector3 &to) {
	Vector3 u = Normalize (from);
	Vector3 v = Normalize (to);

	// 1. 内積(Dot)で cosθ を求める
	float dot = Dot (u, v);

	// 2. 外積(Cross)で回転軸の候補を求める
	Vector3 cross = Cross (u, v);

	// 3. 状況に応じて処理を分岐

	// ケースA: 向きが同じ場合 (cosθ が 1 に近い)
	// 回転する必要がないので、そのまま単位行列を返す
	if (dot >= 1.0f - 1e-5f) {
		return MakeIdentity4x4 ();
	}

	// ケースB: 向きが真逆の場合 (cosθ が -1 に近い)
	// 真逆だと外積がゼロベクトル(0,0,0)になり軸が求まらないため、
	// 垂直なベクトルを自作して軸にする
	else if (dot <= -1.0f + 1e-5f) {
		Vector3 axis = { 0.0f, 0.0f, 0.0f };

		// X成分、Y成分の状況を見て、垂直になるようなベクトルを作り出す
		if (u.x != 0.0f || u.y != 0.0f) {
			axis = { u.y, -u.x, 0.0f };
		} else {
			axis = { u.z, 0.0f, -u.x };
		}
		axis = Normalize (axis);

		// その軸で180度(π)回転させる
		return MakeRotateAxisAngle (axis, 3.14159265f);
	}

	// ケースC: 通常の回転
	// 求めた外積を軸に、内積から角度(acos)を算出して回転行列を作る
	Vector3 axis = Normalize (cross);
	float theta = std::acos (dot);

	return MakeRotateAxisAngle (axis, theta);
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
int WINAPI WinMain (HINSTANCE, HINSTANCE, LPSTR, int) {
	Novice::Initialize (kWindowTitle, 1280, 720);

	//=====================
	// 初期化
	//=====================

	// 課題の「完成イメージ」通りの変数を準備
	Vector3 from0 = Normalize (Vector3{ 1.0f, 0.7f, 0.5f });
	Vector3 to0 = { -from0.x, -from0.y, -from0.z }; // 真逆のベクトルを作る

	Vector3 from1 = Normalize (Vector3{ -0.6f, 0.9f, 0.2f });
	Vector3 to1 = Normalize (Vector3{ 0.4f, 0.7f, -0.5f });

	// 計算実行
	// 1. X軸(1,0,0) を 真逆(-1,0,0) にする回転 (ケースBのテスト)
	Matrix4x4 rotateMatrix0 = DirectionToDirection (
		Normalize (Vector3{ 1.0f, 0.0f, 0.0f }), Normalize (Vector3{ -1.0f, 0.0f, 0.0f }));

	// 2. 任意のベクトルを 真逆 にする回転 (ケースBのテスト)
	Matrix4x4 rotateMatrix1 = DirectionToDirection (from0, to0);

	// 3. 通常の回転 (ケースCのテスト)
	Matrix4x4 rotateMatrix2 = DirectionToDirection (from1, to1);


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

		// 結果を表示
		MatrixScreenPrintf (0, 0, rotateMatrix0, "rotateMatrix0");
		MatrixScreenPrintf (0, kRowHeight * 5, rotateMatrix1, "rotateMatrix1"); // 5行分あける
		MatrixScreenPrintf (0, kRowHeight * 10, rotateMatrix2, "rotateMatrix2"); // さらに5行分あける

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
