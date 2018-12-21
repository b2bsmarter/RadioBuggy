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

#include "ModelLoader.h"



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
	int type;

	DirectX::XMFLOAT4 col;

} Shoot_t;


typedef struct
{
	int bActive;
	int bCollide;

	btVector3 pos;
	btVector3 mom;
	btVector3 dir; // normal mom

	float life;
	float v1, v2; // texture offsey

	float angle, angle_mom;
	int flaketype;
	int stopped;
	float size;
	int start_delay;

	btVector3 point;

	DirectX::XMFLOAT4 col;

	DirectX::XMMATRIX p_dir;

} Arrow_t;



namespace Simple3DGame
{

	class Shoot
	{
	public:
		Shoot(std::shared_ptr<DX::DeviceResources> pm_deviceResources, Level* pp_Level, Player* pp_Player, Camera^ pp_Camera);
		~Shoot(void);

		int m_maxParticles;
		int m_maxArrows;

		part_index* p_index;

		Shoot_t* spark;

		Arrow_t* arrow;

		ModelLoader* m_model_rocket;
		ID3D11ShaderResourceView* m_Texture_rocket;

		int total_pills;
		int total_collected;

		bool bUpdate;

		int current_index;

		Camera^ p_Camera;

		Player* p_Player;

		int current_point;
		int current_arrow;

		Level* p_Level;

		MVPConstantBuffer m_constantBufferB;

		int current_particle_que;

		void Initialize();

		float angle;
		float m_particleSize;

		void ActivateParticles(float posx, float posy, float posz);

		bool LoadLocations();


		void Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> deviceContext, Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer, ModelViewProjectionConstantBuffer* constantBufferData);
		void RenderRockets(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> deviceContext, Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer, ModelViewProjectionConstantBuffer* constantBufferData);
		//void RenderRotating(ComPtr<ID3D11DeviceContext1> deviceContext, ComPtr<ID3D11Buffer> constantBuffer);
		void UpdateProjectionMatrix(DirectX::XMMATRIX *projectionMatrix);

		void Update(float timeTotal, float timeDelta);

		bool InitializeBuffers();

		void CreateOne(float x, float y, float z, float mom_x, float mom_y, float mom_z, int type);
		void CreateRocket(float x, float y, float z, float mom_x, float mom_y, float mom_z, int type, DirectX::XMFLOAT3X3 point_dir, float p_x, float p_y,float p_z);


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