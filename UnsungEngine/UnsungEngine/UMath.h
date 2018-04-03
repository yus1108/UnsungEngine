#pragma once
#include "AABB.h"
#include "OOBB.h"

struct SIMPLE_VERTEX;
class UMath {
public:
	static float Convert_DegreeToRad(float degree);
	static double Convert_DegreeToRad(double degree);

	static DirectX::XMFLOAT2 ConvertPixelToNDC(POINT mousePos, HWND * window);

	static void CalculateAABB(std::vector<SIMPLE_VERTEX> vertices, std::vector<unsigned int> indices,
		DirectX::XMMATRIX worldmat, AABB & outAABB);

	static bool CollisionTest(DirectX::XMFLOAT2 mousePos, AABB * b);
	static bool CollisionTest(AABB a, AABB b);
	static bool CollisionTest(AABB a, OOBB b);
	static bool CollisionTest(OOBB * a, OOBB * b);


	//bool CollisionTest(Trapezoid trapzoid, AABB obb);
	//bool CollisionTest(Trapezoid trapzoid, OBB obb);
	//bool CollisionTest(SIMPLE_VERTEX startRay, SIMPLE_VERTEX endRay, AABB b);

	//void RayCasting(DirectX::XMMATRIX & worldMat, AABB & lineSeg);
	//DirectX::XMMATRIX GetWorldPosFromScreen(UCamera * myCam, DirectX::XMFLOAT2 NDCPos);
	//DirectX::XMVECTOR GetWorldPosFromScreenVec(UCamera * myCam, DirectX::XMFLOAT2 NDCPos);
};

