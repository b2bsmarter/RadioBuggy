#pragma once

#include "../DirectXTK/Inc/DDSTextureLoader.h"
#include "WorldObject.h"

#include "Level.h"
#include "Statics.h"

#include "Textures.h"


namespace Simple3DGame
{
	typedef struct
	{
		int shape;
		float mass;
		float restitution;
		float friction;
		
		bool bAlpha;

		float physics_size; // 1.0f for full size

		ModelLoader* m_model;
		ID3D11ShaderResourceView* m_Texture;
	} phob_t;

	class Boxes
	{
	public:
		Boxes(std::shared_ptr<DX::DeviceResources> pm_deviceResources, Level* pp_Level, Physics* pp_physics, Camera^ pp_Camera, Statics* pp_Statics, Textures* pp_Textures);

		~Boxes(void);

		int m_maxBoxes;
		int m_totalBoxes;

		int current_level;
		int current_pill;
		int total_pills;

		void DeleteBoxes();

		Textures* p_Textures;

		phob_t phobject[20];

		static_t* statics; // turned into world objects on load

		concurrency::task<void> LoadModels();
		void LoadModel(int index, char* obj_filename, char* tex_filename, int shape, float scale, float mass, float restitution, float friction, bool _bAlpha, float _physics_size=1.0f);
		float GetWorldHeight(float posx, float posz);
		void SetDesignPointer(float x, float y, float z, int type);

		void SaveBinary(int level);
		bool LoadBinary(int level, int location);
		bool LoadLevel(int level, int location);

		void CreateOne(float x, float y, float z, int type);
		void MakeObjects();

		WorldObject* m_box[500]; // max 100;

		static_t design_pointer;
		
		float design_height;
		float design_height_f;
		bool bIgnoreHeight;
		bool bDesignPointer;
		int current_index;

		bool bHidePlacements;

		part_index* p_index;

		btRigidBody* p_player;

		Level* p_level;

		Physics* m_phys;

		Camera^ p_Camera;


		Statics* p_Statics;

		int current_item_id;

		void Update();

		void ExplodeArea(float x, float y, float z, float radius, float strength);

		void UpdateProjectionMatrix(DirectX::XMMATRIX *projectionMatrix);

		void Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> deviceContext, Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer, ModelViewProjectionConstantBuffer* constantBufferData, bool bRenderAlpha);

		bool LoadBoxLocations(int level, int phob_index, char* map_filename, float mass, float restitution, float friction, bool is_static);

		void LoadLevel(int level);

	private:
		std::shared_ptr<DX::DeviceResources> m_deviceResources;
	};

}