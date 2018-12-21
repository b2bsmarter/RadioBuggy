#pragma once
#include <vector>

//#include "Constants.h"

#include "ModelLoader.h"

#include "DDSTextureLoader.h"
#include "Physics.h"

//#include "OneParticle.h"

#include "Camera.h"

#include "Level.h"

#include "Player.h"

//#include "Particles.h"

#include "Points.h"

#include "Audio.h"

using namespace std;


typedef struct
{
	int bActive;
	float x, y, z;
	float frame;
	float size;

} explode_t;

typedef struct
{
	int bActive;
	float x, y, z;
	float pos;
	float strength;

} exp_light_t;

namespace Simple3DGame
{

	class Explode
	{
	public:
		Explode(Microsoft::WRL::ComPtr<ID3D11Device1> pm_d3dDevice, Level* pp_Level, Player* pp_Player, Camera^ pp_Camera);
		~Explode(void);

		int m_maxParticles;

		part_index* p_index;

		explode_t* explodes;

		exp_light_t light[12];

		int total_lights;

		int total_pills;
		int total_collected;

		bool bUpdate;

		float* cam_z;

		int current_index;

		Camera^ p_Camera;

		Player* p_Player;

		int current_pill;

		Level* p_Level;

		MVPConstantBuffer m_constantBufferB;

		int current_particle_que;

		void Initialize();

		float angle;
		float m_particleSize;

		void ActivateParticles(float posx, float posy, float posz);

		void CheckCollision();

		void Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> deviceContext, Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer, ModelViewProjectionConstantBuffer* constantBufferData);
		//void RenderRotating(ComPtr<ID3D11DeviceContext1> deviceContext, ComPtr<ID3D11Buffer> constantBuffer);
		void UpdateProjectionMatrix(DirectX::XMMATRIX *projectionMatrix);

		void Update(float timeTotal, float timeDelta);

		bool InitializeBuffers();

		void CreateOne(float x, float y, float z);


		Microsoft::WRL::ComPtr<ID3D11Device1> m_d3dDevice;

		Microsoft::WRL::ComPtr<ID3D11VertexShader> m_point_vertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> m_point_pixelShader;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> m_point_inputLayout;

		ID3D11ShaderResourceView* m_Texture;

		ParticleVertexType* m_vertices;


		int m_vertexCount, m_indexCount;

		ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;


		bool UpdateVertecies(ID3D11DeviceContext* deviceContext);
	private:
		std::shared_ptr<DX::DeviceResources> m_deviceResources;


	};
}

