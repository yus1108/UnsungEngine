#include "stdafx.h"
#include "CollisionManager.h"


CollisionManager::CollisionManager()
{
	root = new UEngine::CollisionTree;
}


CollisionManager::~CollisionManager()
{
	UVector<UEngine::CollisionTree*> queueUnit;
	queueUnit.push_back(root);
	while (queueUnit.size() > 0) {
		UEngine::CollisionTree *currNode = queueUnit[0];
		queueUnit.erase(0);
		for (size_t i = 0; i < currNode->children.size(); i++)
			queueUnit.push_back(currNode->children[i]);
		delete currNode;
	}
}

void CollisionManager::TreeConstruction(UEngine::CollisionTree * currNode, int levels) {
	float x = currNode->grid.xaxis.y - currNode->grid.xaxis.x;
	float y = currNode->grid.yaxis.y - currNode->grid.yaxis.x;
	float z = currNode->grid.zaxis.y - currNode->grid.zaxis.x;

	float unitLength = x * y * z;
	if (unitLength <= 1)
	{
		return;
	}

	if (levels == 0) {
	return;
	}

	AABB temp = currNode->grid;
	temp.xaxis.x += x / 2;
	temp.yaxis.x += y / 2;
	temp.zaxis.x += z / 2;
	currNode->children.push_back(new UEngine::CollisionTree);
	currNode->children[currNode->children.size() - 1]->grid = temp;

	temp = currNode->grid;
	temp.xaxis.y -= x / 2;
	temp.yaxis.x += y / 2;
	temp.zaxis.x += z / 2;
	currNode->children.push_back(new UEngine::CollisionTree);
	currNode->children[currNode->children.size() - 1]->grid = temp;

	temp = currNode->grid;
	temp.xaxis.y -= x / 2;
	temp.yaxis.x += y / 2;
	temp.zaxis.y -= z / 2;
	currNode->children.push_back(new UEngine::CollisionTree);
	currNode->children[currNode->children.size() - 1]->grid = temp;


	temp = currNode->grid;
	temp.xaxis.x += x / 2;
	temp.yaxis.x += y / 2;
	temp.zaxis.y -= z / 2;
	currNode->children.push_back(new UEngine::CollisionTree);
	currNode->children[currNode->children.size() - 1]->grid = temp;

	temp = currNode->grid;
	temp.xaxis.x += x / 2;
	temp.yaxis.y -= y / 2;
	temp.zaxis.x += z / 2;
	currNode->children.push_back(new UEngine::CollisionTree);
	currNode->children[currNode->children.size() - 1]->grid = temp;

	temp = currNode->grid;
	temp.xaxis.y -= x / 2;
	temp.yaxis.y -= y / 2;
	temp.zaxis.x += z / 2;
	currNode->children.push_back(new UEngine::CollisionTree);
	currNode->children[currNode->children.size() - 1]->grid = temp;

	temp = currNode->grid;
	temp.xaxis.y -= x / 2;
	temp.yaxis.y -= y / 2;
	temp.zaxis.y -= z / 2;
	currNode->children.push_back(new UEngine::CollisionTree);
	currNode->children[currNode->children.size() - 1]->grid = temp;


	temp = currNode->grid;
	temp.xaxis.x += x / 2;
	temp.yaxis.y -= y / 2;
	temp.zaxis.y -= z / 2;
	currNode->children.push_back(new UEngine::CollisionTree);
	currNode->children[currNode->children.size() - 1]->grid = temp;

	for (size_t i = 0; i < currNode->children.size(); i++)
		TreeConstruction(currNode->children[i], levels - 1);
}

void CollisionManager::Init(AABB _boundary, int levels)
{
	root->grid = _boundary;
	TreeConstruction(root, levels);
}

void CollisionManager::Update(CollisionComponent * component) {
#ifdef _DEBUG
	AABB tempAABB = root->grid;
	debugRenderer.Add_AABB(tempAABB, DirectX::XMFLOAT4(1, 1, 0, 1));
#endif // DEBUG
	Traverse(component, root);
}

void CollisionManager::Clear(UEngine::CollisionTree * currNode) {
	for (size_t i = 0; i < currNode->children.size(); i++)
		Clear(currNode->children[i]);
	currNode->objs.clear();
}

void CollisionManager::Traverse(CollisionComponent * component, UEngine::CollisionTree * currNode) {
	/*AABB currCB = currObj->collisionBox;
	float x, y, z;
	x = currObj->worldMat.r[3].m128_f32[0];
	y = currObj->worldMat.r[3].m128_f32[1];
	z = currObj->worldMat.r[3].m128_f32[2];

	currCB.xaxis = DirectX::XMFLOAT2(x - currCB.xaxis.x, x + currCB.xaxis.y);
	currCB.yaxis = DirectX::XMFLOAT2(y - currCB.yaxis.x, y + currCB.yaxis.y);
	currCB.zaxis = DirectX::XMFLOAT2(z - currCB.zaxis.x, z + currCB.zaxis.y);

	if (UMath::CollisionTest(currCB, currNode->collisionBox)) {
		float x1, x2, z1, z2;
		x1 = currCB.xaxis.y - currCB.xaxis.x;
		x2 = (currNode->collisionBox.xaxis.y - currNode->collisionBox.xaxis.x);
		z1 = currCB.zaxis.y - currCB.zaxis.x;
		z2 = (currNode->collisionBox.zaxis.y - currNode->collisionBox.zaxis.x);

		if (currNode->children.size() == 0) {
			for each (UnitNode * otherObj in currNode->objs)
			{
				if (currObj == otherObj)
					continue;

				AABB otherCB = otherObj->collisionBox;
				x = otherObj->worldMat.r[3].m128_f32[0];
				y = otherObj->worldMat.r[3].m128_f32[1];
				z = otherObj->worldMat.r[3].m128_f32[2];

				otherCB.xaxis = DirectX::XMFLOAT2(x - otherCB.xaxis.x, x + otherCB.xaxis.y);
				otherCB.yaxis = DirectX::XMFLOAT2(y - otherCB.yaxis.x, y + otherCB.yaxis.y);
				otherCB.zaxis = DirectX::XMFLOAT2(z - otherCB.zaxis.x, z + otherCB.zaxis.y);

				if (UMath::CollisionTest(currCB, otherCB)) {
					if (UMath::CollisionTest(currObj->obb, otherObj->obb)) {
						currObj->isColliding = true;
						otherObj->isColliding = true;

						float x1, z1;
						float x2, z2;
						x1 = (otherCB.xaxis.y - currCB.xaxis.x);
						z1 = (otherCB.zaxis.y - currCB.zaxis.x);

						x2 = (otherCB.xaxis.x - currCB.xaxis.y);
						z2 = (otherCB.zaxis.x - currCB.zaxis.y);

						x1 = abs(x1) < abs(x2) ? 1.0f : -1.0f;
						z1 = abs(z1) < abs(z2) ? 1.0f : -1.0f;

						float ratio = otherObj->teamUnit->weight / currObj->teamUnit->weight;
						if (ratio >= 1) {
							if (!currObj->isStatic) {
								currObj->worldMat.r[3].m128_f32[0] += x1 * (float)xtime->SmoothDelta() * currObj->maxSpeed;
								currObj->worldMat.r[3].m128_f32[2] += z1 * (float)xtime->SmoothDelta() * currObj->maxSpeed;
								currObj->velocity = DirectX::XMVectorSet(0, 0, 0, 0);
							}
						}
						if (ratio <= 1) {
							if (!otherObj->isStatic) {
								otherObj->worldMat.r[3].m128_f32[0] -= x1 * (float)xtime->SmoothDelta() * otherObj->maxSpeed;
								otherObj->worldMat.r[3].m128_f32[2] -= z1 * (float)xtime->SmoothDelta() * otherObj->maxSpeed;
								otherObj->velocity = DirectX::XMVectorSet(0, 0, 0, 0);
							}
						}
					}
				}
			}
		}*/

		//currNode->objs.push_back(currObj);
	if (currNode->children.size() == 0)
	{
//#ifdef _DEBUG
//		AABB tempAABB = currNode->grid;
//		debugRenderer.Add_AABB(tempAABB, DirectX::XMFLOAT4(1, 1, 0, 1));
//#endif // DEBUG
	}

		for (size_t i = 0; i < currNode->children.size(); i++)
			Traverse(component, currNode->children[i]);
	//}
}