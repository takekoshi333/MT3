#include <Novice.h>
#include <cmath>
#include <assert.h>

const char kWindowTitle[] = "LE2A_09_タケコシケイ_MT3_01_01_確認課題";

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

Vector3 Cross(const Vector3& v1, const Vector3& v2);
Matrix4x4 MakeIdentity4x4();
float Determinant3x3(const Matrix4x4& m, int row, int col);
Matrix4x4 Inverse(const Matrix4x4& m);
Matrix4x4 Multiply(Matrix4x4 matrix1, Matrix4x4 matrix2);
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);
void VectorScreenPrintf(int x, int y, const Vector3& vector, const char* label);
Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix);
Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);
Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth);

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = {0};
	char preKeys[256] = {0};

	//=========================================
	// Vector3の変数
	//=========================================
	Vector3 v1 = { 1.2f, -3.9f, 2.5f };
	Vector3 v2 = { 2.8f, 0.4f, -1.3f };
	Vector3 cross = Cross(v1, v2);
	Vector3 rotate = { 0.0f, 0.0f, 0.0f };
	Vector3 translate = { 0.0f, 0.0f, 0.0f };
	Vector3 cameraTranslate = { 0.0f, 0.0f, -5.0f };
	Vector3 kLocalVertices[3];
	kLocalVertices[0] = { 0.0f, 0.5f, 0.0f };
	kLocalVertices[1] = { 0.5f, -0.5f, 0.0f };
	kLocalVertices[2] = { -0.5f, -0.5f, 0.0f };
	Vector3 screenVertices[3];
	Vector3 ndcVertex;

	//=========================================
	// Matrix4x4の変数
	//=========================================
	Matrix4x4 worldMatrix = MakeAffineMatrix({ 1.0f, 1.0f, 1.0f }, rotate, translate);
	Matrix4x4 cameraMatrix = MakeAffineMatrix({ 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, cameraTranslate);
	Matrix4x4 viewMatrix = Inverse(cameraMatrix);
	Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, 1280.0f / 720.0f, 0.1f, 100.0f);
	Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
	Matrix4x4 viewportMatrix = MakeViewportMatrix(0.0f, 0.0f, 1280.0f, 720.0f, 0.0f, 1.0f);

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

		//======================================
		// 三角形の更新処理
		//======================================
		// y軸の回転
		rotate.y += 0.03f;

		// 左右移動
		if (keys[DIK_A]) {
			translate.x -= 0.03f;
		} else {
			if (keys[DIK_D]) {
				translate.x += 0.03f;
			}
		}

		// 前後移動
		if (keys[DIK_W]) {
			translate.z += 0.03f;
		} else {
			if (keys[DIK_S]) {
				translate.z -= 0.03f;
			}
		}

		//======================================
		// 各行列の更新処理
		//======================================
		worldMatrix = MakeAffineMatrix({ 1.0f, 1.0f, 1.0f }, rotate, translate);
		cameraMatrix = MakeAffineMatrix({ 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, cameraTranslate);
		viewMatrix = Inverse(cameraMatrix);
		projectionMatrix = MakePerspectiveFovMatrix(0.45f, 1280.0f / 720.0f, 0.1f, 100.0f);
		worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
		viewportMatrix = MakeViewportMatrix(0.0f, 0.0f, 1280.0f, 720.0f, 0.0f, 1.0f);
		for (uint32_t i = 0; i < 3; ++i) {
			ndcVertex = Transform(kLocalVertices[i], worldViewProjectionMatrix);
			screenVertices[i] = Transform(ndcVertex, viewportMatrix);
		}

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		//======================================
		// screenPrintf
		//======================================
		VectorScreenPrintf(0, 0, cross, "Cross");

		//======================================
		// 三角形
		//======================================
		Novice::DrawTriangle(
			static_cast<int>(screenVertices[0].x), static_cast<int>(screenVertices[0].y),
			static_cast<int>(screenVertices[1].x), static_cast<int>(screenVertices[1].y),
			static_cast<int>(screenVertices[2].x), static_cast<int>(screenVertices[2].y),
			0xFF0000FF, kFillModeSolid
		);

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
/// crossProduct関数
/// </summary>
/// <param name="v1">三次元ベクトルv1</param>
/// <param name="v2">三次元ベクトルv2</param>
/// <returns>crossProduct</returns>
Vector3 Cross(const Vector3& v1, const Vector3& v2) {
	Vector3 crossProduct;
	crossProduct = { v1.y * v2.z - v1.z * v2.y,
		v1.z * v2.x - v1.x * v2.z,
		v1.x * v2.y - v1.y * v2.x };

	return crossProduct;
}

// <summary>
/// 3次元ベクトルを同次座標に変換
/// </summary>
/// <param name="vector">3次元ベクトル</param>
/// <param name="matrix">4x4の行列</param>
/// <returns>変換後の3次元ベクトル</returns>
Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix) {
	Vector3 result;
	result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + 1.0f * matrix.m[3][0];
	result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + 1.0f * matrix.m[3][1];
	result.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + 1.0f * matrix.m[3][2];
	float w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + 1.0f * matrix.m[3][3];
	assert(w != 0.0f);
	result.x /= w;
	result.y /= w;
	result.z /= w;
	return result;
}

/// <summary>
/// 4x4の単位行列の作成
/// </summary>
/// <returns>4x4の単位行列</returns>
Matrix4x4 MakeIdentity4x4(){
	Matrix4x4 identityMatrix;

	identityMatrix.m[0][0] = 1.0f;
	identityMatrix.m[0][1] = 0.0f;
	identityMatrix.m[0][2] = 0.0f;
	identityMatrix.m[0][3] = 0.0f;

	identityMatrix.m[1][0] = 0.0f;
	identityMatrix.m[1][1] = 1.0f;
	identityMatrix.m[1][2] = 0.0f;
	identityMatrix.m[1][3] = 0.0f;

	identityMatrix.m[2][0] = 0.0f;
	identityMatrix.m[2][1] = 0.0f;
	identityMatrix.m[2][2] = 1.0f;
	identityMatrix.m[2][3] = 0.0f;

	identityMatrix.m[3][0] = 0.0f;
	identityMatrix.m[3][1] = 0.0f;
	identityMatrix.m[3][2] = 0.0f;
	identityMatrix.m[3][3] = 1.0f;

	return identityMatrix;
}

/// <summary>
/// 3x3の行列式を計算する関数
/// </summary>
/// <param name="m">元の4x4行列</param>
/// <param name="row">対象の行</param>
/// <param name="col">対象の列</param>
/// <returns>3x3行列の行列式</returns>
float Determinant3x3(const Matrix4x4& m, int row, int col) {
	// 3x3の部分行列を取得
	float sub[3][3];
	int subRow = 0;
	for (int i = 0; i < 4; i++) {
		if (i == row) continue;
		int subCol = 0;
		for (int j = 0; j < 4; j++) {
			if (j == col) continue;
			sub[subRow][subCol] = m.m[i][j];
			subCol++;
		}
		subRow++;
	}

	// 3x3の行列式を計算
	return sub[0][0] * (sub[1][1] * sub[2][2] - sub[1][2] * sub[2][1])
		- sub[0][1] * (sub[1][0] * sub[2][2] - sub[1][2] * sub[2][0])
		+ sub[0][2] * (sub[1][0] * sub[2][1] - sub[1][1] * sub[2][0]);
}

/// <summary>
/// 4x4の逆行列の作成
/// </summary>
/// <param name="m">元の行列</param>
/// <returns>逆行列</returns>
Matrix4x4 Inverse(const Matrix4x4& m) {
	Matrix4x4 inverseMatrix;
	float determinant;

	// 4x4行列の行列式を計算
	determinant = m.m[0][0] * Determinant3x3(m, 0, 0)
		- m.m[0][1] * Determinant3x3(m, 0, 1)
		+ m.m[0][2] * Determinant3x3(m, 0, 2)
		- m.m[0][3] * Determinant3x3(m, 0, 3);

	// 余因子行列を求める
	Matrix4x4 cofactorMatrix;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			int sign;
			if ((i + j) % 2 == 0) {
				sign = 1;
			} else {
				sign = -1;
			}
			cofactorMatrix.m[i][j] = sign * Determinant3x3(m, i, j);
		}
	}

	// 余因子行列の転置（共役行列）を求める
	Matrix4x4 adjugateMatrix;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			adjugateMatrix.m[i][j] = cofactorMatrix.m[j][i];
		}
	}

	// 逆行列 = (1 / 行列式) * 共役行列
	float invDet = 1.0f / determinant;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			inverseMatrix.m[i][j] = adjugateMatrix.m[i][j] * invDet;
		}
	}

	return inverseMatrix;
}

/// <summary>
/// 4x4行列の乗算
/// </summary>
/// <param name="matrix1">4x4行列 1</param>
/// <param name="matrix2">4x4行列 2</param>
/// <returns>4x4行列の乗算</returns>
Matrix4x4 Multiply(Matrix4x4 matrix1, Matrix4x4 matrix2) {
	Matrix4x4 multiplyMatrix;
	float multiply[4];  // 4x4行列に合わせてサイズを変更

	for (int row = 0; row < 4; row++) {
		for (int column = 0; column < 4; column++) {
			for (int i = 0; i < 4; i++) {
				multiply[i] = matrix1.m[row][i] * matrix2.m[i][column];
			}
			multiplyMatrix.m[row][column] = multiply[0] + multiply[1] + multiply[2] + multiply[3];
		}
	}

	return multiplyMatrix;
}

/// <summary>
/// アフィン行列を作成
/// </summary>
/// <param name="scale">拡縮</param>
/// <param name="rotate">回転</param>
/// <param name="translate">移動</param>
/// <returns>4x4アフィン行列</returns>
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate) {
	Matrix4x4 affineMatrix;

	// 回転行列（X軸回転）
	Matrix4x4 rotateX = {
		1.0f,  0.0f, 0.0f, 0.0f,
		0.0f, std::cos(rotate.x), std::sin(rotate.x), 0.0f,
		0.0f, std::sin(-rotate.x), std::cos(rotate.x), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	// 回転行列（Y軸回転）
	Matrix4x4 rotateY = {
		std::cos(rotate.y), 0.0f, std::sin(-rotate.y), 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		std::sin(rotate.y), 0.0f, std::cos(rotate.y), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	// 回転行列（Z軸回転）
	Matrix4x4 rotateZ = {
		std::cos(rotate.z), std::sin(rotate.z), 0.0f, 0.0f,
		std::sin(-rotate.z), std::cos(rotate.z), 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	// 回転行列を合成
	Matrix4x4 rotationMatrix = Multiply(rotateX, Multiply(rotateY, rotateZ));

	// スケーリング行列
	Matrix4x4 scaleMatrix = {
		scale.x, 0.0f, 0.0f, 0.0f,
		0.0f, scale.y, 0.0f, 0.0f,
		0.0f, 0.0f, scale.z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	// 平行移動行列
	Matrix4x4 translateMatrix = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		translate.x, translate.y, translate.z, 1.0f
	};

	affineMatrix = Multiply(scaleMatrix, Multiply(rotationMatrix, translateMatrix));

	return affineMatrix;
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
/// MakeViewportMatrix関数
/// </summary>
/// <param name="left">left</param>
/// <param name="top">top</param>
/// <param name="width">width</param>
/// <param name="height">height</param>
/// <param name="minDepth">minDepth</param>
/// <param name="maxDepth">maxDepth</param>
/// <returns>ビューポート行列</returns>
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
/// ベクトルの値を表示
/// </summary>
/// <param name="x">x座標</param>
/// <param name="y">y座標</param>
/// <param name="vector">表示するベクトル</param>
/// <param name="label">ラベル</param>
void VectorScreenPrintf(int x, int y, const Vector3& vector, const char* label) {
	Novice::ScreenPrintf(x, y, "%.02f", vector.x);
	Novice::ScreenPrintf(x + kColumnWidth, y, "%.02f", vector.y);
	Novice::ScreenPrintf(x + kColumnWidth * 2, y, "%.02f", vector.z);
	Novice::ScreenPrintf(x + kColumnWidth * 3, y, "%s", label);
}