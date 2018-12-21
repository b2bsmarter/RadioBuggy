#pragma once
#include <vector>
//#include "Object3D.h"

//#include "MarbleMaze.h"

//#include "GameHelpers.h"
//#include "Constants.h"
#include "SoundEffect.h"
#include "ModelLoader.h"

#include "../DirectXTK/Inc/DDSTextureLoader.h"
//#include "Physics.h"

//#include "VertexData.h"

#include "Points.h"

#include "Camera.h"

#include "Level.h"

#include "Player.h"

#include "Audio.h"

#include "Cash.h"

#include "Textures.h"

//#include "Splash.h"

//#include "CubeRenderer.h"

using namespace std;


typedef struct
{
	int bActive;
	int bCollected; // rings
	int ring_num; // ring number
	float x, y, z;
	float angle;
	float anglex[3];
	float anglez[3];
	float scale;
	DirectX::XMFLOAT4X4 model;
	DirectX::XMMATRIX rotation;
	int type;
	float extra_spin;
	float yaw, pitch, roll;

	bool bAlpha;

} fruit_t;


namespace Simple3DGame
{

	class Fruit
	{
	public:
		Fruit(std::shared_ptr<DX::DeviceResources> pm_deviceResources, Level* pp_Level, Player* pp_Player, Camera^ pp_Camera, Points* pp_Points, Cash* pp_Cash, Textures* pp_Textures);
		~Fruit(void);

		int m_maxParticles;

		part_index* p_index;

		ModelLoader* m_model[11];

		ModelLoader* m_numb[10]; // numbers

		ModelLoader* m_Beam;

		ModelLoader* m_Sphere;

		bool bDesignMode;

		Textures* p_Textures;

		Audio* p_audio;

		fruit_t design_pointer;

		Cash* p_Cash;

		fruit_t* fruit;
		//Splash* p_Splash;

		//	Audio* p_Audio;

		//	Stage* p_Stage;

		//MarbleMaze^ p_Maze;
		//ComPtr<MarbleMaze>           p_Maze;

		int total_pills;
		int total_collected;
		int current_level;
		int current_ring_num;

		int total_rings;
		int rings_collected;
		int current_ring_to_collect;

		float beam_posx;
		float beam_posy;
		float beam_posz;

		bool bUpdate;

		bool bDesignPointer;

		float* cam_z;

		SoundEffect^ m_SFPop;

		int current_index;

		Camera^ p_Camera;

		Player* p_Player;

		Points* p_Points;

		int current_pill;

		Level* p_Level;

		MVPConstantBuffer m_constantBufferB;

		int current_particle_que;

		int current_point;

		void Initialize();

		float angle;
		float m_particleSize;

		void SetBeamColor(float r, float g, float b);

		void CreateOne(float x, float y, float z, int type);
		void CreateOneMatrix(float x, float y, float z, int type, DirectX::XMMATRIX* _rotation);

		void SetDesignPointer(float x, float y, float z, int type);
		void SetDesignPointer(float x, float y, float z, float _yaw, float _pitch, float _roll, int type);

		void ActivateParticles(float posx, float posy, float posz);

		bool LoadLocations(int level);

		void CheckCollisionUpdate();

		bool LoadBinary(int level,int location);
		void SaveBinary(int level);

		void Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> deviceContext, Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer, ModelViewProjectionConstantBuffer* constantBufferData);
		void RenderBeam(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> deviceContext, Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer, ModelViewProjectionConstantBuffer* constantBufferData);
		//void RenderRotating(ComPtr<ID3D11DeviceContext1> deviceContext, ComPtr<ID3D11Buffer> constantBuffer);
		void UpdateProjectionMatrix(DirectX::XMMATRIX *projectionMatrix);

		void Update(float timeDelta);

		float min_dist(float x, float y, float x1, float y1, float x2, float y2);


		bool InitializeBuffers();

		bool LoadLevel(int level, int location);

		ID3D11ShaderResourceView* m_BeamTexture;

		ID3D11ShaderResourceView* m_CrystalTexture;

		ID3D11ShaderResourceView* m_Texture[11];
		//ID3D11ShaderResourceView* m_Rin
		bool UpdateVertecies(ID3D11DeviceContext* deviceContext);

		//std::unique_ptr<DirectX::SoundEffectInstance>   m_pop_sound;

	private:
		std::shared_ptr<DX::DeviceResources> m_deviceResources;
	};
}