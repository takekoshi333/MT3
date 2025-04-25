#include <Novice.h>
#include <cstdint>
#include <assert.h>
#include <cmath>
#define _USE_MATH_DEFINES
#include <math.h>
#include <ImGui.h>

const char kWindowTitle[] = "LE2A_09_タケコシケイ_MT3_02_00_確認課題";

struct Vector3 {
	float x;
	float y;
	float z;
};

struct Matrix4x4 {
	float m[4][4];
};

struct Line {
	Vector3 origin;
	Vector3 diff;
};

struct Ray {
	Vector3 origin;
	Vector3 diff;
};

struct Segment {
	Vector3 origin;
	Vector3 diff;
};

struct Sphere {
	Vector3 center;
	float radius;
};

void DrawGrid(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix);
void DrawSphere(const Sphere& sphere, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color);
Matrix4x4 MakeIdentity4x4();
float Determinant3x3(const Matrix4x4& m, int row, int col);
Matrix4x4 Inverse(const Matrix4x4& m);
Matrix4x4 Multiply(Matrix4x4 matrix1, Matrix4x4 matrix2);
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);
Vector3 Add(const Vector3& v1, const Vector3& v2);
Vector3 Subtract(const Vector3& v1, const Vector3& v2);
Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix);
Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);
Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth);
Vector3 Project(const Vector3& v1, const Vector3& v2);
Vector3 ClosestPoint(const Vector3& point, const Segment& segment);

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = {0};
	char preKeys[256] = {0};

	//============================================
	// Segmentの変数
	//============================================
	Segment segment = { {-2.0f, -1.0f, 0.0f}, {3.0f, 2.0f, 2.0f} };

	//============================================
	// Vector3の変数
	//============================================
	Vector3 cameraTranslate = { 0.0f, 1.9f, -6.49f };
	Vector3 cameraRotate = { 0.26f, 0.0f, 0.0f };
	Vector3 point = { -1.5f, 0.6f, 0.6f };
	Vector3 project = Project(Subtract(point, segment.origin), segment.diff);
	Vector3 closestPoint = ClosestPoint(point, segment);
	Vector3 start;
	Vector3 end;

	//============================================
	// Matrix4x4の変数
	//============================================
	Matrix4x4 worldMatrix = MakeAffineMatrix({ 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f });
	Matrix4x4 cameraMatrix = MakeAffineMatrix({ 1.0f, 1.0f, 1.0f }, cameraRotate, cameraTranslate);
	Matrix4x4 viewMatrix = Inverse(cameraMatrix);
	Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, 1280.0f / 720.0f, 0.1f, 100.0f);
	Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
	Matrix4x4 viewportMatrix = MakeViewportMatrix(0.0f, 0.0f, 1280.0f, 720.0f, 0.0f, 1.0f);
	
	//============================================
	// Sphereの変数
	//============================================
	Sphere pointSphere = { point, 0.01f };
	Sphere closestPointSphere = { closestPoint, 0.01f };

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
		// ImGuiの処理
		//======================================
		ImGui::Begin("Window");
		ImGui::DragFloat3("cameraTranslate", &cameraTranslate.x, 0.01f);;
		ImGui::DragFloat3("cameraRotate", &cameraRotate.x, 0.01f);
		ImGui::InputFloat3("Project", &project.x, "%.3f", ImGuiInputTextFlags_ReadOnly);
		ImGui::DragFloat3("Project", &project.x, 0.01f);
		ImGui::DragFloat3("ClosestPoint", &closestPoint.x, 0.01f);
		ImGui::DragFloat3("start", &start.x, 0.01f);
		ImGui::DragFloat3("end", &end.x, 0.01f);
		ImGui::DragFloat3("segment.origin", &segment.origin.x, 0.01f);
		ImGui::DragFloat3("segment.diff", &segment.diff.x, 0.01f);
		ImGui::End();

		//======================================
		// 線分の処理
		//======================================
		start = Transform(Transform(segment.origin, worldViewProjectionMatrix), viewportMatrix);
		end = Transform(Transform(Add(segment.origin, segment.diff), worldViewProjectionMatrix), viewportMatrix);

		//======================================
		// 各行列の更新処理
		//======================================
		worldMatrix = MakeAffineMatrix({ 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f });
		cameraMatrix = MakeAffineMatrix({ 1.0f, 1.0f, 1.0f }, cameraRotate, cameraTranslate);
		viewMatrix = Inverse(cameraMatrix);
		projectionMatrix = MakePerspectiveFovMatrix(0.45f, 1280.0f / 720.0f, 0.1f, 100.0f);
		worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
		viewportMatrix = MakeViewportMatrix(0.0f, 0.0f, 1280.0f, 720.0f, 0.0f, 1.0f);

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		//======================================
		// grid
		//======================================
		DrawGrid(worldViewProjectionMatrix, viewportMatrix);

		//======================================
		// sphere
		//======================================
		DrawSphere(pointSphere, worldViewProjectionMatrix, viewportMatrix, RED);
		DrawSphere(closestPointSphere, worldViewProjectionMatrix, viewportMatrix, BLACK);

		//======================================
		// Line
		//======================================
		Novice::DrawLine(
			static_cast<int>(start.x), static_cast<int>(start.y),
			static_cast<int>(end.x), static_cast<int>(end.y),
			0xFFFFFFFF
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
///  3次元ベクトルの加算
/// </summary>
/// <param name="v1">加算するベクトルv1</param>
/// <param name="v2">加算するベクトルv2</param>
/// <returns>加算されたベクトル</returns>
Vector3 Add(const Vector3& v1, const Vector3& v2) {
	Vector3 addVecotr;
	addVecotr.x = v1.x + v2.x;
	addVecotr.y = v1.y + v2.y;
	addVecotr.z = v1.z + v2.z;
	return addVecotr;
}

/// <summary>
///  3次元ベクトルの減算
/// </summary>
/// <param name="v1">減算するベクトルv1</param>
/// <param name="v2">減算するベクトルv2</param>
/// <returns>減算されたベクトル</returns>
Vector3 Subtract(const Vector3& v1, const Vector3& v2) {
	Vector3 subtractVector;
	subtractVector.x = v1.x - v2.x;
	subtractVector.y = v1.y - v2.y;
	subtractVector.z = v1.z - v2.z;
	return subtractVector;
}

/// <summary>
/// Gridを描画
/// </summary>
/// <param name="viewProjectionMatrix">正射影行列</param>
/// <param name="viewportMatrix">ビューポート行列</param>
void DrawGrid(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix) {
	const float kGridhalfWidth = 2.0f;
	const uint32_t kSubdivision = 10;
	const float kGridEvery = (kGridhalfWidth * 2.0f) / static_cast<float>(kSubdivision);
	const unsigned int kGridColor = 0xAAAAAAFF;

	for (uint32_t xIndex = 0; xIndex <= kSubdivision; ++xIndex) {
		float x = -kGridhalfWidth + kGridEvery * xIndex;

		Vector3 start = { x, 0.0f, -kGridhalfWidth };
		Vector3 end = { x, 0.0f, kGridhalfWidth };

		Vector3 ndcStart = Transform(start, viewProjectionMatrix);
		Vector3 ndcEnd = Transform(end, viewProjectionMatrix);

		Vector3 screenStart = Transform(ndcStart, viewportMatrix);
		Vector3 screenEnd = Transform(ndcEnd, viewportMatrix);

		Novice::DrawLine(static_cast<int>(screenStart.x), static_cast<int>(screenStart.y),
			static_cast<int>(screenEnd.x), static_cast<int>(screenEnd.y),
			kGridColor);
	}

	for (uint32_t zIndex = 0; zIndex <= kSubdivision; ++zIndex) {
		float z = -kGridhalfWidth + kGridEvery * zIndex;

		Vector3 start = { -kGridhalfWidth, 0.0f, z };
		Vector3 end = { kGridhalfWidth, 0.0f, z };

		Vector3 ndcStart = Transform(start, viewProjectionMatrix);
		Vector3 ndcEnd = Transform(end, viewProjectionMatrix);

		Vector3 screenStart = Transform(ndcStart, viewportMatrix);
		Vector3 screenEnd = Transform(ndcEnd, viewportMatrix);

		Novice::DrawLine(static_cast<int>(screenStart.x), static_cast<int>(screenStart.y),
			static_cast<int>(screenEnd.x), static_cast<int>(screenEnd.y),
			kGridColor);
	}
}

/// <summary>
/// sphereを描画
/// </summary>
/// <param name="sphere">sphere</param>
/// <param name="viewProjectionMatrix">正射影行列</param>
/// <param name="viewportMatrix">ビューポート行列</param>
/// <param name="color">色</param>
void DrawSphere(const Sphere& sphere, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	const uint32_t kSubdivision = 16;
	const float kLonEvery = 360.0f / float(kSubdivision);
	const float kLatEvery = 180.0f / float(kSubdivision);

	for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
		float lat = -90.0f + kLatEvery * latIndex;
		float latNext = lat + kLatEvery;

		for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
			float lon = lonIndex * kLonEvery;
			float lonNext = lon + kLonEvery;

			Vector3 a = {
				sphere.center.x + sphere.radius * std::cosf(lat * (float)M_PI / 180.0f) * std::cosf(lon * (float)M_PI / 180.0f),
				sphere.center.y + sphere.radius * std::sinf(lat * (float)M_PI / 180.0f),
				sphere.center.z + sphere.radius * std::cosf(lat * (float)M_PI / 180.0f) * std::sinf(lon * (float)M_PI / 180.0f)
			};

			Vector3 b = {
				sphere.center.x + sphere.radius * std::cosf(lat * (float)M_PI / 180.0f) * std::cosf(lonNext * (float)M_PI / 180.0f),
				sphere.center.y + sphere.radius * std::sinf(lat * (float)M_PI / 180.0f),
				sphere.center.z + sphere.radius * std::cosf(lat * (float)M_PI / 180.0f) * std::sinf(lonNext * (float)M_PI / 180.0f)
			};

			Vector3 c = {
				sphere.center.x + sphere.radius * std::cosf(latNext * (float)M_PI / 180.0f) * std::cosf(lon * (float)M_PI / 180.0f),
				sphere.center.y + sphere.radius * std::sinf(latNext * (float)M_PI / 180.0f),
				sphere.center.z + sphere.radius * std::cosf(latNext * (float)M_PI / 180.0f) * std::sinf(lon * (float)M_PI / 180.0f)
			};

			Vector3 a_ndc = Transform(a, viewProjectionMatrix);
			Vector3 b_ndc = Transform(b, viewProjectionMatrix);
			Vector3 c_ndc = Transform(c, viewProjectionMatrix);

			Vector3 a_screen = Transform(a_ndc, viewportMatrix);
			Vector3 b_screen = Transform(b_ndc, viewportMatrix);
			Vector3 c_screen = Transform(c_ndc, viewportMatrix);

			Novice::DrawLine(
				static_cast<int>(a_screen.x), static_cast<int>(a_screen.y),
				static_cast<int>(b_screen.x), static_cast<int>(b_screen.y),
				color
			);

			Novice::DrawLine(
				static_cast<int>(a_screen.x), static_cast<int>(a_screen.y),
				static_cast<int>(c_screen.x), static_cast<int>(c_screen.y),
				color
			);
		}
	}
}

/// <summary>
/// 4x4の単位行列の作成
/// </summary>
/// <returns>4x4の単位行列</returns>
Matrix4x4 MakeIdentity4x4() {
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
	float multiply[4];

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

Vector3 Project(const Vector3& v1, const Vector3& v2) {
	Vector3 result;

	// v2の長さの2乗（ノルム二乗）
	float v2LengthSq = v2.x * v2.x + v2.y * v2.y + v2.z * v2.z;
	assert(v2LengthSq != 0.0f); // ゼロ除算を防ぐ

	// スカラー係数を計算
	float dot = v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
	float scalar = dot / v2LengthSq;

	// 射影ベクトルを計算
	result.x = v2.x * scalar;
	result.y = v2.y * scalar;
	result.z = v2.z * scalar;

	return result;
}


Vector3 ClosestPoint(const Vector3& point, const Segment& segment) {
	Vector3 result;

	// 線分の始点からpointへのベクトル
	Vector3 segmentToPoint = Subtract(point, segment.origin);

	// 線分方向ベクトルの長さの2乗
	float segmentLengthSq = segment.diff.x * segment.diff.x + segment.diff.y * segment.diff.y + segment.diff.z * segment.diff.z;
	assert(segmentLengthSq != 0.0f); // ゼロ除算防止

	// スカラー係数tの計算（0 <= t <= 1にクランプ）
	float dot = segmentToPoint.x * segment.diff.x + segmentToPoint.y * segment.diff.y + segmentToPoint.z * segment.diff.z;
	float t = dot / segmentLengthSq;
	t = std::fmax(0.0f, std::fmin(1.0f, t)); // 線分内にクランプ

	// 最近接点 = 始点 + t * 方向ベクトル
	result.x = segment.origin.x + segment.diff.x * t;
	result.y = segment.origin.y + segment.diff.y * t;
	result.z = segment.origin.z + segment.diff.z * t;

	return result;
}
