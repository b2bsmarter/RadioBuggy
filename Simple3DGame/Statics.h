#pragma once
#include <vector>
#include "WorldObject.h"

//#include "MarbleMaze.h"

//#include "GameHelpers.h"
//#include "Constants.h"

#include "ModelLoader.h"

#include "../DirectXTK/Inc/DDSTextureLoader.h"
//#include "Physics.h"

//#include "VertexData.h"

#include "Points.h"

#include "Camera.h"

#include "Level.h"

#include "Player.h"

#include "Audio.h"

#include "Dots.h"

#include "Textures.h"

//#include "CubeRenderer.h"

using namespace std;





namespace Simple3DGame
{
	typedef struct
	{
	public:
		int bActive;
		float x, y, z;
		float rot_x,rot_y,rot_z;
		DirectX::XMFLOAT4X4 model_matrix;

		float col_r, col_g, col_b; // color weights



		int type;
	} static_t;



	typedef struct
	{
		ModelLoader* m_solid_model;
		ModelLoader* m_alpha_model;

		ID3D11ShaderResourceView* m_solid_texture;
		ID3D11ShaderResourceView* m_alpha_texture;

		float trunk_radius;

		bool bSolid; // has sold
		bool bAlpha; // has alpha

	} static_model_t;

	class Statics
	{
	public:
		Statics(std::shared_ptr<DX::DeviceResources> pm_deviceResources, Level* pp_Level, Player* pp_Player, Camera^ pp_Camera, Points* pp_Points, Physics* pp_physics, Textures* pp_Textures);
		~Statics(void);

		int m_maxParticles;

		int moveable_models[10];

		bool bIgnoreHeight;

		part_index* p_index;
		part_index* p_index_car;

		static_t* statics;

		WorldObject* static_ob[10][50];

		int numbs_of_physical[10];

		Textures* p_Textures;

		static_t design_pointer;

		static_model_t static_model[10];

		int shader_mode;

		float design_height;
		float design_height_f;

		int total_static_physical;

		concurrency::task<void> LoadModels();

		void LoadTreeModel(int index, bool has_solid, bool has_alpha, char* solid_obj_filename, char* alpha_obj_filename, char* solid_tex_fname, char* alpha_tex_fname, float scale, float _trunk_radius);

		void SetDesignPointer(float x, float y, float z, int type);
		void SetDesignPointer(float x, float y, float z, float _y, float _p, float _r, int type);
		void CreateOne(float x, float y, float z, int type);

		void SaveBinary(int level);
		bool LoadBinary(int level, int location);

		void MakeLocal(float x, float z);

		float GetStaticWorldHeight(float posx, float posz);

		void MakeAllObjects();
		void ScatterPhysical();

		void LoadPhysical();

		void SetShaderMode(int mode) { shader_mode = mode; };

		int update_frame; // make static after 2 frames
		int total_pills;
		int total_collected;

		bool bUpdate;

		bool bDesignPointer;

		bool bBarrelsLoaded;

		float* cam_z;

		int current_level;

		int current_index;

		Camera^ p_Camera;

		Player* p_Player;

		Points* p_Points;

		Dots* p_dots;

		int current_pill;

		Level* p_Level;

		MVPConstantBuffer m_constantBufferB;

		WorldObject* m_box[1000]; // max 100;

		Physics* m_phys;
		int m_totalBoxes;

		int current_particle_que;

		void Initialize();

		float angle;
		float m_particleSize;

		void ActivateParticles(float posx, float posy, float posz);

		void Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> deviceContext, Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer, ModelViewProjectionConstantBuffer* constantBufferData, int part_to_render);
		//void RenderRotating(ComPtr<ID3D11DeviceContext1> deviceContext, ComPtr<ID3D11Buffer> constantBuffer);
		void UpdateProjectionMatrix(DirectX::XMMATRIX *projectionMatrix);

		void Update(DirectX::XMMATRIX *viewMatrix, float timeTotal);

		float min_dist(float x, float y, float x1, float y1, float x2, float y2);


		bool InitializeBuffers();

		bool LoadLevel(int level, int location);

		Microsoft::WRL::ComPtr<ID3D11Device1> m_d3dDevice;

		//std::unique_ptr<DirectX::SoundEffectInstance>   m_pop_sound;
			private:
				std::shared_ptr<DX::DeviceResources> m_deviceResources;

	};
}