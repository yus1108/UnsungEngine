#include "stdafx.h"
#include "CollisionManager.h"


CollisionManager::CollisionManager()
{
	root = new UEngine::CollisionTree;
	uiComponentOnMouse = nullptr;
}


CollisionManager::~CollisionManager()
{
	OnDestroy(root);
}

void CollisionManager::OnDestroy(UEngine::CollisionTree * currNode) {
	for (unsigned i = 0; i < currNode->children.size(); i++)
		OnDestroy(currNode->children[i]);
	delete currNode;
}

void CollisionManager::Reset() {
	UVector<UEngine::CollisionTree*> queueUnit;
	queueUnit.push_back(root);
	while (queueUnit.size() > 0) {
		UEngine::CollisionTree *currNode = queueUnit[0];
		queueUnit.erase(0);
		for (unsigned i = 0; i < currNode->children.size(); i++)
			queueUnit.push_back(currNode->children[i]);
		delete currNode;
	}
	root = new UEngine::CollisionTree;
	uiComponentOnMouse = nullptr;
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
	//AABB UIAABB;
	switch (component->GetParent()->GetRenderComponent()->GetType())
	{
	case UEngine::DrawType_WORLD:
		Traverse(component, root);
		break;
	case UEngine::DrawType_UI:
		uiComponentOnMouse = UMath::CollisionTest(input.GetMousePos(), (AABB*)component->GetCollisionBox()) ? component : uiComponentOnMouse;
//#ifdef _DEBUG
//		// temporarily only work in origin camera pos
//		UIAABB = *(AABB*)component->GetCollisionBox();
//		debugRenderer.Add_UI_AABB(UIAABB, DirectX::XMFLOAT4(0, 0, 1, 1));
//#endif // _DEBUG
		if (uiComponentOnMouse)
			std::cout << uiComponentOnMouse << std::endl;
		break;
	default:
		break;
	}
}

void CollisionManager::Clear(UEngine::CollisionTree * currNode) {
	if (currNode->objs.size() > 0)
	{
		for (unsigned i = 0; i < currNode->children.size(); i++)
			Clear(currNode->children[i]);
		currNode->objs.clear();
	}
	uiComponentOnMouse = nullptr;
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
						if (!(component->GetTrigger() || otherObj->GetTrigger()))
						{
							using namespace DirectX;
							DirectX::XMVECTOR otherPos = otherOOBB->GetWorldMat().r[3];
							DirectX::XMVECTOR currPos = currOOBB->GetWorldMat().r[3];
							DirectX::XMVECTOR diff = otherPos - currPos;
							if (diff.m128_f32[0] > 0)
								diff.m128_f32[0] = 1;
							else if (diff.m128_f32[0] < 0)
								diff.m128_f32[0] = -1;
							if (diff.m128_f32[1] > 0)
								diff.m128_f32[1] = 1;
							else if (diff.m128_f32[1] < 0)
								diff.m128_f32[1] = -1;
							if (diff.m128_f32[2] > 0)
								diff.m128_f32[2] = 1;
							else if (diff.m128_f32[2] < 0)
								diff.m128_f32[2] = -1;

							double delta = utime.DeltaTime();
							XMVECTOR otherPrevPos = otherObj->GetParent()->GetTransform()->GetPrevPos();
							XMVECTOR currPrevPos = component->GetParent()->GetTransform()->GetPrevPos();
							otherPos -= otherPrevPos;
							otherPos.m128_f32[0] = otherPos.m128_f32[0] / delta;
							otherPos.m128_f32[1] = otherPos.m128_f32[1] / delta;
							otherPos.m128_f32[2] = otherPos.m128_f32[2] / delta;
							currPos -= currPrevPos;
							currPos.m128_f32[0] = currPos.m128_f32[0] / delta;
							currPos.m128_f32[1] = currPos.m128_f32[1] / delta;
							currPos.m128_f32[2] = currPos.m128_f32[2] / delta;
							currPos = XMVector3Length(currPos).m128_f32[0] >= XMVector3Length(otherPos).m128_f32[0] ? currPos : otherPos;
							if (!component->GetStatic()) {
								DirectX::XMFLOAT3 pos = component->GetParent()->GetTransform()->GetPosition3();
								component->GetParent()->GetTransform()->SetPosition(
									DirectX::XMFLOAT3(pos.x - diff.m128_f32[0] * delta * currPos.m128_f32[0],
										pos.y - diff.m128_f32[1] * delta * currPos.m128_f32[1]
										, pos.z - diff.m128_f32[2] * delta * currPos.m128_f32[2]));
							}
							if (!otherObj->GetStatic()) {
								DirectX::XMFLOAT3 pos = otherObj->GetParent()->GetTransform()->GetPosition3();
								otherObj->GetParent()->GetTransform()->SetPosition(
									DirectX::XMFLOAT3(pos.x + diff.m128_f32[0] * delta * currPos.m128_f32[0],
										pos.y + diff.m128_f32[1] * delta * currPos.m128_f32[1]
										, pos.z + diff.m128_f32[2] * delta * currPos.m128_f32[2]));
							}
						}
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