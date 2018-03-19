#pragma once

class UMath {
public:
	static float Convert_DegreeToRad(float degree);
	static double Convert_DegreeToRad(double degree);
	static DirectX::XMFLOAT2 ConvertPixelToNDC(POINT mousePos, HWND * window);
	//void CalculateAABB(std::vector<SIMPLE_VERTEX> vertices, std::vector<unsigned int> indices,
	//	DirectX::XMMATRIX worldmat, AABB & outAABB);


	//bool CollisionTest(Trapezoid trapzoid, AABB obb);
	//bool CollisionTest(Trapezoid trapzoid, OBB obb);
	//bool CollisionTest(AABB a, AABB b);
	//bool CollisionTest(OBB a, OBB b);
	//bool CollisionTest(SIMPLE_VERTEX startRay, SIMPLE_VERTEX endRay, AABB b);

	//void RayCasting(DirectX::XMMATRIX & worldMat, AABB & lineSeg);
	//DirectX::XMMATRIX GetWorldPosFromScreen(UCamera * myCam, DirectX::XMFLOAT2 NDCPos);
	//DirectX::XMVECTOR GetWorldPosFromScreenVec(UCamera * myCam, DirectX::XMFLOAT2 NDCPos);
};

