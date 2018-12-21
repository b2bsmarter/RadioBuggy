#pragma once

//#include "Coin.h"


#include "ModelLoader.h"

#include "Terrain.h"
#include "Camera.h"
#include "../DirectXTK/Inc/DDSTextureLoader.h"

#pragma pack(2) // Add this

typedef struct
{
	unsigned short bfType;
	unsigned int   bfSize;
	unsigned short bfReserved1;
	unsigned short bfReserved2;
	unsigned int   bfOffBits;
} BITMAPFILEHEADER;

#pragma pack() // and this

typedef struct
{
	float x;
	float y;
	float z;
}norm_t;

namespace Simple3DGame
{
	class Level
	{
	public:
		Level(std::shared_ptr<DX::DeviceResources> pm_deviceResources, Physics* phys, int num, Camera^ p_camera);
		~Level(void);

		int total_planes;

		int left_wall_id;
		int right_wall_id;
		int front_wall_id;
		int back_wall_id;

		int current_index;

		float scale_height;

		float** stack_height; // used when objects are stacked
		float** shadows;		// used to work out shadows on terrain

							  /*
							  float3 player_pos;
							  */
		Camera^ m_camera;

		btRigidBody* m_FloorPlane;
		btRigidBody* m_GroundPlane;

		int map_x_size, map_y_size;

		int total_platforms;

		float left_right_walls;
		float front_back_walls;

		float back_position;

		float* cam_z;

		float** height_map;
		norm_t** normals;
		DirectX::XMFLOAT4** colours;

		float** ray_height_map;
		norm_t** stack_normals;

		float highest_point;
		float lowest_point;
		
		int total_x_points;
		int total_y_points;

		char ground1[30];
		char ground2[30];
		float noise_scale;

		//int total_walls_physical;

		//int ground_color;

		int x_planes;// = ((map_x_size) / 9);
		int y_planes;// = ((map_y_size) / 9);

		int lev_num;

		int clampB(int pX, int pMax);

		int current_level;

		//void InitializeTerrain( char* terrain_filename,wchar_t* texture_filename, Physics* phys , float scale);

		void InitializeTerrain(int level, float height_scale, float terrain_scale);
		void Update(DirectX::XMMATRIX *viewMatrix, float timeTotal);

		void Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> deviceContext, Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer, ModelViewProjectionConstantBuffer* constantBufferData);

		float GetTerrainHeight(float xPos, float zPos);
		void ResetStack();

		void CalculateStackNormals();

		void SetupTerrainNormals();
		void SetupTerrainColours();

		float RayGetTerrainHeight(float xPos, float zPos);

		btVector3 RayGetNormal(float xPos, float zPos);

		void SaveRayBinary(int level);
		bool LoadRayBinary(int level, int location);

		void SaveShadowBinary(int level);
		bool LoadShadowBinary(int level, int location);
		void SetLevelLocation(int level, int location);

		void ResetShadows();

		void MakePhysics();

		btVector3 Level::GetNormal(float xPos, float zPos);

		void SetFriction(float fric);
		void SetRestitution(float rest);

		void ClearMemory();

		Terrain* m_Terrain[500];

		int current_location;
		bool bLoadedShadows;

		HeightMapInfo m_hminfo;

		Physics* m_phys;
		float m_terrain_scale;

		int fin_state;

		ID3D11ShaderResourceView* m_Texture;
		ID3D11ShaderResourceView* m_Texture_2;
		ID3D11ShaderResourceView* m_TextureFinish;
		ID3D11ShaderResourceView* m_TextureFinishNot;
		ID3D11ShaderResourceView* m_TextureFinishOK;


		bool HeightMapLoad(char* filename, float height_scale);
	private:
		std::shared_ptr<DX::DeviceResources> m_deviceResources;
	};

}