#pragma once
#include <vector>
#include "WorldObject.h"


//#include "Constants.h"

#include "ModelLoader.h"

#include "../DirectXTK/Inc/DDSTextureLoader.h"
#include "Physics.h"

//#include "OneParticle.h"

//#include "VertexData.h"

#include "Camera.h"

#include "Level.h"

#include "Player.h"

//#include "Particles.h"


#include "Audio.h"

using namespace std;

namespace Simple3DGame
{
	class Fence
	{
	public:
		Fence(Microsoft::WRL::ComPtr<ID3D11Device1> pm_d3dDevice, Level* pp_Level, Camera^ pp_Camera);
		~Fence(void);

		int m_maxParticles;

		part_index* p_index;

		pill_t* pills;

		//Particles* p_particles;

		int total_pills;
		int total_collected;

		int cur_pop_sound;

		float light_position;
		float last_time;

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

		//float angle;
		float m_particleSize;

		void ActivateParticles(float posx, float posy, float posz);

		bool LoadLevel(int level);
		bool LoadLocations(int level);

		void Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> deviceContext, Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer, ModelViewProjectionConstantBuffer* constantBufferData);

		bool InitializeBuffers();

		Microsoft::WRL::ComPtr<ID3D11Device1> m_d3dDevice;

		Microsoft::WRL::ComPtr<ID3D11VertexShader> m_point_vertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> m_point_pixelShader;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> m_point_inputLayout;

		ID3D11ShaderResourceView* m_Texture;

		//ParticleVertexType* m_vertices;

		VertexPositionTex* m_vertices;

		int m_vertexCount, m_indexCount;

		ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;

		bool UpdateVertecies(ID3D11DeviceContext* deviceContext);
	private:
		std::shared_ptr<DX::DeviceResources> m_deviceResources;


	};
}