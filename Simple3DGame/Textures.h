#pragma once
#include "../DirectXTK/Inc/DDSTextureLoader.h"



namespace Simple3DGame
{
	typedef struct
	{
		ID3D11ShaderResourceView* m_Texture;
		char filename[30];

	} texture_t;



	class Textures
	{
	public:
		Textures(std::shared_ptr<DX::DeviceResources> pm_deviceResources, bool _bHighRez);
		~Textures();

		int current;
		bool bHighRez;

		ID3D11ShaderResourceView* LoadTexture(char* filename); // returns a pointer if already loaded

		texture_t textures[50];

	private:
		std::shared_ptr<DX::DeviceResources> m_deviceResources;
	};
}
