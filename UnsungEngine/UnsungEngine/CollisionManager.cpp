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
		for (unsigned i = 0; i < currNode->children.size(); i++)
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

	for (unsigned i = 0; i < currNode->children.size(); i++)
		TreeConstruction(currNode->children[i], levels - 1);
}

void CollisionManager::Init(AABB _boundary, int levels)
{
	root->grid = _boundary;
	TreeConstruction(root, levels);
}

void CollisionManager::Update(CollisionComponent * component) {

	Traverse(component, root);
}

void CollisionManager::Clear(UEngine::CollisionTree * currNode) {
	if (currNode->objs.size() > 0)
	{
		for (unsigned i = 0; i < currNode->children.size(); i++)
			Clear(currNode->children[i]);
		currNode->objs.clear();
	}
}

void CollisionManager::Traverse(CollisionComponent * component, UEngine::CollisionTree * currNode) {
	if (UMath::CollisionTest(currNode->grid, *((OOBB*)component->GetCollisionBox())))
	{
		if (currNode->children.size() == 0)
		{
			for (unsigned i = 0; i < currNode->objs.size(); i++)
			{
				CollisionComponent * otherObj = (CollisionComponent*)currNode->objs[i];
				if (component->GetCollisionBox()->GetCollisionType() == UEngine::Collision_OOBB
					&& otherObj->GetCollisionBox()->GetCollisionType() == UEngine::Collision_OOBB)
				{
					OOBB * currOOBB = (OOBB*)component->GetCollisionBox();
					OOBB * otherOOBB = (OOBB*)otherObj->GetCollisionBox();
					if (UMath::CollisionTest(currOOBB, otherOOBB))
					{
						std::cout << "colliding" << std::endl;
					}
				}
			}
		}

#ifdef _DEBUG
		AABB tempAABB = currNode->grid;
		debugRenderer.Add_AABB(tempAABB, DirectX::XMFLOAT4(1, 1, 0, 1));
#endif // DEBUG

		currNode->objs.push_back(component);
		
		for (unsigned i = 0; i < currNode->children.size(); i++)
			Traverse(component, currNode->children[i]);
	}
}