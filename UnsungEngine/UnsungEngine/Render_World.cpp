#include "stdafx.h"
#include "Render_World.h"


Render_World::Render_World()
{
	geometryComponent = new Geometry;
	materialComponent = new Material;
	animationComponent = new Animation;
}


Render_World::~Render_World()
{
	delete geometryComponent;
	delete materialComponent;
	delete animationComponent;
}

void Render_World::Init(ID3D11Device * m_pDevice, const WCHAR * textString, UINT32 textLength, const WCHAR * msc_fontName, const FLOAT msc_fontSize, UEngine::TextFormat textFormat)
{
}

void Render_World::Init(ID3D11DeviceContext * deviceContext, pipeline_state_t * pipeline, D3D11_VIEWPORT viewport)
{
	m_pDeviceContext = deviceContext;
	m_pPipeline = pipeline;
	m_viewport = viewport;
}

void Render_World::DrawObj(Renderer * render)
{
	if (loadingDone) {
		render->RenderSet(m_pDeviceContext, *m_pPipeline, m_viewport, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		m_pDeviceContext->VSSetConstantBuffers(0, 1, &render->constBufferWorld);
		m_pDeviceContext->VSSetConstantBuffers(1, 1, &render->constBufferScene);
		m_pDeviceContext->PSSetConstantBuffers(2, 1, &render->constBufferDLight);
		m_pDeviceContext->PSSetConstantBuffers(3, 1, &render->constBufferPLight);
		m_pDeviceContext->PSSetConstantBuffers(4, 1, &render->constBufferSLight);
		m_pDeviceContext->PSSetConstantBuffers(5, 1, &render->constBufferLightInfo);

		UINT stride = sizeof(SIMPLE_VERTEX);
		UINT offset = 0;

		//AnimateModel(obj, 1);

		// world matrix
		worldMat = DirectX::XMMatrixMultiply(worldMat, DirectX::XMMatrixRotationY((float)utime.DeltaTime() / 10.0f));
		//if (utime.DeltaTime() < 1)
		//{
		//	worldMat.r[3].m128_f32[0] += (float)utime.DeltaTime();
		//}
		
		worldMat = DirectX::XMMatrixTranspose(worldMat);
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
		m_pDeviceContext->Map(render->constBufferWorld, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mappedResource);
		memcpy(mappedResource.pData, &worldMat, sizeof(DirectX::XMMATRIX));
		m_pDeviceContext->Unmap(render->constBufferWorld, 0);
		worldMat = DirectX::XMMatrixTranspose(worldMat);

		DirectX::XMMATRIX originalView = DirectX::XMMatrixIdentity();
		originalView.r[3] = DirectX::XMVectorSet(0, 5, -20, 1);
		DirectX::XMVECTOR determinant = DirectX::XMMatrixDeterminant(originalView);
		float aspectRatio = (m_viewport.Width / m_viewport.Height);
		SCENE sceneToShader;
		sceneToShader.viewMat = DirectX::XMMatrixInverse(&determinant, originalView);
		sceneToShader.viewMat = DirectX::XMMatrixTranspose(sceneToShader.viewMat);
		sceneToShader.perspectivMat = DirectX::XMMatrixPerspectiveFovLH(3.14159f / 3.0f, aspectRatio, 0.1f, 100.0f);
		sceneToShader.perspectivMat = DirectX::XMMatrixTranspose(sceneToShader.perspectivMat);
		DLIGHT dLight;
		dLight.lightColor = DirectX::XMFLOAT4(1, 1, 1, 1);
		dLight.lightDirection = DirectX::XMFLOAT3(0, 0, 1);
		dLight.ambient = 0;
		PLIGHT pLight;
		ZeroMemory(&pLight, sizeof(pLight));
		SLIGHT sLight;
		ZeroMemory(&sLight, sizeof(sLight));

		mappedResource;
		// view matrix buffer
		ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
		m_pDeviceContext->Map(render->constBufferScene, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mappedResource);
		memcpy(mappedResource.pData, &sceneToShader, sizeof(SCENE));
		m_pDeviceContext->Unmap(render->constBufferScene, 0);

		// directional light buffer
		ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
		m_pDeviceContext->Map(render->constBufferDLight, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mappedResource);
		memcpy(mappedResource.pData, &dLight, sizeof(DLIGHT));
		m_pDeviceContext->Unmap(render->constBufferDLight, 0);

		// point light buffer
		ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
		m_pDeviceContext->Map(render->constBufferPLight, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mappedResource);
		memcpy(mappedResource.pData, &pLight, sizeof(PLIGHT));
		m_pDeviceContext->Unmap(render->constBufferPLight, 0);

		// spot light buffer
		ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
		m_pDeviceContext->Map(render->constBufferSLight, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mappedResource);
		memcpy(mappedResource.pData, &sLight, sizeof(SLIGHT));
		m_pDeviceContext->Unmap(render->constBufferSLight, 0);


		for (unsigned int i = 0; i < geometryComponent->countIndexBuffer.size(); i++)
		{
			if (i < materialComponent->matCount) {
				LIGHTINFO lightInfo;
				// light information buffer
				DirectX::XMStoreFloat4(&lightInfo.specConstant, originalView.r[3]);
				lightInfo.specConstant.w = materialComponent->material_info[i][graphics::components::SHININESS].value[0];
				lightInfo.ambient = materialComponent->material_info[i][graphics::components::AMBIENT].value;
				lightInfo.diffuse = materialComponent->material_info[i][graphics::components::DIFFUSE].value;
				lightInfo.specular = materialComponent->material_info[i][graphics::components::SPECULAR].value;
				lightInfo.emissive = materialComponent->material_info[i][graphics::components::EMISSIVE].value;
				ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
				m_pDeviceContext->Map(render->constBufferLightInfo, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mappedResource);
				memcpy(mappedResource.pData, &lightInfo, sizeof(LIGHTINFO));
				m_pDeviceContext->Unmap(render->constBufferLightInfo, 0);

				// set texture info
				ID3D11ShaderResourceView *diffuseView[]{
					(ID3D11ShaderResourceView*)
					materialComponent->material_info[i][graphics::components::DIFFUSE].input.texture_resource
				};
				m_pDeviceContext->PSSetShaderResources(0, 1, diffuseView);
				ID3D11ShaderResourceView *specularView[]{
					(ID3D11ShaderResourceView*)
					materialComponent->material_info[i][graphics::components::SPECULAR].input.texture_resource
				};
				m_pDeviceContext->PSSetShaderResources(1, 1, specularView);
				ID3D11ShaderResourceView *emissiveView[]{
					(ID3D11ShaderResourceView*)
					materialComponent->material_info[i][graphics::components::EMISSIVE].input.texture_resource
				};
				m_pDeviceContext->PSSetShaderResources(2, 1, emissiveView);
				ID3D11ShaderResourceView *normalView[]{
					(ID3D11ShaderResourceView*)
					materialComponent->material_info[i][graphics::components::NORMAL].input.texture_resource
				};
				if (!normalView[0])
				{
					normalView[0] = (ID3D11ShaderResourceView*)materialComponent->material_info[i][graphics::components::BUMP].input.texture_resource;
				}
				m_pDeviceContext->PSSetShaderResources(3, 1, normalView);
			}

			// Set the index buffer.
			m_pDeviceContext->IASetVertexBuffers(0, 1, geometryComponent->vertexBuffer[i].GetAddressOf(), &stride, &offset);
			m_pDeviceContext->IASetIndexBuffer(geometryComponent->indexBuffers[i].Get(), DXGI_FORMAT_R32_UINT, 0);
			m_pDeviceContext->DrawIndexed(geometryComponent->countIndexBuffer[i], 0, 0);
		}
	}
}

void Render_World::ReadBin(const char * filename, ID3D11Device * m_pDevice, ID3D11DeviceContext * m_pDeviceContext, DirectX::XMFLOAT4 color)
{
	std::ifstream file(filename, std::ios::in | std::ios::binary);
	if (file.is_open())
	{
		std::cout << filename << std::endl;

		uint32_t vSize, iSize;

		size_t delimter = -1;
		for (size_t i = 0; i < strlen(filename); i++)
		{
			if (filename[i] == '\\' || filename[i] == '/')
				delimter = i;
		}
		// geometry info
		int32_t geoCount = -1;
		int32_t poseCount = -1;
		file.read((char*)&geoCount, sizeof(geoCount));
		file.read((char*)&poseCount, sizeof(poseCount));

		for (size_t geoIndex = 0; geoIndex < geoCount; geoIndex++)
		{
			file.read((char*)&vSize, sizeof(uint32_t));
			end::simple_vert *vertices = new end::simple_vert[vSize];
			file.read((char*)vertices, vSize * sizeof(end::simple_vert));
			file.read((char*)&iSize, sizeof(uint32_t));
			uint32_t *revIndices = new uint32_t[iSize];
			file.read((char*)revIndices, iSize * sizeof(uint32_t));

			// influences
			std::vector<graphics::animation::influence_set> control_point_influences;
			if (geoIndex < poseCount) {
				uint32_t numControlPoints = -1;
				file.read((char*)&numControlPoints, sizeof(uint32_t));
				control_point_influences.resize(numControlPoints);
				if (numControlPoints > 0)
					file.read((char*)&control_point_influences[0], sizeof(graphics::animation::influence_set) * numControlPoints);
				else
				{
					control_point_influences.resize(1);
					file.read((char*)&control_point_influences[0], sizeof(graphics::animation::influence_set) * numControlPoints);
					control_point_influences.clear();
				}

				std::cout << "\tanimation influences done" << std::endl;

				// Read bind pose joint
				size_t bindPosSize = 0;
				std::vector<graphics::animation::joint> bindPosejoints;
				file.read((char*)&bindPosSize, sizeof(size_t));
				if (bindPosSize > 0) {
					bindPosejoints.resize(bindPosSize);
					file.read((char*)&bindPosejoints[0], sizeof(graphics::animation::joint) * bindPosSize);
					animationComponent->bindPose = bindPosejoints;

					// Read animation clip
					size_t frameSize = 0;
					graphics::animation::anim_clip animationClip;
					file.read((char*)&animationClip.duration, sizeof(double));
					file.read((char*)&frameSize, sizeof(size_t));
					animationClip.frames.resize(frameSize);
					for (size_t i = 0; i < frameSize; i++)
					{
						size_t jointSize = 0;
						file.read((char*)&animationClip.frames[i].time, sizeof(double));
						file.read((char*)&jointSize, sizeof(size_t));
						animationClip.frames[i].joints.resize(jointSize);
						file.read((char*)&animationClip.frames[i].joints[0], sizeof(graphics::animation::joint) * jointSize);
					}
					animationComponent->animations.push_back(animationClip);
				}

				std::cout << "\tanimation bindpose and clip done" << std::endl;
			}

			geometryComponent->countVertexBuffer.push_back(vSize);
			geometryComponent->countIndexBuffer.push_back(iSize);

			SIMPLE_VERTEX *mVertices = new SIMPLE_VERTEX[vSize];
			for (uint32_t i = 0; i < vSize; i++)
			{
				mVertices[i].pos = DirectX::XMFLOAT3(vertices[i].pos[0], vertices[i].pos[1], vertices[i].pos[2]);
				mVertices[i].rgba = color;
				mVertices[i].normals = DirectX::XMFLOAT3(vertices[i].normals[0], vertices[i].normals[1], vertices[i].normals[2]);
				mVertices[i].tex = DirectX::XMFLOAT2(vertices[i].u, 1 - vertices[i].v);

				// normal mapping manipulator
				using namespace DirectX;
				DirectX::XMVECTOR normal = XMVector3Normalize(XMLoadFloat3(&mVertices[i].normals));
				XMVECTOR tangent1 = XMVector3Cross(XMVectorSet(0, 1, 0, 0), normal);
				XMVECTOR tangent2 = XMVector3Cross(XMVectorSet(0, 0, 1, 0), normal);
				XMVECTOR tangent = XMVECTOR();
				if (XMVector3Length(tangent1).m128_f32[0] >= XMVector3Length(tangent2).m128_f32[0])
				{
					tangent = tangent1;
				}
				else
				{
					tangent = tangent2;
				}
				DirectX::XMVECTOR binormal = DirectX::XMVector3Cross(normal, tangent);
				DirectX::XMStoreFloat3(&mVertices[i].tangents, tangent);
				DirectX::XMStoreFloat3(&mVertices[i].binomals, binormal);

				if (geoIndex < poseCount)
				{
					using namespace DirectX;
					mVertices[i].joints = DirectX::XMUINT4(control_point_influences[i][0].joint, control_point_influences[i][1].joint,
						control_point_influences[i][2].joint, control_point_influences[i][3].joint);

					DirectX::XMVECTOR tempWeight = DirectX::XMVectorSet(control_point_influences[i][0].weight, control_point_influences[i][1].weight,
						control_point_influences[i][2].weight, control_point_influences[i][3].weight);
					float sum = tempWeight.m128_f32[0] + tempWeight.m128_f32[1] + tempWeight.m128_f32[2] + tempWeight.m128_f32[3];
					tempWeight /= sum;
					XMStoreFloat4(&mVertices[i].weights, tempWeight);
				}
				else {
					mVertices[i].joints = DirectX::XMUINT4(0, 0, 0, 0);

					DirectX::XMVECTOR tempWeight = DirectX::XMVectorSet(0, 0, 0, 0);
					XMStoreFloat4(&mVertices[i].weights, tempWeight);
				}

				geometryComponent->vertices.push_back(mVertices[i]);
			}
			uint32_t *indices = new uint32_t[iSize];
			DirectX::XMVECTOR centroid = DirectX::XMVectorSet(0, 0, 0, 0);
			DirectX::XMFLOAT3 pos[3] = { DirectX::XMFLOAT3(0, 0, 0) };
			Triangle t;
			for (uint32_t i = 0; i < iSize; i++)
			{
				using namespace DirectX;
				indices[i] = revIndices[iSize - i - 1];
				geometryComponent->indices.push_back(indices[i]);

				centroid += DirectX::XMLoadFloat3(&mVertices[indices[i]].pos);
				pos[i % 3] = mVertices[indices[i]].pos;
				t.indices.push_back(indices[i]);
				if (i % 3 == 2) {
					centroid /= 3;
					DirectX::XMStoreFloat3(&t.centroid, centroid);

					DirectX::XMVECTOR U = DirectX::XMLoadFloat3(&pos[1]) - DirectX::XMLoadFloat3(&pos[0]);
					DirectX::XMVECTOR V = DirectX::XMLoadFloat3(&pos[2]) - DirectX::XMLoadFloat3(&pos[0]);
					DirectX::XMVECTOR W = DirectX::XMVector3Cross(U, V);
					W = DirectX::XMVector3Normalize(W);
					DirectX::XMStoreFloat3(&t.faceNormal, W);

					geometryComponent->triangles.push_back(t);
					centroid = DirectX::XMVectorSet(0, 0, 0, 0);
					t.indices.clear();
				}
			}

			// create all buffers
			D3D11_BUFFER_DESC bufferDesc;
			ZeroMemory(&bufferDesc, sizeof(bufferDesc));
			bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
			bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bufferDesc.ByteWidth = (UINT)(sizeof(SIMPLE_VERTEX) * geometryComponent->countVertexBuffer[geoIndex]);
			D3D11_SUBRESOURCE_DATA initData;
			ZeroMemory(&initData, sizeof(initData));
			initData.pSysMem = mVertices;
			geometryComponent->vertexBuffer.push_back(nullptr);
			m_pDevice->CreateBuffer(&bufferDesc, &initData, geometryComponent->vertexBuffer[geoIndex].GetAddressOf());


			// Fill in a buffer description.
			ZeroMemory(&bufferDesc, sizeof(bufferDesc));
			bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
			bufferDesc.ByteWidth = (UINT)(sizeof(unsigned int) * geometryComponent->countIndexBuffer[geoIndex]);
			bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			// Define the resource data.
			ZeroMemory(&initData, sizeof(initData));
			initData.pSysMem = indices;
			// Create the buffer with the device.
			geometryComponent->indexBuffers.push_back(nullptr);
			m_pDevice->CreateBuffer(&bufferDesc, &initData, geometryComponent->indexBuffers[geoIndex].GetAddressOf());

			delete[] vertices;
			delete[] mVertices;
			delete[] indices;
			delete[] revIndices;
		}


		std::cout << "\tgeometry done" << std::endl;

		// material info
		int matCount = 0;
		file.read((char*)&matCount, sizeof(int));

		graphics::material_t *in_material = new graphics::material_t[matCount];
		for (int j = 0; j < matCount; j++)
		{
			bool isPhong = false;
			bool isLambert = false;
			file.read((char*)&isPhong, sizeof(bool));
			file.read((char*)&isLambert, sizeof(bool));
			for (unsigned int i = 0; i < graphics::components::COUNT; i++)
			{
				file.read((char*)&in_material[j][i].value, sizeof(float4));
				size_t recvSize;
				file.read((char*)&recvSize, sizeof(size_t));
				if (recvSize != 0) {
					wchar_t * tempStr;
					if (delimter != -1) {
						size_t newsize = delimter + 2;
						tempStr = new wchar_t[newsize + recvSize];
						size_t convertedChars = 0;
						mbstowcs_s(&convertedChars, tempStr, delimter + 2, filename, _TRUNCATE);
						file.read((char*)&tempStr[delimter + 1], sizeof(wchar_t) * recvSize);
					}
					else {
						tempStr = new wchar_t[recvSize];
						file.read((char*)tempStr, sizeof(wchar_t) * recvSize);
					}

					in_material[j][i].input.file_path = tempStr;
				}
				else {
					in_material[j][i].input.file_path = nullptr;
				}
			}
		}

		std::cout << "\tmaterial done" << std::endl;

		file.close();

		worldMat = DirectX::XMMatrixIdentity();
		loadingDone = true;

		// Load Textures
		materialComponent->material_info = in_material;
		materialComponent->matCount = matCount;
		for (int i = 0; i < matCount; i++)
		{
			for (unsigned int j = 0; j < graphics::components::COUNT; j++)
			{
				if (in_material[i][j].input.file_path) {
					Microsoft::WRL::ComPtr<ID3D11Texture2D> tex = nullptr;
					Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv = nullptr;
					CreateWICTextureFromFile(m_pDevice, m_pDeviceContext, in_material[i][j].input.file_path,
						(ID3D11Resource**)tex.GetAddressOf(), srv.GetAddressOf());
					if (srv) {
						materialComponent->srv.push_back(srv);
						delete[] in_material[i][j].input.file_path;
						in_material[i][j].input.texture_resource = srv.Get();
					}
					else {
						delete[] in_material[i][j].input.file_path;
						in_material[i][j].input.texture_resource = nullptr;
					}
				}
			}
		}

		std::cout << filename << " content is in memory" << std::endl;
	}
	else std::cout << "Unable to open file" << std::endl;
}
