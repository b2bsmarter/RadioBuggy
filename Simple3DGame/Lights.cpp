#include "pch.h"
#include "Lights.h"


using namespace Simple3DGame;

using namespace concurrency;
using namespace DirectX;
using namespace Windows::Foundation;
using namespace Microsoft::WRL;


Lights::Lights(std::shared_ptr<DX::DeviceResources> pm_deviceResources)
{
	m_deviceResources = pm_deviceResources;

}


void Lights::SetLights1()
{
	m_lightBufferData.ambientColor = XMFLOAT4(0.55f, 0.55f, 0.55f, 1.0f);
	m_lightBufferData.diffuseColor = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_lightBufferData.lightDirection = XMFLOAT3(-0.5f, -0.5f, 0.5f);
	m_lightBufferData.specularColor = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);

	num_point_lights = 0;
}

void Lights::SetFog(DirectX::XMFLOAT4 col)
{
	m_lightBufferData.fogColor = col;
}


void Lights::SetSpecular(float level)
{
	m_lightBufferData.specularPower = level;
}

void Lights::UpdateBuffer(Microsoft::WRL::ComPtr<ID3D11Buffer>	p_lightBuffer)
{
	m_deviceResources->GetD3DDeviceContext()->UpdateSubresource(p_lightBuffer.Get(), 0, NULL, &m_lightBufferData, 0, 0);
}

void Lights::SetNumTextures(int _texs)
{
	//m_lightBufferData.numTextures = _texs;

}

void Lights::Addpoint(
	DirectX::XMFLOAT4 ambient,
	DirectX::XMFLOAT4 diffuse,
	DirectX::XMFLOAT4 specular,
	DirectX::XMFLOAT3 pos,
	float radius)
{
	if (num_point_lights == MAX_POINT_LIGHTS)
		return;

	m_lightBufferData.lights[num_point_lights].ambient = ambient;
	m_lightBufferData.lights[num_point_lights].diffuse = diffuse;
	m_lightBufferData.lights[num_point_lights].specular = specular;
	m_lightBufferData.lights[num_point_lights].pos = pos;
	m_lightBufferData.lights[num_point_lights].radius = radius;
	m_lightBufferData.lights[num_point_lights].spot = 0;
	num_point_lights++;
	m_lightBufferData.numLights = num_point_lights;

}

void Lights::AddSpot(
	DirectX::XMFLOAT4 ambient,
	DirectX::XMFLOAT4 diffuse,
	DirectX::XMFLOAT4 specular,
	DirectX::XMFLOAT3 pos,
	DirectX::XMFLOAT3 dir,
	float spot,
	float radius)
{
	if (num_point_lights == MAX_POINT_LIGHTS)
		return;

	m_lightBufferData.lights[num_point_lights].ambient = ambient;
	m_lightBufferData.lights[num_point_lights].diffuse = diffuse;
	m_lightBufferData.lights[num_point_lights].specular = specular;
	m_lightBufferData.lights[num_point_lights].pos = pos;
	m_lightBufferData.lights[num_point_lights].radius = radius;
	m_lightBufferData.lights[num_point_lights].spot = spot;
	m_lightBufferData.lights[num_point_lights].dir = dir;
	num_point_lights++;
	m_lightBufferData.numLights = num_point_lights;

}

