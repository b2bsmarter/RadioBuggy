#include "pch.h"
#include "Level.h"
#include "PerlinNoise.h"


using namespace Simple3DGame;
using namespace DirectX;
using namespace DX;
using namespace Microsoft::WRL;

Level::Level(std::shared_ptr<DX::DeviceResources> pm_deviceResources, Physics* phys, int num, Camera^ p_camera)
{
	FILE* f_ptr;
	char filename[100];

	m_phys = phys;

	lev_num = num;

	m_deviceResources = pm_deviceResources;

	total_planes = 0;
	m_camera = p_camera;


	CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), L"Assets/Textures/fin-flag.dds", nullptr, &m_TextureFinish, MAXSIZE_T);
	CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), L"Assets/Textures/fin-flag-ok.dds", nullptr, &m_TextureFinishOK, MAXSIZE_T);
	CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), L"Assets/Textures/fin-flag-ok-not.dds", nullptr, &m_TextureFinishNot, MAXSIZE_T);


	m_Texture = nullptr;
	m_Texture_2 = nullptr;

	fin_state = 0;

	noise_scale = 0.2f;

	bLoadedShadows = false;
	/*
	sprintf( filename , "\\Assets\\Levels\\Level%d.txt" , num );

	// Open the file
	f_ptr = fopen(filename,"rb");
	//if (f_ptr == NULL)
	//	return 0;

	// Read bitmaps header
	//fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1,filePtr);

	fclose(  f_ptr );
	*/

	for (int i = 0; i < 500; i++)
	{
		m_Terrain[i] = new Terrain(m_deviceResources);
	}
}



int Level::clampB(int pX, int pMax)
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


float Level::RayGetTerrainHeight(float xPos, float zPos)
{
	float scaleFactor = 1.0f;
	// we first get the height of four points of the quad underneath the point
	// Check to make sure this point is not off the map at all

	xPos += left_right_walls + 1.0f;
	zPos += front_back_walls + 1.0f;

	//xPos =  xPos;
	//zPos =  zPos;
	int x = (int)(xPos);// / 2.0f);
	int z = (int)(zPos);// / 2.0f);


	int xPlusOne = x + 1;
	int zPlusOne = z + 1;

	if (x > (total_x_points * 2) - 2)
		x = (total_x_points * 2) - 2;
	//return -999.0f;
	if (x < 1)
		x = 1;
	//return -999.0f;
	if (z > (total_y_points*2) - 2)
		z = (total_y_points * 2) - 2;
	//return -999.0f;
	if (z < 0)
		z = 0;
	//return -999.0f;
	/*
	if (x*m_hminfo.terrainWidth + z < 0 || x*m_hminfo.terrainWidth + z > m_hminfo.terrainHeight*m_hminfo.terrainWidth)
	return 0.0f;

	if (xPlusOne*m_hminfo.terrainWidth + z < 0 || xPlusOne*m_hminfo.terrainWidth + z > m_hminfo.terrainHeight*m_hminfo.terrainWidth)
	return 0.0f;

	if (x*m_hminfo.terrainWidth + zPlusOne < 0 || x*m_hminfo.terrainWidth + zPlusOne > m_hminfo.terrainHeight*m_hminfo.terrainWidth)
	return 0.0f;

	if (xPlusOne*m_hminfo.terrainWidth + zPlusOne < 0 || xPlusOne*m_hminfo.terrainWidth + zPlusOne > m_hminfo.terrainHeight*m_hminfo.terrainWidth)
	return 0.0f;
	*/
	//height_map[x][z]

	float triZ0 = stack_height[z][x];//(m_hminfo.heightMap[x*m_hminfo.terrainWidth + z].y);
	float triZ1 = stack_height[z][x + 1];//(m_hminfo.heightMap[xPlusOne*m_hminfo.terrainWidth + z].y);
	float triZ2 = stack_height[z + 1][x];//(m_hminfo.heightMap[x*m_hminfo.terrainWidth + zPlusOne].y);
	float triZ3 = stack_height[z + 1][x + 1];//(m_hminfo.heightMap[xPlusOne*m_hminfo.terrainWidth + zPlusOne].y);

	float height = 0.0f;
	float sqX = (xPos / scaleFactor) - (float)xPos;
	float sqZ = (zPos / scaleFactor) - (float)zPos;
	if ((sqX + sqZ) < 1)
	{
		height = triZ0;
		height += (triZ1 - triZ0) * sqX;
		height += (triZ2 - triZ0) * sqZ;
	}
	else
	{
		height = triZ3;
		height += (triZ1 - triZ3) * (1.0f - sqZ);
		height += (triZ2 - triZ3) * (1.0f - sqX);
	}
	height = height;// *0.5f;
	return height*scaleFactor;
}


void Level::SaveShadowBinary(int level)
{
	int i;
	char local_file_folder[140];
	//level;
	Windows::Storage::StorageFolder^ folder = Windows::Storage::ApplicationData::Current->LocalFolder;
	wcstombs(local_file_folder, folder->Path->Data(), sizeof(local_file_folder));
	// update local crazy
	//sprintf(local_file_folder, "C:\\Users\\craig\\Desktop\\Simple3DGame3\\Simple3DGame\\Assets");

	// Load level scores
	char info_filename[140];
	wchar_t w_info_filename[140];
	sprintf(info_filename, "%s\\LevelBinary\\RayData", local_file_folder);
	_swprintf(w_info_filename, L"%hs", info_filename);
	CreateDirectory(w_info_filename, NULL);

	sprintf(info_filename, "%s\\LevelBinary\\RayData\\s%d.bmp", local_file_folder, level);

	FILE* pFile = fopen(info_filename, "wb");
	if (pFile != NULL)
	{
		for (i = 0; i < total_y_points; i++)
		{
			fwrite(shadows[i], sizeof(float), total_x_points, pFile);
		}

		fclose(pFile);
		//fwrite(&game_settings, sizeof(game_settings_t) * 1, 1, pFile);
		//fclose(pFile);
	}
}

void Level::SetLevelLocation(int level, int location)
{
	current_level = level;
	current_location = location;

}

bool Level::LoadShadowBinary(int level, int location)
{
	int i, j;
	//return true;
	char local_file_folder[140];
	//current_level = level;
	Windows::Storage::StorageFolder^ folder = Windows::Storage::ApplicationData::Current->LocalFolder;
	wcstombs(local_file_folder, folder->Path->Data(), sizeof(local_file_folder));

	location = current_location;

	shadows = new float*[m_hminfo.terrainHeight];

	for (int i = 0; i < m_hminfo.terrainHeight; ++i)
	{
		shadows[i] = new float[m_hminfo.terrainWidth];
		for (int j = 0; j<m_hminfo.terrainWidth; j++)
		{
			shadows[i][j] = 0.0f;
		}
	}

	// update local crazy
	//sprintf(local_file_folder, "C:\\Users\\craig\\Desktop\\Simple3DGame3\\Simple3DGame\\Assets");

	//return true;
	// Load level scores
	char info_filename[140];

	if (location == 0)
	{
		sprintf(info_filename, "%s\\LevelBinary\\RayData\\s%d.bmp", local_file_folder, level);
	}
	else
	{
		sprintf(info_filename, "Assets\\LevelBinary\\RayData\\s%d.bmp", level);
	}

	// LoadLevelData
	FILE * pFile;

	pFile = fopen(info_filename, "r");
	if (pFile != NULL)
	{
		for (i = 0; i < total_y_points; i++)
		{
			fread(shadows[i], sizeof(float), total_x_points, pFile);
		}


		fclose(pFile);

	}
	else
	{
		return false;
	}
}


void Level::SaveRayBinary(int level)
{
	int i;
	char local_file_folder[140];
	//level;
	Windows::Storage::StorageFolder^ folder = Windows::Storage::ApplicationData::Current->LocalFolder;
	wcstombs(local_file_folder, folder->Path->Data(), sizeof(local_file_folder));
	// update local crazy
	//sprintf(local_file_folder, "C:\\Users\\craig\\Desktop\\Simple3DGame3\\Simple3DGame\\Assets");

	// Load level scores
	char info_filename[140];
	wchar_t w_info_filename[140];
	sprintf(info_filename, "%s\\LevelBinary\\RayData", local_file_folder);
	_swprintf(w_info_filename, L"%hs", info_filename);
	CreateDirectory(w_info_filename, NULL);

	sprintf(info_filename, "%s\\LevelBinary\\RayData\\%d.bmp", local_file_folder, level);

	FILE* pFile = fopen(info_filename, "wb");
	if (pFile != NULL)
	{
		for (i = 0; i < total_y_points*2; i++)
		{
			fwrite(stack_height[i], sizeof(float), total_x_points*2, pFile);
		}
		
		for (i = 0; i < total_y_points * 2; i++)
		{
			fwrite(stack_normals[i], sizeof(norm_t), total_x_points * 2, pFile);
		}

		fclose(pFile);
		//fwrite(&game_settings, sizeof(game_settings_t) * 1, 1, pFile);
		//fclose(pFile);
	}
}



bool Level::LoadRayBinary(int level, int location)
{
	int i, j;
	//return true;
	char local_file_folder[140];
	current_level = level;
	Windows::Storage::StorageFolder^ folder = Windows::Storage::ApplicationData::Current->LocalFolder;
	wcstombs(local_file_folder, folder->Path->Data(), sizeof(local_file_folder));

	// update local crazy
	//sprintf(local_file_folder, "C:\\Users\\craig\\Desktop\\Simple3DGame3\\Simple3DGame\\Assets");

	//return true;
	// Load level scores
	char info_filename[140];

	if (location == 0)
	{
		sprintf(info_filename, "%s\\LevelBinary\\RayData\\%d.bmp", local_file_folder, level);
	}
	else
	{
		sprintf(info_filename, "Assets\\LevelBinary\\RayData\\%d.bmp", level);
	}

	// LoadLevelData
	FILE * pFile;
	
	pFile = fopen(info_filename, "r");
	if (pFile != NULL)
	{
		for (i = 0; i < total_y_points*2; i++)
		{
			fread(stack_height[i], sizeof(float), total_x_points*2, pFile);
		}

		for (i = 0; i < total_y_points * 2; i++)
		{
			fread(stack_normals[i], sizeof(norm_t), total_x_points * 2, pFile);
		}

		fclose(pFile);

	}
	else
	{
		return false;
	}
}


/// <summary>
/// Get the height of the terrain at given horizontal coordinates.
/// </summary>
/// <param name="xPos">X coordinate</param>
/// <param name="zPos">Z coordinate</param>
/// <returns>Height at given coordinates</returns>
float Level::GetTerrainHeight(float xPos, float zPos)
{
	float scaleFactor = 2.0f;
	// we first get the height of four points of the quad underneath the point
	// Check to make sure this point is not off the map at all

	xPos += left_right_walls-1.0f;
	zPos += front_back_walls-1.0f;

	//xPos =  xPos;
	//zPos =  zPos;
	int x = (int)(xPos / 2.0f);
	int z = (int)(zPos / 2.0f);


	int xPlusOne = x + 1;
	int zPlusOne = z + 1;

	if (x > total_x_points - 2)
		x = total_x_points - 2;
		//return -999.0f;
	if (x < 1)
		x = 1;
		//return -999.0f;
	if (z > total_y_points - 2)
		z = total_y_points - 2;
		//return -999.0f;
	if (z < 0)
		z = 0;
		//return -999.0f;
	/*
	if (x*m_hminfo.terrainWidth + z < 0 || x*m_hminfo.terrainWidth + z > m_hminfo.terrainHeight*m_hminfo.terrainWidth)
		return 0.0f;

	if (xPlusOne*m_hminfo.terrainWidth + z < 0 || xPlusOne*m_hminfo.terrainWidth + z > m_hminfo.terrainHeight*m_hminfo.terrainWidth)
		return 0.0f;

	if (x*m_hminfo.terrainWidth + zPlusOne < 0 || x*m_hminfo.terrainWidth + zPlusOne > m_hminfo.terrainHeight*m_hminfo.terrainWidth)
		return 0.0f;

	if (xPlusOne*m_hminfo.terrainWidth + zPlusOne < 0 || xPlusOne*m_hminfo.terrainWidth + zPlusOne > m_hminfo.terrainHeight*m_hminfo.terrainWidth)
		return 0.0f;
		*/
	//height_map[x][z]

	float triZ0 = height_map[z][x];//(m_hminfo.heightMap[x*m_hminfo.terrainWidth + z].y);
	float triZ1 = height_map[z][x+1];//(m_hminfo.heightMap[xPlusOne*m_hminfo.terrainWidth + z].y);
	float triZ2 = height_map[z+1][x];//(m_hminfo.heightMap[x*m_hminfo.terrainWidth + zPlusOne].y);
	float triZ3 = height_map[z+1][x+1];//(m_hminfo.heightMap[xPlusOne*m_hminfo.terrainWidth + zPlusOne].y);

	float height = 0.0f;
	float sqX = (xPos / scaleFactor) - (float)x;
	float sqZ = (zPos / scaleFactor) - (float)z;
	if ((sqX + sqZ) < 1)
	{
		height = triZ0;
		height += (triZ1 - triZ0) * sqX;
		height += (triZ2 - triZ0) * sqZ;
	}
	else
	{
		height = triZ3;
		height += (triZ1 - triZ3) * (1.0f - sqZ);
		height += (triZ2 - triZ3) * (1.0f - sqX);
	}
	height = height*0.5f;
	return height*scaleFactor;
}




btVector3 Level::RayGetNormal(float xPos, float zPos)
{
	float scaleFactor = 1.0f;

	xPos += left_right_walls + 1.0f;
	zPos += front_back_walls + 1.0f;

	//xPos =  xPos;
	//zPos =  zPos;
	int x = (int)(xPos);// / 2.0f);
	int z = (int)(zPos);// / 2.0f);


	int xPlusOne = x + 1;
	int zPlusOne = z + 1;

	if (x > (total_x_points*2) - 2)
		x = (total_x_points*2) - 2;
	//return -999.0f;
	if (x < 1)
		x = 1;
	//return -999.0f;
	if (z > (total_y_points*2) - 2)
		z = (total_y_points*2) - 2;
	//return -999.0f;
	if (z < 0)
		z = 0;
	//return -999.0f;


	/*
	if (x > total_x_points - 2)
	return btVector3(-999.0f, -999.0f, -999.0f);
	if (x < 1)
	return btVector3(-999.0f, -999.0f, -999.0f);
	if (z > total_y_points - 2)
	return btVector3(-999.0f, -999.0f, -999.0f);
	if (z < 0)
	return btVector3(-999.0f, -999.0f, -999.0f);
	*/


	btVector3 triZ0 = btVector3(stack_normals[z][x].x, stack_normals[z][x].y, stack_normals[z][x].z);//(m_hminfo.heightMap[x*m_hminfo.terrainWidth + z].y);
	btVector3 triZ1 = btVector3(stack_normals[z][x + 1].x, stack_normals[z][x + 1].y, stack_normals[z][x + 1].z);//(m_hminfo.heightMap[xPlusOne*m_hminfo.terrainWidth + z].y);
	btVector3 triZ2 = btVector3(stack_normals[z + 1][x].x, stack_normals[z + 1][x].y, stack_normals[z + 1][x].z);//(m_hminfo.heightMap[x*m_hminfo.terrainWidth + zPlusOne].y);
	btVector3 triZ3 = btVector3(stack_normals[z + 1][x + 1].x, stack_normals[z + 1][x + 1].y, stack_normals[z + 1][x + 1].z);//(m_hminfo.heightMap[xPlusOne*m_hminfo.terrainWidth + zPlusOne].y);

	btVector3 avgNormal;


	float sqX = (xPos / scaleFactor) - (float)x;
	float sqZ = (zPos / scaleFactor) - (float)z;
	if ((sqX + sqZ) < 1)
	{
		avgNormal = triZ0;
		avgNormal += (triZ1 - triZ0) * sqX;
		avgNormal += (triZ2 - triZ0) * sqZ;
	}
	else
	{
		avgNormal = triZ3;
		avgNormal += (triZ1 - triZ3) * (1.0f - sqZ);
		avgNormal += (triZ2 - triZ3) * (1.0f - sqX);
	}
	return avgNormal;
	/*
	int x = (int)(xPos);
	int z = (int)(zPos);

	int xPlusOne = x + 1;
	int zPlusOne = z + 1;

	btVector3 triZ0 = btVector3(m_hminfo.normal[x*m_hminfo.terrainWidth + z].x, m_hminfo.normal[x*m_hminfo.terrainWidth + z].y, m_hminfo.normal[x*m_hminfo.terrainWidth + z].z);
	btVector3 triZ1 = btVector3(m_hminfo.normal[xPlusOne*m_hminfo.terrainWidth + z].x, m_hminfo.normal[xPlusOne*m_hminfo.terrainWidth + z].y, m_hminfo.normal[xPlusOne*m_hminfo.terrainWidth + z].z);
	btVector3 triZ2 = btVector3(m_hminfo.normal[x*m_hminfo.terrainWidth + zPlusOne].x, m_hminfo.normal[x*m_hminfo.terrainWidth + zPlusOne].y, m_hminfo.normal[x*m_hminfo.terrainWidth + zPlusOne].z);
	btVector3 triZ3 = btVector3(m_hminfo.normal[xPlusOne*m_hminfo.terrainWidth + zPlusOne].x, m_hminfo.normal[xPlusOne*m_hminfo.terrainWidth + zPlusOne].y, m_hminfo.normal[xPlusOne*m_hminfo.terrainWidth + zPlusOne].z);

	btVector3 avgNormal;
	float sqX = (xPos)-x;
	float sqZ = (zPos)-z;
	if ((sqX + sqZ) < 1)
	{
	avgNormal = triZ0;
	avgNormal += (triZ1 - triZ0) * sqX;
	avgNormal += (triZ2 - triZ0) * sqZ;
	}
	else
	{
	avgNormal = triZ3;
	avgNormal += (triZ1 - triZ3) * (1.0f - sqZ);
	avgNormal += (triZ2 - triZ3) * (1.0f - sqX);
	}
	return avgNormal;
	*/
}


/// <summary>
/// Gets the normal of a position on the heightmap.
/// </summary>
/// <param name="xPos">X position on the map</param>
/// <param name="zPos">Z position on the map</param>
/// <returns>Normal vector of this spot on the terrain</returns>
btVector3 Level::GetNormal(float xPos, float zPos)
{
	float scaleFactor = 2.0f;

	xPos += left_right_walls - 1.0f;
	zPos += front_back_walls - 1.0f;

	//xPos =  xPos;
	//zPos =  zPos;
	int x = (int)(xPos / 2.0f);
	int z = (int)(zPos / 2.0f);


	int xPlusOne = x + 1;
	int zPlusOne = z + 1;

	if (x > total_x_points - 2)
		x = total_x_points - 2;
	//return -999.0f;
	if (x < 1)
		x = 1;
	//return -999.0f;
	if (z > total_y_points - 2)
		z = total_y_points - 2;
	//return -999.0f;
	if (z < 0)
		z = 0;
	//return -999.0f;


	/*
	if (x > total_x_points - 2)
		return btVector3(-999.0f, -999.0f, -999.0f);
	if (x < 1)
		return btVector3(-999.0f, -999.0f, -999.0f);
	if (z > total_y_points - 2)
		return btVector3(-999.0f, -999.0f, -999.0f);
	if (z < 0)
		return btVector3(-999.0f, -999.0f, -999.0f);
		*/


	btVector3 triZ0 = btVector3(normals[z][x].x, normals[z][x].y, normals[z][x].z);//(m_hminfo.heightMap[x*m_hminfo.terrainWidth + z].y);
	btVector3 triZ1 = btVector3(normals[z][x + 1].x, normals[z][x + 1].y, normals[z][x + 1].z);//(m_hminfo.heightMap[xPlusOne*m_hminfo.terrainWidth + z].y);
	btVector3 triZ2 = btVector3(normals[z + 1][x].x, normals[z + 1][x].y, normals[z + 1][x].z);//(m_hminfo.heightMap[x*m_hminfo.terrainWidth + zPlusOne].y);
	btVector3 triZ3 = btVector3(normals[z + 1][x + 1].x, normals[z + 1][x + 1].y, normals[z + 1][x + 1].z);//(m_hminfo.heightMap[xPlusOne*m_hminfo.terrainWidth + zPlusOne].y);

	btVector3 avgNormal;


	float sqX = (xPos / scaleFactor) - (float)x;
	float sqZ = (zPos / scaleFactor) - (float)z;
	if ((sqX + sqZ) < 1)
	{
		avgNormal = triZ0;
		avgNormal += (triZ1 - triZ0) * sqX;
		avgNormal += (triZ2 - triZ0) * sqZ;
	}
	else
	{
		avgNormal = triZ3;
		avgNormal += (triZ1 - triZ3) * (1.0f - sqZ);
		avgNormal += (triZ2 - triZ3) * (1.0f - sqX);
	}
	return avgNormal;
	/*
	int x = (int)(xPos);
	int z = (int)(zPos);

	int xPlusOne = x + 1;
	int zPlusOne = z + 1;

	btVector3 triZ0 = btVector3(m_hminfo.normal[x*m_hminfo.terrainWidth + z].x, m_hminfo.normal[x*m_hminfo.terrainWidth + z].y, m_hminfo.normal[x*m_hminfo.terrainWidth + z].z);
	btVector3 triZ1 = btVector3(m_hminfo.normal[xPlusOne*m_hminfo.terrainWidth + z].x, m_hminfo.normal[xPlusOne*m_hminfo.terrainWidth + z].y, m_hminfo.normal[xPlusOne*m_hminfo.terrainWidth + z].z);
	btVector3 triZ2 = btVector3(m_hminfo.normal[x*m_hminfo.terrainWidth + zPlusOne].x, m_hminfo.normal[x*m_hminfo.terrainWidth + zPlusOne].y, m_hminfo.normal[x*m_hminfo.terrainWidth + zPlusOne].z);
	btVector3 triZ3 = btVector3(m_hminfo.normal[xPlusOne*m_hminfo.terrainWidth + zPlusOne].x, m_hminfo.normal[xPlusOne*m_hminfo.terrainWidth + zPlusOne].y, m_hminfo.normal[xPlusOne*m_hminfo.terrainWidth + zPlusOne].z);

	btVector3 avgNormal;
	float sqX = (xPos)-x;
	float sqZ = (zPos)-z;
	if ((sqX + sqZ) < 1)
	{
		avgNormal = triZ0;
		avgNormal += (triZ1 - triZ0) * sqX;
		avgNormal += (triZ2 - triZ0) * sqZ;
	}
	else
	{
		avgNormal = triZ3;
		avgNormal += (triZ1 - triZ3) * (1.0f - sqZ);
		avgNormal += (triZ2 - triZ3) * (1.0f - sqX);
	}
	return avgNormal;
	*/
}


/// Setup <see cref="Terrain"/> normals. Normals are used for lighting, normal mapping, and physics with terrain.
/// </summary>

void Level::SetupTerrainNormals()
{
	// Setup normals for lighting and physics (Credit: Riemer's method)

	int widthMinusOne = m_hminfo.terrainWidth;

	int heightMinusOne = m_hminfo.terrainHeight;
	//exit(0);
	for (int x = 1; x < widthMinusOne; ++x)
		for (int z = 1; z < heightMinusOne; ++z)
		{
			int ZTimesSize = (z * m_hminfo.terrainHeight);
			btVector3 normX = btVector3((m_hminfo.heightMap[x - 1 + ZTimesSize].y - m_hminfo.heightMap[x + 1 + ZTimesSize].y) / 2, 1, 0);
			btVector3 normZ = btVector3(0, 1, (m_hminfo.heightMap[x + (z - 1) * m_hminfo.terrainWidth].z - m_hminfo.heightMap[x + (z + 1) * m_hminfo.terrainWidth].z) / 2);

			// We inline the normalize method here since it is used alot, this is faster than calling Vector3.Normalize()
			btVector3 normal = normX + normZ;
			float length = sqrt((float)((normal.getX() * normal.getX()) + (normal.getY() * normal.getY()) + (normal.getZ() * normal.getZ())));
			float num = 1.0f / length;
			normal.setX(normal.getX() * num);
			normal.setY(normal.getY() * num);
			normal.setZ(normal.getZ() * num);

			m_hminfo.normal[x + ZTimesSize] = DirectX::XMFLOAT3(normal.getX(), normal.getY(), normal.getZ());    // Stored for use in physics and for the
																												 // quad-tree component to reference.
		}
}

void Level::SetupTerrainColours()
{
	// Setup some colours

	int widthMinusOne = m_hminfo.terrainWidth;

	int heightMinusOne = m_hminfo.terrainHeight;

	for (int x = 1; x < widthMinusOne; ++x)
		for (int z = 1; z < heightMinusOne; ++z)
		{
			int ZTimesSize = (z * m_hminfo.terrainHeight);
			float rand_shade = ((rand() % 100)*0.01f);

			m_hminfo.colour[x + ZTimesSize] = DirectX::XMFLOAT4(rand_shade, rand_shade, rand_shade, 0.9f);

		}
}



void Level::Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> deviceContext, Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer, ModelViewProjectionConstantBuffer* constantBufferData)
{
	deviceContext->PSSetShaderResources(0, 1, &m_Texture);
	deviceContext->PSSetShaderResources(1, 1, &m_Texture_2);
	int i;
	current_index = 0;
	for (i = 0; i < total_planes - x_planes; i++)
	{

	}

	for (i = 0; i<total_planes-x_planes; i++)
	{
		if (m_camera->CheckPoint(m_Terrain[i]->xpos, 1.0f, m_Terrain[i]->zpos,12.0f, FAR_DIST))
		{
			m_Terrain[i]->Render(deviceContext, constantBuffer, constantBufferData,0);
			m_Terrain[i]->m_rigidbody->setActivationState(1);
		}
		else
		{
			m_Terrain[i]->m_rigidbody->setActivationState(WANTS_DEACTIVATION);
			m_Terrain[i]->m_rigidbody->setActivationState(0);

			if (m_camera->CheckPoint(m_Terrain[i]->xpos, 1.0f, m_Terrain[i]->zpos, 12.0f, FAR_DIST+60.0f))
			{
				m_Terrain[i]->Render(deviceContext, constantBuffer, constantBufferData, 1);
				//m_Terrain[i]->m_rigidbody->setActivationState(1);
			}
		}
	}


	if (fin_state == 0)
	{
		deviceContext->PSSetShaderResources(0, 1, &m_TextureFinishNot);
	}
	else
	{
		if (fin_state == 1)
		{
			deviceContext->PSSetShaderResources(0, 1, &m_TextureFinishOK);
		}
		else
		{
			deviceContext->PSSetShaderResources(0, 1, &m_TextureFinish);
		}
	}
	
	
	for (i = total_planes - x_planes; i<total_planes; i++)
	{
		if (m_camera->CheckPoint(m_Terrain[i]->xpos, 1.0f, m_Terrain[i]->zpos, 12.0f, FAR_DIST + 80.0f))
		{
			m_Terrain[i]->Render(deviceContext, constantBuffer, constantBufferData,0);
			m_Terrain[i]->m_rigidbody->setActivationState(1);
		}
		else
		{
			m_Terrain[i]->m_rigidbody->setActivationState(WANTS_DEACTIVATION);
			m_Terrain[i]->m_rigidbody->setActivationState(0);
		}
	}
	
	//deviceContext->PSSetShaderResources(1, 1, nullptr);
}

void Level::ResetShadows()
{
	for (int i = 0; i < total_y_points; i++)
	{
		for (int j = 0; j<total_y_points; j++)
		{
			shadows[i][j] = 0.0f;
		}
	}

}


bool Level::HeightMapLoad(char* filename, float height_scale)
{
	FILE *filePtr;							// Point to the current position in the file
	BITMAPFILEHEADER bitmapFileHeader;		// Structure which stores information about file
	BITMAPINFOHEADER bitmapInfoHeader;		// Structure which stores information about image
	int imageSize, index;
	int heightb;
	int heightg;
	int heightr;

	highest_point = -1000.0f;
	lowest_point = 1000.0f;

	// Open the file
	filePtr = fopen(filename, "rb");
	if (filePtr == NULL)
		return 0;

	// Read bitmaps header
	fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);

	// Read the info header
	fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);

	// Get the width and height (width and length) of the image
	m_hminfo.terrainWidth = bitmapInfoHeader.biWidth;
	m_hminfo.terrainHeight = bitmapInfoHeader.biHeight;

	// Size of the image in bytes. the 3 represents RBG (byte, byte, byte) for each pixel
	imageSize = m_hminfo.terrainWidth * m_hminfo.terrainHeight * 3;

	stack_height = new float*[m_hminfo.terrainHeight * 2];

	for (int i = 0; i < m_hminfo.terrainHeight * 2; ++i) {
		stack_height[i] = new float[m_hminfo.terrainWidth * 2];
		for (int j = 0; j<m_hminfo.terrainWidth * 2; j++)
		{
			stack_height[i][j] = 0.0f;
		}
	}

	/*
	shadows = new float*[m_hminfo.terrainHeight];

	for (int i = 0; i < m_hminfo.terrainHeight; ++i) 
	{
		shadows[i] = new float[m_hminfo.terrainWidth];
		for (int j = 0; j<m_hminfo.terrainWidth; j++)
		{
			shadows[i][j] = 0.0f;
		}
	}
	*/
	map_x_size = m_hminfo.terrainWidth;
	map_y_size = m_hminfo.terrainHeight;
	
	x_planes = ((map_x_size) / 9);
	y_planes = ((map_y_size) / 9);

	total_x_points = (x_planes * 8) + 1;
	total_y_points = (y_planes * 8) + 1;

	if (true)//(bLoadedShadows == false)
	{
		//bLoadedShadows = true;
		LoadShadowBinary(current_level, current_location);

	}
	

	// Initialize the array which stores the image data
	//unsigned char* bitmapImage = new unsigned char[imageSize];

	/*
	unsigned char* bitmapImage = new unsigned char[imageSize];
	hminfo.heightMap = new XMFLOAT3[imageSize];
	hminfo.normal = new XMFLOAT3[imageSize];
	*/

	unsigned char* bitmapImage = new unsigned char[imageSize];
	m_hminfo.heightMap = new DirectX::XMFLOAT3[imageSize*sizeof(DirectX::XMFLOAT3)];
	m_hminfo.normal = new DirectX::XMFLOAT3[imageSize*sizeof(DirectX::XMFLOAT3)];
	m_hminfo.colour = new DirectX::XMFLOAT4[imageSize*sizeof(DirectX::XMFLOAT4)];
	/*
	unsigned char* bitmapImage = (unsigned char*)malloc(imageSize);
	hminfo.heightMap = (XMFLOAT3 *)malloc(imageSize*sizeof(XMFLOAT3));
	hminfo.normal = (XMFLOAT3 *)malloc(imageSize*sizeof(XMFLOAT3));
	*/

	/*
	char str[40];
	sprintf_s(str , "%f \n" , imageSize*sizeof(XMFLOAT3)  );

	OutputDebugStringA(str);
	*/

	// Set the file pointer to the beginning of the image data
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	// Store image data in bitmapImage
	fread(bitmapImage, 1, imageSize, filePtr);

	// Close file
	fclose(filePtr);

	// Initialize the heightMap array (stores the vertices of our terrain)
	//hminfo.heightMap = new XMFLOAT3[hminfo.terrainWidth * hminfo.terrainHeight];
	//hminfo.normal = new XMFLOAT3[hminfo.terrainWidth * hminfo.terrainHeight];
	// We use a greyscale image, so all 3 rgb values are the same, but we only need one for the height
	// So we use this counter to skip the next two components in the image data (we read R, then skip BG)
	int k = 0;

	// We divide the height by this number to "water down" the terrains height, otherwise the terrain will
	// appear to be "spikey" and not so smooth.

	// Read the image data into our heightMap array
	int row_multiple_8 = (m_hminfo.terrainWidth / 9) * 9;
	
	int row_size = row_multiple_8 * 3;

	unsigned char* height_row = new unsigned char[row_size];

	for (int j = 0; j< m_hminfo.terrainHeight; j++)
	{
		// flip the row
		int l = row_multiple_8 * 3;
		for (int i = 0; i < row_multiple_8; i++)
		{
			
			height_row[(i*3)] = bitmapImage[k+l];//+bitmapImage[k+1]+bitmapImage[k+2];
			height_row[(i*3) + 1] = bitmapImage[k+l + 1];//+bitmapImage[k+1]+bitmapImage[k+2];
			height_row[(i*3) + 2] = bitmapImage[k+l + 2];//+bitmapImage[k+1]+bitmapImage[k+2];
			l -= 3;
		}
		l = 0;
		for (int i = 0; i < row_multiple_8; i++)
		{
			bitmapImage[k + l] = height_row[(i * 3)];//+bitmapImage[k+1]+bitmapImage[k+2];
			bitmapImage[k + l + 1] = height_row[(i * 3) + 1];//+bitmapImage[k+1]+bitmapImage[k+2];
			bitmapImage[k + l + 2] = height_row[(i * 3) + 2];//+bitmapImage[k+1]+bitmapImage[k+2];
			l += 3;
		}
		
		for (int i = 0; i< m_hminfo.terrainWidth; i++)
		{
			//flip the x;

			heightb = bitmapImage[k];//+bitmapImage[k+1]+bitmapImage[k+2];
			heightg = bitmapImage[k + 1];//+bitmapImage[k+1]+bitmapImage[k+2];
			heightr = bitmapImage[k + 2];//+bitmapImage[k+1]+bitmapImage[k+2];

			index = (m_hminfo.terrainWidth * j) + i;

			// blue
			m_hminfo.heightMap[index].x = (float)i;
			m_hminfo.heightMap[index].y = (float)(heightb*2.0f)* height_scale;
			m_hminfo.heightMap[index].z = (float)j;

			m_hminfo.colour[index].x = (float)heightr/128.0f;
			m_hminfo.colour[index].y = (float)heightg/128.0f;
			m_hminfo.colour[index].z = (float)heightb/128.0f;

			// green x2
			//m_hminfo.heightMap[index].y = m_hminfo.heightMap[index].y + (float)((heightg)* 2) * height_scale;

			// red x3
			//m_hminfo.heightMap[index].y = m_hminfo.heightMap[index].y + (float)((heightr)* 3) * height_scale;

			k += 3;

			if (m_hminfo.heightMap[index].y > highest_point)
				highest_point = m_hminfo.heightMap[index].y;

			if (m_hminfo.heightMap[index].y < lowest_point)
				lowest_point = m_hminfo.heightMap[index].y;

			//highest_point = -1000.0f;
			//lowest_point = 1000.0f;

			//stack_height[j][i]=0.0f;
		}
	}
	//m_hminfo.terrainWidth = 18; //m_hminfo.terrainWidth - 2;
	//m_hminfo.terrainHeight =18; // map_y_size = m_hminfo.terrainHeight - 4;

	map_x_size = m_hminfo.terrainWidth;
	map_y_size = m_hminfo.terrainHeight;

	//hminfo.terrainWidth=32;
	//hminfo.terrainHeight=32;

	//char str[40];
	//sprintf_s(str , "%d  %d \n" , hminfo.terrainWidth , hminfo.terrainHeight );

	//OutputDebugStringA(str);

	// normals here
	unsigned int seed = 237;
	
	float highest=-100.0f;
	float lowest=100.0f;
	float ratio = 0.0f;

	PerlinNoise pn(seed);

	// work out high and low
	for (int j = 0; j < m_hminfo.terrainHeight; j++)
	{
		for (int i = 0; i < m_hminfo.terrainWidth; i++)
		{
			float rand_shade = pn.noise((float)i*noise_scale, (float)j *noise_scale, 0.8);

			//m_hminfo.heightMap[clampB(i - 1, m_hminfo.terrainWidth) + (clampB((j - 1), m_hminfo.terrainHeight)*m_hminfo.terrainWidth)].y = rand_shade*5.0f;
			if (rand_shade < lowest)
			{
				lowest = rand_shade;
			}
			if (rand_shade > highest)
			{
				highest = rand_shade;
			}
		}
	}
	ratio = 1.0f/(highest - lowest);

	for (int j = 0; j< m_hminfo.terrainHeight; j++)
	{
		for (int i = 0; i< m_hminfo.terrainWidth; i++)
		{
			index = (m_hminfo.terrainWidth * j) + i;

			float tl = m_hminfo.heightMap[clampB(i - 1, m_hminfo.terrainWidth) + (clampB((j - 1), m_hminfo.terrainHeight)*m_hminfo.terrainWidth)].y;
			float t = m_hminfo.heightMap[clampB(i, m_hminfo.terrainWidth) + (clampB((j - 1), m_hminfo.terrainHeight)*m_hminfo.terrainWidth)].y;
			float tr = m_hminfo.heightMap[clampB(i + 1, m_hminfo.terrainWidth) + (clampB((j - 1), m_hminfo.terrainHeight)*m_hminfo.terrainWidth)].y;
			float r = m_hminfo.heightMap[clampB(i + 1, m_hminfo.terrainWidth) + (clampB((j), m_hminfo.terrainHeight)*m_hminfo.terrainWidth)].y;
			float br = m_hminfo.heightMap[clampB(i + 1, m_hminfo.terrainWidth) + (clampB((j + 1), m_hminfo.terrainHeight)*m_hminfo.terrainWidth)].y;
			float b = m_hminfo.heightMap[clampB(i, m_hminfo.terrainWidth) + (clampB((j + 1), m_hminfo.terrainHeight)*m_hminfo.terrainWidth)].y;
			float bl = m_hminfo.heightMap[clampB(i - 1, m_hminfo.terrainWidth) + (clampB((j + 1), m_hminfo.terrainHeight)*m_hminfo.terrainWidth)].y;
			float l = m_hminfo.heightMap[clampB(i - 1, m_hminfo.terrainWidth) + (clampB((j), m_hminfo.terrainHeight)*m_hminfo.terrainWidth)].y;

			float dX = -((tr + (1.5f * r) + br) - (tl + (1.5f * l) + bl));
			float dZ = (tl + (1.5f * t) + tr) - (bl + (1.5f * b) + br);
			float dY = 35.5;// / pStrength;

			btVector3 v(dX, dY, dZ);
			v.normalize();

			m_hminfo.normal[index].x = (float)v.getX();
			m_hminfo.normal[index].y = (float)v.getY();
			m_hminfo.normal[index].z = (float)v.getZ();

			//m_hminfo.heightMap[2].
			float rand_shade = (pn.noise((float)i*noise_scale, (float)j *noise_scale, 0.8)- lowest)*ratio;
			float rand_shade2 = (pn.noise((float)i*(noise_scale*0.5f), (float)j *(noise_scale*0.5f), 0.8) - lowest)*ratio;
			float rand_shade3 = ((rand() % 100)*0.0008f);

			float col_r = m_hminfo.colour[index].x;
			float col_g = m_hminfo.colour[index].y += ((rand_shade2*0.2f) -(rand_shade*0.2f))- rand_shade3+ shadows[j][i];
			float col_b = m_hminfo.colour[index].z;
			// level colour
			m_hminfo.colour[index] = DirectX::XMFLOAT4(col_g, col_g, col_g, col_r);

		}
	}

	//hminfo.terrainWidth=32;
	//hminfo.terrainHeight=64;




	delete[] bitmapImage;
	bitmapImage = 0;

	return true;
}


void Level::Update(XMMATRIX *viewMatrix, float timeTotal)
{
	for (int i = 0; i<total_planes; i++)
	{
		m_Terrain[i]->Update(viewMatrix, timeTotal);
	}
}

void Level::SetFriction(float fric)
{
	int i, j;

	for (j = 0; j < y_planes; j++)
	{
		for (i = 0; i < x_planes; i++)
		{
			m_Terrain[i + (j*x_planes)]->m_rigidbody->setFriction(fric);
		}
	}

	m_GroundPlane->setFriction(fric);
	m_FloorPlane->setFriction(fric);
}

void Level::SetRestitution(float rest)
{
	int i, j;

	for (j = 0; j < y_planes; j++)
	{
		for (i = 0; i < x_planes; i++)
		{
			m_Terrain[i + (j*x_planes)]->m_rigidbody->setRestitution(rest);
		}
	}
	m_GroundPlane->setRestitution(rest);
	m_FloorPlane->setRestitution(rest);
}

//void Level::InitializeTerrain( char* terrain_filename,wchar_t* texture_filename, Physics* phys , float height_scale)

void Level::InitializeTerrain(int level, float height_scale, float terrain_scale)
{
	int loop;
	char terrain_filename[40];
	wchar_t texture_filename[40];
	wchar_t texture_filename_2[40];

	wchar_t ground_filename1[30];
	wchar_t ground_filename2[30];

	fin_state = 0;

	m_terrain_scale = terrain_scale;



	sprintf(terrain_filename, "Assets/TerrainMaps/%d.bmp", level);

	mbstowcs(ground_filename1, ground1, 30);
	mbstowcs(ground_filename2, ground2, 30);

	swprintf(texture_filename, 40, L"Assets/Ground/%s.dds", ground_filename1);
	swprintf(texture_filename_2, 40, L"Assets/Ground/%s.dds", ground_filename2);

	//switch(level)
	//{
	//case 1:

	scale_height = 0.5f;//height_scale;
	HeightMapLoad(terrain_filename, height_scale);

	int j,i,k,l;
	x_planes = ((map_x_size) / 9);
	y_planes = ((map_y_size) / 9);



	CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), texture_filename, nullptr, &m_Texture, MAXSIZE_T);
	CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), texture_filename_2, nullptr, &m_Texture_2, MAXSIZE_T);

	//x_planes=3;
	//y_planes=6;
	total_platforms = x_planes*y_planes;
	//m_Terrain = new *Terrain[x_planes*y_planes];
	total_planes = 0;

	total_x_points = (x_planes * 8) + 1;
	total_y_points = (y_planes * 8) + 1;

	height_map = new float*[total_y_points];
	
	for (i = 0; i <total_y_points; i++) {
		height_map[i] = new float[total_x_points];
	}

	normals = new norm_t*[total_y_points];

	for (i = 0; i <total_y_points; i++) {
		normals[i] = new norm_t[total_x_points];
	}


	stack_normals = new norm_t*[total_y_points*2];

	for (i = 0; i <total_y_points*2; i++) {
		stack_normals[i] = new norm_t[total_x_points*2];
	}

/*
	colours = new DirectX::XMFLOAT4*[total_y_points];

	for (j = 0; j < total_y_points; j++)
	{
		for (int i = 0; i < total_x_points; i++)
		{
			float rand_shade = ((rand() % 100)*0.01f);
			height_map[j][i]. = DirectX::XMFLOAT4(rand_shade, rand_shade, rand_shade, 1.0f);
		}
	}
	*/
	/*
	for (i = 0; i <total_y_points; i++) {
		colours[i] = new DirectX::XMFLOAT4[total_x_points];
	}

	
	for (j = 0; j < total_y_points; j++)
	{
		for (int i = 0; i < total_x_points; i++)
		{
			float rand_shade = ((rand() % 100)*0.01f);
			colours[j][i] = DirectX::XMFLOAT4(rand_shade, rand_shade, rand_shade, 1.0f);
		}
	}
	*/



	for (j = 0; j < y_planes; j++)
	{
		for (int i = 0; i < x_planes; i++)
		{
			//m_Terrain[i + (j*x_planes)] = new Terrain();
			m_Terrain[i + (j*x_planes)]->InitTerrain(&m_hminfo, i * 8, j * 8);
			total_planes++;
		}
	}
	for (j = 0; j<y_planes; j++)
	{
		for (i = 0; i<x_planes; i++)
		{
			//if( i+(j*x_planes)>total_planes-1 )
			//{
			 
			//} 
			//m_Terrain[i + (j*x_planes)]->m_rigidbody->setFriction();
			m_Terrain[i + (j*x_planes)]->Load(9, 9, (20.0f*terrain_scale), i);

			// copy heightmap and normals localy
			for (k = 0; k < 9; k++)
			{
				for (l = 0; l < 9; l++)
				{
					height_map[(j * 8) + k][(i * 8) + l] = m_Terrain[i + (j*x_planes)]->height_map[l][k];

					norm_t norm;
					norm.x = m_Terrain[i + (j*x_planes)]->normals[l][k].x;
					norm.y = m_Terrain[i + (j*x_planes)]->normals[l][k].y;
					norm.z = m_Terrain[i + (j*x_planes)]->normals[l][k].z;

					normals[(j * 8) + k][(i * 8) + l] = norm;
				}
			}

			//m_Terrain[i+(j*x_planes)]->LoadTexture( m_d3dDevice.Get() , texture_filename );
			/*
			float posx;//=-200.0f+(((float)(i%3))*200.0f);
			float posz;//=-200.0f+(((float)(i/3))*200.0f);
			*/
			float shift = 160.0f;// -100.0f + (100.0f*x_planes);

			float posx = -(shift*terrain_scale) + (((float)(i))*(shift*terrain_scale));
			float posz = -(shift*terrain_scale) + (((float)(j))*(shift*terrain_scale));

			posx += ((80.0f - (80.0f*(x_planes - 2)))*terrain_scale); // for 2
			posz += ((80.0f - (80.0f*(y_planes - 2)))*terrain_scale); // for 2

			//posz=-200.0f;
			/*
			switch(i+(j*x_planes))
			{
			case 0:posx=-200.0f;
			posz=-200.0f;
			break;
			case 1:posx=0.0f;
			posz=-200.0f;
			break;
			case 2:posx=200.0f;
			posz=-200.0f;
			break;
			case 3:posx=-200.0f;
			posz=0.0f;
			break;
			case 4:posx=0.0f;
			posz=0.0f;
			break;
			case 5:posx=200.0f;
			posz=0.0f;
			break;
			case 6:posx=-200.0f;
			posz=200.0f;
			break;
			case 7:posx=0.0f;
			posz=200.0f;
			break;
			case 8:posx=200.0f;
			posz=200.0f;
			break;
			}
			*/

			m_Terrain[i + (j*x_planes)]->SetPosition(posx, 0.0f, posz);
			//m_Terrain[i + (j*x_planes)]->MakePhysicsConvexTriangleTerrain(phys);

			// clear some memory
			//delete[] m_Terrain[i + (j*x_planes)]->m_phy_vertices;

			/*
			for (int i = 0; i < m_Terrain[i + (j*x_planes)]->ycoords; ++i) {
				delete[] m_Terrain[i + (j*x_planes)]->height_map[i];
			}
			delete[] m_Terrain[i + (j*x_planes)]->height_map;
			*/
			m_Terrain[i + (j*x_planes)]->zpos = posz;
			m_Terrain[i + (j*x_planes)]->xpos = posx;
			m_Terrain[i + (j*x_planes)]->cam_z = cam_z;
		}
	}



	//int y, x;
	/*
	for (int i = 0; i < total_y_points * 2; i++) {

		for (j = 0; j<total_x_points * 2; j++)
		{
			stack_height[i][j] = height_map[i/2][j/2];
		}
	}
	*/
	
	ResetStack();


	//delete[] m_hminfo.heightMap;
	//delete[] m_hminfo.normal;
}


void Level::ResetStack()
{
	int j;
	for (int i = 0; i < (total_y_points * 2) - 2; i += 2) {

		for (j = 0; j<(total_x_points * 2) - 2; j += 2)
		{
			stack_height[i + 2][j + 2] = height_map[i / 2][j / 2];
			stack_height[i + 2][j + 3] = (height_map[i / 2][j / 2] + height_map[i / 2][(j / 2) + 1])*0.5f;
			stack_height[i + 3][j + 2] = (height_map[i / 2][j / 2] + height_map[(i / 2) + 1][(j / 2)])*0.5f;
			stack_height[i + 3][j + 3] = (height_map[i / 2][j / 2] + height_map[(i / 2) + 1][(j / 2) + 1])*0.5f;
		}
	}

	for (int i = 0; i < (total_y_points * 2) - 2; i += 2) {

		for (j = 0; j<(total_x_points * 2) - 2; j += 2)
		{
			stack_normals[i + 2][j + 2] = normals[i / 2][j / 2];

			stack_normals[i + 2][j + 3].x = (normals[i / 2][j / 2].x + normals[i / 2][(j / 2) + 1].x)*0.5f;
			stack_normals[i + 2][j + 3].y = (normals[i / 2][j / 2].y + normals[i / 2][(j / 2) + 1].y)*0.5f;
			stack_normals[i + 2][j + 3].z = (normals[i / 2][j / 2].z + normals[i / 2][(j / 2) + 1].z)*0.5f;

			stack_normals[i + 3][j + 2].x = (normals[i / 2][j / 2].x + normals[(i / 2) + 1][(j / 2)].x)*0.5f;
			stack_normals[i + 3][j + 2].y = (normals[i / 2][j / 2].y + normals[(i / 2) + 1][(j / 2)].y)*0.5f;
			stack_normals[i + 3][j + 2].z = (normals[i / 2][j / 2].z + normals[(i / 2) + 1][(j / 2)].z)*0.5f;


			stack_normals[i + 3][j + 3].x = (normals[i / 2][j / 2].x + normals[(i / 2) + 1][(j / 2) + 1].x)*0.5f;
			stack_normals[i + 3][j + 3].y = (normals[i / 2][j / 2].y + normals[(i / 2) + 1][(j / 2) + 1].y)*0.5f;
			stack_normals[i + 3][j + 3].z = (normals[i / 2][j / 2].z + normals[(i / 2) + 1][(j / 2) + 1].z)*0.5f;
		}
	}



}

void Level::MakePhysics()
{
	for (int j = 0; j < y_planes; j++)
	{
		for (int i = 0; i < x_planes; i++)
		{
			m_Terrain[i + (j*x_planes)]->MakePhysicsConvexTriangleTerrain(m_phys);
		}
	}


	ObjInfo dum_objInfo;
	//SetupTerrainNormals();
	back_position = (float((y_planes * 200.0f) - 100.0f));

	back_position = back_position * m_terrain_scale;

	left_right_walls = 8.0f * x_planes;
	front_back_walls = 8.0f * y_planes;

	dum_objInfo.group = (COL_CAR | COL_WORLDSTUFF | COL_BOXES);
	dum_objInfo.mask = COL_WALLS;

	btDefaultMotionState* wall1 = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(left_right_walls + 1.8f, -1, 0)));
	auto wall1Shape = new btStaticPlaneShape(btVector3(-1, 0, 0), 1);//left
	m_phys->AddPhysicalObject(wall1Shape, wall1, 0, btVector3(0, 0, 0), 0.9f, 0.001f, &left_wall_id, &dum_objInfo);

	btDefaultMotionState* wall2 = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(-left_right_walls+0.2f, -1, 0)));
	auto wall2Shape = new btStaticPlaneShape(btVector3(1, 0, 0), 1);//right
	m_phys->AddPhysicalObject(wall2Shape, wall2, 0, btVector3(0, 0, 0), 0.9f, 0.001f, &right_wall_id, &dum_objInfo);

	btDefaultMotionState* wall3 = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, -1, front_back_walls-0.2f)));
	auto wall3Shape = new btStaticPlaneShape(btVector3(0, 0, -1), 1);//back
	m_phys->AddPhysicalObject(wall3Shape, wall3, 0, btVector3(0, 0, 0), 0.9f, 0.001f, &back_wall_id, &dum_objInfo);

	btDefaultMotionState* wall4 = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, -1, -front_back_walls+0.2f)));
	auto wall4Shape = new btStaticPlaneShape(btVector3(0, 0, 1), 1);//front
	m_phys->AddPhysicalObject(wall4Shape, wall4, 0, btVector3(0, 0, 0), 0.9f, 0.001f, &front_wall_id, &dum_objInfo);

	// ground plane
	btDefaultMotionState* ground = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, -1, 0)));
	auto groundShape = new btStaticPlaneShape(btVector3(0, 1, 0), 1);//ground
	m_GroundPlane = m_phys->AddPhysicalObject(groundShape, ground, 0, btVector3(0, 0, 0), 0.9f, 0.001f, &front_wall_id, &dum_objInfo);


	dum_objInfo.group = COL_CAR;
	dum_objInfo.mask = COL_FLOORPLANE;
	btDefaultMotionState* floor = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 1.0f, 0.0f)));
	auto floorShape = new btStaticPlaneShape(btVector3(0, 1, 0), 1);//floor
	m_FloorPlane = m_phys->AddPhysicalObject(floorShape, floor, 0, btVector3(0, 0, 0), 0.001f, 1.0f, &front_wall_id, &dum_objInfo);

}

void Level::CalculateStackNormals()
{
	//return;

	/*
	for (int i = 0; i < (total_y_points * 2) - 2; i += 2) {

		for (int j = 0; j<(total_x_points * 2) - 2; j += 2)
		{
			stack_normals[i + 2][j + 2] = normals[i / 2][j / 2];

			stack_normals[i + 2][j + 3].x = (normals[i / 2][j / 2].x + normals[i / 2][(j / 2) + 1].x)*0.5f;
			stack_normals[i + 2][j + 3].y = (normals[i / 2][j / 2].y + normals[i / 2][(j / 2) + 1].y)*0.5f;
			stack_normals[i + 2][j + 3].z = (normals[i / 2][j / 2].z + normals[i / 2][(j / 2) + 1].z)*0.5f;

			stack_normals[i + 3][j + 2].x = (normals[i / 2][j / 2].x + normals[(i / 2) + 1][(j / 2)].x)*0.5f;
			stack_normals[i + 3][j + 2].y = (normals[i / 2][j / 2].y + normals[(i / 2) + 1][(j / 2)].y)*0.5f;
			stack_normals[i + 3][j + 2].z = (normals[i / 2][j / 2].z + normals[(i / 2) + 1][(j / 2)].z)*0.5f;

			stack_normals[i + 3][j + 3].x = (normals[i / 2][j / 2].x + normals[(i / 2) + 1][(j / 2) + 1].x)*0.5f;
			stack_normals[i + 3][j + 3].y = (normals[i / 2][j / 2].y + normals[(i / 2) + 1][(j / 2) + 1].y)*0.5f;
			stack_normals[i + 3][j + 3].z = (normals[i / 2][j / 2].z + normals[(i / 2) + 1][(j / 2) + 1].z)*0.5f;
		}
	}
	*/


	
	for (int j = 0; j<total_y_points*2; j++)
	{
		for (int i = 0; i<total_x_points*2; i++)
		{
			//index = (m_hminfo.terrainWidth * j) + i;

			float tl = stack_height[clampB(j - 1, total_y_points*2)][clampB(i - 1, total_x_points * 2)];
			float t = stack_height[clampB(j - 1, total_y_points * 2)][clampB(i, total_x_points * 2)];
			float tr = stack_height[clampB(j - 1, total_y_points * 2)][clampB(i + 1, total_x_points * 2)];
			float r = stack_height[clampB(j, total_y_points * 2)][clampB(i + 1, total_x_points * 2)];
			float br = stack_height[clampB(j +1, total_y_points * 2)][clampB(i + 1, total_x_points * 2)];
			float b = stack_height[clampB(j +1, total_y_points * 2)][clampB(i , total_x_points * 2)];
			float bl = stack_height[clampB(j - 1, total_y_points * 2)][clampB(i - 1, total_x_points * 2) ];
			float l = stack_height[clampB(j - 1, total_y_points * 2)][clampB(i, total_x_points * 2)];

			float dX = -((tr + (2.0f * r) + br) - (tl + (2.0f * l) + bl));
			float dZ = (tl + (2.0f * t) + tr) - (bl + (2.0f * b) + br);
			float dY = 11.5;// / pStrength;

			btVector3 v(dX, dY, dZ);
			v.normalize();

			stack_normals[j][i].x = (float)v.getX();
			stack_normals[j][i].y = (float)v.getY();
			stack_normals[j][i].z = (float)v.getZ();


		}
	}


}


Level::~Level(void)
{
	/*
	for(int i=0;i<9;i++)
	{
	m_Terrain[i]->~m_Terrain(void);
	}
	*/
	//delete[] m_hminfo.heightMap;
	//delete[] m_hminfo.normal;

	ClearMemory();
}

void Level::ClearMemory()
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

	for (i = 0; i <total_y_points*2; i++) {
		delete[] stack_normals[i];// = new norm_t[total_x_points];
	}
	delete[] stack_normals;

	for (i = 0; i <total_y_points * 2; i++) {
		delete[] stack_height[i];// = new norm_t[total_x_points];
	}
	delete[] stack_height;

	
	for (i = 0; i <total_y_points; i++) {
		delete[] shadows[i];// = new norm_t[total_x_points];
	}
	delete[] shadows;
	
	for (i = 0; i < total_planes; i++)
	{
		m_Terrain[i]->ClearMemory();
	}


	/*
	for (i = 0; i <total_y_points; i++) {
		delete[] colours[i];// = new norm_t[total_x_points];
	}
	delete[] colours;
	*/

	//for (int loop = 0; loop<total_planes; loop++)
	//{
	//	delete m_Terrain[loop];
	//}
	delete[] m_hminfo.heightMap;
	delete[] m_hminfo.normal;
	delete[] m_hminfo.colour;
	total_planes = 0;



}
