#pragma once

namespace Simple3DGame
{
	class Lights
	{
	public:
		Lights::Lights(std::shared_ptr<DX::DeviceResources> pm_deviceResources);

		std::shared_ptr<DX::DeviceResources> m_deviceResources;

		int num_point_lights;

		void SetLights1();
		
		void SetSpecular(float level);
		void SetNumTextures(int _texs);

		void ResetPoints() { num_point_lights = 0; m_lightBufferData.numLights = num_point_lights; };

		void Addpoint(
			DirectX::XMFLOAT4 ambient,
			DirectX::XMFLOAT4 diffuse,
			DirectX::XMFLOAT4 specular,
			DirectX::XMFLOAT3 pos,
			float radius);

		void Lights::AddSpot(
			DirectX::XMFLOAT4 ambient,
			DirectX::XMFLOAT4 diffuse,
			DirectX::XMFLOAT4 specular,
			DirectX::XMFLOAT3 pos,
			DirectX::XMFLOAT3 dir,
			float spot,
			float radius);

		void SetFog(DirectX::XMFLOAT4 col);

		void Lights::UpdateBuffer(Microsoft::WRL::ComPtr<ID3D11Buffer>	p_lightBuffer);

		LightBufferType m_lightBufferData;

	};
}

