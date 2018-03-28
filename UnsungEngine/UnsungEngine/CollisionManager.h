#pragma once
#include "CollisionComponent.h"

class CollisionManager
{
private:
	UEngine::CollisionTree * root;
	AABB boundary;

	void TreeConstruction(UEngine::CollisionTree * currNode, int levels);
public:
	CollisionManager();
	~CollisionManager();

	UEngine::CollisionTree * GetRoot() { return root; };
	AABB GetMap() { return boundary; };

	void Init(AABB _boundary, int levels);
	void Update(CollisionComponent * component);
	void Clear(UEngine::CollisionTree * currNode);
	void Traverse(CollisionComponent * component, UEngine::CollisionTree * currNode);
};

