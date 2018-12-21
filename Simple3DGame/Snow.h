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

#include "sparticle.h"

#include "PerlinNoise.h"


typedef struct
{
	int bActive;
	float x, y, z;

	float life;
	float v1, v2; // texture offsey
	float vel_x, vel_y, vel_z;
	float angle, angle_mom;
	int flaketype;
	int stopped;

} snow_t;



namespace Simple3DGame
{

	class Snow : Sparticle
	{
	public:
		Snow(std::shared_ptr<DX::DeviceResources> pm_deviceResources) : Sparticle(pm_deviceResources) {};
		~Snow(void);

		part_index* p_index;

		snow_t* snow;

		int total_pills;
		int total_collected;

		bool bUpdate;

		int current_index;

		Camera^ p_Camera;

		Player* p_Player;

		int current_point;

		Level* p_Level;

		PerlinNoise* pn;

		float noise_position;

		int current_particle_que;

		void Initialize(Level* pp_Level, Player* pp_Player, Camera^ pp_Camera);

		float angle;
		float m_particleSize;


		void Reset();

		void Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> deviceContext, Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer, ModelViewProjectionConstantBuffer* constantBufferData);
		//void RenderRotating(ComPtr<ID3D11DeviceContext1> deviceContext, ComPtr<ID3D11Buffer> constantBuffer);
		void UpdateProjectionMatrix(DirectX::XMMATRIX *projectionMatrix);

		void Update(float timeTotal, float timeDelta);

		void CreateOne(float x, float y, float z);




		void CreateVerticies();

	};

}