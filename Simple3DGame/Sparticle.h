#pragma once


namespace Simple3DGame
{
	class Sparticle
	{
	public:
		Sparticle(std::shared_ptr<DX::DeviceResources> pm_deviceResources);
		~Sparticle(void);

		int m_maxParticles;

		MVPConstantBuffer m_constantBufferB;

		std::shared_ptr<DX::DeviceResources> m_deviceResources;

		Microsoft::WRL::ComPtr<ID3D11VertexShader> m_point_vertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> m_point_pixelShader;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> m_point_inputLayout;

		ID3D11ShaderResourceView* m_Texture;


		VertexPositionTex* m_vertices;
		unsigned long* m_indices;

		int m_vertexCount, m_indexCount;

		ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;

		bool InitializeBuffers();
		bool UpdateVertecies(ID3D11DeviceContext* deviceContext);
		void Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> deviceContext, Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer, ModelViewProjectionConstantBuffer* constantBufferData);

	};

}
