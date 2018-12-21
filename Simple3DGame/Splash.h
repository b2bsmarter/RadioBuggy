#pragma once
#include <vector>
#include "WorldObject.h"

//#include "GameHelpers.h"a
//#include "Constants.h"

#include "ModelLoader.h"

#include "../DirectXTK/Inc/DDSTextureLoader.h"

#include "Physics.h"

#include "Camera.h"

#include "Level.h"

#include "Player.h"




typedef struct
{
	int bActive;

	btVector3 pos;
	btVector3 mom;

	float life;
	float v1, v2; // texture offsey

	float angle, angle_mom;
	int flaketype;
	int stopped;
	float size;
	int start_delay;
	float nx, ny, nz;
	DirectX::XMFLOAT4 col;

} Splash_t;



namespace Simple3DGame
{

	class Splash : Sparticle
	{
	public:
		Splash(std::shared_ptr<DX::DeviceResources> pm_deviceResources) : Sparticle(pm_deviceResources) {};
		~Splash(void);

		part_index* p_index;

		Splash_t* spark;

		int total_pills;
		int total_collected;

		bool bUpdate;

		int current_index;

		Camera^ p_Camera;

		Player* p_Player;

		int current_point;

		Level* p_Level;

		float splash_lightness;
		void SetSplashLigtness(float _splash_lightness);

		int current_particle_que;


		float angle;
		float m_particleSize;

		void ActivateParticles(float posx, float posy, float posz);
		void Initialize(Level* pp_Level, Player* pp_Player, Camera^ pp_Camera);

		void Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> deviceContext, Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer, ModelViewProjectionConstantBuffer* constantBufferData);

		void Update(float timeTotal, float timeDelta);

		void CreateOne(float x, float y, float z, DirectX::XMFLOAT4 col, int start_delay);

		void CreateVerticies();

	};

}