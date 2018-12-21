#include "pch.h"
#include "Fruit.h"

using namespace Simple3DGame;
using namespace DX;
using namespace DirectX;

int FruitSortCB(const VOID* arg1, const VOID* arg2)
{
	part_index* p1 = (part_index*)arg1;
	part_index* p2 = (part_index*)arg2;

	if (p1->dist == p2->dist)
	{
		return 0;
	}
	else
	{
		if (p1->dist<p2->dist)
		{
			return 1;
		}
		else
		{
			return -1;
		}
	}
}

void Fruit::CheckCollisionUpdate()
{
	int i, j;
	//return;

	btVector3 player_pos = btVector3(p_Player->m_constantBufferData.model._14, p_Player->m_constantBufferData.model._24, p_Player->m_constantBufferData.model._34);

	btVector3 pill_pos;


	for (i = 0; i<m_maxParticles; i++)
	{
		//btTransform worldTransform = m_box[i]->m_rigidbody->getWorldTransform();
		//float val = DX::Min_dist(fruit[i].x, fruit[i].z, p_Camera->EyeX(), p_Camera->EyeZ(), p_Camera->EyeX() + p_Camera->LookingTanX(), p_Camera->EyeZ() + p_Camera->LookingTanZ());

		if (fruit[i].bActive == 1 &&
			fruit[i].type != 10
			/*
			p_Camera->CheckPoint()
			val>-20.0f && val<50.0f*/)
		{
			total_collected++;

			pill_pos.setX(fruit[i].x);
			pill_pos.setY(fruit[i].y);
			pill_pos.setZ(fruit[i].z);

			pill_pos = pill_pos - player_pos;

			if (fruit[i].type == 4)
			{
				if (current_ring_to_collect == fruit[i].ring_num)
				{
					beam_posx = fruit[i].x;
					beam_posy = fruit[i].y;
					beam_posz = fruit[i].z;
				}
			}
			if ((fruit[i].type == 4 && pill_pos.length()<2.4f)||
				(pill_pos.length()<1.8f))
			{
				if (fruit[i].type == 4)
				{
					if (fruit[i].ring_num == current_ring_to_collect)
					{
						current_ring_to_collect++;
						fruit[i].bActive = 1;
						fruit[i].type = 10;
						fruit[i].extra_spin = 20.0f;
						rings_collected++;

						if (rings_collected == total_rings)
						{
							p_audio->PlaySoundEffect(SFXFallingEvent);
							p_audio->PlaySoundEffect(SFXAllRingsCollectedSound);
						}
						else
						{
							
							p_audio->PlaySoundEffect(SFXFallingEvent);
						}

					}
				}
				else
				{
					fruit[i].bActive = 0;
				}
				//p_particles->ActivateParticles(fruit[i].x,fruit[i].y+10.0f,fruit[i].z);

				btVector3 vel = p_Player->m_rigidbody->getLinearVelocity();
				float vel_y = vel.getY();
				vel.setY(0.0f);
				float length = vel.length();
				vel.normalize();
				//vel.setY(
				//
				//p_Audio->PlaySoundEffect(SFFruitPop);
				//if( )
				if (fruit[i].type < 4)
				{
					if (fruit[i].type == 3)
					{
						for (j = 0; j < 10; j++)
						{
							p_Cash->CreateOne(fruit[i].x, fruit[i].y + 2.0f, fruit[i].z, 2);
						}
					}
					else
					{
						for (j = 0; j < fruit[i].type+1;j++)
						{
							p_Cash->CreateOne(fruit[i].x, fruit[i].y + 2.0f, fruit[i].z, 2);
						}
					}
				}

				if (fruit[i].type > 6 && fruit[i].type < 11)
				{
					if (fruit[i].type == 10)
					{
						for (j = 0; j < 20; j++)
						{
							p_Cash->CreateOne(fruit[i].x, fruit[i].y + 2.0f, fruit[i].z, 2);
						}
					}
					else
					{
						for (j = 0; j < fruit[i].type - 6; j++)
						{
							p_Cash->CreateOne(fruit[i].x, fruit[i].y + 2.0f, fruit[i].z, 2);
						}
					}
				}

				if (fruit[i].type == 5)
				{
					p_Points->CreateOne(fruit[i].x, fruit[i].y + 1.0f, fruit[i].z, 0);
				}

				if (fruit[i].type == 6)
				{
					p_Player->rockets++;
				}


				XMFLOAT4 col = DirectX::XMFLOAT4(float((rand() % 100) * 0.1f), float((rand() % 100) * 0.1f), float((rand() % 100) * 0.1f), 1.0f);

				/*
				for (j = 0; j < 50; j++)
				{
					p_Splash->CreateOne(fruit[i].x, fruit[i].y + 0.1f, fruit[i].z, col, j / 2);
				}
				*/
				//m_SFPop->PlaySound(0.3f);
				if (fruit[i].type == 5)
				{
					p_audio->PlaySoundEffect(SFXSFCoinCollect);
				}
				else
				{
					if (fruit[i].type != 10 && fruit[i].type != 4)
					{
						if (fruit[i].type == 3)
						{
							p_audio->PlaySoundEffect(SFXChing);
						}
						else
						{
							if (fruit[i].type == 6)
							{
								p_audio->PlaySoundEffect(SFXRocket);
							}
							else
							{
								p_audio->PlaySoundEffect(SFXSFFruitPop);
							}
						}
					}
				}

				if (true) //( length<150.0f )
				{
					vel.normalize();
					vel *= length*1.7f;
					vel.setY(vel_y);

					if (vel.length()>150.0f)
					{
						vel.normalize();
						vel *= 150.0f;
					}

					//p_Player->m_rigidbody->setLinearVelocity(vel);
					//btVector3 player_pos(p_Player->m_constantBuffer.model._14,p_Player->m_constantBuffer.model._24,p_Player->m_constantBuffer.model._34);

					//m_particles->ActivateNext(m_Stage->m_player->m_constantBuffer.model._14,m_Stage->m_player->m_constantBuffer.model._24,m_Stage->m_player->m_constantBuffer.model._34);
				}

				//p_points->CreateOne(fruit[i].x,fruit[i].y+10.0f,fruit[i].z);
			}
			else
			{
				// update model matrix
				//fruit[i].angle+=0.02f;
				//if( fruit[i].angle>2.0f*M_PI )
				//{
				//		fruit[i].angle-=2.0f*M_PI;
				//}
				//	XMMATRIX scaleMatrix = XMMatrixScaling(1.0f , 1.0f, 1.0f);

				//auto rotationMatrix = XMMatrixRotationRollPitchYaw(0.0f,fruit[i].angle,0.0f);
				//	auto translationMatrix = XMMatrixTranslation(fruit[i].x,fruit[i].y+10.0f,fruit[i].z);

				//	XMStoreFloat4x4( &fruit[i].model,XMMatrixTranspose(rotationMatrix * translationMatrix));
			}
		}
		else
		{
			fruit[i].angle += 0.04f;
			if (fruit[i].angle>2.0f*M_PI)
			{
				fruit[i].angle -= 2.0f*M_PI;
			}
		}
	}
}


Fruit::~Fruit(void)
{
	delete[] fruit;
	delete[] p_index;


}


Fruit::Fruit(std::shared_ptr<DX::DeviceResources> pm_deviceResources, Level* pp_Level, Player* pp_Player, Camera^ pp_Camera, Points* pp_Points, Cash* pp_Cash, Textures* pp_Textures)
{
	m_deviceResources = pm_deviceResources;

	p_Points = pp_Points;

	p_Level = pp_Level;

	p_Cash = pp_Cash;

	p_Textures = pp_Textures;

	p_Player = pp_Player;
	//p_Splash = pp_Splash;

	fruit = new fruit_t[5000];

	p_index = new part_index[5000];


	
	bUpdate = true;

	current_particle_que = 0;

	m_particleSize = 2.0f;

	m_maxParticles = 5000;

	current_index = 0;

	total_collected = 0;

	bDesignPointer = false;

	//InitializeBuffers();
	p_Camera = pp_Camera;

	for (int i = 0; i<m_maxParticles; i++)
	{
		fruit[i].bActive = 0;
	}
	//LoadLocations();

	m_Texture[0] = p_Textures->LoadTexture("melon.dds");
	m_Texture[1] = p_Textures->LoadTexture("pear.dds");
	m_Texture[2] = p_Textures->LoadTexture("red_apple.dds");
	m_Texture[3] = p_Textures->LoadTexture("crystal2.dds");
	m_Texture[4] = p_Textures->LoadTexture("cherries.dds");
	m_Texture[5] = p_Textures->LoadTexture("coin-texture.dds");
	m_Texture[6] = p_Textures->LoadTexture("rock1.dds");
	m_Texture[7] = p_Textures->LoadTexture("mushroomred.dds");
	m_Texture[8] = p_Textures->LoadTexture("mushroomgreen.dds");
	m_Texture[9] = p_Textures->LoadTexture("mushroomblue.dds");
	m_Texture[10] = p_Textures->LoadTexture("pica.dds");
	m_BeamTexture = p_Textures->LoadTexture("beam.dds");

	/*
//	CreateDDSTextureFromFile((ID3D11Device *)pm_d3dDevice.Get(), L"Assets/orange.dds", nullptr, &m_Texture[0], MAXSIZE_T);
	CreateDDSTextureFromFile((ID3D11Device *)pm_d3dDevice.Get(), L"Assets/Textures/melon.dds", nullptr, &m_Texture[0], MAXSIZE_T);
	CreateDDSTextureFromFile((ID3D11Device *)pm_d3dDevice.Get(), L"Assets/Textures/pear.dds", nullptr, &m_Texture[1], MAXSIZE_T);
	CreateDDSTextureFromFile((ID3D11Device *)pm_d3dDevice.Get(), L"Assets/Textures/red_apple.dds", nullptr, &m_Texture[2], MAXSIZE_T);
	CreateDDSTextureFromFile((ID3D11Device *)pm_d3dDevice.Get(), L"Assets/Textures/pica.dds", nullptr, &m_Texture[3], MAXSIZE_T);
	CreateDDSTextureFromFile((ID3D11Device *)pm_d3dDevice.Get(), L"Assets/Textures/cherries.dds", nullptr, &m_Texture[4], MAXSIZE_T);
	CreateDDSTextureFromFile((ID3D11Device *)pm_d3dDevice.Get(), L"Assets/Textures/coin-texture.dds", nullptr, &m_Texture[5], MAXSIZE_T);
	CreateDDSTextureFromFile((ID3D11Device *)pm_d3dDevice.Get(), L"Assets/Textures/poke.dds", nullptr, &m_Texture[6], MAXSIZE_T);

	CreateDDSTextureFromFile((ID3D11Device *)pm_d3dDevice.Get(), L"Assets/Textures/mushroomred.dds", nullptr, &m_Texture[7], MAXSIZE_T);
	CreateDDSTextureFromFile((ID3D11Device *)pm_d3dDevice.Get(), L"Assets/Textures/mushroomgreen.dds", nullptr, &m_Texture[8], MAXSIZE_T);
	CreateDDSTextureFromFile((ID3D11Device *)pm_d3dDevice.Get(), L"Assets/Textures/mushroomblue.dds", nullptr, &m_Texture[9], MAXSIZE_T);

	CreateDDSTextureFromFile((ID3D11Device *)pm_d3dDevice.Get(), L"Assets/Textures/pica.dds", nullptr, &m_Texture[10], MAXSIZE_T);

	CreateDDSTextureFromFile((ID3D11Device *)pm_d3dDevice.Get(), L"Assets/Textures/beam.dds", nullptr, &m_BeamTexture, MAXSIZE_T);
	*/

	m_Beam = new ModelLoader(m_deviceResources);

	m_Beam->Load("Assets/Models/beam.obj", 3.0f,1);

	m_Beam->SetModelColourWeight(1.5f, 1.5f, 1.5f, 0.0f);
	m_Beam->SetModelNormals(0.0f, 1.0f, 0.0f);

	for (int i = 0; i < 11; i++)
	{
		m_model[i] = new ModelLoader(m_deviceResources);
	}

//	m_model[0]->Load("Assets/orange.obj", pm_d3dDevice, -0.3f);
	m_model[0]->Load("Assets/Models/melon.obj",  -0.3f,1);
	m_model[1]->Load("Assets/Models/pear.obj",  -0.3f,1);
	m_model[2]->Load("Assets/Models/heart.obj",  -0.3f,1);
	m_model[3]->Load("Assets/Models/crystal2.obj",  -1.3f,1); // 0.3f banana
	m_model[4]->Load("Assets/Models/ring.obj",  -1.3f,1);
	m_model[5]->Load("Assets/Models/coin2.obj",  -0.3f,1);
	m_model[6]->Load("Assets/Models/rocket_pick.obj",  -1.5f,1);

	m_model[7]->Load("Assets/Models/mushroom.obj",  -0.6f,1);
	m_model[8]->Load("Assets/Models/mushroom.obj",  -0.6f,1);
	m_model[9]->Load("Assets/Models/mushroom.obj",  -0.6f,1);

	m_model[10]->Load("Assets/Models/ring.obj",  -1.3f,1);

	m_model[3]->UpsideDown();
	m_model[7]->UpsideDown();
	m_model[8]->UpsideDown();
	m_model[9]->UpsideDown();

	m_model[3]->FlipVertDir();
	m_model[7]->FlipVertDir();
	m_model[8]->FlipVertDir();
	m_model[9]->FlipVertDir();

	m_model[0]->SetModelSpecular(1.0f);
	m_model[1]->SetModelSpecular(1.0f);
	m_model[2]->SetModelSpecular(1.0f);

	m_model[4]->SetModelSpecular(0.6f);
	m_model[5]->SetModelSpecular(0.4f);

	m_model[10]->SetModelSpecular(1.0f);

	m_model[3]->SetModelSpecular(10.5f);

	m_Sphere = new ModelLoader(m_deviceResources);
	m_Sphere->Load("Assets/Models/isospere.obj", 0.5f);
	m_Sphere->FlipVertDir();
	m_Sphere->FlipNorms();
	m_Sphere->SetModelSpecular(1.0f);
	//m_Sphere->Optimize();

	m_CrystalTexture = p_Textures->LoadTexture("ice.dds");

	design_pointer.bActive = 0;
	/*
	for (int i = 0; i < 10; i++)
	{
		m_numb[i] = new ModelLoader(m_deviceResources);
	}

	m_numb[0]->Load("Assets/Models/num0.obj",  1.0f);
	m_numb[1]->Load("Assets/Models/num1.obj",  -1.0f);
	m_numb[2]->Load("Assets/Models/num2.obj",  1.0f);
	m_numb[3]->Load("Assets/Models/num3.obj",  -1.0f);
	m_numb[4]->Load("Assets/Models/num4.obj",  -1.0f);
	m_numb[5]->Load("Assets/Models/num5.obj",  -1.0f);
	m_numb[6]->Load("Assets/Models/num6.obj",  1.0f);
	m_numb[7]->Load("Assets/Models/num7.obj",  1.0f);
	m_numb[8]->Load("Assets/Models/num8.obj",  1.0f);
	m_numb[9]->Load("Assets/Models/num9.obj",  1.0f);

	//m_numb[0]->FlipVertDir();
	m_numb[2]->UpsideDown();
	m_numb[2]->FlipNorms();

	//m_numb[6]->UpsideDown();
	m_numb[6]->FlipNorms();

	//m_numb[0]->FlipNorms();
	m_numb[8]->FlipNorms();

	m_numb[7]->FlipNorms();

	m_numb[6]->UpsideDown();
	*/

	//m_numb[6]->FlipVertDir();
	//m_numb[6]->FlipNorms();
	//m_model[4]->Load("Assets/cherries.obj", pm_d3dDevice, -0.3f);
	/*
	auto pointVSTask = DX::ReadDataAsync("PointVertexShader.cso");
	auto pointPSTask = DX::ReadDataAsync("PointPixelShader.cso");

	auto createpointVSTask = pointVSTask.then([this](Platform::Array<byte>^ fileData) {
	DX::ThrowIfFailed(
	m_d3dDevice->CreateVertexShader(
	fileData->Data,
	fileData->Length,
	nullptr,
	&m_point_vertexShader
	)
	);

	const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
	{ "POSITION", 0,   DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0,    DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR", 0,   DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	DX::ThrowIfFailed(
	m_d3dDevice->CreateInputLayout(
	vertexDesc,
	ARRAYSIZE(vertexDesc),
	fileData->Data,
	fileData->Length,
	&m_point_inputLayout
	)
	);
	});

	auto createpointPSTask = pointPSTask.then([this](Platform::Array<byte>^ fileData) {
	DX::ThrowIfFailed(
	m_d3dDevice->CreatePixelShader(
	fileData->Data,
	fileData->Length,
	nullptr,
	&m_point_pixelShader
	)
	);
	});
	*/


	XMMATRIX scaleMatrix = XMMatrixScaling(1.0f, 1.0f, 1.0f);

	auto rotationMatrix = XMMatrixRotationRollPitchYaw(0.0f, 0.0f, 0.0f);
	auto translationMatrix = XMMatrixTranslation(0.0f, 20.0f, 0.0f);

	XMStoreFloat4x4(&m_constantBufferB.model, XMMatrixTranspose(rotationMatrix * translationMatrix));

	//XMStoreFloat4x4( &m_constantBuffer.model,modelMatrix);

}

void Fruit::SetBeamColor(float r, float g, float b)
{
	m_Beam->SetModelColourWeight(r, g, b, 0.0f);


}

void Fruit::SaveBinary(int level)
{
	char local_file_folder[140];
	level = current_level;
	Windows::Storage::StorageFolder^ folder = Windows::Storage::ApplicationData::Current->LocalFolder;
	wcstombs(local_file_folder, folder->Path->Data(), sizeof(local_file_folder));
	// update local crazy
	//sprintf(local_file_folder, "C:\\Users\\craig\\Desktop\\Simple3DGame3\\Simple3DGame\\Assets");

	// Load level scores
	char info_filename[140];
	wchar_t w_info_filename[140];
	sprintf(info_filename, "%s\\LevelBinary\\FruitData", local_file_folder);
	_swprintf(w_info_filename, L"%hs", info_filename);
	CreateDirectory(w_info_filename, NULL);

	sprintf(info_filename, "%s\\LevelBinary\\FruitData\\%d.bmp", local_file_folder, level);

	FILE* pFile = fopen(info_filename, "wb");
	if (pFile != NULL)
	{
		fwrite(fruit, sizeof(fruit_t) , 5000, pFile);
		fclose(pFile);
	}

}

bool Fruit::LoadBinary(int level,int location)
{
	char local_file_folder[140];

	Windows::Storage::StorageFolder^ folder = Windows::Storage::ApplicationData::Current->LocalFolder;
	wcstombs(local_file_folder, folder->Path->Data(), sizeof(local_file_folder));
	// update local crazy
	//sprintf(local_file_folder, "C:\\Users\\craig\\Desktop\\Simple3DGame3\\Simple3DGame\\Assets");

	// Load level scores
	char info_filename[140];
	if (location == 0)
	{
		sprintf(info_filename, "%s\\LevelBinary\\FruitData\\%d.bmp", local_file_folder, level);
	}
	else
	{
		sprintf(info_filename, "Assets\\LevelBinary\\FruitData\\%d.bmp", level);
	}

	// LoadLevelData
	FILE * pFile;

	current_pill = 0;
	/**/
	total_rings = 0;
	rings_collected = 0;
	current_ring_num = 1;

	pFile = fopen(info_filename, "r");
	if (pFile != NULL)
	{
		fread(fruit, sizeof(fruit_t) , 5000, pFile);
		fclose(pFile);
	}
	else
	{
		return false;
	}

	for (int i = 0; i < m_maxParticles; i++)
	{
		if (fruit[i].bActive == 1)
		{
			//fruit[i].y = 0.5f + p_Level->RayGetTerrainHeight(fruit[i].x, fruit[i].z);
			//auto rotationMatrix = XMMatrixRotationRollPitchYaw(0.0f, fruit[i].angle, 0.0f);
			//auto translationMatrix = XMMatrixTranslation(fruit[i].x, fruit[i].y, fruit[i].z);
			//DirectX::XMStoreFloat4x4(&tree[i].model_matrix, XMMatrixTranspose(rotationMatrix * translationMatrix));
			if (fruit[i].type == 4)
				total_rings++;

			if (fruit[i].ring_num >= current_ring_num)
			{
				current_ring_num = fruit[i].ring_num + 1;

			}

			if (current_pill<i + 1 )
			{
				current_pill = i + 1;
			}
		}
	}
	total_pills = current_pill;
	return true;
	//TreeLights();
}




bool Fruit::LoadLevel(int level, int location)
{
	current_level = level;
	current_ring_to_collect = 1;

	for (int i = 0; i < m_maxParticles; i++)
	{
		fruit[i].bActive = 0;
	}

	if (LoadBinary(level, location) == false)
	{
		return true;// LoadLocations(level);
	}
	else
	{
		return true;
	}
	//return LoadLocations(level);
}

void Fruit::SetDesignPointer(float x, float y, float z, int type)
{
	design_pointer.bActive = 1;
	design_pointer.x = x;
	design_pointer.y = y;
	design_pointer.z = z;
	design_pointer.type = type;

	if (type == 4)
		design_pointer.y += 1.5f;

	//design_pointer.
	bDesignPointer = true;
}

void Fruit::SetDesignPointer(float x, float y, float z, float _yaw, float _pitch , float _roll, int type)
{
	design_pointer.bActive = 1;
	design_pointer.x = x;
	design_pointer.y = y;
	design_pointer.z = z;
	design_pointer.type = type;
	design_pointer.yaw = _yaw;
	design_pointer.pitch = _pitch;
	design_pointer.roll = _roll;

	if (type == 4)
		design_pointer.y += 1.5f;
	//design_pointer.
	bDesignPointer = true;
}


void Fruit::CreateOneMatrix(float x, float y, float z, int type, DirectX::XMMATRIX* _rotation)
{
	int i;
	bool bFound = false;;
	int point_find = 0;
	DirectX::XMFLOAT4 col = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	//if (type > 9) return;

	// Check exists
	for (i = 0; i < m_maxParticles; i++)
	{
		if (fruit[i].bActive == 1 && fruit[i].x == x && fruit[i].z == z)
		{
			bFound = true;
			if (type == -1)
			{
				fruit[i].bActive = 0;
			}
			else
			{

				fruit[i].type = type;
			}
		}
	}

	if (bFound == true || type == -1)
		return;

	for (i = 0; i < m_maxParticles; i++)
	{
		if (fruit[i].bActive == 0)
		{
			fruit[i].bActive = 1;

			if (type == 4)
				y += 1.5f;

			fruit[i].x = x;
			fruit[i].y = y;
			fruit[i].z = z;

			fruit[i].bCollected = false;
			fruit[i].extra_spin = 0.0f;
			//if (type == 4)
			//	design_pointer.y += 2.0f;

			fruit[i].yaw = design_pointer.yaw;
			fruit[i].pitch = design_pointer.pitch;
			fruit[i].roll = design_pointer.roll;

			fruit[i].rotation = *_rotation;

			fruit[i].type = type;

			fruit[i].angle = 0.0f;

			if (total_pills < i + 1)
				total_pills = i + 1;
			break;
		}
	}
	// angle based on x and z
	for (i = 0; i < m_maxParticles; i++)
	{
		if (fruit[i].bActive == 1)
		{
			fruit[i].angle = ((fruit[i].z / 100.0f) + (fruit[i].x / 20.0f))*M_PI;;
		}
	}
}


void Fruit::CreateOne(float x, float y, float z, int type)
{
	int i;
	bool bFound = false;;
	int point_find = 0;
	DirectX::XMFLOAT4 col = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	//if (type > 9) return;

	// Check exists
	for (i = 0; i < m_maxParticles; i++)
	{
		if (fruit[i].bActive == 1 && fruit[i].x == x && fruit[i].z == z)
		{
			bFound = true;
			if (type == -1)
			{
				fruit[i].bActive = 0;
			}
			else
			{

				fruit[i].type = type;
			}
		}
	}

	if (bFound == true || type==-1)
		return;

	for (i = 0; i < m_maxParticles; i++)
	{
		if (fruit[i].bActive == 0)
		{
			fruit[i].bActive = 1;

			if (type == 4)
				y += 1.5f;

			fruit[i].x = x;
			fruit[i].y = y;
			fruit[i].z = z;
			fruit[i].bCollected = false;
			fruit[i].extra_spin = 0.0f;
			//if (type == 4)
			//	design_pointer.y += 2.0f;

			fruit[i].yaw = design_pointer.yaw;
			fruit[i].pitch = design_pointer.pitch;
			fruit[i].roll = design_pointer.roll;

			fruit[i].rotation = XMMatrixRotationRollPitchYaw(design_pointer.yaw, design_pointer.pitch, design_pointer.roll);

			fruit[i].type = type;

			if (type == 4)
			{
				fruit[i].ring_num = current_ring_num;
				current_ring_num++;
			}

			fruit[i].scale = 1.0f;

			fruit[i].angle = 0.0f;
			fruit[i].anglex[0] = 0.0f;
			fruit[i].anglex[1] = 0.0f;
			fruit[i].anglex[2] = 0.0f;
			fruit[i].anglez[0] = 0.0f;
			fruit[i].anglez[1] = 0.0f;
			fruit[i].anglez[2] = 0.0f;

			if (total_pills < i + 1)
				total_pills = i + 1;
			break;
		}
	}
	// angle based on x and z
	for (i = 0; i < m_maxParticles; i++)
	{
		if (fruit[i].bActive == 1)
		{
			fruit[i].angle = ((fruit[i].z / 100.0f) + (fruit[i].x / 20.0f))*M_PI;;
		}
	}
}



bool Fruit::LoadLocations(int level)
{
	FILE *filePtr;							// Point to the current position in the file
	BITMAPFILEHEADER bitmapFileHeader;		// Structure which stores information about file
	BITMAPINFOHEADER bitmapInfoHeader;		// Structure which stores information about image
	int imageSize, index;
	unsigned char height_r;
	unsigned char height_g;
	unsigned char height_b;
	HeightMapInfo hminfo;
	int fruit_type;
	int bFruit;

	char file_folder[50];

	sprintf(file_folder, "Assets\\LevelMaps\\%d\\fruitmap.bmp", level);

	// Open the file
	filePtr = fopen(file_folder, "rb");
	if (filePtr == NULL)
		return 0;

	// Read bitmaps header
	fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);

	// Read the info header
	fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);

	// Get the width and height (width and length) of the image
	hminfo.terrainWidth = bitmapInfoHeader.biWidth;
	hminfo.terrainHeight = bitmapInfoHeader.biHeight;

	// Size of the image in bytes. the 3 represents RBG (byte, byte, byte) for each pixel
	imageSize = hminfo.terrainWidth * hminfo.terrainHeight * 3;
	/*
	unsigned char* bitmapImage = (unsigned char*)malloc(imageSize);
	hminfo.heightMap = (XMFLOAT3 *)malloc(imageSize*sizeof(XMFLOAT3));
	hminfo.normal = (XMFLOAT3 *)malloc(imageSize*sizeof(XMFLOAT3));
	*/
	unsigned char* bitmapImage = new unsigned char[imageSize];
	hminfo.heightMap = new DirectX::XMFLOAT3[imageSize*sizeof(DirectX::XMFLOAT3)];
	hminfo.normal = new DirectX::XMFLOAT3[imageSize*sizeof(DirectX::XMFLOAT3)];

	//ZeroMemory(&bitmapImage, sizeof(imageSize));
	//ZeroMemory(&hminfo.heightMap, imageSize*sizeof(XMFLOAT3));
	//ZeroMemory(&hminfo.normal, imageSize*sizeof(XMFLOAT3));

	// Set the file pointer to the beginning of the image data
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	// Store image data in bitmapImage
	fread(bitmapImage, 1, imageSize, filePtr);

	// Close file
	fclose(filePtr);

	// We use a greyscale image, so all 3 rgb values are the same, but we only need one for the height
	// So we use this counter to skip the next two components in the image data (we read R, then skip BG)
	int k = 0;
	int j2;
	int i2;
	int pos;

	for (int i = 0; i<m_maxParticles; i++)
	{
		fruit[i].bActive = 0;
	}

	// We divide the height by this number to "water down" the terrains height, otherwise the terrain will
	// appear to be "spikey" and not so smooth.
	current_pill = 0;
	//p_Level->m_hminfo
	total_pills = 0;
	// Read the image data into our heightMap array
	//return true;
	for (fruit_type = 0; fruit_type<7; fruit_type++)
	{
		k = 0;
		for (int j = 0; j< hminfo.terrainHeight; j++)
		{
			for (int i = 0; i< hminfo.terrainWidth-2; i++)
			{
				pos = (j*(hminfo.terrainWidth * 3)) + ((hminfo.terrainWidth - 1) * 3) - ((i * 3));

				height_b = bitmapImage[pos];
				height_g = bitmapImage[pos + 1];
				height_r = bitmapImage[pos + 2];
				bFruit = 0;

				if (fruit_type == 0) // orange
				{
					if (height_b == 50 && height_g == 150 && height_r == 200)
					{
						bFruit = 1;
					}
				}

				if (fruit_type == 1) // pear
				{
					if (height_b == 50 && height_g == 150 && height_r == 100)
					{
						bFruit = 1;
					}
				}

				if (fruit_type == 2) // apple
				{
					if (height_b == 50 && height_g == 100 && height_r == 150)
					{
						bFruit = 1;
					}
				}

				if (fruit_type == 3) // banana
				{
					if (height_b == 50 && height_g == 150 && height_r == 150)
					{
						bFruit = 1;
					}
				}

				if (fruit_type == 4) // cherries
				{
					if (height_b == 0 && height_g == 0 && height_r == 255)
					{
						bFruit = 1;
					}
				}

				if (fruit_type == 5) // coin
				{
					if (height_b == 255 && height_g == 255 && height_r == 255)
					{
						bFruit = 1;
					}
				}

				if (fruit_type == 6) // rocket
				{
					if (height_b == 0 && height_g == 255 && height_r == 0)
					{
						bFruit = 1;
					}
				}

				if (bFruit == 1)
				{
					fruit[current_pill].x = -((float)p_Level->left_right_walls) - 1.0f + ((float)i*1.0f);

					fruit[current_pill].z = -p_Level->front_back_walls - 1.0f + ((float)j*1.0f);

					fruit[current_pill].y = 0.5f + p_Level->GetTerrainHeight((int)fruit[current_pill].x, (fruit[current_pill].z));

					if (current_pill<m_maxParticles - 1 && fruit[current_pill].y>-990.0f)
					{
						total_pills++;
						fruit[current_pill].bActive = 1;

						fruit[current_pill].type = fruit_type;

						fruit[current_pill].angle = (2.0f*M_PI)*(float(fruit[current_pill].z)*0.0001f);

						current_pill++;

					}
				}
				k += 3;

				/*
				index = ( hminfo.terrainHeight * j) + i;

				hminfo.heightMap[index].x = (float)i;
				hminfo.heightMap[index].y = (float)height * height_scale;
				hminfo.heightMap[index].z = (float)j;
				*/
			}
		}
	}

	delete[] bitmapImage;
	bitmapImage = 0;

	delete[] hminfo.heightMap;
	delete[] hminfo.normal;

	return true;
}






void Fruit::UpdateProjectionMatrix(DirectX::XMMATRIX *projectionMatrix)
{
	XMStoreFloat4x4(&m_constantBufferB.projection, *projectionMatrix);
}


float Fruit::min_dist(float x, float y, float x1, float y1, float x2, float y2)
{
	float A = x - x1;
	float B = y - y1;
	float C = x2 - x1;
	float D = y2 - y1;

	float dist = abs(A * D - C * B) / sqrt(C * C + D * D);

	if ((((x - x1)*(y2 - y1)) - ((y - y1)*(x2 - x1)))<0)
	{
		dist = -dist;
	}

	return dist;
}



void Fruit::Update(float timeDelta)
{
	int i;

	for (i = 0; i < total_pills; i++)
	{
		fruit[i].angle += 1.02f*timeDelta;
		if (fruit[i].extra_spin > 0.0f)
		{
			fruit[i].angle += fruit[i].extra_spin * 1.02f*timeDelta;
			fruit[i].extra_spin -= 15.1f*timeDelta;
			fruit[i].anglex[0] -= fruit[i].anglex[0] * 5.0f*timeDelta;
			fruit[i].anglex[1] -= fruit[i].anglex[1] * 5.0f*timeDelta;
			fruit[i].anglex[2] -= fruit[i].anglex[2] * 5.0f*timeDelta;
			fruit[i].anglez[0] -= fruit[i].anglez[0] * 5.0f*timeDelta;
			fruit[i].anglez[1] -= fruit[i].anglez[1] * 5.0f*timeDelta;
			fruit[i].anglez[2] -= fruit[i].anglez[2] * 5.0f*timeDelta;


			//fruit[i].scale = fruit[i].scale - 0.1f*timeDelta;
		}

		if (fruit[i].angle>2.0f*M_PI)
		{
			fruit[i].angle -= 2.0f*M_PI;
		}

		if (fruit[i].type == 4)
		{
			for (int j = 0; j < 3; j++)
			{
				fruit[i].anglex[j] += (0.2f)*((float)j+1)*timeDelta;
				if (fruit[i].anglex[j]>2.0f*M_PI)
				{
					fruit[i].anglex[j] -= 2.0f*M_PI;
				}
			}
		}
	}
}

void Fruit::RenderBeam(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> deviceContext, Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer, ModelViewProjectionConstantBuffer* constantBufferData)
{
	UINT stride = sizeof(VertexPositionTex);
	UINT offset = 0;

	if (rings_collected == total_rings)
		return;

	XMFLOAT4X4 x_mat = *p_Camera->GetViewMatrix();

	deviceContext->PSSetShaderResources(0, 1, &m_BeamTexture);

	btVector3 tran;
	btQuaternion quat_rot;
	//btMatrix3x3 rot_mat = btMatrix3x3(mats._11, mats._12, mats._13, mats._21, mats._22, mats._23, mats._31, mats._32, mats._33);
	//btMatrix3x3 rot_matb
	//particle_count=1;
	float rot_angle_yaw, rot_angle_pitch, rot_angle_roll;
	btMatrix3x3 rot_mat = btMatrix3x3(x_mat._11, x_mat._12, x_mat._13, x_mat._21, x_mat._22, x_mat._23, x_mat._31, x_mat._32, x_mat._33);
	rot_mat.getEulerYPR(rot_angle_yaw, rot_angle_pitch, rot_angle_roll);


	deviceContext->IASetVertexBuffers(0, 1, m_Beam->GetVertexBuffer().GetAddressOf(), &stride, &offset);
	deviceContext->IASetIndexBuffer(m_Beam->GetIndexBuffer().Get(), DXGI_FORMAT_R16_UINT, 0);

	auto rotationMatrix = XMMatrixRotationRollPitchYaw(0.0f, -rot_angle_yaw, 0.0f);
	auto translationMatrix = XMMatrixTranslation(beam_posx, beam_posy+4.0f, beam_posz);

	XMStoreFloat4x4(&constantBufferData->model, XMMatrixTranspose(rotationMatrix * translationMatrix));

	deviceContext->UpdateSubresource(constantBuffer.Get(), 0, NULL, constantBufferData, 0, 0);

	deviceContext->DrawIndexed(
		m_Beam->m_indicesCount,
		0,
		0
		);
}


void Fruit::Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> deviceContext, Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer, ModelViewProjectionConstantBuffer* constantBufferData)
{
	UINT stride = sizeof(VertexPositionTex);
	UINT offset = 0;

	int current_fruit = 999;
	if (bDesignMode == false)
	{
		CheckCollisionUpdate();
	}

	XMMATRIX scaleMatrix;// = XMMatrixRotationRollPitchYaw(1.0f - (0.1f*j), 1.0f - (0.1f*j), 1.0f - (0.1f*j));
	XMMATRIX rotationMatrix;// = XMMatrixRotationRollPitchYaw(fruit[i].anglex, fruit[i].angle, fruit[i].anglez);
	XMMATRIX translationMatrix;// = XMMatrixTranslation(fruit[i].x, fruit[i].y, fruit[i].z);

	for (int i = 0; i<total_pills; i++)
	{


		//fruit[i].x += 0.01f;
		//fruit[i].y = 0.5f + p_Level->GetTerrainHeight(20.0f-fruit[i].x, 10.0f-fruit[i].y, p_Level->scale_height*2.0f);

		//float val = DX::Min_dist(fruit[i].x, fruit[i].z, p_Camera->EyeX(), p_Camera->EyeZ(), p_Camera->EyeX() + p_Camera->LookingTanX(), p_Camera->EyeZ() + p_Camera->LookingTanZ());

		//float val = DX::Min_dist(worldTransform.getOrigin().getX(), worldTransform.getOrigin().getZ(), p_Camera->EyeX(), p_Camera->EyeZ(), p_Camera->EyeX() + p_Camera->LookingTanX(), p_Camera->EyeZ() + p_Camera->LookingTanZ());

		if (fruit[i].bActive == 1 && p_Camera->CheckPoint(fruit[i].x, fruit[i].y, fruit[i].z,1.0f)) /*(fruit[i].bActive == 1 &&
			val>FIN_DIST && val<FAR_DIST)*/
		{

			if(fruit[i].type != 4 && fruit[i].type != 10)
			{
				if (current_fruit != fruit[i].type)
				{
					current_fruit = fruit[i].type;
				}

				deviceContext->IASetVertexBuffers(0, 1, m_model[current_fruit]->GetVertexBuffer().GetAddressOf(), &stride, &offset);
				deviceContext->IASetIndexBuffer(m_model[current_fruit]->GetIndexBuffer().Get(), DXGI_FORMAT_R16_UINT, 0);

				deviceContext->PSSetShaderResources(0, 1, &m_Texture[current_fruit]);


				rotationMatrix = XMMatrixRotationRollPitchYaw(0.0f, fruit[i].angle, 0.0f);
				translationMatrix = XMMatrixTranslation(fruit[i].x, fruit[i].y, fruit[i].z);
				XMStoreFloat4x4(&constantBufferData->model, XMMatrixTranspose(rotationMatrix * fruit[i].rotation * translationMatrix));

				//constantBufferData->model = m_constantBuffer.model;
				//constantBufferData->model = m_constantBuffer.model;

				//m_constantBufferB.model = fruit[i].model;
				deviceContext->UpdateSubresource(constantBuffer.Get(), 0, NULL, constantBufferData, 0, 0);

				deviceContext->DrawIndexed(
					m_model[current_fruit]->m_indicesCount,
					0,
					0
					);
			}
			// Set the vertex buffer to active in the input assembler so it can be rendered.

			//XMMATRIX scaleMatrix = XMMatrixScaling(1.0f , 1.0f, 1.0f);

			//auto rotationMatrix = XMMatrixRotationRollPitchYaw(0.0f,fruit[i].angle,0.0f);
			//auto translationMatrix = XMMatrixTranslation(fruit[i].x,fruit[i].y+10.0f,fruit[i].z);

			//MVPConstantBuffer m_constantBuffer = m_constantBufferB;

			//XMStoreFloat4x4( &m_constantBuffer.model,XMMatrixTranspose(rotationMatrix * translationMatrix));

			//m_constantBuffer.projection=m_constantBufferB.projection;
			//m_constantBuffer.view=m_constantBufferB.view;
			
			//auto rotationMatrix_land = XMMatrixRotationRollPitchYaw(fruit[i].yaw, fruit[i].pitch, fruit[i].roll);



			if ((fruit[i].type == 4 || fruit[i].type == 10) && fruit[i].ring_num>0 )
			{
				current_fruit = fruit[i].type;



				deviceContext->IASetVertexBuffers(0, 1, m_model[current_fruit]->GetVertexBuffer().GetAddressOf(), &stride, &offset);
				deviceContext->IASetIndexBuffer(m_model[current_fruit]->GetIndexBuffer().Get(), DXGI_FORMAT_R16_UINT, 0);

				deviceContext->PSSetShaderResources(0, 1, &m_Texture[current_fruit]);

				for (int j = 0; j < 3;j++)
				{
					scaleMatrix = XMMatrixScaling(1.0f -(0.1f*j), 1.0f - (0.1f*j), 1.0f - (0.1f*j));
					rotationMatrix = XMMatrixRotationRollPitchYaw(fruit[i].anglex[j], fruit[i].angle, fruit[i].anglez[j]);
					translationMatrix = XMMatrixTranslation(fruit[i].x, fruit[i].y, fruit[i].z);
					XMStoreFloat4x4(&constantBufferData->model, XMMatrixTranspose(scaleMatrix * rotationMatrix * fruit[i].rotation * translationMatrix));

					//constantBufferData->model = m_constantBuffer.model;
					//constantBufferData->model = m_constantBuffer.model;

					//m_constantBufferB.model = fruit[i].model;
					deviceContext->UpdateSubresource(constantBuffer.Get(), 0, NULL, constantBufferData, 0, 0);

					deviceContext->DrawIndexed(
						m_model[current_fruit]->m_indicesCount,
						0,
						0
						);
				}

				if (fruit[i].ring_num == rings_collected+1)
				{
					rotationMatrix = XMMatrixRotationRollPitchYaw(0.0f, fruit[i].angle, 0.0f);
					translationMatrix = XMMatrixTranslation(fruit[i].x, fruit[i].y, fruit[i].z);
					XMStoreFloat4x4(&constantBufferData->model, XMMatrixTranspose(rotationMatrix * translationMatrix));
					deviceContext->UpdateSubresource(constantBuffer.Get(), 0, NULL, constantBufferData, 0, 0);

					deviceContext->PSSetShaderResources(0, 1, &m_CrystalTexture);

					deviceContext->IASetVertexBuffers(0, 1, m_Sphere->GetVertexBuffer().GetAddressOf(), &stride, &offset);
					deviceContext->IASetIndexBuffer(m_Sphere->GetIndexBuffer().Get(), DXGI_FORMAT_R16_UINT, 0);

					deviceContext->DrawIndexed(
						m_Sphere->m_indicesCount,
						0,
						0
						);

				}
				
				/*
				if (fruit[i].ring_num < 10)
				{
					rotationMatrix = XMMatrixRotationRollPitchYaw(0.0f, fruit[i].angle, 0.0f);
					translationMatrix = XMMatrixTranslation(fruit[i].x, fruit[i].y, fruit[i].z);
					XMStoreFloat4x4(&constantBufferData->model, XMMatrixTranspose(rotationMatrix * translationMatrix));
					deviceContext->UpdateSubresource(constantBuffer.Get(), 0, NULL, constantBufferData, 0, 0);

					deviceContext->IASetVertexBuffers(0, 1, m_numb[fruit[i].ring_num]->GetVertexBuffer().GetAddressOf(), &stride, &offset);
					deviceContext->IASetIndexBuffer(m_model[fruit[i].ring_num]->GetIndexBuffer().Get(), DXGI_FORMAT_R16_UINT, 0);

					deviceContext->DrawIndexed(
						m_numb[fruit[i].ring_num]->m_indicesCount,
						0,
						0
						);
				}
				else
				{
					int num1 = fruit[i].ring_num / 10;
					int num2 = fruit[i].ring_num % 10;

					deviceContext->IASetVertexBuffers(0, 1, m_numb[num1]->GetVertexBuffer().GetAddressOf(), &stride, &offset);
					deviceContext->IASetIndexBuffer(m_model[num1]->GetIndexBuffer().Get(), DXGI_FORMAT_R16_UINT, 0);

					auto tran2aMatrix = XMMatrixTranslation(-0.2f, 0.0f, 0.0f);
					rotationMatrix = XMMatrixRotationRollPitchYaw(0.0f, fruit[i].angle, 0.0f);
					translationMatrix = XMMatrixTranslation(fruit[i].x, fruit[i].y, fruit[i].z);
					XMStoreFloat4x4(&constantBufferData->model, XMMatrixTranspose((tran2aMatrix * rotationMatrix) * (fruit[i].rotation * translationMatrix)));

					deviceContext->UpdateSubresource(constantBuffer.Get(), 0, NULL, constantBufferData, 0, 0);

					deviceContext->DrawIndexed(
						m_numb[num1]->m_indicesCount,
						0,
						0
						);


					deviceContext->IASetVertexBuffers(0, 1, m_numb[num2]->GetVertexBuffer().GetAddressOf(), &stride, &offset);
					deviceContext->IASetIndexBuffer(m_model[num2]->GetIndexBuffer().Get(), DXGI_FORMAT_R16_UINT, 0);

					tran2aMatrix = XMMatrixTranslation(0.2f, 0.0f, 0.0f);
					rotationMatrix = XMMatrixRotationRollPitchYaw(0.0f, fruit[i].angle, 0.0f);
					translationMatrix = XMMatrixTranslation(fruit[i].x, fruit[i].y, fruit[i].z);
					XMStoreFloat4x4(&constantBufferData->model, XMMatrixTranspose((tran2aMatrix * rotationMatrix) * (fruit[i].rotation * translationMatrix)));

					deviceContext->UpdateSubresource(constantBuffer.Get(), 0, NULL, constantBufferData, 0, 0);

					deviceContext->DrawIndexed(
						m_numb[num2]->m_indicesCount,
						0,
						0
						);

				}
				*/
			}

		}
	}


	// design arrow
	if(bDesignPointer==true && current_fruit!=999)
	{
		if(design_pointer.type!=-1)
		{
			if (true)//(current_fruit != design_pointer.type)
			{
				current_fruit = design_pointer.type;

				deviceContext->IASetVertexBuffers(0, 1, m_model[current_fruit]->GetVertexBuffer().GetAddressOf(), &stride, &offset);
				deviceContext->IASetIndexBuffer(m_model[current_fruit]->GetIndexBuffer().Get(), DXGI_FORMAT_R16_UINT, 0);

				deviceContext->PSSetShaderResources(0, 1, &m_Texture[current_fruit]);
			}
		}
		auto rotationMatrix_land = XMMatrixRotationRollPitchYaw(design_pointer.yaw, design_pointer.pitch, design_pointer.roll);

		auto rotationMatrix = XMMatrixRotationRollPitchYaw(0.0f, design_pointer.angle, 0.0f);
		auto translationMatrix = XMMatrixTranslation(design_pointer.x, design_pointer.y, design_pointer.z);
		XMStoreFloat4x4(&constantBufferData->model, XMMatrixTranspose(rotationMatrix * rotationMatrix_land * translationMatrix));

		deviceContext->UpdateSubresource(constantBuffer.Get(), 0, NULL, constantBufferData, 0, 0);

		deviceContext->DrawIndexed(
			m_model[current_fruit]->m_indicesCount,
			0,
			0
			);
	}
}


