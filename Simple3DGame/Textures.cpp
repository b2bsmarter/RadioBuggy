#include "pch.h"
#include "Textures.h"

using namespace Simple3DGame;
using namespace DirectX;
using namespace DX;

Textures::Textures(std::shared_ptr<DX::DeviceResources> pm_deviceResources, bool _bHighRez)
{
	int i;
	current = 0;
	bHighRez = _bHighRez;

	m_deviceResources = pm_deviceResources;


}

Textures::~Textures()
{
	// Release all textures

}


ID3D11ShaderResourceView* Textures::LoadTexture(char* filename)
{
	int i;
	bool bFound;

	char full_filename[60];
	wchar_t w_full_filename[60];

	for (i = 0; i < current; i++)
	{
		if (!strcmp(textures[i].filename, filename))
		{
			bFound = true;
			return textures[i].m_Texture;
		}
	}

	if (bHighRez == false)
	{
		sprintf(full_filename, "Assets/Textures/Low/%s", filename);
	}
	else
	{
		sprintf(full_filename, "Assets/Textures/%s", filename);
	}
	mbstowcs(w_full_filename, full_filename, 60);

	CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), w_full_filename, nullptr, &textures[current].m_Texture, MAXSIZE_T);

	strcpy(textures[current].filename, filename);

	current++;

	return textures[current-1].m_Texture;
}
