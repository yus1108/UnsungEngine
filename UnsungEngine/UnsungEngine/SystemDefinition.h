#pragma once
#include "stdafx.h"

// enum definition
namespace UEngine {
	enum DrawType
	{
		WORLD,
		UI,
		COUNT
	};

	struct TextFormat
	{
		UINT width = 512;
		UINT height = 512; 
		UINT textColor = D2D1::ColorF::Black;
		UINT backgroundColor = D2D1::ColorF::White;
		FLOAT dpiX = 96;
		FLOAT dpiY = 96;
		bool hasBackgroundColor = false;
	};
}


// structure definition
struct alignas(16) float4 : std::array<float, 4> { using std::array<float, 4>::array;  };
struct alignas(64) float4x4 : std::array<float4, 4> { using std::array<float4, 4>::array;  };

struct pipeline_state_t
{
	Microsoft::WRL::ComPtr<ID3D11InputLayout>		input_layout;
	Microsoft::WRL::ComPtr<ID3D11VertexShader>		vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>		pixel_shader;
	Microsoft::WRL::ComPtr<ID3D11GeometryShader>	geometry_shader;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>	render_target;
	Microsoft::WRL::ComPtr<ID3D11Texture2D>			depthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>	depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>	rasterState;
	Microsoft::WRL::ComPtr<ID3D11SamplerState>		samplerState;
	Microsoft::WRL::ComPtr<ID3D11BlendState>		blendingState;
};

struct RenderToTexture
{
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>	depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> renderTargetTextureMap;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>	renderTargetViewMap;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceViewMap;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> outTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> outSRV;
	UINT width;
	UINT height;
};

namespace end
{
	struct simple_vert
	{
		float4 pos;
		float4 normals;
		float u, v;
	};

	struct simple_mesh
	{
		uint32_t vert_count = 0;
		uint32_t index_count = 0;
		end::simple_vert* verts = nullptr;
		uint32_t* indices = nullptr;
	};
}

namespace graphics
{
	namespace material
	{
		enum components {
			EMISSIVE = 0, AMBIENT, DIFFUSE, NORMAL, BUMP, TRANSPARENCY,
			DISPLACEMENT, VECTOR_DISPLACEMENT, SPECULAR, SHININESS, REFLECTION, COUNT
		};

		union source_t
		{
			uint64_t id;
			const wchar_t* file_path;
			const void* texture_resource;
		};

		struct component_t
		{
			float4 value;
			source_t input;
		};
	}

	namespace animation
	{
		struct joint {
			float4x4 global_xform;
			int parent_index;
		};

		struct keyframe {
			double time;
			std::vector<joint> joints;
		};

		struct anim_clip {
			double duration;
			std::vector<keyframe> frames;
		};

		const int MAX_INFLUENCES = 4;

		struct influence {
			int joint;
			float weight;
		};
		using influence_set = std::array<influence, MAX_INFLUENCES>;
	}

	using namespace material;
	struct material_t : std::array< component_t, components::COUNT >
	{
		std::bitset<64> pipeline_id;
	};
}

struct DefaultVertex
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT2 tex;
};

struct SIMPLE_VERTEX {
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT4 rgba;
	DirectX::XMFLOAT2 tex;
	DirectX::XMFLOAT3 normals;
	DirectX::XMFLOAT3 tangents;
	DirectX::XMFLOAT3 binomals;
	DirectX::XMFLOAT4 weights;
	DirectX::XMUINT4 joints;
};
struct SCENE {
	DirectX::XMMATRIX viewMat;
	DirectX::XMMATRIX perspectivMat;
};
struct DLIGHT
{
	DirectX::XMFLOAT4 lightColor;
	DirectX::XMFLOAT3 lightDirection;
	float ambient;
};
struct PLIGHT
{
	DirectX::XMFLOAT4 lightColor;
	DirectX::XMFLOAT3 lightPos;
	float lightRadius;
};
struct SLIGHT
{
	DirectX::XMFLOAT4 lightColor;
	DirectX::XMFLOAT3 lightPos;
	float coneRatioIn;
	DirectX::XMFLOAT3 coneDir;
	float coneRatioOut;
};
struct LIGHTINFO
{
	DirectX::XMFLOAT4 specConstant;
	float4 diffuse;
	float4 ambient;
	float4 specular;
	float4 emissive;
};

struct Triangle {
	std::vector<unsigned int> indices;
	DirectX::XMFLOAT3 faceNormal;
	DirectX::XMFLOAT3 centroid;

	~Triangle() {
		indices.clear();
	}
};

struct Animation {
	// animation info
	std::vector<graphics::animation::joint> bindPose;
	std::vector<graphics::animation::anim_clip> animations;
	std::vector<DirectX::XMMATRIX> transforms;
	float playAnim = 0;
	int currAnim = 0;
};

struct Material {
	// texture info
	graphics::material_t *material_info = nullptr;
	unsigned int matCount = 0;
	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> srv;

	~Material() {
		if (material_info)
			delete[] material_info;
	}
};

struct Geometry {
	// basic vertices info
	std::vector<SIMPLE_VERTEX> vertices;
	std::vector<unsigned int> indices;
	std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>> vertexBuffer;
	std::vector<unsigned int> countVertexBuffer;
	std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>> indexBuffers;
	std::vector<unsigned int> countIndexBuffer;
	std::vector<Triangle> triangles;
};