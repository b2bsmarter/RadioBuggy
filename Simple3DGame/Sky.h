#pragma once
#include <vector>

#include "../DirectXTK/Inc/DDSTextureLoader.h"


#include "Camera.h"

#include "modelloader.h"


namespace Simple3DGame
{

	class Sky
	{
	public:
		Sky(std::shared_ptr<DX::DeviceResources> pm_deviceResources);
		~Sky(void);

		float angle;

		float x, y, z;
		bool m_loadingComplete;

		ModelLoader* m_model;

		ModelViewProjectionConstantBuffer	m_constantBufferData;

		void SetBuffers(Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer, Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer, UINT indexCount);

		void LoadLevel(int level);

		int m_indexCount;
		char skybox[30];

		void Initialize();

		void Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> deviceContext, Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer, ModelViewProjectionConstantBuffer* constantBufferData);

		void ReleaseTexture();

		Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;

		ID3D11ShaderResourceView* m_Texture;

		Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
	private:
		std::shared_ptr<DX::DeviceResources> m_deviceResources;

	};

}