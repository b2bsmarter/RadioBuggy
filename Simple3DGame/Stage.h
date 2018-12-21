#pragma once

#include "WorldObject.h"
#include "Sky.h"
#include "Level.h"
#include "player.h"
#include "fence.h"
#include "snow.h"
#include "Rain.h"
#include "Boxes.h"
#include "Fruit.h"
#include "Points.h"
#include "Trees.h"
#include "Splash.h"
#include "Cash.h"
#include "lights.h"
#include "Shoot.h"
#include "explode.h"
#include "statics.h"

#include "Dots.h"

#include "Flag.h"

#include "Textures.h"

#include "GameUIControl.h"



namespace Simple3DGame
{

	enum GameState {
		STATE_PLAYING = 0, // Start the game
		STATE_START = 1, // Start Screen
		START_PLAYING = 2,
		STATE_CHOOSE_PLAYER = 3, // Choose Player
		STATE_ENTER_PLAYER = 4, // New Player
		
	}; // +1 collide with ray cast

	typedef struct
	{
		int shader_mode; // 0 = low , 1 = high
		int music;
		int screen_controls;
	} game_settings_t;

	class Stage
	{
	public:
		Stage::Stage(std::shared_ptr<DX::DeviceResources> pm_deviceResources, Physics* pm_physics, Camera^ p_camera, Simple3DGame::IGameUIControl^ UIControl);
		~Stage();

		DirectX::XMMATRIX m_projectionMatrix;

		std::shared_ptr<DX::DeviceResources> m_deviceResources;

		game_settings_t game_settings;

		void LazerTest(DirectX::XMFLOAT3* pos, DirectX::XMFLOAT3* dir, bool bClick);

		void SetMusicMode(int mode);

		void MakeRayMap();

		void SetShaderMode(int mode);

		bool bDesignMode;
		int current_design_item;
		bool bPlayerPosition;
		bool bRayCheck; // use physics ray for fruit height
		float GetWorldHeight(float posx, float posz);
		int iLoadLocal;

		float respot_wait;

		player_info player_data[40];
		char current_playername[40];
		int current_player_index;
		int total_player_names;

		float specular_power;

		int season; // season    0 - summer   1 - autum   2 - winter  3 - spring

		int current_game_state;

		int current_level;
		bool bInitialized;

		bool bInitialMusicStarted;

		bool bAppInTrialMode;

		bool bLevelLoaded;

		int state_to;
		float start_state_to_delay;
		float start_state_to_val;

		bool bFadeIn;
		bool bLevelStarting;
		float fade_in_to_delay;
		float fade_in_to_val;

		bool bSaveLandRay;

		bool bSkipIntro;

		int total_rings;
		int rings_collected;

		float music_volume;

		bool bLevelComplete;
		float level_complete_event_timer;
		float finish_delay;

		float render_delay;
		
		bool bTimedOut;

		//int level_scores[100];

		level_info level_data[100];
		level_info* GetLevelData() { return level_data; };
		bool bLoadLevel;
		bool bRain;
		bool bSnow;


		bool bMusic;
		int iMusicTrack;



		bool bResetLevelUI;


		char local_file_folder[140];

		bool bDesignlevelLoaded;

		int b_headlights;

		int next_new_level;

		bool bClicked;
		bool bContinualPlace;
		
		bool bRotateRay;

		float player_mass;

		float leveltime;
		float startleveltime;

		bool bRespot;
		float respot_delay;

		bool bPaused;
		float diffuse_average;
		float ambient_average;

		Flag* m_Flag;

		bool bAllLoaded;

		float start_render_delay;
		float noise_z;
		Textures* m_Textures;

		Shoot* m_shoot;

		Dots* m_dots;

		Snow* m_Snow;
		Rain* m_Rain;

		Trees* m_Trees;

		Statics* m_Statics;

		Fence* m_Fence;

		Splash* m_Splash;
		Explode* m_Explode;

		Cash* m_Cash;

		Camera^ m_camera;
		Physics* m_phys;

		Level*	m_Level;
		Player* m_Banana;
		Boxes* m_Boxes;

		Fruit* m_Fruit;
		Points* m_Points;

		Lights* m_Lights;
		
		Sky* m_Sky;
		
		Audio  m_audio;

		SoundEffect^ m_SFFlick;
		SoundEffect^ m_SFBasket;

		void SaveAll();
		void SetDesignItem(int item_to);

		void LoadLevel(int level);
		void ChangeLevel(int diff);
		void ChangeLevelTo(int level_to);

		void LoadSettings();
		void SaveSettings();

		void SetScreenControls(int mode);

		void DoRespot();

		void Respot();

		bool CheckName(char* name_str);
		void AddName(char* name_str);

		void SavePlayerScores();
		void LoadPlayerScores(char* player_name);

		void LoadPlayerNames();
		void SavePlayerNames();
		void SetPlayer(int ind);

		void LoadDeviceResources();
		void SetLevelUI();
		void SetPlayersUI();

		Simple3DGame::IGameUIControl^ m_uiControl;

		ID3D11BlendState* m_alphaEnableBlendingState;
		ID3D11BlendState* m_alphaDisableBlendingState;
		ID3D11BlendState* m_alphaEnableBlendingTexAlph;
		ID3D11BlendState* m_alphaEnableBlendingGlass;

		ID3D11DepthStencilState* m_depthDisabledStencilState;

		ID3D11RasterizerState * g_pRasterState_cull;
		ID3D11RasterizerState * g_pRasterState_nocull;

		Microsoft::WRL::ComPtr<ID3D11VertexShader> m_model_vertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> m_model_pixelShader;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> m_model_inputLayout;

		Microsoft::WRL::ComPtr<ID3D11VertexShader> m_point_vertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> m_point_pixelShader;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> m_point_inputLayout;

		Microsoft::WRL::ComPtr<ID3D11VertexShader> m_shinemodel_vertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> m_shinemodel_pixelShader;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> m_shinemodel_inputLayout;

		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_cameraBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_lightBuffer;

		CameraBufferType m_cameraBufferData;

		
		float blendFactor[4];

		concurrency::task<void> Stage::LoadShaders();
		void LoadShadersSync();

		concurrency::task<void> Stage::InitializeStuff();

		void SetupAlphaBlendingStates();
		void SetProjectionMatrixs();
		void UpdateProjectionMatrix(DirectX::XMMATRIX *projectionMatrix);
		void Update(DirectX::XMFLOAT4X4 *viewMatrix, float timeDelta, float timeTotal, float fps);
		void Render(Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer, ModelViewProjectionConstantBuffer* constantBufferData);

		void SetModelShader();
		void SetPointShader();
		void SetShinyShader();

		void SetGameState(int state);
		void SetGameStateTo(int state, float delay);

		void UpdateCameraBuffer();

	};
}

