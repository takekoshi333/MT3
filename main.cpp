#include <Novice.h>
#include <cmath>

const char kWindowTitle[] = "LE2A_09_タケコシケイ_MT3_00_04_確認課題";

static const int kRowHeight = 20;
static const int kColumnWidth = 60;

struct Vector3 {
	float x;
	float y;
	float z;
};

struct Matrix4x4 {
	float m[4][4];
};

Matrix4x4 MakeRotateXMatrix(float radian);
Matrix4x4 MakeRotateYMatrix(float radian);
Matrix4x4 MakeRotateZMatrix(float radian);
Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2);
void MatrixScreenPrintf(int x, int y, const Matrix4x4& matrix, const char* label);

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	//===========================================
	// Vector3の変数 
	//===========================================
	Vector3 rotate = { 0.4f, 1.43f, -0.8f };

	//===========================================
	// Matrix4x4の変数 
	//===========================================
	Matrix4x4 rotateXMatrix = MakeRotateXMatrix(rotate.x);
	Matrix4x4 rotateYMatrix = MakeRotateYMatrix(rotate.y);
	Matrix4x4 rotateZMatrix = MakeRotateZMatrix(rotate.z);
	Matrix4x4 rotateXYZMatrix = Multiply(rotateXMatrix, Multiply(rotateYMatrix, rotateZMatrix));

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

		//========================================
		// screenPrintf
		//========================================
		MatrixScreenPrintf(0, 0, rotateXMatrix, "rotateXMatrix");
		MatrixScreenPrintf(0, kRowHeight * 5, rotateYMatrix, "rotateYMatrix");
		MatrixScreenPrintf(0, kRowHeight * 5 * 2, rotateZMatrix, "rotateZMatrix");
		MatrixScreenPrintf(0, kRowHeight * 5 * 3, rotateXYZMatrix, "rotateXYZMatrix");

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

/// <summary>
/// x軸の回転行列を作成
/// </summary>
/// <param name="radian">角度</param>
/// <returns>x軸の回転行列</returns>
Matrix4x4 MakeRotateXMatrix(float radian) {
	Matrix4x4 rotateXMatrix;
	rotateXMatrix = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, std::cos(radian), std::sin(radian), 0.0f,
		0.0f, std::sin(-radian), std::cos(radian), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	return rotateXMatrix;
}

/// <summary>
/// y軸の回転行列を作成
/// </summary>
/// <param name="radian">角度</param>
/// <returns>y軸の回転行列</returns>
Matrix4x4 MakeRotateYMatrix(float radian) {
	Matrix4x4 rotateYMatrix;
	rotateYMatrix = {
		std::cos(radian), 0.0f, std::sin(-radian), 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		std::sin(radian), 0.0f, std::cos(radian), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	return rotateYMatrix;
}

/// <summary>
/// z軸の回転行列を作成
/// </summary>
/// <param name="radian">角度</param>
/// <returns>z軸の回転行列</returns>
Matrix4x4 MakeRotateZMatrix(float radian) {
	Matrix4x4 rotateZMatrix;
	rotateZMatrix = {
		std::cos(radian), std::sin(radian), 0.0f, 0.0f,
		std::sin(-radian), std::cos(radian), 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	return rotateZMatrix;
}

/// <summary>
/// 4x4行列の乗算
/// </summary>
/// <param name="matrix1">4x4行列 1</param>
/// <param name="matrix2">4x4行列 2</param>
/// <returns>4x4行列の乗算</returns>
Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 multiplyMatrix;
	float multiply[4];

	for (int row = 0; row < 4; row++) {
		for (int column = 0; column < 4; column++) {
			for (int i = 0; i < 4; i++) {
				multiply[i] = m1.m[row][i] * m2.m[i][column];
			}
			multiplyMatrix.m[row][column] = multiply[0] + multiply[1] + multiply[2] + multiply[3];
		}
	}

	return multiplyMatrix;
}

/// <summary>
/// 4x4行列の値をっ表示
/// </summary>
/// <param name="x">x座標</param>
/// <param name="y">y座標</param>
/// <param name="matrix">表示する行列</param>
/// <param name="label">ラベル</param>
void MatrixScreenPrintf(int x, int y, const Matrix4x4& matrix, const char* label) {
	for (int row = 0; row < 4; ++row) {
		for (int column = 0; column < 4; ++column) {
			Novice::ScreenPrintf(x + column * kColumnWidth, y + row * kRowHeight, "%6.02f", matrix.m[row][column]);
		}
	}
	Novice::ScreenPrintf(x + kColumnWidth * 4, y, "%s", label);
}