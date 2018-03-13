#pragma once
class GameObject
{
private:
	RenderComponent * renderComponent;
	UEngine::DrawType drawType;
public:
	GameObject();
	~GameObject();

	RenderComponent * GetRenderComponent() { return renderComponent; }
	UEngine::DrawType GetDrawType() { return drawType; }
	void SetRenderComponent(RenderComponent * _renderComponent) { renderComponent = _renderComponent; }
	void SetDrawType(UEngine::DrawType _drawType) { drawType = _drawType; }
};

