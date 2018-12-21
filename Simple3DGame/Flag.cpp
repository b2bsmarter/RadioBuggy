#include "pch.h"
#include "Flag.h"
#include "PerlinNoise.h"



//#if defined __ARMCC_VERSION
#if defined _WIN64

#else
#pragma pack(2) // Add this
/*|
typedef struct
{
	unsigned short bfType;
	unsigned int   bfSize;
	unsigned short bfReserved1;
	unsigned short bfReserved2;
	unsigned int   bfOffBits;
} BITMAPFILEHEADER;
*/
#pragma pack() // and this

#endif


using namespace Simple3DGame;
using namespace DirectX;
using namespace DX;
using namespace Microsoft::WRL;

Flag::Flag(std::shared_ptr<DX::DeviceResources> pm_deviceResources)
{
	FILE* f_ptr;
	char filename[100];

	lev_num = 1;

	m_deviceResources = pm_deviceResources;

	total_planes = 0;


	m_Texture = nullptr;
	m_Texture_2 = nullptr;

	fin_state = 0;

	noise_scale = 0.2f;

	bLoadedShadows = false;

	for (int i = 0; i < 9; i++)
	{
		m_FlagNode[i] = new FlagNode(m_deviceResources);
	}
}



int Flag::clampB(int pX, int pMax)
{
	if (pX > pMax)
	{
		return pMax;
	}
	else if (pX < 0)
	{
		return 0;
	}
	else
	{
		return pX;
	}
}


void Flag::SetLevelLocation(int level, int location)
{
	current_level = level;
	current_location = location;

}




void Flag::Render(Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer, ModelViewProjectionConstantBuffer* constantBufferData)
{
	m_deviceResources->GetD3DDeviceContext()->PSSetShaderResources(0, 1, &m_Texture);

	int i;
	current_index = 0;
	for (i = 0; i < total_planes - x_planes; i++)
	{

	}

	for (i = 0; i<total_planes; i++)
	{
		constantBufferData->model = m_FlagNode[i]->m_constantBufferData.model;

		m_deviceResources->GetD3DDeviceContext()->UpdateSubresource(constantBuffer.Get(), 0, NULL, constantBufferData, 0, 0);


		if (true)//(m_Resources->m_Camera->CheckPoint(m_Terrain[i]->xpos, 1.0f, m_Terrain[i]->zpos, 12.0f, FAR_DIST))
		{
			m_FlagNode[i]->Render(0);
			//m_Terrain[i]->m_rigidbody->setActivationState(1);
		}
		else
		{

			//m_Terrain[i]->m_rigidbody->setActivationState(WANTS_DEACTIVATION);
			//m_Terrain[i]->m_rigidbody->setActivationState(0);

		}
	}

}




//void Level::InitializeTerrain( char* terrain_filename,wchar_t* texture_filename, Physics* phys , float height_scale)
void Flag::UpdateGeneratedTerrain(int level, float noise_z)
{
	unsigned int seed = 237;
	float scale = 0.1f;
	PerlinNoise pn(seed);

	for (int i = 0; i <total_y_points; i++) {
		height_map[i] = new float[total_x_points];

		for (int j = 0; j < total_x_points; j++)
		{
			height_map[i][j] = (pn.noise((float)j*scale + noise_z, (float)i*scale, noise_z*0.1f))*20.0f;//*
																										//pn.noise((float)j*scale*3.0f, (float)i*scale*3.0f, 1.0f))*20.0f;
		}
	}

	for (int j = 0; j < total_y_points; j++)
	{
		for (int i = 0; i < total_x_points; i++)
		{


			float tl = height_map[clampB(j - 1, total_y_points - 1)][clampB(i - 1, total_x_points - 1)];// m_hminfo.heightMap[clampB(i - 1, m_hminfo.terrainWidth) + (clampB((j - 1), m_hminfo.terrainHeight)*m_hminfo.terrainWidth)].y;
			float t = height_map[clampB(j - 1, total_y_points - 1)][clampB(i, total_x_points - 1)];//m_hminfo.heightMap[clampB(i, m_hminfo.terrainWidth) + (clampB((j - 1), m_hminfo.terrainHeight)*m_hminfo.terrainWidth)].y;
			float tr = height_map[clampB(j - 1, total_y_points - 1)][clampB(i + 1, total_x_points - 1)];// m_hminfo.heightMap[clampB(i + 1, m_hminfo.terrainWidth) + (clampB((j - 1), m_hminfo.terrainHeight)*m_hminfo.terrainWidth)].y;
			float r = height_map[clampB(j, total_y_points - 1)][clampB(i + 1, total_x_points - 1)];//m_hminfo.heightMap[clampB(i + 1, m_hminfo.terrainWidth) + (clampB((j), m_hminfo.terrainHeight)*m_hminfo.terrainWidth)].y;
			float br = height_map[clampB(j + 1, total_y_points - 1)][clampB(i + 1, total_x_points - 1)];//m_hminfo.heightMap[clampB(i + 1, m_hminfo.terrainWidth) + (clampB((j + 1), m_hminfo.terrainHeight)*m_hminfo.terrainWidth)].y;
			float b = height_map[clampB(j + 1, total_y_points - 1)][clampB(i, total_x_points - 1)];//m_hminfo.heightMap[clampB(i, m_hminfo.terrainWidth) + (clampB((j + 1), m_hminfo.terrainHeight)*m_hminfo.terrainWidth)].y;
			float bl = height_map[clampB(j + 1, total_y_points - 1)][clampB(i - 1, total_x_points - 1)];//m_hminfo.heightMap[clampB(i - 1, m_hminfo.terrainWidth) + (clampB((j + 1), m_hminfo.terrainHeight)*m_hminfo.terrainWidth)].y;
			float l = height_map[clampB(j, total_y_points - 1)][clampB(i - 1, total_x_points - 1)];//m_hminfo.heightMap[clampB(i - 1, m_hminfo.terrainWidth) + (clampB((j), m_hminfo.terrainHeight)*m_hminfo.terrainWidth)].y;

			float dX = -((tr + (1.5f * r) + br) - (tl + (1.5f * l) + bl));
			float dZ = (tl + (1.5f * t) + tr) - (bl + (1.5f * b) + br);
			float dY = 5.5;// / pStrength;

			btVector3 v(dX, dY, dZ);
			v.normalize();

			normals[j][i].x = (float)v.getX();
			normals[j][i].y = (float)v.getY();
			normals[j][i].z = (float)v.getZ();

		}
	}


	for (int j = 0; j < y_planes; j++)
	{
		for (int i = 0; i < x_planes; i++)
		{
			m_FlagNode[i + (j*x_planes)]->UpdateFromHeightMap(height_map, normals);

		}
	}



}

void Flag::InitializeGeneratedTerrain(int level, int x_nodes, int y_nodes, float height_scale, float terrain_scale, float noise_z)
{
	wchar_t texture_filename[40];
	wchar_t texture_filename_2[40];

	wchar_t ground_filename1[30];
	wchar_t ground_filename2[30];

	x_planes = x_nodes;
	y_planes = y_nodes;

	unsigned int seed = 237;
	float scale = 0.05f;
	PerlinNoise pn(seed);

	total_x_points = (x_planes * 8) + 1;
	total_y_points = (y_planes * 8) + 1;

	height_map = new float*[total_y_points];


	for (int i = 0; i <total_y_points; i++) {
		height_map[i] = new float[total_x_points];

		for (int j = 0; j < total_x_points; j++)
		{
			height_map[i][j] = (pn.noise((float)j*scale, (float)i*scale, noise_z))*20.0f;//*
																						 //pn.noise((float)j*scale*2.0f, (float)i*scale*2.0f, 1.0f)*
																						 //pn.noise((float)j*scale*3.0f, (float)i*scale*3.0f, 1.0f))*20.0f;
		}

	}

	normals = new norm_t*[total_y_points];

	for (int i = 0; i <total_y_points; i++) {
		normals[i] = new norm_t[total_x_points];
	}

	for (int j = 0; j < total_y_points; j++)
	{
		for (int i = 0; i < total_x_points; i++)
		{


			float tl = height_map[clampB(j - 1, total_y_points - 1)][clampB(i - 1, total_x_points - 1)];// m_hminfo.heightMap[clampB(i - 1, m_hminfo.terrainWidth) + (clampB((j - 1), m_hminfo.terrainHeight)*m_hminfo.terrainWidth)].y;
			float t = height_map[clampB(j - 1, total_y_points - 1)][clampB(i, total_x_points - 1)];//m_hminfo.heightMap[clampB(i, m_hminfo.terrainWidth) + (clampB((j - 1), m_hminfo.terrainHeight)*m_hminfo.terrainWidth)].y;
			float tr = height_map[clampB(j - 1, total_y_points - 1)][clampB(i + 1, total_x_points - 1)];// m_hminfo.heightMap[clampB(i + 1, m_hminfo.terrainWidth) + (clampB((j - 1), m_hminfo.terrainHeight)*m_hminfo.terrainWidth)].y;
			float r = height_map[clampB(j, total_y_points - 1)][clampB(i + 1, total_x_points - 1)];//m_hminfo.heightMap[clampB(i + 1, m_hminfo.terrainWidth) + (clampB((j), m_hminfo.terrainHeight)*m_hminfo.terrainWidth)].y;
			float br = height_map[clampB(j + 1, total_y_points - 1)][clampB(i + 1, total_x_points - 1)];//m_hminfo.heightMap[clampB(i + 1, m_hminfo.terrainWidth) + (clampB((j + 1), m_hminfo.terrainHeight)*m_hminfo.terrainWidth)].y;
			float b = height_map[clampB(j + 1, total_y_points - 1)][clampB(i, total_x_points - 1)];//m_hminfo.heightMap[clampB(i, m_hminfo.terrainWidth) + (clampB((j + 1), m_hminfo.terrainHeight)*m_hminfo.terrainWidth)].y;
			float bl = height_map[clampB(j + 1, total_y_points - 1)][clampB(i - 1, total_x_points - 1)];//m_hminfo.heightMap[clampB(i - 1, m_hminfo.terrainWidth) + (clampB((j + 1), m_hminfo.terrainHeight)*m_hminfo.terrainWidth)].y;
			float l = height_map[clampB(j, total_y_points - 1)][clampB(i - 1, total_x_points - 1)];//m_hminfo.heightMap[clampB(i - 1, m_hminfo.terrainWidth) + (clampB((j), m_hminfo.terrainHeight)*m_hminfo.terrainWidth)].y;

			float dX = -((tr + (1.5f * r) + br) - (tl + (1.5f * l) + bl));
			float dZ = (tl + (1.5f * t) + tr) - (bl + (1.5f * b) + br);
			float dY = 5.5;// / pStrength;

			btVector3 v(dX, dY, dZ);
			v.normalize();

			normals[j][i].x = (float)v.getX();
			normals[j][i].y = (float)v.getY();
			normals[j][i].z = (float)v.getZ();

		}
	}






	stack_normals = new norm_t*[total_y_points * 2];

	for (int i = 0; i <total_y_points * 2; i++) {
		stack_normals[i] = new norm_t[total_x_points * 2];
	}


	stack_height = new float*[total_y_points * 2];

	for (int i = 0; i < total_y_points * 2; ++i) {
		stack_height[i] = new float[total_x_points * 2];
		for (int j = 0; j<total_x_points * 2; j++)
		{
			stack_height[i][j] = 0.0f;
		}
	}


	total_planes = 0;
	for (int j = 0; j < y_planes; j++)
	{
		for (int i = 0; i < x_planes; i++)
		{
			//m_Terrain[i + (j*x_planes)] = new Terrain();
			m_FlagNode[i + (j*x_planes)]->InitTerrain(&m_hminfo, i * 8, j * 8);
			total_planes++;
		}
	}

	for (int j = 0; j < y_planes; j++)
	{
		for (int i = 0; i < x_planes; i++)
		{
			m_FlagNode[i + (j*x_planes)]->LoadFromHeightMap(height_map, normals, 9, 9, (20.0f*terrain_scale), i);




			float shift = 160.0f;

			float posx = -(shift*terrain_scale) + (((float)(i))*(shift*terrain_scale));
			float posz = -(shift*terrain_scale) + (((float)(j))*(shift*terrain_scale));

			posx += ((80.0f - (80.0f*(x_planes - 2)))*terrain_scale); // for 2
			posz += ((80.0f - (80.0f*(y_planes - 2)))*terrain_scale); // for 2

			m_FlagNode[i + (j*x_planes)]->SetPosition(posx, 0.0f, posz);

			m_FlagNode[i + (j*x_planes)]->zpos = posz;
			m_FlagNode[i + (j*x_planes)]->xpos = posx;
			m_FlagNode[i + (j*x_planes)]->cam_z = cam_z;
		}

	}



	mbstowcs(ground_filename1, ground1, 30);


	swprintf(texture_filename, 40, L"Assets/Ground/%s.dds", ground_filename1);


	CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), texture_filename, nullptr, &m_Texture, MAXSIZE_T);


}




Flag::~Flag(void)
{
	ClearMemory();
}

void Flag::ClearMemory()
{
	int i;
	//height_map = new float*[total_y_points];
	if (m_Texture != nullptr)
		m_Texture->Release();
	if (m_Texture_2 != nullptr)
		m_Texture_2->Release();

	for (i = 0; i <total_y_points; i++) {
		delete[] height_map[i]; // = new float[total_x_points];
	}
	delete[] height_map;

	//normals = new norm_t*[total_y_points];

	for (i = 0; i <total_y_points; i++) {
		delete[] normals[i];// = new norm_t[total_x_points];
	}
	delete[] normals;


	//normals = new norm_t*[total_y_points];

	for (i = 0; i <total_y_points * 2; i++) {
		delete[] stack_normals[i];// = new norm_t[total_x_points];
	}
	delete[] stack_normals;

	for (i = 0; i <total_y_points * 2; i++) {
		delete[] stack_height[i];// = new norm_t[total_x_points];
	}
	delete[] stack_height;


	//for (i = 0; i <total_y_points; i++) {
	//	delete[] shadows[i];// = new norm_t[total_x_points];
	//}
	//delete[] shadows;

	for (i = 0; i < total_planes; i++)
	{
		m_FlagNode[i]->ClearMemory();
	}


	//delete[] m_hminfo.heightMap;
	//delete[] m_hminfo.normal;
	//delete[] m_hminfo.colour;
	total_planes = 0;



}
