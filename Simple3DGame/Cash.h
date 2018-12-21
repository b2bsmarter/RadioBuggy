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

#include "Points.h"

#include "Audio.h"

typedef struct
{
	int bActive;

	btVector3 pos;
	btVector3 mom;

	float life;
	float v1, v2; // texture offsey

	float angle, angle_mom;
	
	int stopped;
	float size;
	int start_delay;
	int type; // 0 = coin;

	DirectX::XMFLOAT4 col;

} Cash_t;



namespace Simple3DGame
{
	class Cash
	{
	public:
		Cash(std::shared_ptr<DX::DeviceResources> pm_deviceResources, Level* pp_Level, Player* pp_Player, Camera^ pp_Camera, Points* pp_Points, Audio* pp_Audio);
		~Cash(void);

		int m_maxParticles;

		part_index* p_index;

		Cash_t* spark;


		ModelLoader* m_model[10];
		ID3D11ShaderResourceView* m_Texture[10]; // up to 10 different things

		int total_pills;
		int total_collected;

		bool bUpdate;

		int current_index;

		Points* p_Points;

		Camera^ p_Camera;

		Player* p_Player;
		
		Audio* p_Audio;

		int current_point;

		Level* p_Level;

		MVPConstantBuffer m_constantBufferB;

		int current_particle_que;

		void Initialize();

		float angle;
		float m_particleSize;

		void ActivateParticles(float posx, float posy, float posz);

		bool LoadLocations();

		void CheckCollisionUpdate(float timeDelta);


		void Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> deviceContext, Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer, ModelViewProjectionConstantBuffer* constantBufferData);
		//void RenderRotating(ComPtr<ID3D11DeviceContext1> deviceContext, ComPtr<ID3D11Buffer> constantBuffer);
		void UpdateProjectionMatrix(DirectX::XMMATRIX *projectionMatrix);

		void Update(float timeTotal, float timeDelta);

		bool InitializeBuffers();

		void CreateOne(float x, float y, float z, int start_delay);



		Microsoft::WRL::ComPtr<ID3D11VertexShader> m_point_vertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> m_point_pixelShader;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> m_point_inputLayout;


		ParticleVertexType* m_vertices;


		int m_vertexCount, m_indexCount;

		ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;


		bool UpdateVertecies(ID3D11DeviceContext* deviceContext);
	private:
		std::shared_ptr<DX::DeviceResources> m_deviceResources;

	};

}