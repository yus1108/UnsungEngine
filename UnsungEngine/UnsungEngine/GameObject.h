#pragma once
#include "Transform.h"

class GameObject
{
private:
	Transform transform;
	RenderComponent * renderComponent;
	UEngine::DrawType drawType;
	unsigned referenceNum;
public:
	GameObject();
	~GameObject();

	unsigned GetReferenceNum() { return referenceNum; }
	void SetReferenceNum(unsigned _ref) { referenceNum = _ref; }

	UEngine::DrawType GetDrawType() { return drawType; }
	void SetDrawType(UEngine::DrawType _drawType) { drawType = _drawType; }

	Transform * GetTransform() { return &transform; }
	void SetTransform(Transform * _transform) { transform = *_transform; }
	RenderComponent * GetRenderComponent() { return renderComponent; }
	void SetRenderComponent(RenderComponent * _renderComponent) { renderComponent = _renderComponent; }

	virtual void Update();
};

