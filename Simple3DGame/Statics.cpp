#include "pch.h"
#include "Statics.h"

using namespace Simple3DGame;
using namespace DX;
using namespace DirectX;
using namespace concurrency;


int StaticsSortCB(const VOID* arg1, const VOID* arg2)
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


int StaticsSortAlphCB(const VOID* arg1, const VOID* arg2)
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
			return -1;
		}
		else
		{
			return 1;
		}
	}
}


int StaticsSortCarCB(const VOID* arg1, const VOID* arg2)
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


Statics::~Statics(void)
{
	delete[] statics;
	delete[] p_index;
	delete[] p_index_car;

}








Statics::Statics(std::shared_ptr<DX::DeviceResources> pm_deviceResources, Level* pp_Level, Player* pp_Player, Camera^ pp_Camera, Points* pp_Points, Physics* pp_physics, Textures* pp_Textures)
{
	int i;


	m_deviceResources = pm_deviceResources;

	p_Points = pp_Points;

	p_Level = pp_Level;

	p_Player = pp_Player;

	m_phys = pp_physics;

	p_Textures = pp_Textures;

	statics = new static_t[500];

	bDesignPointer = false;

	//barrel = new WorldObject[10];

	p_index = new part_index[500];
	p_index_car = new part_index[500];

	//barrel[0].m_rigidbody = nullptr;
	bBarrelsLoaded = false;

	bUpdate = true;

	current_particle_que = 0;

	m_particleSize = 2.0f;

	m_maxParticles = 500;

	current_index = 0;

	total_collected = 0;

	p_Camera = pp_Camera;

	//moveable_models = 5; // lower for better performance

	moveable_models[0] = 3;
	moveable_models[1] = 4;
	moveable_models[3] = 5;
	moveable_models[4] = 5;
	moveable_models[5] = 5;
	moveable_models[6] = 5;
	moveable_models[7] = 6; // LOOP AND JUMP
	moveable_models[8] = 6;
	moveable_models[9] = 4;
/*	moveable_models[0] = 3;
	moveable_models[0] = 3;
	moveable_models[0] = 3;
	moveable_models[0] = 3;
	*/
	design_height = 0.0f;
	design_height_f = 0.0f;

	bIgnoreHeight = false;



	//static_model[9].m_solid_model->UpsideDown();

	//static_model[1].m_solid_model->FlipNorms();
	//static_model[1].m_alpha_model->FlipNorms();
	total_static_physical = 0;

	for (i = 0; i < m_maxParticles; i++)
	{
		statics[i].bActive = false;
	}
}

task<void> Statics::LoadModels()
{
	///	LoadModel(0, "Assets/skull.obj", L"Assets/skull.dds", 2, -0.8f);

	std::vector<task<void>> tasks;

	LoadTreeModel(0, true, false, "Assets/Models/board1.obj", "Assets/Models/board1.obj", "woodfloor.dds", "woodfloor.dds", -1.0f, 0.5f);
	LoadTreeModel(1, true, false, "Assets/Models/board2.obj", "Assets/Models/board2.obj", "woodfloor.dds", "woodfloor.dds", 1.0f, 0.5f);
	LoadTreeModel(2, false, true, "Assets/Models/board2.obj", "Assets/Models/board2.obj", "banana.dds", "old_grate.dds", 1.0f, 0.5f);
	LoadTreeModel(3, true, false, "Assets/Models/rock2.obj", "Assets/Models/cube_round.obj", "stone1.dds", "crate.dds", -0.4f, 0.5f);
	LoadTreeModel(4, true, false, "Assets/Models/rock2.obj", "Assets/Models/cube_round.obj", "stone1.dds", "crate.dds", -1.4f, 0.5f);
	LoadTreeModel(5, true, false, "Assets/Models/rock2.obj", "Assets/Models/cube_round.obj", "stone1.dds", "crate.dds", -2.4f, 0.5f);
	LoadTreeModel(6, true, false, "Assets/Models/arrow.obj", "Assets/Models/arrow.obj", "woodfin.dds", "woodfin.dds", -0.6f, 0.5f);
	LoadTreeModel(7, true, false, "Assets/Models/thin_board.obj", "Assets/Models/thin_board.obj", "woodfloor.dds", "woodfloor.dds", 1.0f, 0.5f);
	LoadTreeModel(8, true, false, "Assets/Models/thin_board2.obj", "Assets/Models/thin_board2.obj", "woodfloor.dds", "woodfloor.dds", -1.0f, 0.5f);
	LoadTreeModel(9, true, false, "Assets/Models/stump.obj", "Assets/Models/stump.obj", "stump.dds", "stump.dds", -1.5f, 0.5f);

	//LoadTreeModel(9, true, false, "Assets/Models/crystal2.obj", "Assets/Models/crystal2.obj", "crystal2.dds", "crystal2.dds", -1.0f, 0.5f);



	static_model[5].m_solid_model->SetModelSpecular(0.2f);
	//static_model[0].m_solid_model->UpsideDown();
	static_model[0].m_solid_model->FlipVertDir();


	return when_all(tasks.begin(), tasks.end());
}


void Statics::LoadTreeModel(int index, bool has_solid, bool has_alpha, char* solid_obj_filename, char* alpha_obj_filename, char* solid_tex_fname, char* alpha_tex_fname, float scale, float _trunk_radius)
{
	static_model[index].bSolid = has_solid;
	static_model[index].bAlpha = has_alpha;

	if (true)//(static_model[index].bSolid == true)
	{
		if(index==3 || index == 4 || index == 5)
		{
			static_model[index].m_solid_model = new ModelLoader(m_deviceResources,true); // updateable
		}
		else
		{
			static_model[index].m_solid_model = new ModelLoader(m_deviceResources);
		}

		static_model[index].m_solid_model->Load(solid_obj_filename, scale);

		static_model[index].m_solid_texture = p_Textures->LoadTexture(solid_tex_fname);
		//CreateDDSTextureFromFile((ID3D11Device *)m_d3dDevice.Get(), solid_tex_fname, nullptr, &static_model[index].m_solid_texture, MAXSIZE_T);
	}

	if (true)//(static_model[index].bAlpha == true)
	{
		static_model[index].m_alpha_model = new ModelLoader(m_deviceResources);
		static_model[index].m_alpha_model->Load(alpha_obj_filename, scale);
		static_model[index].m_alpha_texture = p_Textures->LoadTexture(alpha_tex_fname);
		//CreateDDSTextureFromFile((ID3D11Device *)m_d3dDevice.Get(), alpha_tex_fname, nullptr, &static_model[index].m_alpha_texture, MAXSIZE_T);
	}

	static_model[index].trunk_radius = _trunk_radius;


}

void Statics::LoadPhysical()
{
	int i;
	if (bBarrelsLoaded == false)
	{
		bBarrelsLoaded = true;

		for (i = 0; i < moveable_models[0]; i++)
		{

			ObjInfo info = { "Assets/Models/board1.obj"
				, L"Assets/none.dds"
				//		, L"Assets/foot_ball.dds"
				//, L"Assets/sphere.dds"
				, -1.0f		// Scale
				, 10.0f	*i	// X,y,z
				, 30.0f
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
			static_ob[0][i] = new WorldObject(m_deviceResources,m_phys);
			static_ob[0][i]->MakePhysicsBoxFromMesh(&info);
			static_ob[0][i]->m_rigidbody->setActivationState(1);

			total_static_physical++;
			//barrel->
		}

		for (i = 0; i < moveable_models[1]; i++)
		{

			ObjInfo info = { "Assets/Models/board2.obj"
				, L"Assets/none.dds"
				//		, L"Assets/foot_ball.dds"
				//, L"Assets/sphere.dds"
				, 1.0f		// Scale
				, 10.0f	*i	// X,y,z
				, 40.0f
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
			static_ob[1][i] = new WorldObject(m_deviceResources, m_phys);
			static_ob[1][i]->MakePhysicsBoxFromMesh(&info);
			static_ob[1][i]->m_rigidbody->setActivationState(1);
			total_static_physical++;
			//barrel->
		}

		for (i = 0; i < moveable_models[3]; i++)
		{

			ObjInfo info = { "Assets/Models/rock2.obj"
				, L"Assets/none.dds"
				//		, L"Assets/foot_ball.dds"
				//, L"Assets/sphere.dds"
				, -0.4f		// Scale
				, 10.0f	*i	// X,y,z
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
			static_ob[3][i] = new WorldObject(m_deviceResources, m_phys);
			static_ob[3][i]->MakePhysicsSphereFromMesh(&info);
			static_ob[3][i]->m_rigidbody->setActivationState(1);
			total_static_physical++;
			//barrel->
		}


		for (i = 0; i < moveable_models[4]; i++)
		{

			ObjInfo info = { "Assets/Models/rock2.obj"
				, L"Assets/none.dds"
				//		, L"Assets/foot_ball.dds"
				//, L"Assets/sphere.dds"
				, -1.2f		// Scale
				, 10.0f	*i	// X,y,z
				, 60.0f
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
			static_ob[4][i] = new WorldObject(m_deviceResources, m_phys);
			static_ob[4][i]->MakePhysicsSphereFromMesh(&info);
			static_ob[4][i]->m_rigidbody->setActivationState(1);
			total_static_physical++;
			//barrel->
		}

		for (i = 0; i < moveable_models[5]; i++)
		{

			ObjInfo info = { "Assets/Models/rock2.obj"
				, L"Assets/none.dds"
				//		, L"Assets/foot_ball.dds"
				//, L"Assets/sphere.dds"
				, -2.1f		// Scale
				, 10.0f	*i	// X,y,z
				, 70.0f 
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
			static_ob[5][i] = new WorldObject(m_deviceResources, m_phys);
			static_ob[5][i]->MakePhysicsSphereFromMesh( &info);
			static_ob[5][i]->m_rigidbody->setActivationState(1);
			total_static_physical++;
			//barrel->
		}

		for (i = 0; i < moveable_models[6]; i++)
		{

			ObjInfo info = { "Assets/Models/arrow.obj"
				, L"Assets/none.dds"
				//		, L"Assets/foot_ball.dds"
				//, L"Assets/sphere.dds"
				, -0.6f		// Scale
				, 10.0f	*i	// X,y,z
				, 80.0f 
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
			static_ob[6][i] = new WorldObject(m_deviceResources, m_phys);
			static_ob[6][i]->MakePhysicsBoxFromMesh(&info);
			static_ob[6][i]->m_rigidbody->setActivationState(1);
			total_static_physical++;
			//barrel->
		}

		for (i = 0; i < moveable_models[7]; i++)
		{

			ObjInfo info = { "Assets/Models/thin_board.obj"
				, L"Assets/none.dds"
				//		, L"Assets/foot_ball.dds"
				//, L"Assets/sphere.dds"
				, 1.0f		// Scale
				, 10.0f	*i	// X,y,z
				, 90.0f 
				, 0.0f
				, 0.0f		// mass
				, 0.0f		// yaw,pitch,roll
				, 0.0f
				, 0.0f
				, 0.0f		// restitution
				, 1.5f
				,0,false
				,(COL_CAR | COL_BOXES | COL_CARBODY)
				,(COL_TREES) };
			//info.mask = (COL_BOXES);
			//info.group = (COL_WORLDSTUFF | COL_CAR | COL_TREES | COL_WALLS);
			static_ob[7][i] = new WorldObject(m_deviceResources, m_phys);
			static_ob[7][i]->MakePhysicsBoxFromMeshLoopSpecial(&info);
			static_ob[7][i]->m_rigidbody->setActivationState(1);
			total_static_physical++;
			//barrel->
		}



		for (i = 0; i < moveable_models[8]; i++)
		{

			ObjInfo info = { "Assets/Models/thin_board2.obj"
				, L"Assets/none.dds"
				//		, L"Assets/foot_ball.dds"
				//, L"Assets/sphere.dds"
				, -1.0f		// Scale
				, 10.0f	*i	// X,y,z
				, 100.0f
				, 0.0f
				, 0.0f		// mass
				, 0.0f		// yaw,pitch,roll
				, 0.0f
				, 0.0f
				, 0.0f		// restitution
				, 1.5f
				,0,false
				,(COL_CAR | COL_BOXES | COL_CARBODY)
				,(COL_TREES) };
			//info.mask = (COL_BOXES);
			//info.group = (COL_WORLDSTUFF | COL_CAR | COL_TREES | COL_WALLS);
			static_ob[8][i] = new WorldObject(m_deviceResources, m_phys);
			static_ob[8][i]->MakePhysicsBoxFromMesh( &info);
			static_ob[8][i]->m_rigidbody->setActivationState(1);
			total_static_physical++;
			//barrel->
		}

		for (i = 0; i < moveable_models[9]; i++)
		{

			ObjInfo info = { "Assets/Models/trunk.obj"
				, L"Assets/none.dds"
				//		, L"Assets/foot_ball.dds"
				//, L"Assets/sphere.dds"
				, -1.0f		// Scale
				, 10.0f	*i	// X,y,z
				, 100.0f
				, 0.0f
				, 0.0f		// mass
				, 0.0f		// yaw,pitch,roll
				, 0.0f
				, 0.0f
				, 0.0f		// restitution
				, 1.5f
				,0,false
				,(COL_CAR | COL_BOXES | COL_CARBODY)
				,(COL_TREES) };
			//info.mask = (COL_BOXES);
			//info.group = (COL_WORLDSTUFF | COL_CAR | COL_TREES | COL_WALLS);
			static_ob[9][i] = new WorldObject(m_deviceResources, m_phys);
			static_ob[9][i]->MakePhysicsCylinderFromMesh(&info,0.8f);
			static_ob[9][i]->m_rigidbody->setActivationState(1);
			total_static_physical++;
			//barrel->
		}

	}
}

// scatter at level start
void Statics::ScatterPhysical()
{
	int i, j;
	for (i = 0; i < 9; i++)
	{
		for (j = 0; j < moveable_models[i]; j++)
		{
			if (i == 2)
				continue;
			static_ob[i][j]->SetPosition(10.0f*i, 30.0f + (j*10.0f), 0.0f, 0.0f, 0.0f, 0.0f);
		}
	}


}

void Statics::SaveBinary(int level)
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
	sprintf(info_filename, "%s\\LevelBinary\\StaticData", local_file_folder);
	_swprintf(w_info_filename, L"%hs", info_filename);
	CreateDirectory(w_info_filename, NULL);

	sprintf(info_filename, "%s\\LevelBinary\\StaticData\\%d.bmp", local_file_folder, level);

	FILE* pFile = fopen(info_filename, "wb");
	if (pFile != NULL)
	{
		fwrite(statics, sizeof(static_t), m_maxParticles, pFile);

		fclose(pFile);
		//fwrite(&game_settings, sizeof(game_settings_t) * 1, 1, pFile);
		//fclose(pFile);
	}
}

bool Statics::LoadBinary(int level, int location)
{
	int i,j;
	char local_file_folder[140];

	//return true;
	current_level = level;
	Windows::Storage::StorageFolder^ folder = Windows::Storage::ApplicationData::Current->LocalFolder;
	wcstombs(local_file_folder, folder->Path->Data(), sizeof(local_file_folder));
	// update local crazy
	//sprintf(local_file_folder, "C:\\Users\\craig\\Desktop\\Simple3DGame3\\Simple3DGame\\Assets");
	//ScatterPhysical();
	// Load level scores
	char info_filename[140];

	if (location == 0)
	{
		sprintf(info_filename, "%s\\LevelBinary\\StaticData\\%d.bmp", local_file_folder, level);
	}
	else
	{
		sprintf(info_filename, "Assets\\LevelBinary\\StaticData\\%d.bmp", level);
	}

	/*
	if (bBarrelsLoaded == true)
	{
		for (j = 0; j < 8;j++ )
		{
			for (i = 0; i < moveable_models; i++)
			{
				delete static_ob[j][i];
			}
		}
	}
	else
	{
		bBarrelsLoaded = true;
	}
	*/


	// LoadLevelData
	FILE * pFile;
	current_pill = 0;
	pFile = fopen(info_filename, "r");
	if (pFile != NULL)
	{
		fread(statics, sizeof(static_t), m_maxParticles, pFile);

		fclose(pFile);

	}
	else
	{
		return false;
	}
	current_pill = 0;
	for (int i = 0; i < m_maxParticles; i++)
	{
		if (statics[i].bActive == 1)
		{
			//statics[i].y = p_Level->GetTerrainHeight(statics[i].x, statics[i].z);
			//auto rotationMatrix = XMMatrixRotationRollPitchYaw(statics[i].rot_x, statics[i].rot_y, statics[i].rot_z);
			//auto translationMatrix = XMMatrixTranslation(statics[i].x, statics[i].y, statics[i].z);
			//DirectX::XMStoreFloat4x4(&statics[i].model_matrix, XMMatrixTranspose(rotationMatrix * translationMatrix));


			// clear the loops and verts
			/*
			if (level == 1)
			{
				if (statics[i].type == 6 || statics[i].type == 7 || statics[i].type == 8)
				{
					statics[i].bActive = 0;
				}
			}
			*/
			if (current_pill < i + 1)
			{
				current_pill = i + 1;
			}

			//info.PosX-(-((float)p_level->left_right_walls) - 1.0f) =  ((float)i*1.0f);
			//info.PosZ+p_level->front_back_walls =  ((float)j*1.0f);
			//p_Statics->MakeLocal(info.PosX, info.PosZ);

			/*
			int ypos = (int)(p_Level->front_back_walls + statics[i].z);
			int xpos = (int)(p_Level->left_right_walls + 1.0f + statics[i].x);

			//p_Level->stack_height[ypos][xpos] = 10.0f;
			float hh = p_Level->stack_height[ypos][xpos];
			if( p_Level->stack_height[ypos][xpos] < statics[i].y)
			{
				p_Level->stack_height[ypos][xpos] = statics[i].y+5.0f;
			}
			*/
		}
	}
	total_pills = current_pill;


	return true;
}



void Statics::MakeAllObjects()
{
	int i;

	ObjInfo info;

	for (i = 0; i < m_maxParticles; i++)
	{
		if (statics[i].bActive == true)
		{
			info.Mass = 0.0f;
			info.Yaw = statics[i].rot_x;
			info.Pitch = statics[i].rot_y;
			info.Roll = statics[i].rot_z;
			info.Restitution = 0.0f;// phobject[statics[i].type].restitution;
			info.Friction = 0.0f;// phobject[statics[i].type].friction;

			info.PosX = statics[i].x;
			info.PosY = statics[i].y;
			info.PosZ = statics[i].z;

			info.item_id = statics[i].type;

			info.raise_by_height = true;
			//m_totalBoxes = 0;
			m_box[m_totalBoxes] = new WorldObject(m_deviceResources, m_phys);

			if (true)// (current_stack_item>0)
			{
				m_box[m_totalBoxes]->m_model = static_model[statics[i].type].m_solid_model;
				m_box[m_totalBoxes]->bTexture = 1;// 1;
												  //info.PosY += m_box[m_totalBoxes]->m_model->total_height;

												  //m_box[m_totalBoxes]->SetTexturePointer(phobject[phob_index].m_Texture);
			}
			info.mask = (COL_BOXES);
			info.group = (COL_WORLDSTUFF | COL_CAR | COL_TREES | COL_WALLS);
			/*
			if (phobject[statics[i].type].shape == 1)
			{
				m_box[m_totalBoxes]->MakePhysicsBoxFromMesh(m_d3dDevice.Get(), p_physics, &info);
			}
			if (phobject[statics[i].type].shape == 2)
			{
				m_box[m_totalBoxes]->MakePhysicsSphereFromMesh(m_d3dDevice.Get(), p_physics, &info);
			}
			if (phobject[statics[i].type].shape == 3)
			{
				m_box[m_totalBoxes]->MakePhysicsCylinderFromMesh(m_d3dDevice.Get(), p_physics, &info);
			}
			if (phobject[statics[i].type].shape == 4)
			{
				m_box[m_totalBoxes]->MakePhysicsConeFromMesh(m_d3dDevice.Get(), p_physics, &info);
			}
			*/
			if (true) //(phobject[statics[i].type].shape == 5)
			{
				m_box[m_totalBoxes]->MakePhysicsConvexRotation(&info);
			}
			//m_box[m_totalBoxes]->m_rigidbody->setCollisionFlags(COL_WORLDSTUFF)
			//m_box[m_totalBoxes]->SetPosition(statics[i].x, statics[i].y, statics[i].z, statics[i].rot_x, statics[i].rot_y, statics[i].rot_z);
			/*
			int ypos = (int)(p_Level->front_back_walls + statics[i].z);
			int xpos = (int)(p_Level->left_right_walls + 1.0f + statics[i].x);

			p_Level->stack_height[ypos][xpos] += (m_box[m_totalBoxes]->m_model->total_height*1.0f) + 0.1f;
			*/

			if (false)//(is_static == true)
			{
				//info.Mass = 0.0f;
				m_box[m_totalBoxes]->bStatic = true;
				m_box[m_totalBoxes]->active_frames = 20;
			}
			//m_box[m_totalBoxes]->m_camera = p_Camera;
			m_totalBoxes++;
		}
	}

}



bool Statics::LoadLevel(int level, int location)
{
	current_level = level;
	for (int i = 0; i < m_maxParticles; i++)
	{
		statics[i].bActive = 0;
	}

	if (LoadBinary(level,location) == false)
	{
		return true;// LoadLocations(level);
	}
	else
	{
		return true;
	}

	update_frame = 0;
}

void Statics::SetDesignPointer(float x, float y, float z, int type)
{
	if (bIgnoreHeight == true)
	{
		design_pointer.y = design_height;
	}
	else
	{
		design_pointer.y = y + design_height;// +static_ob[type][0]->m_model->total_height*0.5f;
	}

	design_pointer.bActive = 1;
	design_pointer.x = x;

	design_pointer.z = z;
	design_pointer.type = type;
	bDesignPointer = true;

	//bIgnoreHeight = true;
}

void Statics::SetDesignPointer(float x, float y, float z, float _y,float _p,float _r,int type)
{
	if (bIgnoreHeight == true)
	{
		design_pointer.y = design_height;
	}
	else
	{
		design_pointer.y = y + design_height;// +static_ob[type][0]->m_model->total_height*0.5f;
	}

	design_pointer.bActive = 1;
	design_pointer.x = x;

	design_pointer.z = z;
	design_pointer.type = type;
	bDesignPointer = true;

	design_pointer.rot_x = _y;
	design_pointer.rot_y = _p;
	design_pointer.rot_z = _r;


	//bIgnoreHeight = true;
}


float Statics::GetStaticWorldHeight(float posx, float posz)
{
	bool bHit = false;
	float dist = 0.0f;
	for (int i = 0; i < m_maxParticles; i++)
	{
		if (statics[i].bActive == true)
		{
			int ob_type = statics[i].type;
			if (ob_type == 2)
				ob_type = 1;
			// check if in range
			btVector3 check_pos = btVector3(posx, statics[i].y, posz);
			btVector3 sta_pos = btVector3(statics[i].x, statics[i].y, statics[i].z);

			if (true)//(check_pos.distance(sta_pos) < m_Statics->static_model[ob_type].m_alpha_model->furthest_point)
			{
				float dist = static_model[ob_type].m_alpha_model->RayHit(posx, 30.0f, posz, 0.0f, -1.0f, 0.0f, statics[i].model_matrix);
				if (dist == 999.0f)
				{
					dist = 0.0f;
				}
				else
				{
					bHit = true;
					return 30.0f - dist;
					//dist;
				}
			}
		}
	}
	return dist;
}





void Statics::CreateOne(float x, float y, float z, int type)
{
	int i, l;
	bool bFound = false;
	int point_find = 0;
	DirectX::XMFLOAT4 col = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	// Check exists
	if(true)
	{
		if (type == -1)
		{
			bFound = true;
			for (i = 0; i < m_maxParticles; i++)
			{
				if (statics[i].bActive == 1 && statics[i].x == x && statics[i].z == z)
				{
					statics[i].bActive = 0;
				}
			}
		}
	}
	if (bFound == true || type == -1)
		return;

	for (i = 0; i < m_maxParticles; i++)
	{
		if (statics[i].bActive == 0)
		{
			statics[i].x = x;
			statics[i].z = z;
			if(true)// bIgnoreHeight==true)
			{
				statics[i].y = design_pointer.y;// 0.5f + design_height + p_Level->GetTerrainHeight((int)x, (int)z);
			}

			if (i<m_maxParticles - 1 && statics[i].y > -990.0f)
			{
				//total_pills++;
				statics[i].bActive = 1;

				statics[i].type = type;


				if (type == 7)
				{

					int iterations = 15;
					for (int p = 0; p < iterations; p++)
					{
						float fract = (2.0f / iterations)*p;
						float xpos, ypos, zpos;
						float yaw, pitch, roll;
						float size = 8.1f;

						zpos = size + (sin(((M_PI)*fract) + M_PI)*size);
						ypos = size + (cos(((M_PI)*fract) + M_PI)*size);
						xpos = p*1.2f;
						yaw = ((M_PI)*fract) + M_PI;
						roll = 0.0f;
						pitch = 0.0f;


						statics[i+p].rot_x = yaw;
						statics[i + p].rot_y = pitch;
						statics[i + p].rot_z = roll;

						statics[i + p].x = xpos+ x;
						statics[i + p].y = ypos + design_pointer.y;
						statics[i + p].z = zpos+ z;

						auto rotationMatrix2 = XMMatrixRotationRollPitchYaw(yaw, pitch, roll);
						auto translationMatrix2 = XMMatrixTranslation(xpos, ypos, zpos);

						auto rotationMatrix = XMMatrixRotationRollPitchYaw(design_pointer.rot_x, design_pointer.rot_y, design_pointer.rot_z);
						auto translationMatrix = XMMatrixTranslation(design_pointer.x, design_pointer.y, design_pointer.z);
						auto malfMatrix = XMMatrixTranspose(rotationMatrix2 * translationMatrix2);
						XMStoreFloat4x4(&statics[i+p].model_matrix, XMMatrixTranspose(rotationMatrix * translationMatrix) * malfMatrix);
						statics[i + p].bActive = 1;
						statics[i + p].type = 7;
						current_pill=i+p;
					}

					// draw loop

				}
				else
				{
					if (type == 8)
					{
						int iterations = 20;
						for (int p = 1; p < iterations / 4; p++)
						{
							float fract = (2.0f / iterations)*p;
							float xpos, ypos, zpos;
							float yaw, pitch, roll;
							float size = 6.0f;

							zpos = size + (sin(((M_PI)*fract) + M_PI)*size);
							ypos = size + (cos(((M_PI)*fract) + M_PI)*size)-0.1f;
							xpos = 0.0f;
							yaw = ((M_PI)*fract) + M_PI;
							roll = 0.0f;
							pitch = 0.0f;

							statics[i + p].rot_x = yaw;
							statics[i + p].rot_y = pitch;
							statics[i + p].rot_z = roll;

							statics[i + p].x = xpos + x;
							statics[i + p].y = ypos + design_pointer.y;
							statics[i + p].z = zpos + z;

							auto rotationMatrix2 = XMMatrixRotationRollPitchYaw(yaw, pitch, roll);
							auto translationMatrix2 = XMMatrixTranslation(xpos, ypos, zpos);

							auto rotationMatrix = XMMatrixRotationRollPitchYaw(design_pointer.rot_x, design_pointer.rot_y, design_pointer.rot_z);
							auto translationMatrix = XMMatrixTranslation(design_pointer.x, design_pointer.y, design_pointer.z);
							auto malfMatrix = XMMatrixTranspose(rotationMatrix2 * translationMatrix2);
							XMStoreFloat4x4(&statics[i + p].model_matrix, XMMatrixTranspose(rotationMatrix * translationMatrix) * malfMatrix);
							statics[i + p].bActive = 1;
							statics[i + p].type = 8;
							current_pill = i + p;
						}
					}
					else
					{

						float rand_scalea = 1.0f;
						float rand_scaleb = 1.0f;

						if (type == 3 || type == 4 || type == 5)
						{
							statics[i].rot_x = ((rand() % 1000)*0.001f)* 2.0f * M_PI;
							statics[i].rot_y = ((rand() % 1000)*0.001f)* 2.0f * M_PI;
							statics[i].rot_z = ((rand() % 1000)*0.001f)* 2.0f * M_PI;

							rand_scalea = 0.8f + ((rand() % 1000)*0.0002f);
							rand_scaleb = 0.8f + ((rand() % 1000)*0.0002f);

							statics[i].col_r = ((rand() % 1000)*0.001f)*0.1f;
							statics[i].col_g = ((rand() % 1000)*0.001f)*0.1f;
							statics[i].col_b = ((rand() % 1000)*0.001f)*0.1f; // random color weights for stones
						}
						else
						{
							if (type == 8)
							{
								statics[i].col_r = -((rand() % 1000)*0.001f)*0.5f;
								statics[i].col_g = -((rand() % 1000)*0.001f)*0.5f;
								statics[i].col_b = -((rand() % 1000)*0.001f)*0.5f; // random color weights for stones
							}
							else
							{
								statics[i].rot_x = design_pointer.rot_x;
								statics[i].rot_y = design_pointer.rot_y;
								statics[i].rot_z = design_pointer.rot_z;
							}
						}
						//statics[i].angle = (rand() % 100)*0.01f*(2.0f*M_PI);

						/*
						btTransform m_initialTransform = btTransform(btQuaternion(0.0f, 0.0f, 0.0f), btVector3(statics[i]->x, statics[i]->y + (tree_model[statics[i]->type].m_solid_model->total_height) + 1.0f, statics[i]->z));

						statics[i]->setWorldTransform(m_initialTransform);

						MakePhysicsBody(statics[i]);
						statics[i]->m_body->setWorldTransform(m_initialTransform);
						*/
						//statics[i]->m_body = nullptr;
						//float rand_scalea = 0.9f + (rand() % 100)*0.002f;
						//float rand_scaleb = 0.9f + (rand() % 100)*0.002f;


						XMMATRIX scaleMatrix = XMMatrixScaling(rand_scaleb, rand_scalea, rand_scaleb);

						auto rotationMatrix = XMMatrixRotationRollPitchYaw(statics[i].rot_x, statics[i].rot_y, statics[i].rot_z);
						XMMATRIX scaleRotationMatrix = XMMatrixMultiply(scaleMatrix, rotationMatrix);
						auto translationMatrix = XMMatrixTranslation(statics[i].x, statics[i].y, statics[i].z);
						DirectX::XMStoreFloat4x4(&statics[i].model_matrix, XMMatrixTranspose(scaleRotationMatrix * translationMatrix));

						current_pill++;
					}
				}
			}

			//if (total_pills < i + 1)
			//	total_pills = i + 1;
			total_pills = current_pill;
			break;
		}
	}
}



void Statics::MakeLocal(float x, float z)
{
	int i;
	
	return;

	btVector3 player_posA = btVector3(x, p_Level->GetTerrainHeight(x, z), z);

	

	current_index = 0;
	for (i = 0; i < m_maxParticles; i++)
	{
		if (statics[i].bActive == true)
		{
			p_index_car[current_index].dist = player_posA.distance(btVector3(statics[i].x, statics[i].y, statics[i].z));
			p_index_car[current_index].part_no = i;

			current_index++;
		}
		else
		{

		}
	}


	if (current_index>0)
	{
		qsort(p_index, current_index, sizeof(part_index), StaticsSortCB);
		qsort(p_index_car, current_index, sizeof(part_index), StaticsSortCarCB);
	}

	int ob_type;

	for (int i = 0; i < moveable_models[0]; i++)
	{
		if (i < current_index)
		{
			ob_type = statics[p_index_car[i].part_no].type;
			if (ob_type == 2)
				ob_type = 1;
			static_ob[ob_type][i]->SetPosition(statics[p_index_car[i].part_no].x, statics[p_index_car[i].part_no].y, statics[p_index_car[i].part_no].z, statics[p_index_car[i].part_no].rot_y, statics[p_index_car[i].part_no].rot_x, statics[p_index_car[i].part_no].rot_z);
			//static_ob[i]->SetPosition(design_pointer.x, design_pointer.y, design_pointer.z, design_pointer.rot_y, design_pointer.rot_x, design_pointer.rot_z);

			//static_ob[i]->Render(deviceContext, constantBuffer, constantBufferData);
		}
		else
		{
			//static_ob[statics[p_index_car[i].part_no].type][i]->SetPosition(i * 10.0f, 50.0f+(20.0f*statics[p_index_car[i].part_no].type), i * 10.0f, 0.0f, 0.0f, 0.0f);
			//treebody[i].m_body->setWorldTransform(btTransform(btQuaternion(0.0f, 0.0f, 0.0f), btVector3(i * 10.0f, 50.0f, i * 10.0f)));
		}
	}
}


void Statics::Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> deviceContext, Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer, ModelViewProjectionConstantBuffer* constantBufferData, int part_to_render)
{
	UINT stride = sizeof(VertexPositionTex);
	UINT offset = 0;
	int i,j;

	//return;
	//MVPConstantBuffer m_constantBuffer = m_constantBufferB;

	int current_tree = 999;
	bool bSet = false;
	int tree_type;

	if (update_frame < 5)
	{
		update_frame++;
		if (update_frame == 5)
		{
			for (i = 0; i < total_pills; i++)
			{
				//if (statics[i]->m_body != nullptr)
				//{
				//	statics[i]->m_body->setMassProps(0.0f, btVector3(0.0f, 0.0f, 0.0f));
				//	statics[i]->m_body->setRestitution(0.5f);
				//}
			}
		}
	}

	btVector3 player_posA = btVector3(p_Player->m_constantBufferData.model._14, p_Player->m_constantBufferData.model._24, p_Player->m_constantBufferData.model._34);
	btVector3 cam_pos = btVector3(p_Camera->EyeX(), p_Camera->EyeY(), p_Camera->EyeZ());
	current_index = 0;
	for (i = 0; i<total_pills; i++)
	{
		if (( ( static_model[statics[i].type].bSolid==true && part_to_render==0 ) ||
			(static_model[statics[i].type].bAlpha == true && part_to_render == 1) )
			&& statics[i].bActive==true && p_Camera->CheckPoint(statics[i].model_matrix._14, statics[i].model_matrix._24, statics[i].model_matrix._34, static_model[statics[i].type].m_solid_model->furthest_point))
		{
			p_index_car[current_index].dist = player_posA.distance(btVector3(statics[i].x, statics[i].y, statics[i].z));
			p_index_car[current_index].part_no = i;

			p_index[current_index].dist = cam_pos.distance(btVector3(statics[i].x, statics[i].y, statics[i].z));
			p_index[current_index].part_no = i;

			current_index++;
		}
		else
		{

		}
	}

	if (current_index>0)
	{
		if(part_to_render==0)
		{
			qsort(p_index, current_index, sizeof(part_index), StaticsSortCB);
		}
		else
		{
			qsort(p_index, current_index, sizeof(part_index), StaticsSortAlphCB);
		}
		qsort(p_index_car, current_index, sizeof(part_index), StaticsSortCarCB);
	}

	//MakeLocal(player_posA->getX(), player_posA->getZ());
	int ob_type;
	if (part_to_render == 0)
	{
		for (int i = 0; i < 10; i++)
		{
			numbs_of_physical[i] = 0;
		}

		for (int i = 0; i < current_index; i++)
		{
			ob_type = statics[p_index_car[i].part_no].type;
			if (ob_type == 2)
				ob_type = 1;
			if (numbs_of_physical[ob_type] < moveable_models[ob_type])
			{
				if (static_ob[ob_type][numbs_of_physical[ob_type]]->m_model->object_scale == 1.0f || static_ob[ob_type][numbs_of_physical[ob_type]]->m_model->object_scale == -1.0f)
				{
					static_ob[ob_type][numbs_of_physical[ob_type]]->SetPositionMatrix(statics[p_index_car[i].part_no].model_matrix);
				}
				else
				{
					static_ob[ob_type][numbs_of_physical[ob_type]]->SetPosition(statics[p_index_car[i].part_no].x, statics[p_index_car[i].part_no].y, statics[p_index_car[i].part_no].z, statics[p_index_car[i].part_no].rot_y, statics[p_index_car[i].part_no].rot_x, statics[p_index_car[i].part_no].rot_z);
				}
				numbs_of_physical[ob_type]++;
			}
			//static_ob[ob_type][i]->SetPosition(statics[p_index_car[i].part_no].x, statics[p_index_car[i].part_no].y, statics[p_index_car[i].part_no].z, statics[p_index_car[i].part_no].rot_y, statics[p_index_car[i].part_no].rot_x, statics[p_index_car[i].part_no].rot_z);
			//static_ob[i]->SetPosition(design_pointer.x, design_pointer.y, design_pointer.z, design_pointer.rot_y, design_pointer.rot_x, design_pointer.rot_z);
			//if(statics[p_index_car[i].part_no].type )
			//static_ob[i]->Render(deviceContext, constantBuffer, constantBufferData);
		}

		//set the rest in the air
		for (i = 0; i < 10; i++)
		{
			ob_type = i;
			if (ob_type == 2)
				ob_type = 1;
			for (j = numbs_of_physical[ob_type]; j < moveable_models[ob_type]; j++)
			{
				static_ob[ob_type][j]->SetPosition(i * 10.0f, 70.0f, i * 10.0f, 0.0f, 0.0f, 0.0f);
			}
		}
	}

	for (int i = 0; i < current_index; i++)
	{
		if (current_tree != statics[p_index[i].part_no].type)
		{
			current_tree = statics[p_index[i].part_no].type;

			if (part_to_render == 0)
			{
				if (static_model[current_tree].bSolid == true)
				{
					deviceContext->IASetVertexBuffers(0, 1, static_model[current_tree].m_solid_model->GetVertexBuffer().GetAddressOf(), &stride, &offset);
					deviceContext->IASetIndexBuffer(static_model[current_tree].m_solid_model->GetIndexBuffer().Get(), DXGI_FORMAT_R16_UINT, 0);
					deviceContext->PSSetShaderResources(0, 1, &static_model[current_tree].m_solid_texture);
				}
			}
			else
			{
				if (static_model[current_tree].bAlpha == true)
				{
					deviceContext->IASetVertexBuffers(0, 1, static_model[current_tree].m_alpha_model->GetVertexBuffer().GetAddressOf(), &stride, &offset);
					deviceContext->IASetIndexBuffer(static_model[current_tree].m_alpha_model->GetIndexBuffer().Get(), DXGI_FORMAT_R16_UINT, 0);
					deviceContext->PSSetShaderResources(0, 1, &static_model[current_tree].m_alpha_texture);
				}
			}
		}

		if (true)//(part_to_render==0)
		{
			if (part_to_render == 0)
			{
				if (static_model[current_tree].bSolid == true)
				{
					if (shader_mode==1 && (statics[p_index[i].part_no].type == 3 || statics[p_index[i].part_no].type == 4 || statics[p_index[i].part_no].type == 5 ))
					{
						static_model[current_tree].m_solid_model->SetModelColourWeight(statics[p_index[i].part_no].col_r, statics[p_index[i].part_no].col_b, statics[p_index[i].part_no].col_g,0.0f);
					}

					constantBufferData->model = statics[p_index[i].part_no].model_matrix;
					deviceContext->UpdateSubresource(constantBuffer.Get(), 0, NULL, constantBufferData, 0, 0);
					deviceContext->DrawIndexed(
						static_model[current_tree].m_solid_model->m_indicesCount,
						0,
						0
						);
				}
			}
			else
			{
				if (static_model[current_tree].bAlpha == true)
				{
					constantBufferData->model = statics[p_index[i].part_no].model_matrix;
					deviceContext->UpdateSubresource(constantBuffer.Get(), 0, NULL, constantBufferData, 0, 0);
					deviceContext->DrawIndexed(
						static_model[current_tree].m_alpha_model->m_indicesCount,
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
				if (static_model[current_tree].bSolid == true)
				{
					deviceContext->IASetVertexBuffers(0, 1, static_model[current_tree].m_solid_model->GetVertexBuffer().GetAddressOf(), &stride, &offset);
					deviceContext->IASetIndexBuffer(static_model[current_tree].m_solid_model->GetIndexBuffer().Get(), DXGI_FORMAT_R16_UINT, 0);
					deviceContext->PSSetShaderResources(0, 1, &static_model[current_tree].m_solid_texture);
				}
			}
			else
			{
				if (static_model[current_tree].bAlpha == true)
				{
					deviceContext->IASetVertexBuffers(0, 1, static_model[current_tree].m_alpha_model->GetVertexBuffer().GetAddressOf(), &stride, &offset);
					deviceContext->IASetIndexBuffer(static_model[current_tree].m_alpha_model->GetIndexBuffer().Get(), DXGI_FORMAT_R16_UINT, 0);
					deviceContext->PSSetShaderResources(0, 1, &static_model[current_tree].m_alpha_texture); 
				}
			}
		}

		if (true)//(part_to_render==0)
		{
			//MVPConstantBuffer m_constantBuffer = m_constantBufferB;


			if (part_to_render == 0)
			{
				if (current_tree == 7)
				{
					int i;
					int iterations = 15;
					for (i = 0; i < iterations; i++)
					{
						float fract = (2.0f / iterations)*i;
						float xpos, ypos, zpos;
						float yaw, pitch, roll;
						float size = 8.1f;

						zpos = size + (sin(((M_PI)*fract)+ M_PI)*size);
						ypos = size + (cos(((M_PI)*fract)+ M_PI)*size);
						 xpos = i*1.2f;
						yaw = ((M_PI)*fract)+ M_PI;
						roll = 0.0f;
						pitch = 0.0f;

						/*
						auto rotationMatrix = XMMatrixRotationRollPitchYaw(design_pointer.rot_x+ yaw, design_pointer.rot_y+ pitch, design_pointer.rot_z+pitch);
						auto translationMatrix = XMMatrixTranslation(design_pointer.x+ xpos, design_pointer.y+ ypos, design_pointer.z+ zpos);
						XMStoreFloat4x4(&design_pointer.model_matrix, XMMatrixTranspose(rotationMatrix * translationMatrix));
						*/
						
						auto rotationMatrix2 = XMMatrixRotationRollPitchYaw(yaw, pitch, roll);
						auto translationMatrix2 = XMMatrixTranslation(xpos, ypos, zpos);

						auto rotationMatrix = XMMatrixRotationRollPitchYaw(design_pointer.rot_x, design_pointer.rot_y, design_pointer.rot_z);
						auto translationMatrix = XMMatrixTranslation(design_pointer.x , design_pointer.y, design_pointer.z );
						auto malfMatrix = XMMatrixTranspose(rotationMatrix2 * translationMatrix2);
						XMStoreFloat4x4(&design_pointer.model_matrix,  XMMatrixTranspose( rotationMatrix * translationMatrix) * malfMatrix);
						
						constantBufferData->model = design_pointer.model_matrix;
						deviceContext->UpdateSubresource(constantBuffer.Get(), 0, NULL, constantBufferData, 0, 0);

						deviceContext->DrawIndexed(
							static_model[7].m_solid_model->m_indicesCount,
							0,
							0
							);
					}

					// draw loop

				}
				else
				{

					if (current_tree == 8)
					{
						int i;
						int iterations = 20;
						for (i = 1; i < iterations/4; i++)
						{
							float fract = (2.0f / iterations)*i;
							float xpos, ypos, zpos;
							float yaw, pitch, roll;
							float size = 6.0f;

							zpos = size + (sin(((M_PI)*fract) + M_PI)*size);
							ypos = size + (cos(((M_PI)*fract) + M_PI)*size)-0.1f;
							xpos = 0.0f;
							yaw = ((M_PI)*fract) + M_PI;
							roll = 0.0f;
							pitch = 0.0f;

							/*
							auto rotationMatrix = XMMatrixRotationRollPitchYaw(design_pointer.rot_x+ yaw, design_pointer.rot_y+ pitch, design_pointer.rot_z+pitch);
							auto translationMatrix = XMMatrixTranslation(design_pointer.x+ xpos, design_pointer.y+ ypos, design_pointer.z+ zpos);
							XMStoreFloat4x4(&design_pointer.model_matrix, XMMatrixTranspose(rotationMatrix * translationMatrix));
							*/

							auto rotationMatrix2 = XMMatrixRotationRollPitchYaw(yaw, pitch, roll);
							auto translationMatrix2 = XMMatrixTranslation(xpos, ypos, zpos);

							auto rotationMatrix = XMMatrixRotationRollPitchYaw(design_pointer.rot_x, design_pointer.rot_y, design_pointer.rot_z);
							auto translationMatrix = XMMatrixTranslation(design_pointer.x, design_pointer.y, design_pointer.z);
							auto malfMatrix = XMMatrixTranspose(rotationMatrix2 * translationMatrix2);
							XMStoreFloat4x4(&design_pointer.model_matrix, XMMatrixTranspose(rotationMatrix * translationMatrix) * malfMatrix);

							constantBufferData->model = design_pointer.model_matrix;
							deviceContext->UpdateSubresource(constantBuffer.Get(), 0, NULL, constantBufferData, 0, 0);

							deviceContext->DrawIndexed(
								static_model[8].m_solid_model->m_indicesCount,
								0,
								0
								);
						}
					}
					else
					{

						if (static_model[current_tree].bSolid == true)
						{
							auto rotationMatrix = XMMatrixRotationRollPitchYaw(design_pointer.rot_x, design_pointer.rot_y, design_pointer.rot_z);
							auto translationMatrix = XMMatrixTranslation(design_pointer.x, design_pointer.y, design_pointer.z);
							XMStoreFloat4x4(&design_pointer.model_matrix, XMMatrixTranspose(rotationMatrix * translationMatrix));
							constantBufferData->model = design_pointer.model_matrix;
							deviceContext->UpdateSubresource(constantBuffer.Get(), 0, NULL, constantBufferData, 0, 0);

							deviceContext->DrawIndexed(
								static_model[current_tree].m_solid_model->m_indicesCount,
								0,
								0
								);
						}
					}
				}
			}
			else
			{
				if (static_model[current_tree].bAlpha == true)
				{
					auto rotationMatrix = XMMatrixRotationRollPitchYaw(design_pointer.rot_x, design_pointer.rot_y, design_pointer.rot_z);
					auto translationMatrix = XMMatrixTranslation(design_pointer.x, design_pointer.y , design_pointer.z);
					XMStoreFloat4x4(&design_pointer.model_matrix, XMMatrixTranspose(rotationMatrix * translationMatrix));
					constantBufferData->model = design_pointer.model_matrix;
					deviceContext->UpdateSubresource(constantBuffer.Get(), 0, NULL, constantBufferData, 0, 0);

					deviceContext->DrawIndexed(
						static_model[current_tree].m_alpha_model->m_indicesCount,
						0,
						0
						);
				}
			}
		}
	}

}


