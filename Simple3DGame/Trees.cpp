#include "pch.h"
#include "Trees.h"
#include "PerlinNoise.h"


using namespace Simple3DGame;
using namespace DX;
using namespace DirectX;

int TreeSortCB(const VOID* arg1, const VOID* arg2)
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

int TreeSortCarCB(const VOID* arg1, const VOID* arg2)
{
	part_index* p1 = (part_index*)arg1;
	part_index* p2 = (part_index*)arg2;

	if (p1->dist == p2->dist)
	{
		return 0;
	}
	else
	{
		if (p1->dist>p2->dist)
		{
			return 1;
		}
		else
		{
			return -1;
		}
	}
}


Trees::~Trees(void)
{
	delete[] tree;
	delete[] p_index;
	delete[] p_index_car;
	delete[] cols;
}


TreeBody::TreeBody()
{

}

void TreeBody::SetPosition(float x, float y, float z, float yaw, float pitch, float roll)
{
	m_initialTransform = btTransform(btQuaternion(yaw, pitch, roll), btVector3(x, y /*+ (m_model->total_height*0.5f)*/, z));
	m_body->setWorldTransform(m_initialTransform);
	setWorldTransform(m_initialTransform);
}


void TreeBody::setWorldTransform(const btTransform &worldTransform)
{
	//btMotionState:setWorldTransform(worldTransform);
	m_initialTransform = worldTransform;
}

void TreeBody::getWorldTransform(btTransform &worldTransform) const
{


}

void TreeBody::SetTransform(const Transform^ transform)
{
	DirectX::XMVECTOR data;
	DirectX::XMVECTORF32 floatingVector = { transform->quatX, transform->quatY, transform->quatZ, transform->quatW };
	data = floatingVector;

}



void Trees::LoadTreeModel(int index, char* solid_obj_filename, char* alpha_obj_filename, wchar_t* solid_tex_fname, wchar_t* alpha_tex_fname, float scale,float _trunk_radius,bool bTrunk, bool bLeaves)
{
	if (bTrunk == true)
	{
		tree_model[index].m_solid_model = new ModelLoader(m_deviceResources,false,0);
		tree_model[index].m_solid_model->Load(solid_obj_filename, scale,1);
	}

	if(bLeaves==true)
	{
		tree_model[index].m_alpha_model = new ModelLoader(m_deviceResources,true,0);
		tree_model[index].m_alpha_model->Load(alpha_obj_filename, scale,1);
	}
	tree_model[index].bLeaves = bLeaves;
	tree_model[index].bTrunk = bTrunk;

	tree_model[index].trunk_radius = _trunk_radius;

	CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), solid_tex_fname, nullptr, &tree_model[index].m_solid_texture, MAXSIZE_T);

	if(bLeaves==true)
	{
		CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), alpha_tex_fname, nullptr, &tree_model[index].m_alpha_texture, MAXSIZE_T);
	}
}


Trees::Trees(std::shared_ptr<DX::DeviceResources> pm_deviceResources, Level* pp_Level, Player* pp_Player, Camera^ pp_Camera, Points* pp_Points, Physics* pp_physics)
{
	int i;

	m_deviceResources = pm_deviceResources;

	p_Points = pp_Points;

	p_Level = pp_Level;

	p_Player = pp_Player;

	m_phys = pp_physics;

	tree = new tree_t[400];

	treebody = new TreeBody[10];
	
	//barrel = new WorldObject[10];

	p_index = new part_index[400];
	p_index_car = new part_index[400];

	cols = new DirectX::XMFLOAT4*[400];

	//barrel[0].m_rigidbody = nullptr;
	bBarrelsLoaded = false;

	bTreesLoaded = false;

	bUpdate = true;

	current_particle_que = 0;

	m_particleSize = 2.0f;

	m_maxParticles = 400;

	current_index = 0;

	total_collected = 0;

	bRotateRay = false;

	p_Camera = pp_Camera;

	bDesignPointer = false;
	
	LoadTreeModel(0, "Assets/Models/trunk.obj", "Assets/Models/leaves.obj", L"Assets/Textures/trunk.dds", L"Assets/Textures/leaves.dds", 0.9f,0.5f,true,true);
	LoadTreeModel(1, "Assets/Models/trunk2.obj", "Assets/Models/leaves2.obj", L"Assets/Textures/tree_bark.dds", L"Assets/Textures/tree_001.dds", 2.0f, 0.5f, true,true);
	LoadTreeModel(2, "Assets/Models/trunk2.obj", "Assets/Models/leaves2.obj", L"Assets/Textures/tree_bark.dds", L"Assets/Textures/tree_001.dds", 2.0f, 0.5f, true,false);
	LoadTreeModel(3, "Assets/Models/palm_base.obj", "Assets/Models/palm_top.obj", L"Assets/Textures/palm_base.dds", L"Assets/Textures/palm_top.dds", 0.2f, 0.5f, true, true);
	LoadTreeModel(4, "Assets/Models/palm_base.obj", "Assets/Models/tropical1.obj", L"Assets/Textures/tropical1.dds", L"Assets/Textures/LeavesTropical.dds", 5.2f, 0.5f, false, true);
	LoadTreeModel(5, "Assets/Models/palm_base.obj", "Assets/Models/fern.obj", L"Assets/Textures/tropical1.dds", L"Assets/Textures/fern2.dds", 3.2f, 0.5f, false, true);
	LoadTreeModel(6, "Assets/Models/palm_base.obj", "Assets/Models/grass.obj", L"Assets/Textures/tropical1.dds", L"Assets/Textures/grass_tex.dds", 0.7f, 0.5f, false, true);
	LoadTreeModel(7, "Assets/Models/palm_base.obj", "Assets/Models/hemi.obj", L"Assets/Textures/tropical1.dds", L"Assets/Textures/leaf.dds", 1.7f, 0.5f, false, true);

	//tree_model[0].m_solid_model->FlipVertDir();
	tree_model[1].m_solid_model->FlipVertDir();

	tree_model[1].m_solid_model->FlipNorms();
	tree_model[1].m_alpha_model->FlipNorms();

	tree_model[2].m_solid_model->FlipVertDir();

	tree_model[2].m_solid_model->FlipNorms();

	tree_model[3].m_solid_model->FlipVertDir();
	tree_model[3].m_alpha_model->FlipVertDir();
	tree_model[3].m_solid_model->FlipNorms();

	tree_model[4].m_alpha_model->FlipNorms();
	tree_model[5].m_alpha_model->FlipNorms();
	tree_model[6].m_alpha_model->FlipNorms();
	tree_model[7].m_alpha_model->FlipNorms();

	tree_model[6].m_alpha_model->Optimize();
	tree_model[0].m_alpha_model->Optimize();
	tree_model[1].m_alpha_model->Optimize();

	m_Grass = new Modicle(m_deviceResources, tree_model[6].m_alpha_model, 100);
	m_Tree0 = new Modicle(m_deviceResources, tree_model[0].m_alpha_model, 10,1);
	m_Tree1 = new Modicle(m_deviceResources, tree_model[1].m_alpha_model, 10,1);


	//tree_model[4].m_alpha_model->SetColourWeights(&DirectX::XMFLOAT4(0.3f, 0.3f, 0.3f, 0.0f));
	//tree_model[3].m_alpha_model->FlipNorms();
	//tree_model[2].m_alpha_model->FlipNorms();

	total_trees_physical = 0;
}

void Trees::UpdateGrass()
{
	int i,j;
	btMatrix3x3 mat;
	btVector3 pos;
	btVector3* player_posA = p_Player->GetPosition();
	btVector3 cam_pos = btVector3(p_Camera->EyeX(), p_Camera->EyeY(), p_Camera->EyeZ());


	m_Grass->SetCamera(p_Camera->EyeX(), p_Camera->EyeY(), p_Camera->EyeZ());
	m_Tree0->SetCamera(p_Camera->EyeX(), p_Camera->EyeY(), p_Camera->EyeZ());
	m_Tree1->SetCamera(p_Camera->EyeX(), p_Camera->EyeY(), p_Camera->EyeZ());


	current_index = 0;
	for (i = 0; i<m_maxParticles; i++)
	{
		if (tree[i].bActive == true && tree[i].type == 6 && p_Camera->CheckPoint(tree[i].x, tree[i].y, tree[i].z, 3.0f))
		{
			p_index[current_index].dist = cam_pos.distance(btVector3(tree[i].x, tree[i].y, tree[i].z));
			p_index[current_index].part_no = i;

			current_index++;
		}
	}

	if (current_index>0)
	{
		qsort(p_index, current_index, sizeof(part_index), TreeSortCB);
	}

	m_Grass->Reset();
	for (i = 0; i < current_index; i++)
	{
		m_Grass->AddOne(&tree[p_index[i].part_no].model_matrix);
	}
	m_Grass->UpdateVertecies(m_deviceResources->GetD3DDeviceContext());




	current_index = 0;
	for (i = 0; i<m_maxParticles; i++)
	{
		if (tree[i].bActive == true && tree[i].type == 0 && p_Camera->CheckPoint(tree[i].x, tree[i].y, tree[i].z, 3.0f))
		{
			p_index[current_index].dist = cam_pos.distance(btVector3(tree[i].x, tree[i].y, tree[i].z));
			p_index[current_index].part_no = i;

			current_index++;
		}
	}

	if (current_index>0)
	{
		qsort(p_index, current_index, sizeof(part_index), TreeSortCB);
	}

	m_Tree0->Reset();
	for (i = 0; i < current_index; i++)
	{
		m_Tree0->AddOne(&tree[p_index[i].part_no].model_matrix, cols[p_index[i].part_no]);
	}
	m_Tree0->UpdateVertecies(m_deviceResources->GetD3DDeviceContext());




	current_index = 0;
	for (i = 0; i<m_maxParticles; i++)
	{
		if (tree[i].bActive == true && tree[i].type == 1 && p_Camera->CheckPoint(tree[i].x, tree[i].y, tree[i].z, 3.0f))
		{
			p_index[current_index].dist = cam_pos.distance(btVector3(tree[i].x, tree[i].y, tree[i].z));
			p_index[current_index].part_no = i;

			current_index++;
		}
	}

	if (current_index>0)
	{
		qsort(p_index, current_index, sizeof(part_index), TreeSortCB);
	}

	m_Tree1->Reset();
	for (i = 0; i < current_index; i++)
	{
		m_Tree1->AddOne(&tree[p_index[i].part_no].model_matrix, cols[p_index[i].part_no]);
	}
	m_Tree1->UpdateVertecies(m_deviceResources->GetD3DDeviceContext());


}

void Trees::RenderGrass(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> deviceContext, Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer, ModelViewProjectionConstantBuffer* constantBufferData, int part_to_render)
{

	deviceContext->PSSetShaderResources(0, 1, &tree_model[6].m_alpha_texture);

	m_Grass->Render(deviceContext, constantBuffer, constantBufferData);

	deviceContext->PSSetShaderResources(0, 1, &tree_model[0].m_alpha_texture);

	m_Tree0->Render(deviceContext, constantBuffer, constantBufferData);

	deviceContext->PSSetShaderResources(0, 1, &tree_model[1].m_alpha_texture);

	m_Tree1->Render(deviceContext, constantBuffer, constantBufferData);
	//m_Grass->Render()

}


void Trees::SaveBinary(int level)
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
	sprintf(info_filename, "%s\\LevelBinary\\TreeData", local_file_folder);
	_swprintf(w_info_filename, L"%hs", info_filename);
	CreateDirectory(w_info_filename, NULL);

	sprintf(info_filename, "%s\\LevelBinary\\TreeData\\%d.bmp", local_file_folder, level);


	for (int i = 0; i < m_maxParticles; i++)
	{
		if (tree[i].bActive == 1)
		{
			
			tree[i].y = p_Level->GetTerrainHeight(tree[i].x, tree[i].z);
			auto rotationMatrix = XMMatrixRotationRollPitchYaw(0.0f, tree[i].angle, 0.0f);
			auto translationMatrix = XMMatrixTranslation(tree[i].x, tree[i].y, tree[i].z);
			DirectX::XMStoreFloat4x4(&tree[i].model_matrix, XMMatrixTranspose(rotationMatrix * translationMatrix));

		}
	}

	FILE* pFile = fopen(info_filename, "wb");
	if (pFile != NULL)
	{
		fwrite(tree, sizeof(tree_t), m_maxParticles, pFile);

		fclose(pFile);
		//fwrite(&game_settings, sizeof(game_settings_t) * 1, 1, pFile);
		//fclose(pFile);
	}

}

bool Trees::LoadBinary(int level, int location)
{
	int i;
	char local_file_folder[140];
	current_level = level;
	Windows::Storage::StorageFolder^ folder = Windows::Storage::ApplicationData::Current->LocalFolder;
	wcstombs(local_file_folder, folder->Path->Data(), sizeof(local_file_folder));
	// update local crazy
	//sprintf(local_file_folder, "C:\\Users\\craig\\Desktop\\Simple3DGame3\\Simple3DGame\\Assets");

	// Load level scores
	char info_filename[140];
	if (location == 0)
	{
		sprintf(info_filename, "%s\\LevelBinary\\TreeData\\%d.bmp", local_file_folder, level);
	}
	else
	{
		sprintf(info_filename, "Assets\\LevelBinary\\TreeData\\%d.bmp", level);
	}

	if (bBarrelsLoaded == false)
	{
		bBarrelsLoaded = true;
		for (i = 0; i < 10; i++)
		{
			treebody[i].length = 10.0f;
			treebody[i].radius = 2.0f;

			//btTransform m_initialTransform = btTransform(btQuaternion(0.0f, 0.0f, 0.0f), btVector3(i*3.0f, 50.0f, i*3.0f));

			//treebody[i].setWorldTransform(m_initialTransform);
			//MakeTreeBody(&treebody[i]);


			ObjInfo info = { "Assets/Models/tall_cylinder.obj"
				, L"Assets/barrelColor.dds"
				//		, L"Assets/foot_ball.dds"
				//, L"Assets/sphere.dds"
				, -0.2f		// Scale
				, 10.0f*i		// X,y,z
				, 50.0f
				, 0.0f
				, 0.0f		// mass
				, 0.0f		// yaw,pitch,roll
				, 0.0f
				, 0.0f
				, 0.5f		// restitution
				, 0.5f
				,0,false
				,(COL_CAR | COL_BOXES | COL_CARBODY)
				,(COL_TREES) };
			//info.mask = (COL_BOXES);
			//info.group = (COL_WORLDSTUFF | COL_CAR | COL_TREES | COL_WALLS);
			barrel[i] = new WorldObject(m_deviceResources,m_phys);
			barrel[i]->MakePhysicsCylinderFromMesh( &info);
			barrel[i]->m_rigidbody->setActivationState(1);
			total_trees_physical++;
			//barrel->
		}
	}

	if (bTreesLoaded == true)
	{
		for (int i = 0; i < m_maxParticles; i++)
		{
			if(tree[i].bActive == 1 && tree[i].bUpdateCols==true && tree[i].type==1)
			{
				delete[] cols[i];// = new norm_t[total_x_points];
			}
		}
	}
	for (int i = 0; i < m_maxParticles; i++)
	{
		tree[i].bActive = 0;
	}
	// LoadLevelData
	FILE * pFile;
	current_pill = 0;
	pFile = fopen(info_filename, "r");
	if (pFile != NULL)
	{
		fread(tree, sizeof(tree_t), m_maxParticles, pFile);

		fclose(pFile);
	}
	else
	{
		return false;
	}


	bTreesLoaded = true;

	current_pill = 0;
	for (int i = 0; i < m_maxParticles; i++)
	{
		if (tree[i].bActive == 1)
		{
			/*
			tree[i].y = p_Level->GetTerrainHeight(tree[i].x, tree[i].z);
			auto rotationMatrix = XMMatrixRotationRollPitchYaw(0.0f, tree[i].angle, 0.0f);
			auto translationMatrix = XMMatrixTranslation(tree[i].x, tree[i].y, tree[i].z);
			DirectX::XMStoreFloat4x4(&tree[i].model_matrix, XMMatrixTranspose(rotationMatrix * translationMatrix));
			*/

			

			if (tree[i].type == 1 && tree[i].bUpdateCols == true)
			{
				GenerateTreeShades(i);
			}

			if (tree[i].type == 0 )
			{
				tree[i].bUpdateCols = true;
				GenerateTreeShades(i,1); // evergreen
			}

			//if (tree[i].type > 1)
			//	tree[i].type = 1;


			if (current_pill < i + 1)
			{
				current_pill = i + 1;
			}
		}
	}
	total_pills = current_pill;

	TreeLights();
	return true;
}




bool Trees::LoadLevel(int level, int location)
{
	current_level = level;


	if(LoadBinary(level,location) == false)
	{
		return true;// LoadLocations(level);
	}
	else
	{
		return true;
	}

	update_frame = 0;
}

void Trees::SetDesignPointer(float x, float y, float z, int type)
{
	design_pointer.bActive = 1;
	design_pointer.x = x;
	design_pointer.y = y;
	design_pointer.z = z;
	design_pointer.type = type;
	bDesignPointer = true;
}

void Trees::SetDesignPointer(float x, float y, float z, float _y, float _p, float _r, int type)
{
	design_pointer.bActive = 1;
	design_pointer.x = x;
	design_pointer.y = y;
	design_pointer.z = z;
	design_pointer.type = type;
	bDesignPointer = true;

	design_pointer.yaw = _y;
	design_pointer.pitch = _p;
	design_pointer.roll = _r;


	//bIgnoreHeight = true;
}

void Trees::TreeLights()
{
	int i;
	p_dots->Reset();

	for (i = 0; i < m_maxParticles; i++)
	{
		if (tree[i].bActive == 1 && tree[i].type==0)
		{
			btMatrix3x3 rot_mat = btMatrix3x3(tree[i].model_matrix._11, tree[i].model_matrix._12, tree[i].model_matrix._13, tree[i].model_matrix._21, tree[i].model_matrix._22, tree[i].model_matrix._23, tree[i].model_matrix._31, tree[i].model_matrix._32, tree[i].model_matrix._33);
			if (tree_model[tree[i].type].bLeaves == true)
			{
				for (int l = 0; l < 40; l++)
				{
					int rnd_vert = rand() % tree_model[tree[i].type].m_alpha_model->m_verticesCount;
					btVector3 vert_vec = btVector3(tree_model[tree[i].type].m_alpha_model->m_vertices.at(rnd_vert).position.x,
						tree_model[tree[i].type].m_alpha_model->m_vertices.at(rnd_vert).position.y,
						tree_model[tree[i].type].m_alpha_model->m_vertices.at(rnd_vert).position.z);

					vert_vec = vert_vec * rot_mat;

					vert_vec = vert_vec + btVector3(tree[i].x, tree[i].y, tree[i].z);

					p_dots->CreateOne(vert_vec.getX(), vert_vec.getY(), vert_vec.getZ(), DirectX::XMFLOAT4((rand() % 100)*0.01f, (rand() % 100)*0.01f, (rand() % 100)*0.01f, 0.0f), 0);
				}
			}
		}
	}
}


void Trees::GenerateTreeShades(int tree_num,int type)
{
	int i;
	float noise_scale = 1.5f;
	unsigned int seed = rand()%400;
	
	PerlinNoise pn(seed);

	

	if (tree_model[tree[tree_num].type].bLeaves == true)
	{
		int vert_count = tree_model[tree[tree_num].type].m_alpha_model->m_verticesCount;
		cols[tree_num] = new DirectX::XMFLOAT4[vert_count];

		float tree_brightness = (pn.noise(seed, 10.0f, 0.8f) - 0.5f)*2.0f;
		float r_weight = (pn.noise(seed, 10.0f, 0.8f) - 0.5f)*3.0f;
		float g_weight = (pn.noise(seed, 10.0f, 0.8f) - 0.5f)*3.0f;
		float b_weight = (pn.noise(seed, 10.0f, 0.8f) - 0.5f)*3.0f;

		for (i = 0; i < vert_count; i++)
		{
			

			float r = (pn.noise((float)i*noise_scale, 10.0f, 0.8f) - 0.5f)+ tree_brightness+ r_weight;
			float g = (pn.noise((float)i*noise_scale, 20.0f, 0.8f) - 0.5f) + tree_brightness+ g_weight;
			float b = (pn.noise((float)i*noise_scale, 30.0f, 0.8f) - 0.5f) + tree_brightness+ b_weight;

			if (type == 1) // evergreen
			{

			}
			else
			{
				if (season == 0) // summer
				{
					g = g + 0.2f;
				}
				if (season == 1) // autumn
				{
					g = g - 0.6f;
					b = b - 0.2f;
				}
				if (season == 2) // winter
				{
					g = g - 0.5f;
					r = r - 0.2f;
					b = b - 0.2f;
				}
				if (season == 3) // spring
				{
					g = g + 0.0f;
				}
			}


			cols[tree_num][i] = DirectX::XMFLOAT4(r*0.5f, g*0.5f, b*0.5f, 0.0f);
		}
		tree[tree_num].bUpdateCols = true;
	}

	

}

void Trees::CreateOne(float x, float y, float z, int type)
{
	int i,l;
	bool bFound = false;
	int point_find = 0;
	DirectX::XMFLOAT4 col = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	// Check exists
	for (i = 0; i < m_maxParticles; i++)
	{
		if (tree[i].bActive == 1 && tree[i].x == x && tree[i].z == z)
		{
			bFound = true;
			if (type == -1)
				tree[i].bActive = 0;
			else
			{
				tree[i].type = type;
			}
		}
	}

	if (bFound == true || type == -1)
		return;

	for (i = 0; i < m_maxParticles; i++)
	{
		if (tree[i].bActive == 0)
		{
			tree[i].x = x;
			tree[i].z = z;
			tree[i].y = 0.0f + p_Level->GetTerrainHeight((int)x, (int)z);

			if (i<m_maxParticles - 1 && tree[i].y > -990.0f)
			{
				//total_pills++;
				tree[i].bActive = 1;

				tree[i].type = type;

				tree[i].angle = (rand() % 100)*0.01f*(2.0f*M_PI);

				tree[i].bUpdateCols = false;

				GenerateTreeShades(i);



				/*
				btTransform m_initialTransform = btTransform(btQuaternion(0.0f, 0.0f, 0.0f), btVector3(tree[i]->x, tree[i]->y + (tree_model[tree[i]->type].m_solid_model->total_height) + 1.0f, tree[i]->z));

				tree[i]->setWorldTransform(m_initialTransform);

				MakePhysicsBody(tree[i]);
				tree[i]->m_body->setWorldTransform(m_initialTransform);
				*/
				//tree[i]->m_body = nullptr;
				float rand_scalea = 0.8f+(rand() % 100)*0.006f;
				float rand_scaleb = 0.8f + (rand() % 100)*0.006f;
				XMMATRIX scaleMatrix = XMMatrixScaling(rand_scaleb, rand_scalea, rand_scaleb);

				auto rotationMatrix = XMMatrixRotationRollPitchYaw(0.0f, tree[i].angle, 0.0f);
				XMMATRIX scaleRotationMatrix = XMMatrixMultiply(scaleMatrix, rotationMatrix);
				auto translationMatrix = XMMatrixTranslation(tree[i].x, tree[i].y, tree[i].z);
				DirectX::XMStoreFloat4x4(&tree[i].model_matrix, XMMatrixTranspose(scaleRotationMatrix * translationMatrix));

				// put dots in random points on trees
				btMatrix3x3 rot_mat = btMatrix3x3(tree[i].model_matrix._11, tree[i].model_matrix._12, tree[i].model_matrix._13, tree[i].model_matrix._21, tree[i].model_matrix._22, tree[i].model_matrix._23, tree[i].model_matrix._31, tree[i].model_matrix._32, tree[i].model_matrix._33);

				// put dots in random points on trees
				if (false)//(tree_model[tree[i].type].bLeaves == true)
				{
					for (l = 0; l < 40; l++)
					{
						int rnd_vert = rand() % tree_model[tree[i].type].m_alpha_model->m_verticesCount;
						btVector3 vert_vec = btVector3(tree_model[tree[i].type].m_alpha_model->m_vertices.at(rnd_vert).position.x,
							tree_model[tree[i].type].m_alpha_model->m_vertices.at(rnd_vert).position.y,
							tree_model[tree[i].type].m_alpha_model->m_vertices.at(rnd_vert).position.z);

						vert_vec = vert_vec * rot_mat;

						vert_vec = vert_vec + btVector3(tree[i].x, tree[i].y, tree[i].z);

						p_dots->CreateOne(vert_vec.getX(), vert_vec.getY(), vert_vec.getZ(), DirectX::XMFLOAT4((rand() % 100)*0.01f, (rand() % 100)*0.01f, (rand() % 100)*0.01f, 0.0f), 0);
					}
				}
				
				//XMFLOAT3 vert = XMFLOAT3(1.0f, 2.0f, 0.5f);
				//XMFLOAT3 vert_b = vert * tree[current_pill]->model_matrix;

				current_pill++;
			}

			if (total_pills < i + 1)
				total_pills = i + 1;
			break;
		}
	}

	//TreeLights();
}





void Trees::Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> deviceContext, Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer, ModelViewProjectionConstantBuffer* constantBufferData, int part_to_render)
{
	UINT stride = sizeof(VertexPositionTex);
	UINT offset = 0;
	int i;

	//return;

	int current_tree = 999;
	bool bSet = false;
	int tree_type;
	int current_car_index = 0;

	if (update_frame < 5)
	{
		update_frame++;
		if (update_frame == 5)
		{
			for (i = 0; i < total_pills; i++)
			{
				//if (tree[i]->m_body != nullptr)
				//{
				//	tree[i]->m_body->setMassProps(0.0f, btVector3(0.0f, 0.0f, 0.0f));
				//	tree[i]->m_body->setRestitution(0.5f);
				//}
			}
		}
	}
	
	btVector3* player_posA = p_Player->GetPosition();
	btVector3 cam_pos = btVector3(p_Camera->EyeX(), p_Camera->EyeY(), p_Camera->EyeZ());
	current_index = 0;
	for (i = 0; i<m_maxParticles; i++)
	{
		if (tree[i].bActive == true && tree[i].type != 6)
		{
			if (!(part_to_render == 1 && (tree[i].type == 0 || tree[i].type == 1)))
			{
				if(p_Camera->CheckPoint(tree[i].x, tree[i].y, tree[i].z, 3.0f) )
				{
					if (tree_model[tree[i].type].bTrunk == true)
					{
						p_index_car[current_car_index].dist = player_posA->distance(btVector3(tree[i].x, tree[i].y, tree[i].z));
						p_index_car[current_car_index].part_no = i;
						current_car_index++;
					}

					p_index[current_index].dist = cam_pos.distance(btVector3(tree[i].x, tree[i].y, tree[i].z));
					p_index[current_index].part_no = i;

					current_index++;
				}
			}
		}
	}

	if (current_index>0)
	{
		qsort(p_index, current_index, sizeof(part_index), TreeSortCB);
		qsort(p_index_car, current_car_index, sizeof(part_index), TreeSortCarCB);
	}

	if (part_to_render == 0)
	{
		for (int i = 0; i < 10; i++)
		{
			if (i < current_car_index)
			{
				//tree[p_index_car[i].part_no]->bActive = false;
				//tree[p_index_car[i].part_no].y+=0.1f;
				//auto rotationMatrix = XMMatrixRotationRollPitchYaw(0.0f, tree[p_index_car[i].part_no].angle, 0.0f);
				//auto translationMatrix = XMMatrixTranslation(tree[p_index_car[i].part_no].x, tree[p_index_car[i].part_no].y, tree[p_index_car[i].part_no].z);
				//DirectX::XMStoreFloat4x4(&tree[p_index_car[i].part_no].model_matrix, XMMatrixTranspose(rotationMatrix * translationMatrix));
				//btTransform m_initialTransform = btTransform(btQuaternion(0.0f, 0.0f, 0.0f), btVector3(tree[p_index_car[i].part_no].x, tree[p_index_car[i].part_no].y + 5.0f, tree[p_index_car[i].part_no].z));
				//treebody[i].m_body->setWorldTransform(m_initialTransform);
				//barrel[i].SetPosition(tree[p_index_car[i].part_no].x, tree[p_index_car[i].part_no].y + 1.0f, tree[p_index_car[i].part_no].z, 0.0f, 0.0f, 0.0f);

				//if(barrel[i]->)
				barrel[i]->SetPosition(tree[p_index_car[i].part_no].x, tree[p_index_car[i].part_no].y + 1.8f, tree[p_index_car[i].part_no].z, 0.0f, 0.0f, 0.0f);
				//barrel[i].Render(deviceContext, constantBuffer, constantBufferData);
			}
			else
			{
				barrel[i]->SetPosition(i * 10.0f, 50.0f, i * 10.0f,0.0f,0.0f,0.0f);
			}
		}
	}

	for (int i = 0; i < current_index; i++)
	{
		if( shader_mode==1 )
		{
			if (tree[p_index[i].part_no].type==1 && ( tree[p_index[i].part_no].bUpdateCols == true && tree[p_index[i].part_no].bUpdateCols==true))
			{
				//tree_model[tree[p_index[i].part_no].type].m_alpha_model->SetColourWeights(cols[p_index[i].part_no]);
			}
		}
		if (current_tree != tree[p_index[i].part_no].type)
		{
			current_tree = tree[p_index[i].part_no].type;

			if (part_to_render == 0)
			{
				if (tree_model[current_tree].bTrunk == true)
				{
					deviceContext->IASetVertexBuffers(0, 1, tree_model[current_tree].m_solid_model->GetVertexBuffer().GetAddressOf(), &stride, &offset);
					deviceContext->IASetIndexBuffer(tree_model[current_tree].m_solid_model->GetIndexBuffer().Get(), DXGI_FORMAT_R16_UINT, 0);
					deviceContext->PSSetShaderResources(0, 1, &tree_model[current_tree].m_solid_texture);
				}
			}
			else
			{
				if(tree_model[current_tree].bLeaves==true)
				{

					deviceContext->IASetVertexBuffers(0, 1, tree_model[current_tree].m_alpha_model->GetVertexBuffer().GetAddressOf(), &stride, &offset);
					deviceContext->IASetIndexBuffer(tree_model[current_tree].m_alpha_model->GetIndexBuffer().Get(), DXGI_FORMAT_R16_UINT, 0);
					deviceContext->PSSetShaderResources(0, 1, &tree_model[current_tree].m_alpha_texture);
				}
			}
		}

		if (true)
		{
			MVPConstantBuffer m_constantBuffer = m_constantBufferB;

			constantBufferData->model = tree[p_index[i].part_no].model_matrix;

			deviceContext->UpdateSubresource(constantBuffer.Get(), 0, NULL, constantBufferData, 0, 0);

			if (part_to_render == 0)
			{
				if (tree_model[current_tree].bTrunk == true)
				{
					deviceContext->DrawIndexed(
						tree_model[current_tree].m_solid_model->m_indicesCount,
						0,
						0
						);
				}
			}
			else
			{
				if (tree_model[current_tree].bLeaves == true)
				{
					deviceContext->DrawIndexed(
						tree_model[current_tree].m_alpha_model->m_indicesCount,
						0,
						0
						);
				}
			}
		}
	}



	// design pointer
	if(bDesignPointer==true)
	{
		if (current_tree != design_pointer.type)
		{
			current_tree = design_pointer.type;

			if (part_to_render == 0)
			{
				if (tree_model[current_tree].bTrunk == true)
				{
					deviceContext->IASetVertexBuffers(0, 1, tree_model[current_tree].m_solid_model->GetVertexBuffer().GetAddressOf(), &stride, &offset);
					deviceContext->IASetIndexBuffer(tree_model[current_tree].m_solid_model->GetIndexBuffer().Get(), DXGI_FORMAT_R16_UINT, 0);
					deviceContext->PSSetShaderResources(0, 1, &tree_model[current_tree].m_solid_texture);
				}
			}
			else
			{
				if (tree_model[current_tree].bLeaves == true)
				{
					deviceContext->IASetVertexBuffers(0, 1, tree_model[current_tree].m_alpha_model->GetVertexBuffer().GetAddressOf(), &stride, &offset);
					deviceContext->IASetIndexBuffer(tree_model[current_tree].m_alpha_model->GetIndexBuffer().Get(), DXGI_FORMAT_R16_UINT, 0);
					deviceContext->PSSetShaderResources(0, 1, &tree_model[current_tree].m_alpha_texture);
				}
			}
		}

		if (true)
		{
			MVPConstantBuffer m_constantBuffer = m_constantBufferB;

			auto rotationMatrix = XMMatrixRotationRollPitchYaw(0.0f, design_pointer.angle, 0.0f);
			auto translationMatrix = XMMatrixTranslation(design_pointer.x, design_pointer.y, design_pointer.z);

			if (bRotateRay == true)
			{
				auto rotationMatrix = XMMatrixRotationRollPitchYaw(design_pointer.yaw, design_pointer.pitch, design_pointer.roll);
				auto translationMatrix = XMMatrixTranslation(design_pointer.x, design_pointer.y, design_pointer.z);

			}

			XMStoreFloat4x4(&design_pointer.model_matrix, XMMatrixTranspose(rotationMatrix * translationMatrix));

			constantBufferData->model = design_pointer.model_matrix;

			deviceContext->UpdateSubresource(constantBuffer.Get(), 0, NULL, constantBufferData, 0, 0);

			if (part_to_render == 0)
			{
				if (tree_model[current_tree].bTrunk == true)
				{
					deviceContext->DrawIndexed(
						tree_model[current_tree].m_solid_model->m_indicesCount,
						0,
						0
						);
				}
			}
			else
			{
				if (tree_model[current_tree].bLeaves == true)
				{
					deviceContext->DrawIndexed(
						tree_model[current_tree].m_alpha_model->m_indicesCount,
						0,
						0
						);
				}
			}
		}
	}

}


