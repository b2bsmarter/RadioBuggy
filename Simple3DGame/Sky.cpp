#include "pch.h"
#include "Sky.h"


using namespace Simple3DGame;
using namespace DirectX;
using namespace std;


Sky::Sky(std::shared_ptr<DX::DeviceResources> pm_deviceResources)
{
	m_deviceResources = pm_deviceResources;
	m_Texture = nullptr;
	m_loadingComplete = false;
}


void Sky::Initialize()
{

	auto loadVSTask = DX::ReadDataAsync(L"SkyVertexShader.cso");
	auto loadPSTask = DX::ReadDataAsync(L"SkyPixelShader.cso");

	auto createVSTask = loadVSTask.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateVertexShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_vertexShader
				)
			);

		const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, /*0*/D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, /*24*/D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateInputLayout(
				vertexDesc,
				ARRAYSIZE(vertexDesc),
				&fileData[0],
				fileData.size(),
				&m_inputLayout
				)
			);
	});

	auto createPSTask = loadPSTask.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreatePixelShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_pixelShader
				)
			);
	});

	float box_size = 100.0f;

		VertexTex cubeVertices[] =
		{
			{ XMFLOAT3(box_size, box_size, -box_size), XMFLOAT2(1.0f, 1.0f - 0.75f) }, // - x
			{ XMFLOAT3(-box_size, box_size,-box_size), XMFLOAT2(0.6666666f, 1.0f - 0.75f) },
			{ XMFLOAT3(-box_size, box_size, box_size), XMFLOAT2(0.6666666f, 1.0f - 0.5f) },
			//2
			{ XMFLOAT3(box_size, box_size, -box_size), XMFLOAT2(1.0f, 1.0f - 0.75f) }, // - x
			{ XMFLOAT3(-box_size, box_size, box_size), XMFLOAT2(0.6666666f, 1.0f - 0.5f) },
			{ XMFLOAT3(box_size, box_size, box_size), XMFLOAT2(1.0f, 1.0f - 0.5f) },
			//3
			{ XMFLOAT3(-box_size, -box_size, box_size), XMFLOAT2(0.3333333f, 1.0f - 0.5f) }, // - x
			{ XMFLOAT3(box_size, -box_size, box_size),  XMFLOAT2(0.3333333f, 1.0f - 0.25f) },
			{ XMFLOAT3(box_size, box_size, box_size),   XMFLOAT2(0.6666666f, 1.0f - 0.25f) },
			//4
			{ XMFLOAT3(-box_size, -box_size, box_size), XMFLOAT2(0.3333333f, 1.0f - 0.5f) }, // - x
			{ XMFLOAT3(box_size, box_size, box_size),  XMFLOAT2(0.6666666f, 1.0f - 0.25f) },
			{ XMFLOAT3(-box_size, box_size, box_size),   XMFLOAT2(0.6666666f,1.0f - 0.5f) },
			//5
			{ XMFLOAT3(-box_size, -box_size, -box_size), XMFLOAT2(0.3333333f, 1.0f - 0.75f) }, // - x
			{ XMFLOAT3(box_size, -box_size, -box_size),  XMFLOAT2(0.0f,1.0f - 0.75f) },
			{ XMFLOAT3(-box_size, -box_size, box_size),   XMFLOAT2(0.3333333f, 1.0f - 0.5f) },
			//6
			{ XMFLOAT3(box_size, -box_size, -box_size), XMFLOAT2(0.0f, 1.0f - 0.75f) }, // - x
			{ XMFLOAT3(box_size, -box_size, box_size),  XMFLOAT2(0.0f, 1.0f - 0.5f) },
			{ XMFLOAT3(-box_size, -box_size, box_size),   XMFLOAT2(0.3333333f, 1.0f - 0.5f) },
			//7
			{ XMFLOAT3(-box_size, box_size, -box_size), XMFLOAT2(0.6666666f, 1.0f - 0.75f) }, // - x
			{ XMFLOAT3(box_size, box_size, -box_size),  XMFLOAT2(0.6666666f, 1.0f - 1.0f) },
			{ XMFLOAT3(-box_size, -box_size, -box_size),   XMFLOAT2(0.333333f, 1.0f - 0.75f) },
			//8
			{ XMFLOAT3(box_size, box_size, -box_size), XMFLOAT2(0.6666666f, 1.0f - 1.0f) }, // - x
			{ XMFLOAT3(box_size, -box_size, -box_size),  XMFLOAT2(0.3333333f, 1.0f - 1.0f) },
			{ XMFLOAT3(-box_size, -box_size, -box_size),   XMFLOAT2(0.3333333f, 1.0f - 0.75f) },
			//9
			{ XMFLOAT3(box_size, box_size, -box_size), XMFLOAT2(0.6666666f, 1.0f - 0.0f) }, // - x
			{ XMFLOAT3(box_size, box_size, box_size),  XMFLOAT2(0.6666666f, 1.0f - 0.25f) },
			{ XMFLOAT3(box_size, -box_size, -box_size),   XMFLOAT2(0.3333333f, 1.0f - 0.0f) },
			//10
			{ XMFLOAT3(box_size, box_size, box_size), XMFLOAT2(0.6666666f, 1.0f - 0.25f) }, // - x
			{ XMFLOAT3(box_size, -box_size, box_size),  XMFLOAT2(0.3333333f, 1.0f - 0.25f) },
			{ XMFLOAT3(box_size, -box_size, -box_size),   XMFLOAT2(0.3333333f, 1.0f - 0.0f) },
			//11
			{ XMFLOAT3(-box_size, box_size, -box_size), XMFLOAT2(0.6666666f, 1.0f - 0.75f) }, // - x
			{ XMFLOAT3(-box_size, -box_size, -box_size),  XMFLOAT2(0.3333333f, 1.0f - 0.75f) },
			{ XMFLOAT3(-box_size, box_size, box_size),   XMFLOAT2(0.6666666f, 1.0f - 0.5f) },
			//12
			{ XMFLOAT3(-box_size, -box_size, -box_size), XMFLOAT2(0.3333333f, 1.0f - 0.75f) }, // - x
			{ XMFLOAT3(-box_size, -box_size, box_size),  XMFLOAT2(0.3333333f, 1.0f - 0.5f) },
			{ XMFLOAT3(-box_size, box_size, box_size),   XMFLOAT2(0.6666666f, 1.0f - 0.5f) },
		};
		
		D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
		vertexBufferData.pSysMem = cubeVertices;
		vertexBufferData.SysMemPitch = 0;
		vertexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(cubeVertices), D3D11_BIND_VERTEX_BUFFER);
		DX::ThrowIfFailed(
		m_deviceResources->GetD3DDevice()->CreateBuffer(
		&vertexBufferDesc,
		&vertexBufferData,
		&m_vertexBuffer
		)
		);

		unsigned short cubeIndices[] =
		{
			0,1,2,
			3,4,5,
			6,7,8,
			9,10,11,
			12,13,14,
			15,16,17,
			18,19,20,
			21,22,23,
			24,25,26,
			27,28,29,
			30,31,32,
			33,34,35,

		};


		m_indexCount = ARRAYSIZE(cubeIndices);

		D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
		indexBufferData.pSysMem = cubeIndices;
		indexBufferData.SysMemPitch = 0;
		indexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC indexBufferDesc(sizeof(cubeIndices), D3D11_BIND_INDEX_BUFFER);
		DX::ThrowIfFailed(
		m_deviceResources->GetD3DDevice()->CreateBuffer(
		&indexBufferDesc,
		&indexBufferData,
		&m_indexBuffer
		)
		);
	

	m_loadingComplete = true;
	/*
	m_model = new ModelLoader(m_deviceResources);
	m_model->Load("Assets/Models/cube.obj", 170.0f,1);

	m_model->TransOrigin(0.0f, -4.0f, 0.0f);
	m_model->RotateY(3.141);

	SetBuffers(m_model->GetVertexBuffer(), m_model->GetIndexBuffer(), m_model->GetIndicesCount());
	*/
}

void Sky::ReleaseTexture()
{

	if (m_Texture != nullptr)
	{
		m_Texture->Release();
	}
}


void Sky::LoadLevel(int level)
{
	wchar_t texture_filename[40];
	wchar_t sky_filename[30];

	//if (level == 1)
	//	m_model->RotateY(3.141);

	mbstowcs(sky_filename, skybox,30);

	if (m_Texture != nullptr)
		m_Texture->Release();


	swprintf(texture_filename, 40, L"Assets/Skyboxes/Low/%s.dds", sky_filename);

	CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), texture_filename, nullptr, &m_Texture, MAXSIZE_T);
}


void Sky::SetBuffers(Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer, Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer, UINT indexCount)
{
	m_vertexBuffer = vertexBuffer;
	m_indexBuffer = indexBuffer;
	m_indexCount = indexCount;
}




void Sky::Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> deviceContext, Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer, ModelViewProjectionConstantBuffer* constantBufferData)
{
	UINT stride = sizeof(VertexTex);
	UINT offset = 0;
	//return;


	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	deviceContext->IASetInputLayout(m_inputLayout.Get());

	deviceContext->VSSetShader(
		m_vertexShader.Get(),
		nullptr,
		0
		);

	deviceContext->PSSetShader(
		m_pixelShader.Get(),
		nullptr,
		0
		);

	//deviceContext->UpdateSubresource(constantBuffer.Get(), 0, NULL, &m_constantBuffer, 0, 0);


	XMVECTOR data;
	XMVECTORF32 floatingVector = { 0.0f, 0.0f,0.0f,1.0f };
	data = floatingVector;

	auto rotationMatrix = XMMatrixRotationQuaternion(data);
	auto translationMatrix = XMMatrixTranslation(x, y, z);

	//	auto rotationMatrix = XMMatrixRotationRollPitchYaw(0.0f, 0.0f, 0.0f);
	//	auto translationMatrix = XMMatrixTranslation(0.0f, 6.0f, 20.0f);

	XMStoreFloat4x4(&m_constantBufferData.model, XMMatrixTranspose(rotationMatrix * translationMatrix));
	constantBufferData->model = m_constantBufferData.model;

	deviceContext->UpdateSubresource(constantBuffer.Get(), 0, NULL, constantBufferData, 0, 0);

	deviceContext->PSSetShaderResources(0, 1, &m_Texture);


	deviceContext->IASetVertexBuffers(
		0,
		1,
		m_vertexBuffer.GetAddressOf(),
		&stride,
		&offset
	);

	deviceContext->IASetIndexBuffer(
		m_indexBuffer.Get(),
		DXGI_FORMAT_R16_UINT, // Each index is one 16-bit unsigned integer (short).
		0
	);

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	// Draw the objects.
	deviceContext->DrawIndexed(
		m_indexCount,
		0,
		0
	);

}


Sky::~Sky(void)
{
}
