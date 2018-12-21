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

#include "modicle.h"

//#include "CubeRenderer.h"

using namespace std;





namespace Simple3DGame
{
	typedef struct
	{
	public:
		int bActive;
		float x, y, z;
		float angle;
		DirectX::XMFLOAT4X4 model_matrix;
		
		float yaw, pitch, roll;

		bool bUpdateCols;

		int num_cols;

		int type;
	} tree_t;


	class TreeBody : public btMotionState
	{
	public:
		TreeBody();

		float radius;
		float length;
		btRigidBody* m_body;

		btTransform m_initialTransform;

		void SetTransform(const Transform^ transform);

		void getWorldTransform(btTransform &worldTransform) const;
		void setWorldTransform(const btTransform &worldTransform);

		void SetPosition(float x, float y, float z, float yaw, float pitch, float roll);
	};

	typedef struct
	{
		ModelLoader* m_solid_model;
		ModelLoader* m_alpha_model;

		ID3D11ShaderResourceView* m_solid_texture;
		ID3D11ShaderResourceView* m_alpha_texture;

		bool bLeaves;
		bool bTrunk;

		float trunk_radius;

	} tree_model_t;

	class Trees
	{
	public:
		Trees(std::shared_ptr<DX::DeviceResources> pm_deviceResources, Level* pp_Level, Player* pp_Player, Camera^ pp_Camera, Points* pp_Points, Physics* pp_physics);
		~Trees(void);

		int m_maxParticles;

		bool bDesignPointer;
		bool bDesignMode;

		part_index* p_index;
		part_index* p_index_car;

		tree_t* tree;

		TreeBody* treebody;
		WorldObject* barrel[10];

		DirectX::XMFLOAT4** cols;
		bool bTreesLoaded;


		Modicle* m_Grass;

		Modicle* m_Tree0;
		Modicle* m_Tree1;

		tree_t design_pointer;

		tree_model_t tree_model[10];
		void LoadTreeModel(int index, char* solid_obj_filename, char* alpha_obj_filename, wchar_t* solid_tex_fname, wchar_t* alpha_tex_fname, float scale, float _trunk_radius, bool bTrunk, bool bLeaves);

		void UpdateGrass();
		void RenderGrass(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> deviceContext, Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer, ModelViewProjectionConstantBuffer* constantBufferData, int part_to_render);


		void SetShaderMode(int mode) { shader_mode = mode; };
		int total_trees_physical;

		int season; // season    0 - summer   1 - autum   2 - winter  3 - spring

		void SetDesignPointer(float x, float y, float z, int type);
		void SetDesignPointer(float x, float y, float z, float _y, float _p, float _r, int type);
		void CreateOne(float x, float y, float z, int type);

		void TreeLights();

		void SaveBinary(int level);
		bool LoadBinary(int level, int location);

		void GenerateTreeShades(int tree_num, int type = 0);

		int update_frame; // make static after 2 frames
		int total_pills;
		int total_collected;

		int shader_mode;


		bool bUpdate;

		bool bBarrelsLoaded;

		bool bRotateRay;

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

		int current_particle_que;

		void Initialize();

		float angle;
		float m_particleSize;

		void ActivateParticles(float posx, float posy, float posz);

		bool LoadLevel(int level, int location);

		void Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> deviceContext, Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer, ModelViewProjectionConstantBuffer* constantBufferData, int part_to_render);
		//void RenderRotating(ComPtr<ID3D11DeviceContext1> deviceContext, ComPtr<ID3D11Buffer> constantBuffer);
		void UpdateProjectionMatrix(DirectX::XMMATRIX *projectionMatrix);

		void Update(DirectX::XMMATRIX *viewMatrix, float timeTotal);

		float min_dist(float x, float y, float x1, float y1, float x2, float y2);


		bool InitializeBuffers();

		//std::unique_ptr<DirectX::SoundEffectInstance>   m_pop_sound;
	private:
		std::shared_ptr<DX::DeviceResources> m_deviceResources;

	};
}