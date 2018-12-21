#pragma once
#include <vector>
#include "WorldObject.h"

//#include "GameHelpers.h"
//#include "Constants.h"

#include "ModelLoader.h"

#include "../DirectXTK/Inc/DDSTextureLoader.h"

#include "Physics.h"

#include "Camera.h"

#include "Level.h"

#include "Player.h"
#include "Splash.h"

#include "PerlinNoise.h"


typedef struct
{
	int bActive;
	float x, y, z;

	float life;
	float v1, v2; // texture offsey
	float vel_x, vel_y, vel_z;
	float nx, ny, nz;
	float angle, angle_mom;
	float floor_height;

	int flaketype;
	int stopped;

} rain_t;



namespace Simple3DGame
{

	class Rain : Sparticle
	{
	public:
		Rain(std::shared_ptr<DX::DeviceResources> pm_deviceResources) : Sparticle(pm_deviceResources) {};
		~Rain(void);


		part_index* p_index;

		rain_t* snow;

		Splash* p_Splash;

		void Reset();

		int total_pills;
		int total_collected;

		bool bUpdate;

		int current_index;

		Camera^ p_Camera;

		Player* p_Player;

		PerlinNoise* pn;

		int current_point;

		float rain_lightness;

		float noise_position;

		Level* p_Level;

		MVPConstantBuffer m_constantBufferB;

		int current_particle_que;

		void Initialize();

		void SetRainLigtness(float _rain_lightness);

		float angle;
		float m_particleSize;

		void ActivateParticles(float posx, float posy, float posz);

		bool LoadLocations();
		
		void Initialize(Level* pp_Level, Player* pp_Player, Camera^ pp_Camera);


		void Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> deviceContext, Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer, ModelViewProjectionConstantBuffer* constantBufferData);
		//void RenderRotating(ComPtr<ID3D11DeviceContext1> deviceContext, ComPtr<ID3D11Buffer> constantBuffer);
		void UpdateProjectionMatrix(DirectX::XMMATRIX *projectionMatrix);
		bool CreateVerticies();

		void Update(float timeTotal, float timeDelta);


		void CreateOne(float x, float y, float z);



	};

}