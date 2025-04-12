#include <Novice.h>
#include <cmath>

const char kWindowTitle[] = "LE2A_09_タケコシケイ_MT3_01_00_確認課題";

static const int kRowHeight = 20;
static const int kColumnWidth = 60;

struct Matrix4x4 {
	float m[4][4];
};

Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);
Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip);
Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth);
void MatrixScreenPrintf(int x, int y, const Matrix4x4& matrix, const char* label);
// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = {0};
	char preKeys[256] = {0};

	//============================================
	// Matrix4x4の変数
	//============================================
	Matrix4x4 orthographicMatrix =
		MakeOrthographicMatrix(-160.0f, 160.0f, 200.0f, 300.0f, 0.0f, 1000.0f);
	Matrix4x4 perspectiveFovMatirx =
		MakePerspectiveFovMatrix(0.63f, 1.33f, 0.1f, 1000.0f);
	Matrix4x4 viewportMatrix =
		MakeViewportMatrix(100.0f, 200.0f, 600.0f, 300.0f, 0.0f, 1.0f);

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

		//==========================================
		// screenPrintf
		//==========================================
		MatrixScreenPrintf(0, 0, orthographicMatrix, "orthographicMatrix");
		MatrixScreenPrintf(0, kRowHeight * 5, perspectiveFovMatirx, "perspectiveFovMatirx");
		MatrixScreenPrintf(0, kRowHeight * 10, viewportMatrix, "viewportMatrix");

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
/// MakePerspectiveFovMatirx関数
/// </summary>
/// <param name="fovY">画角</param>
/// <param name="aspectRaito">アスペクト比</param>
/// <param name="nearClip">近平面への距離</param>
/// <param name="farClip">遠平面への距離</param>
/// <returns>透視射影行列</returns>
Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip) {
	Matrix4x4 perspectiveFovMatirx;

	perspectiveFovMatirx = {
		1.0f / aspectRatio * (std::cos(fovY / 2.0f) / std::sin(fovY / 2.0f)), 0.0f, 0.0f, 0.0f,
		0.0f, (cosf(fovY / 2.0f) / sinf(fovY / 2.0f)), 0.0f, 0.0f,
		0.0f, 0.0f, farClip / (farClip - nearClip), 1.0f,
		0.0f, 0.0f, -nearClip * farClip / (farClip - nearClip), 0.0f
	};

	return perspectiveFovMatirx;
}

/// <summary>
/// MakeOrthographicMatrix関数
/// </summary>
/// <param name="left">left</param>
/// <param name="top">top</param>
/// <param name="right">right</param>
/// <param name="bottom">bottom</param>
/// <param name="nearClip">nearClip</param>
/// <param name="farClip">farClip</param>
/// <returns>3次元正射影行列</returns>
Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip) {
	Matrix4x4 orthographicMatrix;

	orthographicMatrix.m[0][0] = 2.0f / (right - left);
	orthographicMatrix.m[0][1] = 0.0f;
	orthographicMatrix.m[0][2] = 0.0f;
	orthographicMatrix.m[0][3] = 0.0f;

	orthographicMatrix.m[1][0] = 0.0f;
	orthographicMatrix.m[1][1] = 2.0f / (top - bottom);
	orthographicMatrix.m[1][2] = 0.0f;
	orthographicMatrix.m[1][3] = 0.0f;

	orthographicMatrix.m[2][0] = 0.0f;
	orthographicMatrix.m[2][1] = 0.0f;
	orthographicMatrix.m[2][2] = 1.0f / (farClip - nearClip);
	orthographicMatrix.m[2][3] = 0.0f;

	orthographicMatrix.m[3][0] = (left + right) / (left - right);
	orthographicMatrix.m[3][1] = (top + bottom) / (bottom - top);
	orthographicMatrix.m[3][2] = (nearClip) / (nearClip - farClip);
	orthographicMatrix.m[3][3] = 1.0f;

	return orthographicMatrix;
}

Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth) {
	Matrix4x4 viewportMatrix;
	viewportMatrix = {
		width / 2.0f, 0.0f, 0.0f, 0.0f,
		0.0f, -(height / 2.0f), 0.0f, 0.0f,
		0.0f, 0.0f, maxDepth - minDepth, 0.0f,
		left + (width / 2.0f), top + (height / 2.0f), minDepth, 1.0f
	};

	return viewportMatrix;
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
			Novice::ScreenPrintf(x + column * kColumnWidth, y + 24 + row * kRowHeight, "%6.02f", matrix.m[row][column]);
		}
	}
	Novice::ScreenPrintf(x + 24, y, "%s", label);
}