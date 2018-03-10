#pragma once
class GameObject
{
private:
	RenderComponent * renderComponent;
public:
	GameObject();
	~GameObject();

	RenderComponent * GetRenderComponent() { return renderComponent; }
	void SetRenderComponent(RenderComponent * _renderComponent) { renderComponent = _renderComponent; }
};

