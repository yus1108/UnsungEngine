#pragma once
class GameObject
{
private:
	RenderComponent * renderComponent;
	UEngine::DrawType drawType;
	unsigned referenceNum;
public:
	GameObject();
	~GameObject();

	RenderComponent * GetRenderComponent() { return renderComponent; }
	UEngine::DrawType GetDrawType() { return drawType; }
	unsigned GetReferenceNum() { return referenceNum; }
	void SetRenderComponent(RenderComponent * _renderComponent) { renderComponent = _renderComponent; }
	void SetDrawType(UEngine::DrawType _drawType) { drawType = _drawType; }
	void SetReferenceNum(unsigned _ref) { referenceNum = _ref; }
};

