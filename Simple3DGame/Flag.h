#pragma once

//#include "Coin.h"


#include "ModelLoader.h"

#include "FlagNode.h"
#include "Camera.h"


#include "../DirectXTK/Inc/DDSTextureLoader.h"

/*
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
*/


namespace Simple3DGame
{

	class Flag
	{
	public:
		Flag(std::shared_ptr<DX::DeviceResources> pm_deviceResources);
		~Flag(void);



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

		int x_planes;// = ((map_x_size) / 9);
		int y_planes;// = ((map_y_size) / 9);

		int lev_num;

		int clampB(int pX, int pMax);

		int current_level;

		void InitializeGeneratedTerrain(int level, int x_nodes, int y_nodes, float height_scale, float terrain_scale, float noise_z);
		void UpdateGeneratedTerrain(int level, float noise_z);



		void Render(Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer, ModelViewProjectionConstantBuffer* constantBufferData);

		float GetTerrainHeight(float xPos, float zPos);
		void ResetStack();

		void SetLevelLocation(int level, int location);

		void ClearMemory();

		FlagNode* m_FlagNode[9]; // 3/3

		int current_location;
		bool bLoadedShadows;

		HeightMapInfo m_hminfo;

		Physics* m_phys;
		float m_terrain_scale;

		int fin_state;

		ID3D11ShaderResourceView* m_Texture;
		ID3D11ShaderResourceView* m_Texture_2;


		bool HeightMapLoad(char* filename, float height_scale);
	private:
		std::shared_ptr<DX::DeviceResources> m_deviceResources;
	};

}