#include "pch.h"
#include "Stage.h"

#include "MediaReader.h"
#include "SoundEffect.h"

#include "Level.h"

#include "Common\DirectXHelper.h"

#include "BasicLoader.h"

#include <concrt.h>

using namespace Simple3DGame;

using namespace concurrency;
using namespace DirectX;
using namespace Windows::Foundation;
using namespace Microsoft::WRL;
using namespace Windows::System::Threading;


Stage::Stage(std::shared_ptr<DX::DeviceResources> pm_deviceResources, Physics* pm_physics, Camera^ p_camera, Simple3DGame::IGameUIControl^ UIControl)
{
	int i;
	bInitialized = false;

	m_uiControl = UIControl;

	m_deviceResources = pm_deviceResources;
	m_camera = p_camera;
	m_phys = pm_physics;

	noise_z = 0.0f;

	//m_phys->m_dynamicsWorld->
	/*
	m_Items->m_SFPop = ref new SoundEffect();
	m_Items->m_SFPop->Initialize(
		m_audioController->SoundEffectEngine(),
		mediaReader->GetOutputWaveFormatEx(),
		soundSFFruitPop
		);
		*/
	current_level = 1;
	render_delay = 1.0f;
	start_render_delay = 1.0f;
	bRain = true;
	bSnow = false;

	bPaused = false;

	bPlayerPosition = false;

	bRespot = false;
	respot_delay = 4.0f;

	bLevelComplete = false;
	level_complete_event_timer = 0.0f;

	// get local file folder
	Windows::Storage::StorageFolder^ folder = Windows::Storage::ApplicationData::Current->LocalFolder;
	// use folder->Path->Data() as the path base
	wcstombs(local_file_folder, folder->Path->Data(), sizeof(local_file_folder));
	// Crate Folders
	char info_filename[140];
	wchar_t w_info_filename[140];
	sprintf(info_filename, "%s\\LevelData", local_file_folder);
	_swprintf(w_info_filename, L"%hs", info_filename);
	CreateDirectory(w_info_filename, NULL);
	sprintf(info_filename, "%s\\LevelBinary", local_file_folder);
	_swprintf(w_info_filename, L"%hs", info_filename);
	CreateDirectory(w_info_filename, NULL);

	state_to = -1;

	game_settings.shader_mode = 1;

	bFadeIn=false;
	bLevelStarting = false;

	bSaveLandRay = true;

	bSkipIntro = true;

	bMusic = true;

	bDesignMode = false;

	iLoadLocal = 1;  // 0 = no
	current_design_item = 0;
	bRayCheck = false;

	bTimedOut = false;

	bContinualPlace = false;

	bRotateRay = false;

	bLevelLoaded = false;

	bResetLevelUI = false;

	bAllLoaded = false;

	LoadSettings();

	m_uiControl->SetScreenControls(game_settings.screen_controls);

	LoadPlayerNames();
}

void Stage::SetMusicMode(int mode)
{
	game_settings.music = mode;
	if (mode == 0)
	{
		m_audio.Stop();
	}
	else
	{
		m_audio.Start();
	}
	//m_audio.st
	SaveSettings();
}

void Stage::SetShaderMode(int mode)
{ 
	game_settings.shader_mode = mode; 
	m_Trees->SetShaderMode(mode); 
	m_Statics->SetShaderMode(mode);
	SaveSettings(); 
}

void Stage::SetScreenControls(int mode)
{
	if(game_settings.screen_controls!=mode)
	{
		game_settings.screen_controls = mode;
		m_uiControl->SetScreenControls(mode);
		SaveSettings();
	}
}

void Stage::AddName(char* name_str)
{
	strcpy(player_data[total_player_names].name, name_str);
	strcpy(current_playername, name_str);
	player_data[total_player_names].score = 0;
	player_data[total_player_names].current_level = 1;
	current_level = 1;
	LoadPlayerScores(name_str);
	total_player_names++;
	SavePlayerNames();
	SetPlayersUI();
}

bool Stage::CheckName(char* name_str)
{
	int i;
	bool bFound = false;
	for (i = 0; i < total_player_names; i++)
	{
		if (!strcmp(player_data[i].name, name_str))
		{
			bFound = true;
		}
	}
	return bFound;
}


void Stage::LoadSettings()
{
	int i;


	// Load level scores
	char info_filename[140];
	sprintf(info_filename, "%s/Settings.txt", local_file_folder);

	// LoadLevelData
	FILE * pFile;

	pFile = fopen(info_filename, "r");
	total_player_names = 0;
	if (pFile != NULL)
	{
		fread(&game_settings, sizeof(game_settings_t) * 1, 1, pFile);
		fclose(pFile);
	}
	else
	{
		game_settings.shader_mode = 1;
		game_settings.music = 1;
		game_settings.screen_controls = 1;
	}
}
void Stage::SaveSettings()
{
	int i;
	char info_filename[140];
	FILE* pFile;

	sprintf(info_filename, "%s/Settings.txt", local_file_folder);

	pFile = fopen(info_filename, "wb");
	if (pFile != NULL)
	{
		fwrite(&game_settings, sizeof(game_settings_t) * 1, 1, pFile);
		fclose(pFile);
	}
}

void Stage::LoadPlayerNames()
{
	int i;

	// Load level scores
	char info_filename[140];
	sprintf(info_filename, "%s/PlayerNames.txt", local_file_folder);

	// LoadLevelData
	FILE * pFile;

	pFile = fopen(info_filename, "r");
	total_player_names = 0;
	if (pFile != NULL)
	{
		for (i = 0; i < 40; i++)
		{
			fread(&player_data[i], sizeof(player_info) * 1, 1, pFile);
			if (strlen(player_data[i].name)>0)
			{
				total_player_names++;
			}
		}
		fclose(pFile);
	}
	else
	{
		for (i = 0; i < 40; i++)
		{
			strcpy(player_data[i].name, "");
			player_data[i].index = i;
			player_data[i].score = 0;
		}
		total_player_names = 0;
	}

	SetPlayersUI();
}

void Stage::SavePlayerNames()
{
	int i;
	char info_filename[140];
	FILE* pFile;

	sprintf(info_filename, "%s/PlayerNames.txt", local_file_folder);

	pFile = fopen(info_filename, "wb");
	if (pFile != NULL)
	{
		for (i = 0; i < 40;i++)
		{
			fwrite(&player_data[i], sizeof(player_info) * 1, 1, pFile);
		}
		fclose(pFile);
	}
}

void Stage::LoadPlayerScores(char* player_name)
{
	int i;
	strcpy(current_playername, player_name);
	// Load level scores
	char info_filename[140];
	sprintf(info_filename, "%s/LevelScores_%s.txt", local_file_folder, current_playername);

	// LoadLevelData
	FILE * pFile;

	pFile = fopen(info_filename, "r");

	if (pFile != NULL)
	{
		fread(&level_data[0], sizeof(level_info) * 100, 1, pFile);
		fclose(pFile);
	}
	else
	{
		for (i = 0; i < 100; i++)
		{
			level_data[i].bEnabled = false;
			level_data[i].high_score = 0;
			level_data[i].lev_im_no = 0;
		}
		level_data[0].bEnabled = true;

	}
}
void Stage::SavePlayerScores()
{
	int i;
	char info_filename[140];
	FILE* pFile;

	sprintf(info_filename, "%s/LevelScores_%s.txt", local_file_folder, current_playername);

	pFile = fopen(info_filename, "wb");
	if (pFile != NULL)
	{
		fwrite(&level_data[0], sizeof(level_info) * 100, 1, pFile);
		fclose(pFile);
	}
	player_data[current_player_index].score = 0;
	player_data[current_player_index].current_level = current_level;
	for (i = 0; i < 100; i++)
	{
		player_data[current_player_index].score += level_data[i].high_score;
	}
	SavePlayerNames();
}

void Stage::LoadDeviceResources()
{
	m_audio.Initialize();
	m_audio.CreateResources();

	m_Level = new Level(m_deviceResources, m_phys,current_level, m_camera);

	player_mass = 3.1f;

	m_Textures = new Textures(m_deviceResources,true);

	

	ObjInfo info = { "Assets/Models/globe2.obj"
		, L"Assets/Textures/cartex.dds"
//		, L"Assets/foot_ball.dds"
		//, L"Assets/sphere.dds"
		, -0.8f		// Scale
		, 0.0f		// X,y,z
		, 5.0f
		, 0.0f
		, player_mass		// mass
		, 0.0f		// yaw,pitch,roll
		, 0.0f
		, 0.0f
		, 0.5f		// restitution
		, 0.5f
	,0,false
		,(COL_BOXES | COL_TREES | COL_WALLS)
		,COL_CARBODY };
	// friction
					/*
					info.ObjFilename = "Assets/Orange.obj";
					info.TextureFilename = L"Assets/Orange.dds";
					info.Scale = -0.3f;
					info.PosX = 0.0f;
					info.PosY = 1.0f;
					info.PosZ = 0.0f; //m_Level->back_position+100.0f;
					info.Mass = 1.0f;
					info.Yaw = 0.0f;
					info.Pitch = 0.0f;
					info.Roll = 0.0f;
					info.Restitution = 0.8f;
					info.Friction = 0.5f;
					*/

					//m_Banana->MakePhysicsConvexMesh(m_deviceResources->GetD3DDevice(), m_phys, &info);

	m_Flag = new Flag(m_deviceResources);
	strcpy(m_Flag->ground1, "checker");

	m_Flag->SetLevelLocation(1, 1);
	//m_Level->InitializeTerrain(1, 0.02f, 0.1f);
	m_Flag->InitializeGeneratedTerrain(1, 3, 3, 0.02f, 0.1f, 0.0f);

	m_Banana = new Player(m_deviceResources,m_phys);
	m_Banana->p_Level = m_Level;
	//m_Banana->MakePhysicsBoxFromMesh(m_deviceResources->GetD3DDevice(), m_phys, &info);
	m_Banana->MakePhysicsCompoundBoxFromMesh(&info, 0.0f, 0.6f, 0.0f); // shifts the center of gravity
	m_Banana->m_rigidbody->setActivationState(DISABLE_DEACTIVATION);
	m_Banana->MakeWheels();
	
	m_shoot = new Shoot(m_deviceResources, m_Level, m_Banana, m_camera);
	m_Banana->m_model_rocket = m_shoot->m_model_rocket;
	m_Banana->m_Texture_rocket = m_shoot->m_Texture_rocket;

	//m_Banana->CreateVehicle(m_deviceResources->GetD3DDevice(), m_phys);

	m_Fence = new Fence(m_deviceResources->GetD3DDevice(), m_Level, m_camera);

	m_Snow = new Snow(m_deviceResources);
	m_Snow->Initialize(m_Level, m_Banana, m_camera);

	m_Splash = new Splash(m_deviceResources);
	m_Splash->Initialize(m_Level, m_Banana, m_camera);

	m_dots = new Dots(m_deviceResources);
	m_dots->Initialize(m_Level, m_Banana, m_camera);
	
	

	m_Explode = new Explode(m_deviceResources->GetD3DDevice(), m_Level, m_Banana, m_camera);

	m_Rain = new Rain(m_deviceResources);
	m_Rain->Initialize(m_Level, m_Banana, m_camera);
	m_Rain->p_Splash = m_Splash;

	m_Trees = new Trees(m_deviceResources, m_Level, m_Banana, m_camera, m_Points, m_phys);
	m_Trees->p_dots = m_dots;
	m_Trees->bDesignMode = bDesignMode;

	m_Statics = new Statics(m_deviceResources, m_Level, m_Banana, m_camera, m_Points, m_phys, m_Textures);
	m_Boxes = new Boxes(m_deviceResources, m_Level, m_phys, m_camera, m_Statics, m_Textures);
	m_Boxes->p_player = m_Banana->m_rigidbody;

	InitializeStuff();// .wait();
	LoadShadersSync();

	m_Points = new Points(m_deviceResources->GetD3DDevice(), m_Level, m_Banana, m_camera);
	m_Cash = new Cash(m_deviceResources, m_Level, m_Banana, m_camera,m_Points,&m_audio);

	m_Fruit = new Fruit(m_deviceResources, m_Level, m_Banana, m_camera, m_Points, m_Cash,m_Textures);

	m_Lights = new Lights(m_deviceResources);

	m_Sky = new Sky(m_deviceResources);
	m_Sky->Initialize();


	

	m_Statics->LoadPhysical();




	float wallDistances[8];
	wallDistances[0] = 10.0f;
	wallDistances[1] = 10.0f;
	wallDistances[2] = 10.0f;
	wallDistances[3] = 10.0f;
	wallDistances[4] = 10.0f;
	wallDistances[5] = 10.0f;
	wallDistances[6] = 10.0f;
	wallDistances[7] = 10.0f;

	//m_audio.SetRoomSize(10.0f, wallDistances);
	
	m_audio.SetSoundEffectVolume(SFXSFCoinCollect, 0.1f);

	m_audio.SetSoundEffectPitch(SFXSFCoinCollect, 1.0f);

	m_audio.SetSoundEffectPitch(SFXSFFruitPop, 0.5f);
	m_audio.SetSoundEffectVolume(SFXSFFruitPop, 0.5f);

	m_audio.SetSoundEffectPitch(SFXChing, 1.5f);
	m_audio.SetSoundEffectVolume(SFXChing, 0.5f);


	m_audio.SetSoundEffectPitch(SFXRocket, 0.9f);
	m_audio.SetSoundEffectVolume(SFXRocket, 0.2f);

	m_Fruit->p_audio = &m_audio;

	m_audio.SetSoundEffectVolume(SFXMenuClick, 0.4f);
	/*
	m_audioController = ref new Audio;
	m_audioController->CreateDeviceIndependentResources();

	MediaReader^ mediaReader = ref new MediaReader;
	auto soundSFFruitPop = mediaReader->LoadMedia("Assets/pickup_coin19.wav");
	auto soundFlick = mediaReader->LoadMedia("Assets/ping.wav");
	auto soundBBall = mediaReader->LoadMedia("Assets/bball.wav");
	//targetHitSound->

	m_SFBasket = ref new SoundEffect();
	m_SFBasket->Initialize(
		m_audioController->SoundEffectEngine(),
		mediaReader->GetOutputWaveFormatEx(),
		soundBBall
		);

	m_SFFlick = ref new SoundEffect();
	m_SFFlick->Initialize(
		m_audioController->SoundEffectEngine(),
		mediaReader->GetOutputWaveFormatEx(),
		soundFlick
		);

	m_Fruit->m_SFPop = ref new SoundEffect();
	m_Fruit->m_SFPop->Initialize(
		m_audioController->SoundEffectEngine(),
		mediaReader->GetOutputWaveFormatEx(),
		soundSFFruitPop
		);
		*/

	if (bDesignMode == true)
	{
		m_uiControl->SetEditMode();
	}

	m_Fruit->bDesignMode = bDesignMode;
	m_Statics->bDesignPointer = bDesignMode;
	m_Boxes->bDesignPointer = bDesignMode;

	music_volume = 0.7f;
	m_audio.SetMusicVolume(music_volume);

	bInitialMusicStarted = false;

	blendFactor[0] = 0.0f;
	blendFactor[1] = 0.0f;
	blendFactor[2] = 0.0f;
	blendFactor[3] = 0.0f;

	SetupAlphaBlendingStates();
	

	m_Trees->shader_mode = game_settings.shader_mode;
	m_Statics->shader_mode = game_settings.shader_mode;

	
	//m_audio.Start();

	current_level = 2;
	SetGameState(STATE_START);

	bAllLoaded = true;
}

task<void> Stage::InitializeStuff()
{
	std::vector<task<void>> tasks;

	tasks.push_back(m_Boxes->LoadModels());
	tasks.push_back(m_Statics->LoadModels());

	return when_all(tasks.begin(), tasks.end());
}

void Stage::SetGameStateTo(int state, float delay)
{
	state_to = state;
	start_state_to_delay = delay;
	start_state_to_val = delay;
}

void Stage::SetGameState(int state)
{
	//	STATE_PLAYING = 0, // Start the game
	//	STATE_START = 1, // Start Screen
	//	STATE_CHOOSE_PLAYER = 2, // Choose Player
	//	STATE_ENTER_PLAYER = 3, // New Player
	current_game_state = state;

	switch (state)
	{
	case STATE_START:  if (bDesignMode == true)
						{
							state = START_PLAYING;
							current_level = 4;
						}
					   else
					   {
						
						m_uiControl->ShowStartScreen();
						if (bMusic == true )// m_audio
						{
							m_audio.Stop();
							/*
							m_audio.SetTrack(0); 
							if(game_settings.music == 1)
							{
								m_audio.Start();
							}
							*/
						}
						bPaused = true;
						break;
						 }

	case START_PLAYING: LoadPlayerScores(current_playername);
						next_new_level = 0;
						SetLevelUI();
						m_uiControl->ShowGameScreen();
						current_level = next_new_level;
						if (bAppInTrialMode == true)
						{
							if (current_level > 3)
							{
								current_level = 3;
								next_new_level = 3;
							}
						}

	case STATE_PLAYING:	if (current_level>0)
						{
							m_uiControl->ScrollToLevel(current_level - 1);
						}
						bDesignlevelLoaded = false;
						LoadLevel(current_level);


						break;
	}
}

void Stage::SetPlayer(int ind)
{
	strcpy(current_playername, player_data[total_player_names-1-ind].name);
	current_player_index = ind;
}

void Stage::SetPlayersUI()
{
	int i;
	char fname[40];
	m_uiControl->ClearPlayerList();

	for (i = 0; i < total_player_names; i++)
	{
		//
		//std::string s_str = std::string(player_data[i].name);
		std::string s_str = std::string(player_data[total_player_names - 1 - i].name);
		std::wstring wid_str = std::wstring(s_str.begin(), s_str.end());
		const wchar_t* w_char = wid_str.c_str();

		m_uiControl->AddPlayerListItem(total_player_names - 1 - i, ref new Platform::String(w_char), player_data[total_player_names - 1 - i].score, player_data[total_player_names - 1 - i].current_level);
	}

	m_uiControl->SetPlayerList();
}


void Stage::SetLevelUI()
{
	int i;
	char fname[40];
	m_uiControl->ClearLevelList();

	for (i = 0; i < 13; i++)
	{
		if (false)//(level_data[i].bEnabled == false)
		{
			sprintf(fname, "Assets/Lock.png", i + 1);
		}
		else
		{
			sprintf(fname, "Assets/Levelthumbs/%d.png", i+1);

		}
		if (level_data[i].bEnabled != false )
		{
			if (level_data[i].high_score == 0)
			{
				next_new_level = i + 1;
			}
		}


		std::string s_str = std::string(fname);
		std::wstring wid_str = std::wstring(s_str.begin(), s_str.end());
		const wchar_t* w_char = wid_str.c_str();

		m_uiControl->AddLevelListItem(ref new Platform::String(w_char), (i+1).ToString(), level_data[i].high_score, level_data[i].bEnabled,i+1);

	}
	//next_new_level = 13;
	m_uiControl->SetLevelList();
}

void Stage::SetDesignItem(int item_to)
{
	m_Trees->design_pointer.y = -50.0f;
	m_Statics->design_pointer.y = -50.0f;
	m_Fruit->design_pointer.y = -50.0f;

	current_design_item = item_to;
}



void Stage::SaveAll()
{
	if (bDesignMode == true && bDesignlevelLoaded == true)
	{
		// Save current level
		m_Statics->SaveBinary(current_level);
		m_Boxes->SaveBinary(current_level);
		m_Trees->SaveBinary(current_level);
		m_Fruit->SaveBinary(current_level);

		

	}
}

void Stage::ChangeLevel(int diff)
{
	if(bPaused==false)
	{
		//bPaused = true;
		current_level = current_level + diff;
		bLoadLevel = true;

	}
}

void Stage::ChangeLevelTo(int level_to)
{
	if (true)//(bPaused == false)
	{
		bPaused = true;
		current_level = level_to;
		SetGameStateTo(STATE_PLAYING,1.0f);
		//bLoadLevel = true;
		
	}
}


void Stage::MakeRayMap()
{

	render_delay = 2000.0f;
	bPaused = true;
	int PosX;
	int PosZ;
	int i;

	//m_phys->RemoveAllObjects(5);
	//m_Boxes->DeleteBoxes();

	//m_Level->ClearMemory();
	//m_Level->InitializeTerrain(current_level, m_phys, 0.02f, m_deviceResources->GetD3DDevice(), 0.1f);

	//m_Statics->MakeAllObjects();

	/*
	int ypos = (int)(p_Level->front_back_walls + statics[i].z);
	int xpos = (int)(p_Level->left_right_walls + 1.0f + statics[i].x);
	
	//p_Level->stack_height[ypos][xpos] = 10.0f;
	float hh = p_Level->stack_height[ypos][xpos];
	if (p_Level->stack_height[ypos][xpos] < statics[i].y)
	{
		p_Level->stack_height[ypos][xpos] = statics[i].y + 5.0f;
	}
	*/

	btVector3 lightDirection = btVector3(m_Lights->m_lightBufferData.lightDirection.x, m_Lights->m_lightBufferData.lightDirection.y, m_Lights->m_lightBufferData.lightDirection.z);
	lightDirection = lightDirection.normalize();
	btVector3 treelightDirection = btVector3(m_Lights->m_lightBufferData.lightDirection.x, m_Lights->m_lightBufferData.lightDirection.y, m_Lights->m_lightBufferData.lightDirection.z);
	treelightDirection = treelightDirection.normalize();

	lightDirection.setX(-lightDirection.getX());
	lightDirection.setZ(-lightDirection.getZ());

	m_Level->ResetStack();
	//exit(0);
	for (int x = 0; x < (m_Level->total_x_points*2)-2; x++)
	{
		for (int z = 0; z < (m_Level->total_y_points*2)-2; z++)
		{

			PosX = -((float)m_Level->left_right_walls) - 1.0f + ((float)x);
			PosZ = -m_Level->front_back_walls - 1.0f + ((float)z) ;

			for (int i = 0; i < m_Statics->m_maxParticles;i++)
			{
				if(m_Statics->statics[i].bActive==true)
				{
					int ob_type = m_Statics->statics[i].type;
					if (ob_type == 2)
						ob_type = 1;
					// check if in range
					btVector3 check_pos = btVector3(PosX, m_Statics->statics[i].y, PosZ);
					btVector3 sta_pos = btVector3(m_Statics->statics[i].x, m_Statics->statics[i].y, m_Statics->statics[i].z);

					if (check_pos.distance(sta_pos) < m_Statics->static_model[ob_type].m_alpha_model->furthest_point)
					{

						float dist = m_Statics->static_model[ob_type].m_alpha_model->RayHit(PosX, 30.0f, PosZ, 0.0f, -1.0f, 0.0f, m_Statics->statics[i].model_matrix);
						if (dist < 999.0f)
						{
							
							dist = (30.0f - dist);
							
							if (dist > m_Level->stack_height[z][x])
							{
								m_Level->stack_height[z][x] = dist;
								m_Level->stack_normals[z][x].x = m_Statics->static_model[ob_type].m_alpha_model->ray_normal.getX();
								m_Level->stack_normals[z][x].y = m_Statics->static_model[ob_type].m_alpha_model->ray_normal.getY();
								m_Level->stack_normals[z][x].z = m_Statics->static_model[ob_type].m_alpha_model->ray_normal.getZ();
							}
						}
					}
				}
			}
		}
	}
	m_Level->CalculateStackNormals();
	m_Level->SaveRayBinary(current_level);


	//m_Level->ResetShadows();


	float solid_weight = (0.08f*(diffuse_average));
	float alpha_weight = (0.06f*(diffuse_average));



	for (int x = 0; x < (m_Level->total_x_points * 2) - 2; x++)
	{
		for (int z = 0; z < (m_Level->total_y_points * 2) - 2; z++)
		{
			m_Level->shadows[(z) / 2][(x) / 2] = 0.0f;
			PosX = -((float)m_Level->left_right_walls) - 1.0f + ((float)x);
			PosZ = -m_Level->front_back_walls - 1.0f + ((float)z);

			for (int i = 0; i < m_Statics->m_maxParticles; i++)
			{
				if (m_Statics->statics[i].bActive == true)
				{
					int ob_type = m_Statics->statics[i].type;
					if (ob_type == 2)
						ob_type = 1;
					// check if in range
					btVector3 check_pos = btVector3(PosX, m_Level->stack_height[z][x], PosZ);
					btVector3 sta_pos = btVector3(m_Statics->statics[i].x, m_Statics->statics[i].y, m_Statics->statics[i].z);

					if (check_pos.distance(sta_pos) < 10.0f)
					{


						float dist = m_Statics->static_model[ob_type].m_alpha_model->RayHit(PosX+2.0f-((lightDirection.getX()*(m_Level->stack_height[z][x] + 30.0f))*(1.0f-lightDirection.getY())), m_Level->stack_height[z][x]+30.0f, PosZ +1.0f - ((lightDirection.getZ()*(m_Level->stack_height[z][x] + 30.0f))*(1.0f - lightDirection.getY())), lightDirection.getX(), lightDirection.getY(), lightDirection.getZ(), m_Statics->statics[i].model_matrix);
						//float dist = m_Statics->static_model[ob_type].m_alpha_model->RayHit(PosX+2.0f, 30.0f, PosZ+1.0f, 0.0f, -1.0f, 0.0f, m_Statics->statics[i].model_matrix);
						if (dist < 999.0f)
						{
							m_Level->shadows[(z) / 2][(x) / 2] -=solid_weight;
							/*
							dist = (30.0f - dist);

							if (dist > m_Level->stack_height[z][x])
							{
								m_Level->stack_height[z][x] = dist;
							}
							*/

						}
					}
				}
			}


			//lightDirection.setX(-lightDirection.getX());
			//lightDirection.setZ(-lightDirection.getZ());


			if (true)
			{


				for (int i = 0; i < m_Trees->m_maxParticles; i++)
				{
					if (m_Trees->tree[i].bActive == true)
					{
						// check if in range
						btVector3 check_pos = btVector3(PosX, m_Level->stack_height[z][x], PosZ);
						btVector3 sta_pos = btVector3(m_Trees->tree[i].x, m_Trees->tree[i].y, m_Trees->tree[i].z);

						if (check_pos.distance(sta_pos) < 10.0f)
						{
							m_Trees->tree[i].model_matrix._14 = m_Trees->tree[i].x;
							m_Trees->tree[i].model_matrix._24 = m_Trees->tree[i].y;
							m_Trees->tree[i].model_matrix._34 = m_Trees->tree[i].z;

							float dist_trunk = 999.0f;
							float dist_leaves = 999.0f;
							//float dist = m_Statics->static_model[ob_type].m_alpha_model->RayHit(PosX + 2.0f - ((lightDirection.getX()*(m_Level->stack_height[z][x] + 30.0f))*(1.0f - lightDirection.getY())), m_Level->stack_height[z][x] + 30.0f, PosZ + 1.0f - ((lightDirection.getZ()*(m_Level->stack_height[z][x] + 30.0f))*(1.0f - lightDirection.getY())), lightDirection.getX(), lightDirection.getY(), lightDirection.getZ(), m_Statics->statics[i].model_matrix);
							if (m_Trees->tree_model[m_Trees->tree[i].type].bTrunk == true)
							{
								dist_trunk = m_Trees->tree_model[m_Trees->tree[i].type].m_solid_model->RayHit(PosX + 2.0f - ((treelightDirection.getX()*(m_Level->stack_height[z][x] + 30.0f))*(1.0f - treelightDirection.getY())), m_Level->stack_height[z][x] + 30.0f, PosZ + 1.0f - ((treelightDirection.getZ()*(m_Level->stack_height[z][x] + 30.0f))*(1.0f - treelightDirection.getY())), treelightDirection.getX(), treelightDirection.getY(), treelightDirection.getZ(), m_Trees->tree[i].model_matrix);
							}
								
							if (m_Trees->tree_model[m_Trees->tree[i].type].bLeaves == true)
							{
								dist_leaves = m_Trees->tree_model[m_Trees->tree[i].type].m_alpha_model->RayHit(PosX + 2.0f - ((treelightDirection.getX()*(m_Level->stack_height[z][x] + 30.0f))*(1.0f - treelightDirection.getY())), m_Level->stack_height[z][x] + 30.0f, PosZ + 1.0f - ((treelightDirection.getZ()*(m_Level->stack_height[z][x] + 30.0f))*(1.0f - treelightDirection.getY())), treelightDirection.getX(), treelightDirection.getY(), treelightDirection.getZ(), m_Trees->tree[i].model_matrix);
							}

							if (dist_trunk < 999.0f)
							{
								m_Level->shadows[(z) / 2][(x) / 2] -= solid_weight;
							}
							else
							{
								if (dist_leaves < 999.0f)
								{
									m_Level->shadows[(z) / 2][(x) / 2] -= alpha_weight;
								}
							}
						}
					}
				}
			}

		}
	}

	m_Level->SaveShadowBinary(current_level);


	bPaused = false;
	render_delay = 1.0f;


}


void Stage::LoadLevel(int level)
{
	float r, g, b, a,fric=1.0f,rest=1.0f;
	int is_effect;
	char music_track[30];
	
	float noise_scale;
	diffuse_average = 0.0f;
	char info_filename[40];
	if (level == 0)
		level = 1;
	if (level > 20 || level < 1 || render_delay>1000.0f)
		return;
	render_delay = 2000.0f;
	bPaused = true;

	current_level = level;

	//m_Lights->ResetPoints();
	m_uiControl->ShowHUD();
	sprintf(info_filename, "Assets/LevelInfo/%d.txt", level);

	// LoadLevelData
	FILE * pFile;

	bLevelComplete = false;


	leveltime = 120.0f;
	startleveltime = 5.0f;
	
	pFile = fopen(info_filename, "r");

	rewind(pFile);
	fscanf(pFile, "%f %f %f %f\n", &r, &g, &b, &a);
	m_Lights->m_lightBufferData.ambientColor = XMFLOAT4(r, g, b, a);
	ambient_average = (r + g + b) / 3.0f;

	fscanf(pFile, "%f %f %f %f\n", &r, &g, &b, &a);
	m_Lights->m_lightBufferData.diffuseColor = XMFLOAT4(r, g, b, a);
	diffuse_average = (r + g + b) / 3.0f;

	fscanf(pFile, "%f %f %f\n", &r, &g, &b);
	m_Lights->m_lightBufferData.lightDirection = XMFLOAT3(r, g, b);
	fscanf(pFile, "%f %f %f %f\n", &r, &g, &b, &a);
	m_Lights->m_lightBufferData.specularColor = XMFLOAT4(r, g, b, a);
	fscanf(pFile, "%f %f %f %f\n", &r, &g, &b, &a);
	m_Lights->m_lightBufferData.fogColor = XMFLOAT4(r, g, b, a);
	fscanf(pFile, "%f\n", &r);
	m_Lights->m_lightBufferData.specularPower = r;
	specular_power = r;
	fscanf(pFile, "%d\n", &is_effect);
	if (is_effect == 1) { bRain = true; }else { bRain = false; }	
	fscanf(pFile, "%d\n", &is_effect);
	if (is_effect == 1) { bSnow = true; }else { bSnow = false; }

	fscanf(pFile, "%f\n", &fric);
	fscanf(pFile, "%f\n", &rest);
	fscanf(pFile, "%d\n", &b_headlights);
	fscanf(pFile, "%f\n", &leveltime);

	fscanf(pFile, "%s\n", m_Sky->skybox);
	//fscanf(pFile, "%s\n", m_Level->ground1);
	fscanf(pFile, "%s\n", m_Level->ground1);
	fscanf(pFile, "%s\n", m_Level->ground2);

	fscanf(pFile, "%s\n", music_track);

	fscanf(pFile, "%f\n", &noise_scale);

	fscanf(pFile, "%d\n", &season);

	m_Level->noise_scale = noise_scale;

	m_Trees->season = season;

	//fscanf(pFile, "%d\n", &iMusicTrack);

	fclose(pFile);


	if (bMusic == true)
	{
		m_audio.SetTrack(music_track);
		if (game_settings.music == 1)
		{
			m_audio.Start();
		}
	}

	//m_Sky->ReleaseTexture();


	m_phys->RemoveAllObjects(5+ m_Statics->total_static_physical+m_Trees->total_trees_physical);
	m_Boxes->DeleteBoxes();

	m_Level->ClearMemory();
	m_Level->SetLevelLocation(current_level, iLoadLocal);
	m_Level->InitializeTerrain(current_level,0.02f, 0.1f);




	m_Statics->LoadLevel(current_level, iLoadLocal);
	//m_Statics->ScatterPhysical();

	m_Trees->LoadLevel(current_level, iLoadLocal);

	m_Level->MakePhysics();
	m_Level->SetFriction(fric);
	m_Level->SetRestitution(rest);


	current_level = level;

//	if (bLevelLoaded == false)
//	{
		//bLevelLoaded = true;

//	}


	m_dots->Reset();
	

	//MakeRayMap();
	m_Level->LoadRayBinary(current_level, iLoadLocal);
	//MakeRayMap();

	//m_Level->CalculateStackNormals();





	m_Boxes->LoadLevel(current_level, iLoadLocal);
	//m_Level->LoadRayBinary(current_level, iLoadLocal);
	m_Fruit->LoadLevel(current_level, iLoadLocal);
	m_Fence->LoadLevel(current_level);

	m_Fruit->SetBeamColor(1.5f * diffuse_average, 1.5f * diffuse_average, 1.5f * diffuse_average);
	m_Rain->SetRainLigtness(diffuse_average);
	m_Splash->SetSplashLigtness(diffuse_average);

	total_rings = m_Fruit->total_rings;
	rings_collected = m_Fruit->rings_collected;

	m_uiControl->SetRings(m_Fruit->rings_collected, m_Fruit->total_rings);

	m_Sky->LoadLevel(current_level);

	m_Snow->Reset();
	m_Rain->Reset();


	m_Banana->SetPosition(0.0f, m_Level->GetTerrainHeight(0.0f, -m_Level->front_back_walls)+2.0f, -m_Level->front_back_walls+8.0f, M_PI, M_PI, M_PI);
	//m_Banana->SetPosition(0.0f, 4.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	//m_Banana->SetPosition(0.0f, 20.0f, 0.0f,0.0f,0.0f,0.0f);

	m_Banana->m_rigidbody->setLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));

	m_Banana->points = 0;
	m_Banana->rockets = 0;

	m_uiControl->SetTitle("  "+ current_level);

	m_uiControl->SetRockets(1);
	m_uiControl->SetRockets(0);

	m_uiControl->SetScore(1);
	m_uiControl->SetScore(0);

	m_camera->SetEyeAtThere(0.0f, 6.0f, -m_Level->front_back_walls, 0.0f, 10.0f, -m_Level->front_back_walls+11.0f);
	bPaused = false;
	render_delay = 1.0f;
	bLevelStarting = true;
	bDesignlevelLoaded = true;
	m_audio.SetSoundEffectVolume(SFXRollingEvent, 0.0f);
	m_audio.PlaySoundEffect(SFXRollingEvent); // buggy engine
}

float Stage::GetWorldHeight(float posx, float posz)
{
	btVector3 vec_from = btVector3(posx, 50.0f, posz);
	btVector3 vec_to = btVector3(posx, -5.0f, posz);

	btCollisionWorld::ClosestRayResultCallback RayCallback(vec_from, vec_to);

	// Perform raycast
	m_phys->m_dynamicsWorld->rayTest(vec_from, vec_to, RayCallback);

	if (RayCallback.hasHit())
	{
		return RayCallback.m_hitPointWorld.getY();
	}
	else
	{
		return -10.0f;
	}

}

void Stage::LazerTest(DirectX::XMFLOAT3* pos, DirectX::XMFLOAT3* dir, bool bClick)
{
	int i;
	bool bFound = false;
	btVector3 vpos = btVector3(pos->x, pos->y, pos->z);
	btVector3 vdir = btVector3(dir->x, dir->y, dir->z);

	btCollisionWorld::ClosestRayResultCallback RayCallback(vpos, vdir);

	/*
	m_phys->m_dynamicsWorld->rayTest(vpos, vdir, RayCallback);
	if (RayCallback.hasHit())
	{
		*pos = XMFLOAT3(RayCallback.m_hitPointWorld.getX(), RayCallback.m_hitPointWorld.getY(), RayCallback.m_hitPointWorld.getZ());
		bFound = true;
	}
	*/

	// landscape ray test
	if (bPlayerPosition == true)
	{
		vpos = m_Banana->GetRealPosition();
		//vpos.setZ(vpos.getZ() + 3.0f);
		bFound = true;
	}
	else
	{
		if(bFound!=true)
		{
			for (i = 0; i < 100; i++)
			{
				if (m_Level->GetTerrainHeight(pos->x, pos->z) > pos->y)
				{
					bFound = true;
					break;
				}
				else
				{
					pos->x = pos->x + dir->x;
					pos->y = pos->y + dir->y;
					pos->z = pos->z + dir->z;
			
				}
			}
		}
	}


	if (bFound==true && pos->x < m_Level->left_right_walls && pos->x > -m_Level->left_right_walls && pos->z < m_Level->front_back_walls && pos->z > -m_Level->front_back_walls)
	{
		float height_pos;
		int ypos = (int)(m_Level->front_back_walls + 1.0f + (int)pos->z);
		int xpos = (int)(m_Level->left_right_walls + 1.0f + (int)pos->x);
		float yaw, pitch, roll;
		btVector3 floor_norm = m_Level->RayGetNormal((int)pos->x, (int)pos->z);
		btQuaternion floor_rot = btQuaternion(floor_norm.getX(), floor_norm.getY(), floor_norm.getZ(), 0.0f);
		btMatrix3x3 rotation = btMatrix3x3(floor_rot);
		rotation.getEulerYPR(yaw, pitch, roll);

		if (bPlayerPosition == false)
		{
			if (bRayCheck == true)
			{
				height_pos = GetWorldHeight((int)pos->x, (int)pos->z);

				int ypos = (int)(m_Level->front_back_walls + (int)pos->z);
				int xpos = (int)(m_Level->left_right_walls + (int)pos->x);

				//p_Level->stack_height[ypos][xpos] = 10.0f;
				float hh = m_Level->stack_height[ypos][xpos];
				if (m_Level->stack_height[ypos][xpos] < height_pos)
				{
					m_Level->stack_height[ypos][xpos] = height_pos;
				}

				if (height_pos == -10)
				{
					height_pos = m_Level->GetTerrainHeight((int)pos->x, (int)pos->z);
				}
			}
			else
			{
				/*
				height_pos = m_Statics->GetStaticWorldHeight((int)pos->x, (int)pos->z);
				m_Level->stack_height[ypos][xpos] = height_pos;
				if (height_pos>0)
				{
					if(m_Level->stack_height[ypos][xpos]<height_pos)
					{
						m_Level->stack_height[ypos][xpos] = height_pos;
					}
					//m_Level->ray_height_map[ypos/2][xpos/2] = height_pos;
				}
				//height_pos = m_Level->RayGetTerrainHeight((int)pos->x, (int)pos->z);
				*/
			}
		}
		else
		{
			height_pos = pos->y-5.0f;
		}
		if (current_design_item < 10)
		{
			if (bRotateRay == true)
			{
				m_Fruit->SetDesignPointer((int)pos->x, m_Level->stack_height[ypos][xpos] + 0.5f, (int)pos->z, yaw + (M_PI*1.0f), pitch + (M_PI*1.5f), roll + (M_PI*1.0f), current_design_item);
			}
			else
			{
				m_Fruit->SetDesignPointer((int)pos->x, m_Level->stack_height[ypos][xpos] + 0.5f, (int)pos->z, current_design_item);
			}

		}

		if (current_design_item > 9 && current_design_item < 19)
		{
			if (bRotateRay == true)
			{
				m_Trees->SetDesignPointer((int)pos->x, m_Level->stack_height[ypos][xpos] + 0.5f, (int)pos->z, yaw + (M_PI*1.0f), pitch + (M_PI*1.5f), roll + (M_PI*1.0f) ,current_design_item - 10);
			}
			else
			{
				m_Trees->SetDesignPointer((int)pos->x, m_Level->stack_height[ypos][xpos] + 0.5f, (int)pos->z, current_design_item - 10);
			}
		}

		if (current_design_item > 19 && current_design_item < 48)
		{
			if (bRotateRay == true)
			{
				m_Statics->SetDesignPointer((int)pos->x, m_Level->stack_height[ypos][xpos], (int)pos->z, yaw + (M_PI*1.0f), pitch + (M_PI*1.5f), roll + (M_PI*1.0f), current_design_item - 20);
			}
			else
			{
				m_Statics->SetDesignPointer((int)pos->x, m_Level->stack_height[ypos][xpos] , (int)pos->z, current_design_item - 20);
			}
		}

		if (current_design_item > 49 && current_design_item < 69)
		{
			m_Boxes->SetDesignPointer((int)pos->x, m_Level->stack_height[ypos][xpos] , (int)pos->z, current_design_item - 50);
		}

		if (current_design_item == -1)
		{
			m_Trees->SetDesignPointer((int)pos->x, m_Level->stack_height[ypos][xpos] + 0.5f, (int)pos->z, 0);
			m_Statics->SetDesignPointer((int)pos->x, m_Level->stack_height[ypos][xpos] + 0.5f, (int)pos->z, 0);
			m_Boxes->SetDesignPointer((int)pos->x, m_Level->stack_height[ypos][xpos] + 0.5f, (int)pos->z, 0);
		}

		if (bClick == true)
		{
			if (bClicked == false)
			{
				if (bContinualPlace == false)
				{
					bClicked = true;
				}
				else
				{
					bClicked = false;
				}
				if (current_design_item < 10)
				{
					if (bPlayerPosition == true)
					{
						XMFLOAT3X3 player_mat;
						//btVector3 col = m_Banana->rot_mat.getColumn(0);
						player_mat._11 = m_Banana->m_constantBufferData.model._11;
						player_mat._12 = m_Banana->m_constantBufferData.model._12;
						player_mat._13 = m_Banana->m_constantBufferData.model._13;
						//col = m_Banana->rot_mat.getColumn(1);
						player_mat._21 = m_Banana->m_constantBufferData.model._21;
						player_mat._22 = m_Banana->m_constantBufferData.model._22;
						player_mat._23 = m_Banana->m_constantBufferData.model._23;
						//col = m_Banana->rot_mat.getColumn(2);
						player_mat._31 = m_Banana->m_constantBufferData.model._31;
						player_mat._32 = m_Banana->m_constantBufferData.model._32;
						player_mat._33 = m_Banana->m_constantBufferData.model._33;


						//XMFLOAT4X4 player_render_mat;

						//XMStoreFloat4x4(&player_render_mat, m_Banana->m_constantBufferData.model);

						XMMATRIX rot_mat = XMLoadFloat3x3(&player_mat);

						m_Fruit->CreateOneMatrix((int)m_Banana->m_constantBufferData.model._14, m_Banana->m_constantBufferData.model._24, (int)m_Banana->m_constantBufferData.model._34, current_design_item, &rot_mat);
					}
					else
					{
						m_Fruit->CreateOne((int)pos->x, m_Level->stack_height[ypos][xpos] + 0.5f, (int)pos->z, current_design_item);
					}
				}
				if (current_design_item > 9 && current_design_item < 19)
				{
					m_Trees->CreateOne((int)pos->x, m_Level->stack_height[ypos][xpos] + 0.5f, (int)pos->z, current_design_item - 10);
				}

				if (current_design_item > 19 && current_design_item < 48)
				{
					m_Statics->CreateOne((int)pos->x, m_Level->stack_height[ypos][xpos], (int)pos->z, current_design_item - 20);
				}

				if (current_design_item > 49 && current_design_item < 69)
				{
					m_Boxes->CreateOne((int)pos->x, m_Level->stack_height[ypos][xpos], (int)pos->z, current_design_item - 50);
				}

				if (current_design_item == -1)
				{
					m_Trees->CreateOne((int)pos->x, m_Level->stack_height[ypos][xpos] + 0.5f, (int)pos->z, current_design_item);
					m_Statics->CreateOne((int)pos->x, m_Level->stack_height[ypos][xpos] + 0.5f, (int)pos->z, current_design_item);
					m_Boxes->CreateOne((int)pos->x, m_Level->stack_height[ypos][xpos] + 0.5f, (int)pos->z, current_design_item);
				}
			}
		}
		else
		{
			bClicked = false;
		}
	}

/*

	// Perform raycast
	m_phys->m_dynamicsWorld->rayTest(vpos, vdir, RayCallback);

	if (RayCallback.hasHit())
	{
		//RayCallback.m_hitNormalWorld.

		//End = RayCallback.m_hitPointWorld;
		//Normal = RayCallback.m_hitNormalWorld;

		//m_shoot->spark[i].mom = RayCallback.m_hitNormalWorld * m_shoot->spark[i].mom.length();

		// 3D Vector reflection
		//
		//
		//m_shoot->arrow[i].mom = -(2.0f*(m_shoot->arrow[i].mom.dot(RayCallback.m_hitNormalWorld))*RayCallback.m_hitNormalWorld - m_shoot->arrow[i].mom);


		//XMMatrixReflect()
		//m_shoot->arrow[i].bActive = 0;
		m_Fruit->fruit[0].x = (int)RayCallback.m_hitPointWorld.getX();
		m_Fruit->fruit[0].y = RayCallback.m_hitPointWorld.getY()+0.5f;
		m_Fruit->fruit[0].z = (int)RayCallback.m_hitPointWorld.getZ();

		//m_Boxes->ExplodeArea(RayCallback.m_hitPointWorld.getX(), RayCallback.m_hitPointWorld.getY(), RayCallback.m_hitPointWorld.getZ(), 8.0f, 1.5f);
		//m_Explode->CreateOne(RayCallback.m_hitPointWorld.getX(), RayCallback.m_hitPointWorld.getY(), RayCallback.m_hitPointWorld.getZ());

		//exit(1);
	}
	*/
}

void Stage::Update(DirectX::XMFLOAT4X4 *viewMatrix, float timeDelta, float timeTotal,float fps)
{
	int i;



	if (render_delay > 0.0f)
	{
		render_delay -= timeDelta*2.0f;

	}

	if (bLoadLevel == true)
	{
		bLoadLevel = false;
		LoadLevel(current_level);
		return ;
	}
	
	if (bPaused == true)
		return;

	
	
	if (m_Banana->m_rigidbody->getWorldTransform().getOrigin().getZ() > m_Level->front_back_walls - 13.0f && bLevelComplete == false && m_Level->fin_state==1)
	{
		bLevelComplete = true;
		level_complete_event_timer = 10.0f;
		finish_delay = 1.0f;
		m_uiControl->HideHUD();
		m_Level->fin_state = 2;
	}



	if (( bLevelComplete==true || bTimedOut==true ) && level_complete_event_timer > 0.0f)
	{
		level_complete_event_timer -= timeDelta;

		if (bTimedOut == true)
		{

		}
		else
		{
			if (level_complete_event_timer < 9.0f)
			{
				if (leveltime > 0)
				{
					leveltime--;
					m_Banana->points += 4;
				}
				m_uiControl->SetCountParameters(1.0f, 1.0f, true);

				if(m_Banana->points<level_data[current_level].high_score)
				{
					m_uiControl->SetCountDownScore(m_Banana->points,0);
				}
				else
				{
					// play a sound
					m_uiControl->SetCountDownScore(m_Banana->points, 1);
				}

			}

			if (level_complete_event_timer < 5.0f)
			{
				if (m_Banana->rockets>0)
				{
					m_Banana->rockets--;
					m_Banana->points += 50;
				}
			}

			if (level_complete_event_timer < 2.0f)
			{
				if (leveltime > 0)
				{
					m_Banana->points += 10 * leveltime;
					leveltime = 0.0f;
				}

				if (m_Banana->rockets > 0)
				{
					m_Banana->points += 50 * m_Banana->rockets;
					m_Banana->rockets = 0;
				}
			}
		}


		if (level_complete_event_timer < 1.0f && bDesignMode==false)
		{
			bLoadLevel = true;

			level_complete_event_timer = 0;

			if (m_Banana->points>level_data[current_level - 1].high_score)
			{
				level_data[current_level - 1].high_score = m_Banana->points;
			}

			m_Banana->points = 0;
			
			if (((bAppInTrialMode == true && current_level == 3) || (current_level == 13 )) && bTimedOut == false )
			{
				level_data[current_level].bEnabled = true;
				SetLevelUI();
				SavePlayerScores();
				m_uiControl->SetPause();
			}
			else
			{
				if (bTimedOut == false)
				{
					current_level = current_level + 1;
					level_data[current_level - 1].bEnabled = true;
					SetLevelUI();
					SavePlayerScores();
				}
				bTimedOut = false;
				bPaused = true;
				ChangeLevelTo(current_level);
				m_audio.StopSoundEffect(SFXRollingEvent);
			}
			//m_uiControl->ForcePause(true);

		}

	}

	if (finish_delay > 0.0f)
	{
		finish_delay -= timeDelta;
		if (finish_delay > 1.0f)
		{

		}
		else
		{
			if (bRespot == true)
			{
				Respot();
				bRespot = false;
			}
			finish_delay += timeDelta*0.3f; // slightly slower
			if(bTimedOut==true)
			{
				m_uiControl->SetCountDown("TIMEOUT");
				m_uiControl->SetCountParameters(0.6f - (((finish_delay - (float)((int)finish_delay))*0.4f)) + 0.6f, finish_delay - (float)((int)finish_delay), true);
			}
			else
			{
				m_uiControl->SetCountDown("FINISH");
				m_uiControl->SetCountParameters(0.6f - (((finish_delay - (float)((int)finish_delay))*0.4f)) + 0.6f, finish_delay - (float)((int)finish_delay), true);

			}
		}
	}

	if (m_Banana->down_offset.getY() > 0.0f && respot_delay <= 0.0f)
	{
		respot_wait += timeDelta;
		if(respot_wait>2.0f)
			DoRespot();
	}
	else
	{
		respot_wait = 0.0f;
	}


	if (respot_delay > 0.0f)
	{
		respot_delay -= timeDelta;
		if (respot_delay > 0.0f)
		{
			if (respot_delay > 1.0f)
			{
				m_uiControl->SetCountDown((int)respot_delay);
				m_uiControl->SetCountParameters(((respot_delay - (float)((int)respot_delay))*0.4f) + 0.6f, respot_delay - (float)((int)respot_delay), true);
				//m_Banana->SetCarPosition(0.0f, m_Level->GetTerrainHeight(0.0f, -m_Level->front_back_walls) + 2.0f, -m_Level->front_back_walls + 8.0f, M_PI, M_PI, M_PI);
				//m_Banana->m_rigidbody->setLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));
			}
			else
			{
				if (bRespot == true)
				{
					Respot();
					bRespot = false;
				}
				respot_delay += timeDelta*0.3f; // slightly slower
				m_uiControl->SetCountDown("RESPOT");
				m_uiControl->SetCountParameters(0.6f - (((respot_delay - (float)((int)respot_delay))*0.4f)) + 0.6f, respot_delay - (float)((int)respot_delay), true);
				
			}
		}
		else
		{
			//Respot();
			m_uiControl->SetCountParameters(0.0f, 0.0f, false);
		}
	}

	if (bLevelComplete == false && startleveltime > 0.0f)
	{
		startleveltime -= timeDelta;
		if (startleveltime > 0.0f)
		{
			if (startleveltime > 1.0f)
			{
				m_uiControl->SetCountDown((int)startleveltime);
				m_uiControl->SetCountParameters(((startleveltime - (float)((int)startleveltime))*0.4f) + 0.6f, startleveltime - (float)((int)startleveltime), true);
				m_Banana->SetCarPosition(0.0f, m_Level->GetTerrainHeight(0.0f, -m_Level->front_back_walls) + 2.0f, -m_Level->front_back_walls + 8.0f, M_PI, M_PI, M_PI);
				m_Banana->m_rigidbody->setLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));
			}
			else
			{
				startleveltime += timeDelta*0.3f; // slightly slower
				m_uiControl->SetCountDown("GO!");
				m_uiControl->SetCountParameters(1.0f-(((startleveltime - (float)((int)startleveltime))*0.4f)) + 0.6f, startleveltime - (float)((int)startleveltime), true);
				if (game_settings.shader_mode == 1 && fps < 15)
				{
					game_settings.shader_mode = 0;
					SaveSettings();
				}
			}
		}
		else
		{
			m_uiControl->SetCountParameters(0.0f, 0.0f, false);
		}
	}
	else
	{
		if(bLevelComplete == false)
		{

			if (bLevelComplete == false && bTimedOut==false && leveltime < 0.0f)
			{
				leveltime = 0.0f;
				bTimedOut = true;
				finish_delay = 2.0f;
				level_complete_event_timer = 2.0f;
			}
			else
			{
				if (leveltime < 5.0f)
				{
					if (leveltime < 1.0f)
					{
						m_uiControl->SetCountDown("00:00");
						m_uiControl->SetCountParameters(((leveltime - (float)((int)leveltime))*0.4f) + 0.6f, leveltime - (float)((int)leveltime), true);
					}
					else
					{
						m_uiControl->SetCountDown((int)leveltime);
						m_uiControl->SetCountParameters(((leveltime - (float)((int)leveltime))*0.4f) + 0.6f, leveltime - (float)((int)leveltime), true);
					}
				}

				if (leveltime > 0.0f)
				{
					if (leveltime < 1.0f)
					{
						leveltime -= timeDelta*0.3f;
					}
					else
					{
						leveltime -= timeDelta;
					}
				}
			}
		}
	}

	if(rings_collected!= m_Fruit->rings_collected)
	{
		m_uiControl->SetRings(m_Fruit->rings_collected, m_Fruit->total_rings);
		if (m_Fruit->rings_collected == m_Fruit->total_rings && m_Level->fin_state!=2)
		{
			m_Level->fin_state = 1;
		}
	}

	
	m_uiControl->SetTimeFloat(leveltime);

	if (bSnow == true) { m_Snow->Update(timeTotal, timeDelta); };
	if (bRain == true) { m_Rain->Update(timeTotal, timeDelta); };

	m_Points->Update(timeTotal, timeDelta);
	m_Splash->Update(timeTotal, timeDelta);
	m_shoot->Update(timeTotal, timeDelta);

	m_dots->Update(timeTotal, timeDelta);

	m_Cash->Update(timeTotal, timeDelta);
	m_uiControl->SetScore(m_Banana->points);
	m_uiControl->SetRockets(m_Banana->rockets);
	m_Banana->Update(timeTotal, timeDelta);
	m_Explode->Update(timeTotal, timeDelta);

	m_Trees->UpdateGrass();

	m_Fruit->Update(timeDelta);

	m_Boxes->Update();

	// keep camera above terrain
	float height = m_Level->GetTerrainHeight(m_camera->GetX(), m_camera->GetZ()) + 3.0f;
	m_camera->CheckTerrain(height);

	if (m_Banana->DoShoot() == true)
	{
		//float vel = m_Banana->m_rigidbody->getLinearVelocity().length();
		if (m_Banana->rockets > 0)
		{
			m_shoot->CreateRocket(m_Banana->player_x + m_Banana->x_offset1.getX(), m_Banana->player_y + m_Banana->x_offset1.getY(), m_Banana->player_z + m_Banana->x_offset1.getZ(), m_Banana->car_point.getX()*0.9f, m_Banana->car_point.getY()*0.9f, m_Banana->car_point.getZ()*0.9f, 1, m_Banana->point_martix, m_Banana->car_point.getX(), m_Banana->car_point.getY(), m_Banana->car_point.getZ());
			m_shoot->CreateRocket(m_Banana->player_x + m_Banana->x_offset2.getX(), m_Banana->player_y + m_Banana->x_offset2.getY(), m_Banana->player_z + m_Banana->x_offset2.getZ(), m_Banana->car_point.getX()*0.9f, m_Banana->car_point.getY()*0.9f, m_Banana->car_point.getZ()*0.9f, 1, m_Banana->point_martix, m_Banana->car_point.getX(), m_Banana->car_point.getY(), m_Banana->car_point.getZ());
			m_Banana->b_show_rockets = 18;  // 8 frames without showing rokets
			m_Banana->rockets--;
			m_audio.PlaySoundEffect(SFXCollisionEvent);
		}
	}


	for (i = 0; i < m_shoot->m_maxArrows; i++)
	{

		if (m_shoot->arrow[i].bActive == 1)
		{
			if (m_shoot->arrow[i].bCollide == 0)
			{
				if (m_shoot->arrow[i].pos.distance(*m_Banana->GetPosition())>0.5f)
				{
					m_shoot->arrow[i].bCollide = 1;
				}
			}
			else
			{
				btCollisionWorld::ClosestRayResultCallback RayCallback(m_shoot->arrow[i].pos - (m_shoot->arrow[i].dir*1.0f), m_shoot->arrow[i].pos + (m_shoot->arrow[i].dir*1.0f));

				// Perform raycast
				m_phys->m_dynamicsWorld->rayTest(m_shoot->arrow[i].pos - (m_shoot->arrow[i].dir*1.0f), m_shoot->arrow[i].pos + (m_shoot->arrow[i].dir*1.0f), RayCallback);

				if (RayCallback.hasHit())
				{
					//RayCallback.m_hitNormalWorld.
					/*
					End = RayCallback.m_hitPointWorld;
					Normal = RayCallback.m_hitNormalWorld;rt
					*/
					//m_shoot->spark[i].mom = RayCallback.m_hitNormalWorld * m_shoot->spark[i].mom.length();

					// 3D Vector reflection
					//
					//
					//m_shoot->arrow[i].mom = -(2.0f*(m_shoot->arrow[i].mom.dot(RayCallback.m_hitNormalWorld))*RayCallback.m_hitNormalWorld - m_shoot->arrow[i].mom);


					//XMMatrixReflect()
					m_shoot->arrow[i].bActive = 0;
					m_Boxes->ExplodeArea(RayCallback.m_hitPointWorld.getX(), RayCallback.m_hitPointWorld.getY(), RayCallback.m_hitPointWorld.getZ(), 8.0f, 5.5f);
					m_Explode->CreateOne(RayCallback.m_hitPointWorld.getX(), RayCallback.m_hitPointWorld.getY(), RayCallback.m_hitPointWorld.getZ());
					m_audio.PlaySoundEffect(SFXCheckpointEvent);
					//exit(1);
				}
			}
		}
	}

	m_Lights->ResetPoints();

	for (i = 0; i < m_Explode->total_lights;i++)
	{
		m_Lights->Addpoint(XMFLOAT4(0.55f, 0.55f, 0.55f, 1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), XMFLOAT3(m_Explode->light[i].x, m_Explode->light[i].y, m_Explode->light[i].z), (1.0f- m_Explode->light[i].strength)*12.0f);
	}
	
	//if (car_point.getY() < 0.0f)
	//{
	//	m_Banana->WorldObject::SetPosition(ball_tran.getOrigin().getX(), m_Level->GetTerrainHeight(0.0f, -m_Level->front_back_walls) + 15.0f, ball_tran.getOrigin().getZ(), M_PI, M_PI, M_PI);
	//}

//	return when_all(tasks.begin(), tasks.end());


}

void Stage::DoRespot()
{
	if (!(startleveltime > 0.0f))
	{
		bRespot = true;
		respot_delay = 3.0f;
	}
}
void Stage::Respot()
{
	if (bPaused == false)
	{
		if (bDesignMode == true)
		{
			btTransform ball_tran = m_Banana->m_rigidbody->getWorldTransform();

			m_Banana->SetCarPosition(ball_tran.getOrigin().getX(), m_Level->GetTerrainHeight(ball_tran.getOrigin().getX(), ball_tran.getOrigin().getZ()) + 5.0f, ball_tran.getOrigin().getZ(), M_PI, M_PI, M_PI);
		}
		else
		{
			m_Banana->SetPosition(0.0f, m_Level->GetTerrainHeight(0.0f, -m_Level->front_back_walls) + 2.0f, -m_Level->front_back_walls + 8.0f, M_PI, M_PI, M_PI);

			m_Banana->m_rigidbody->setLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));
		}
	}
}

void Stage::Render(ComPtr<ID3D11Buffer> constantBuffer, ModelViewProjectionConstantBuffer* constantBufferData)
{
	m_audio.Render();


	if (current_game_state == STATE_START)
	{
		if (start_render_delay > 0.0f)
		{
			if(bAllLoaded==true)
				start_render_delay -= 0.05f;
		}
		else
		{
			m_deviceResources->GetD3DDeviceContext()->OMSetBlendState(m_alphaDisableBlendingState, blendFactor, 0xffffffff);
			m_deviceResources->GetD3DDeviceContext()->OMSetDepthStencilState(NULL, 0);

			if (game_settings.shader_mode == 1)
				SetShinyShader();
			else
				SetModelShader();

			m_Lights->SetLights1();
			m_camera->SetEyeAtThere(0.0f, 30.0f, 0.0f, -3.1f, 3.0f, 0.0f);
			XMStoreFloat4x4(&constantBufferData->view, *m_camera->GetMatrix());

			//m_Lights->Addpoint(XMFLOAT4(0.55f, 0.0f, 0.0f, 1.0f), XMFLOAT4(0.55f, 0.0f, 0.55f, 1.0f), XMFLOAT4(0.55f, 0.55f, 0.55f, 1.0f), XMFLOAT3(0.0f,5.0f,0.0f),8.0f);

			m_Lights->UpdateBuffer(m_lightBuffer);
			noise_z += 0.01f;

			
			m_Flag->UpdateGeneratedTerrain(1, noise_z);
			m_Flag->Render(constantBuffer, constantBufferData);
			return;
		}
	}


	if(render_delay > 0.0f)
	{
		if (render_delay < 1.0f && bLevelStarting==true)
		{
			bLevelStarting = false;
			bFadeIn = true;
			fade_in_to_delay = fade_in_to_val = 2.0f;
		}

		return;
	}


	UpdateCameraBuffer();

	m_deviceResources->GetD3DDeviceContext()->OMSetBlendState(m_alphaDisableBlendingState, blendFactor, 0xffffffff);
	m_deviceResources->GetD3DDeviceContext()->OMSetDepthStencilState(NULL, 0);

	if(game_settings.shader_mode==1)
		SetShinyShader();
	else
		SetModelShader();

	//m_Lights->SetLights1();
	m_Lights->SetSpecular(specular_power);

	//m_Lights->SetFog(DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f));

	if(b_headlights==1 && bPaused==false)
	{
		m_Lights->AddSpot(XMFLOAT4(0.55f, 0.0f, 0.0f, 1.0f), XMFLOAT4(0.55f, 0.0f, 0.55f, 1.0f), XMFLOAT4(0.55f, 0.55f, 0.55f, 1.0f), XMFLOAT3(m_Banana->GetPosition()->getX()+m_Banana->y_offset.getX(), m_Banana->GetPosition()->getY() + m_Banana->y_offset.getY(), m_Banana->GetPosition()->getZ() + m_Banana->y_offset.getZ()), XMFLOAT3(m_Banana->car_point.getX(), m_Banana->car_point.getY(), m_Banana->car_point.getZ()), 7.0f, 8.0f);
	}
	/*
	m_Lights->Addpoint(XMFLOAT4(0.55f, 0.55f, 0.55f, 1.0f), XMFLOAT4(0.55f, 1.0f, 0.55f, 1.0f), XMFLOAT4(0.55f, 0.55f, 0.55f, 1.0f), XMFLOAT3(m_Banana->GetPosition()->getX() + 5.0f, m_Banana->GetPosition()->getY() + 5.0f, m_Banana->GetPosition()->getZ() + 5.0f), 4.0f);
	m_Lights->Addpoint(XMFLOAT4(0.55f, 0.55f, 0.55f, 1.0f), XMFLOAT4(0.55f, 1.0f, 0.55f, 1.0f), XMFLOAT4(0.55f, 0.55f, 0.55f, 1.0f), XMFLOAT3(m_Banana->GetPosition()->getX() - 5.0f, m_Banana->GetPosition()->getY(), m_Banana->GetPosition()->getZ()), 4.0f);
	m_Lights->Addpoint(XMFLOAT4(0.55f, 0.55f, 0.55f, 1.0f), XMFLOAT4(0.55f, 1.0f, 0.55f, 1.0f), XMFLOAT4(0.55f, 0.55f, 0.55f, 1.0f), XMFLOAT3(m_Banana->GetPosition()->getX(), m_Banana->GetPosition()->getY(), m_Banana->GetPosition()->getZ() + 5.0f), 4.0f);
	m_Lights->Addpoint(XMFLOAT4(0.55f, 0.55f, 0.55f, 1.0f), XMFLOAT4(0.55f, 1.0f, 0.55f, 1.0f), XMFLOAT4(0.55f, 0.55f, 0.55f, 1.0f), XMFLOAT3(m_Banana->GetPosition()->getX() + 5.0f, m_Banana->GetPosition()->getY(), m_Banana->GetPosition()->getZ()), 4.0f);
	m_Lights->Addpoint(XMFLOAT4(0.55f, 0.55f, 0.55f, 1.0f), XMFLOAT4(0.55f, 1.0f, 0.55f, 1.0f), XMFLOAT4(0.55f, 0.55f, 0.55f, 1.0f), XMFLOAT3(m_Banana->GetPosition()->getX() - 5.0f, m_Banana->GetPosition()->getY(), m_Banana->GetPosition()->getZ() + 5.0f), 4.0f);
	m_Lights->Addpoint(XMFLOAT4(0.55f, 0.55f, 0.55f, 1.0f), XMFLOAT4(0.55f, 1.0f, 0.55f, 1.0f), XMFLOAT4(0.55f, 0.55f, 0.55f, 1.0f), XMFLOAT3(m_Banana->GetPosition()->getX(), m_Banana->GetPosition()->getY(), m_Banana->GetPosition()->getZ() + 5.0f), 4.0f);
	m_Lights->Addpoint(XMFLOAT4(0.55f, 0.55f, 0.55f, 1.0f), XMFLOAT4(0.55f, 1.0f, 0.55f, 1.0f), XMFLOAT4(0.55f, 0.55f, 0.55f, 1.0f), XMFLOAT3(m_Banana->GetPosition()->getX(), m_Banana->GetPosition()->getY(), m_Banana->GetPosition()->getZ() + 10.0f), 4.0f);
	*/

	//m_Lights->SetFog(XMFLOAT4(0.55f, 0.55f, 0.55f, 0.0f)); // alpha zero is no fog

	m_Lights->UpdateBuffer(m_lightBuffer);

	m_Boxes->Render(m_deviceResources->GetD3DDeviceContext(), constantBuffer, constantBufferData,false);
	m_Fruit->Render(m_deviceResources->GetD3DDeviceContext(), constantBuffer, constantBufferData);

	m_Cash->Render(m_deviceResources->GetD3DDeviceContext(), constantBuffer, constantBufferData);

	m_Banana->Render(m_deviceResources->GetD3DDeviceContext(), constantBuffer, constantBufferData);
	//SetModelShader();
	
	m_Trees->Render(m_deviceResources->GetD3DDeviceContext(), constantBuffer, constantBufferData, 0);
	m_Statics->Render(m_deviceResources->GetD3DDeviceContext(), constantBuffer, constantBufferData, 0);

	m_shoot->RenderRockets(m_deviceResources->GetD3DDeviceContext(), constantBuffer, constantBufferData);

	m_Lights->SetSpecular(0.0f);
	m_Lights->SetNumTextures(2);
	m_Lights->UpdateBuffer(m_lightBuffer);
	
	//SetModelShader();
	m_Level->Render(m_deviceResources->GetD3DDeviceContext(), constantBuffer, constantBufferData);

	m_Lights->SetNumTextures(1);
	m_Lights->UpdateBuffer(m_lightBuffer);

	m_Sky->x = m_camera->EyeX();
	m_Sky->y = m_camera->EyeY();
	m_Sky->z = m_camera->EyeZ();
	m_Sky->Render(m_deviceResources->GetD3DDeviceContext(), constantBuffer, constantBufferData);

	if (game_settings.shader_mode == 1)
		SetShinyShader();
	else
		SetModelShader();

	m_deviceResources->GetD3DDeviceContext()->OMSetBlendState(m_alphaEnableBlendingTexAlph, blendFactor, 0xffffffff);
	m_deviceResources->GetD3DDeviceContext()->OMSetDepthStencilState(m_depthDisabledStencilState, 0);



	m_deviceResources->GetD3DDeviceContext()->RSSetState(g_pRasterState_nocull);

	m_Fence->Render(m_deviceResources->GetD3DDeviceContext(), constantBuffer, constantBufferData);
	if (bRain == true) {
		m_Rain->Render(m_deviceResources->GetD3DDeviceContext(), constantBuffer, constantBufferData);
	}
	if (bSnow == true) {
		m_Snow->Render(m_deviceResources->GetD3DDeviceContext(), constantBuffer, constantBufferData);
	}
	m_dots->Render(m_deviceResources->GetD3DDeviceContext(), constantBuffer, constantBufferData);

	SetPointShader();


	m_shoot->Render(m_deviceResources->GetD3DDeviceContext(), constantBuffer, constantBufferData);

	m_Explode->Render(m_deviceResources->GetD3DDeviceContext(), constantBuffer, constantBufferData);

	if (game_settings.shader_mode == 1)
		SetShinyShader();
	else
		SetModelShader();

	m_Lights->SetSpecular(0.0f);
	m_Lights->UpdateBuffer(m_lightBuffer);

	m_deviceResources->GetD3DDeviceContext()->OMSetDepthStencilState(m_depthDisabledStencilState, 0);
	m_Trees->RenderGrass(m_deviceResources->GetD3DDeviceContext(), constantBuffer, constantBufferData, 1);


	m_deviceResources->GetD3DDeviceContext()->OMSetDepthStencilState(NULL, 0);

	m_Trees->Render(m_deviceResources->GetD3DDeviceContext(), constantBuffer, constantBufferData, 1);




	m_deviceResources->GetD3DDeviceContext()->RSSetState(g_pRasterState_cull);

	//m_deviceResources->GetD3DDeviceContext()->OMSetDepthStencilState(NULL, 0);
	m_Statics->Render(m_deviceResources->GetD3DDeviceContext(), constantBuffer, constantBufferData, 1);
	

	//m_Points->Render(m_deviceResources->GetD3DDeviceContext(), constantBuffer, constantBufferData);
	m_Splash->Render(m_deviceResources->GetD3DDeviceContext(), constantBuffer, constantBufferData);

	m_Fruit->RenderBeam(m_deviceResources->GetD3DDeviceContext(), constantBuffer, constantBufferData);


	m_deviceResources->GetD3DDeviceContext()->OMSetBlendState(m_alphaEnableBlendingGlass, blendFactor, 0xffffffff);
	m_Boxes->Render(m_deviceResources->GetD3DDeviceContext(), constantBuffer, constantBufferData, true);
}


void Stage::UpdateCameraBuffer()
{
	//m_cameraBufferData.cameraPosition = XMFLOAT3(m_camera->EyeX(), m_camera->EyeY(), m_camera->EyeZ());
	//m_cameraBufferData.padding = 0.0f;

	//m_deviceResources->GetD3DDeviceContext()->UpdateSubresource(m_cameraBuffer.Get(), 0, NULL, &m_cameraBufferData, 0, 0);
}

void Stage::SetModelShader()
{
	m_deviceResources->GetD3DDeviceContext()->VSSetShader(
		m_model_vertexShader.Get(),
		nullptr,
		0
		);

	m_deviceResources->GetD3DDeviceContext()->IASetInputLayout(m_model_inputLayout.Get());

	m_deviceResources->GetD3DDeviceContext()->PSSetShader(
		m_model_pixelShader.Get(),
		nullptr,
		0
		);

}

void Stage::SetShinyShader()
{
	m_deviceResources->GetD3DDeviceContext()->VSSetShader(
		m_shinemodel_vertexShader.Get(),
		nullptr,
		0
		);
	m_deviceResources->GetD3DDeviceContext()->IASetInputLayout(m_shinemodel_inputLayout.Get());
	m_deviceResources->GetD3DDeviceContext()->PSSetShader(
		m_shinemodel_pixelShader.Get(),
		nullptr,
		0
		);
}

void Stage::SetPointShader()
{
	m_deviceResources->GetD3DDeviceContext()->VSSetShader(
		m_point_vertexShader.Get(),
		nullptr,
		0
		);
	m_deviceResources->GetD3DDeviceContext()->IASetInputLayout(m_point_inputLayout.Get());
	m_deviceResources->GetD3DDeviceContext()->PSSetShader(
		m_point_pixelShader.Get(),
		nullptr,
		0
		);
}

void Stage::UpdateProjectionMatrix(DirectX::XMMATRIX *projectionMatrix)
{
	//	int i;
	m_projectionMatrix = *projectionMatrix;
	SetProjectionMatrixs();
}

void Stage::SetProjectionMatrixs()
{
	//m_Banana->UpdateProjectionMatrix(&m_projectionMatrix);

}

void Stage::SetupAlphaBlendingStates()
{
	bool result;

	/*
	CD3D11_BUFFER_DESC cameraBufferDesc(sizeof(ModelViewProjectionConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
	DX::ThrowIfFailed(
		m_deviceResources->GetD3DDevice()->CreateBuffer(
			&cameraBufferDesc,
			nullptr,
			&m_cameraBuffer
			)
		);
		*/
	CD3D11_BUFFER_DESC lightBufferDesc(sizeof(LightBufferType), D3D11_BIND_CONSTANT_BUFFER);
	DX::ThrowIfFailed(
		m_deviceResources->GetD3DDevice()->CreateBuffer(
			&lightBufferDesc,
			nullptr,
			&m_lightBuffer
			)
		);


	m_deviceResources->GetD3DDeviceContext()->PSSetConstantBuffers(0, 1, m_lightBuffer.GetAddressOf());
	//m_deviceResources->GetD3DDeviceContext()->VSSetConstantBuffers(1, 1, m_cameraBuffer.GetAddressOf());

	D3D11_BLEND_DESC blendStateDescription;
	// Clear the blend state description.
	ZeroMemory(&blendStateDescription, sizeof(D3D11_BLEND_DESC));

	// Create an alpha enabled blend state description.
	blendStateDescription.RenderTarget[0].BlendEnable = TRUE;
	blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

	blendStateDescription.RenderTarget[0].RenderTargetWriteMask = 0x0f;

	result = m_deviceResources->GetD3DDevice()->CreateBlendState(&blendStateDescription, &m_alphaEnableBlendingState);

	// Create an alpha enabled blend state description.
	blendStateDescription.RenderTarget[0].BlendEnable = TRUE;
	blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;


	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	// dest.rgb = src.rgb * src.a + dest.rgb * (1 - src.a) ==> standard alpha blended color

	blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_INV_DEST_ALPHA;
	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_BLEND_FACTOR;
	blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	
	result = m_deviceResources->GetD3DDevice()->CreateBlendState(&blendStateDescription, &m_alphaEnableBlendingTexAlph);

	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC1_COLOR;
	blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

	// dest.a = src.a * (1 - dest.a) + dest.a
	//       == src.a + dest.a - src.a * dest.a
	//       == 1 - (1 - src.a) * (1 - dest.a) ==> preserves alpha to target (optional)


	result = m_deviceResources->GetD3DDevice()->CreateBlendState(&blendStateDescription, &m_alphaEnableBlendingGlass);


	// Modify the description to create an alpha disabled blend state description.
	blendStateDescription.RenderTarget[0].BlendEnable = FALSE;

	// Create the blend state using the description.
	result = m_deviceResources->GetD3DDevice()->CreateBlendState(&blendStateDescription, &m_alphaDisableBlendingState);

	D3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc;

	ZeroMemory(&depthDisabledStencilDesc, sizeof(depthDisabledStencilDesc));

	// Now create a second depth stencil state which turns off the Z buffer for 2D rendering.  The only difference is 
	// that DepthEnable is set to false, all other parameters are the same as the other depth stencil state.
	depthDisabledStencilDesc.DepthEnable = true;
	//depthDisabledStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthDisabledStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depthDisabledStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthDisabledStencilDesc.StencilEnable = false;
	depthDisabledStencilDesc.StencilReadMask = 0xFF;
	depthDisabledStencilDesc.StencilWriteMask = 0xFF;

	depthDisabledStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthDisabledStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	depthDisabledStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthDisabledStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	m_deviceResources->GetD3DDevice()->CreateDepthStencilState(&depthDisabledStencilDesc, &m_depthDisabledStencilState);
}

void Stage::LoadShadersSync()
{
	D3D11_RASTERIZER_DESC rasterizerState;
	//rasterizerState.FillMode = D3D11_FILL_WIREFRAME;
	rasterizerState.FillMode = D3D11_FILL_SOLID;
	rasterizerState.CullMode = D3D11_CULL_NONE;
	rasterizerState.FrontCounterClockwise = false;
	rasterizerState.DepthBias = false;
	rasterizerState.DepthBiasClamp = 0;
	rasterizerState.SlopeScaledDepthBias = 0;
	rasterizerState.DepthClipEnable = true;
	rasterizerState.ScissorEnable = false;
	rasterizerState.MultisampleEnable = false;
	rasterizerState.AntialiasedLineEnable = true;

	m_deviceResources->GetD3DDevice()->CreateRasterizerState(&rasterizerState, &g_pRasterState_nocull);

	rasterizerState.CullMode = D3D11_CULL_BACK;

	m_deviceResources->GetD3DDevice()->CreateRasterizerState(&rasterizerState, &g_pRasterState_cull);
	m_deviceResources->GetD3DDeviceContext()->RSSetState(g_pRasterState_cull);

	BasicLoader^ loader = ref new BasicLoader(m_deviceResources->GetD3DDevice());

	static D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, /*20*/D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXBLEND", 0, DXGI_FORMAT_R32_FLOAT, 0, /*20*/D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }, // two texture blend
		{ "SPECULA", 0, DXGI_FORMAT_R32_FLOAT, 0, /*20*/D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }, // two texture blend
	};
	loader->LoadShader("SimpleVertexShader.cso", vertexDesc, ARRAYSIZE(vertexDesc), &m_model_vertexShader, &m_model_inputLayout);
	loader->LoadShader("SimplePixelShader.cso", &m_model_pixelShader);

	static D3D11_INPUT_ELEMENT_DESC vertexDescC[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, /*20*/D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXBLEND", 0, DXGI_FORMAT_R32_FLOAT, 0, /*20*/D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }, // two texture blend
		{ "SPECULA", 0, DXGI_FORMAT_R32_FLOAT, 0, /*20*/D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }, // two texture blend
	};
	loader->LoadShader("ShinyVertexShader.cso", vertexDescC, ARRAYSIZE(vertexDescC), &m_shinemodel_vertexShader, &m_shinemodel_inputLayout);
	loader->LoadShader("ShinyPixelShader.cso", &m_shinemodel_pixelShader);

	static D3D11_INPUT_ELEMENT_DESC vertexDescB[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, /*0*/D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, /*12*/D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, /*20*/D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	loader->LoadShader("PointVertexShader.cso", vertexDescB, ARRAYSIZE(vertexDescB), &m_point_vertexShader, &m_point_inputLayout);
	loader->LoadShader("PointPixelShader.cso", &m_point_pixelShader);



}

task<void> Stage::LoadShaders()
{
	D3D11_RASTERIZER_DESC rasterizerState;
//rasterizerState.FillMode = D3D11_FILL_WIREFRAME;
	rasterizerState.FillMode = D3D11_FILL_SOLID;
	rasterizerState.CullMode = D3D11_CULL_NONE;
	rasterizerState.FrontCounterClockwise = false;
	rasterizerState.DepthBias = false;
	rasterizerState.DepthBiasClamp = 0;
	rasterizerState.SlopeScaledDepthBias = 0;
	rasterizerState.DepthClipEnable = true;
	rasterizerState.ScissorEnable = false;
	rasterizerState.MultisampleEnable = false;
	rasterizerState.AntialiasedLineEnable = true;

	m_deviceResources->GetD3DDevice()->CreateRasterizerState(&rasterizerState, &g_pRasterState_nocull);

	rasterizerState.CullMode = D3D11_CULL_BACK;

	m_deviceResources->GetD3DDevice()->CreateRasterizerState(&rasterizerState, &g_pRasterState_cull);
	m_deviceResources->GetD3DDeviceContext()->RSSetState(g_pRasterState_cull);

	BasicLoader^ loader = ref new BasicLoader(m_deviceResources->GetD3DDevice());
	
	std::vector<task<void>> tasks;
	
	static D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, /*20*/D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXBLEND", 0, DXGI_FORMAT_R32_FLOAT, 0, /*20*/D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }, // two texture blend
		{ "SPECULA", 0, DXGI_FORMAT_R32_FLOAT, 0, /*20*/D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }, // two texture blend
	};
	tasks.push_back(loader->LoadShaderAsync("SimpleVertexShader.cso", vertexDesc, ARRAYSIZE(vertexDesc), &m_model_vertexShader, &m_model_inputLayout));
	tasks.push_back(loader->LoadShaderAsync("SimplePixelShader.cso", &m_model_pixelShader));

	static D3D11_INPUT_ELEMENT_DESC vertexDescC[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, /*20*/D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXBLEND", 0, DXGI_FORMAT_R32_FLOAT, 0, /*20*/D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }, // two texture blend
		{ "SPECULA", 0, DXGI_FORMAT_R32_FLOAT, 0, /*20*/D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }, // two texture blend
	};
	tasks.push_back(loader->LoadShaderAsync("ShinyVertexShader.cso", vertexDescC, ARRAYSIZE(vertexDescC), &m_shinemodel_vertexShader, &m_shinemodel_inputLayout));
	tasks.push_back(loader->LoadShaderAsync("ShinyPixelShader.cso", &m_shinemodel_pixelShader));

	static D3D11_INPUT_ELEMENT_DESC vertexDescB[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, /*0*/D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, /*12*/D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, /*20*/D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	tasks.push_back(loader->LoadShaderAsync("PointVertexShader.cso", vertexDescB, ARRAYSIZE(vertexDescB), &m_point_vertexShader, &m_point_inputLayout));
	tasks.push_back(loader->LoadShaderAsync("PointPixelShader.cso", &m_point_pixelShader));


	/*

	//return;
	auto model_loadVSTask = DX::ReadDataAsync(L"SimpleVertexShader.cso");
	auto model_loadPSTask = DX::ReadDataAsync(L"SimplePixelShader.cso");


	auto model_createVSTask = model_loadVSTask.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateVertexShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_model_vertexShader
				)
			);

		const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateInputLayout(
				vertexDesc,
				ARRAYSIZE(vertexDesc),
				&fileData[0],
				fileData.size(),
				&m_model_inputLayout
				)
			);
	});

	auto model_createPSTask = model_loadPSTask.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreatePixelShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_model_pixelShader
				)
			);
	});

	*/

	/*
	auto shinemodel_loadVSTask = DX::ReadDataAsync(L"ShinyVertexShader.cso");
	auto shinemodel_loadPSTask = DX::ReadDataAsync(L"ShinyPixelShader.cso");


	auto shinemodel_createVSTask = shinemodel_loadVSTask.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateVertexShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_shinemodel_vertexShader
				)
			);

		const D3D11_INPUT_ELEMENT_DESC vertexDescC[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateInputLayout(
				vertexDescC,
				ARRAYSIZE(vertexDescC),
				&fileData[0],
				fileData.size(),
				&m_shinemodel_inputLayout
				)
			);
	});

	auto shinemodel_createPSTask = shinemodel_loadPSTask.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreatePixelShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_shinemodel_pixelShader
				)
			);
	});
	*/


	/*
	auto point_loadVSTask = DX::ReadDataAsync(L"PointVertexShader.cso");
	auto point_loadPSTask = DX::ReadDataAsync(L"PointPixelShader.cso");

	auto createpointVSTask = point_loadVSTask.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateVertexShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_point_vertexShader
				)
			);

		const D3D11_INPUT_ELEMENT_DESC vertexDescB[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateInputLayout(
				vertexDescB,
				ARRAYSIZE(vertexDescB),
				&fileData[0],
				fileData.size(),
				&m_point_inputLayout
				)
			);
	});


	auto createpointPSTask = point_loadPSTask.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreatePixelShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_point_pixelShader
				)
			);
	});
	*/

	/*
	D3D11_BUFFER_DESC cameraBufferDesc;
	D3D11_BUFFER_DESC lightBufferDesc;


	// Setup the description of the camera dynamic constant buffer that is in the vertex shader.
	cameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cameraBufferDesc.ByteWidth = sizeof(CameraBufferType);
	cameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cameraBufferDesc.MiscFlags = 0;
	cameraBufferDesc.StructureByteStride = 0;

	// Create the camera constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	DX::ThrowIfFailed(
	m_deviceResources->GetD3DDevice()->CreateBuffer(&cameraBufferDesc, NULL, &m_cameraBuffer));


	*/
	// Setup the description of the light dynamic constant buffer that is in the pixel shader.
	// Note that ByteWidth always needs to be a multiple of 16 if using D3D11_BIND_CONSTANT_BUFFER or CreateBuffer will fail.
	//D3D11_BUFFER_DESC lightBufferDesc;
		
	//lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	//lightBufferDesc.ByteWidth = sizeof(LightBufferType);r
	//lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	//lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	//lightBufferDesc.MiscFlags = 0;
	//lightBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	//DX::ThrowIfFailed(
	//m_deviceResources->GetD3DDevice()->CreateBuffer(&lightBufferDesc, NULL, &m_lightBuffer));






	return when_all(tasks.begin(), tasks.end());
}


