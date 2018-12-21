#pragma once

#include "..\Common\DeviceResources.h"

#include "..\Common\StepTimer.h"

#include "Physics.h"
#include "Stage.h"

#include "MoveLookController.h"

namespace Simple3DGame
{
	// This sample renderer instantiates a basic rendering pipeline.
	class Sample3DSceneRenderer
	{
	public:
		Sample3DSceneRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources, MoveLookController^	controller, Simple3DGame::IGameUIControl^ UIControl);
		void CreateDeviceDependentResources();
		void CreateWindowSizeDependentResources();
		void ReleaseDeviceDependentResources();
		void Update(DX::StepTimer const& timer);
		void Render();
		void StartTracking();
		void TrackingUpdate(float positionX);
		void StopTracking();
		bool IsTracking() { return m_tracking; }
		Stage* GetStage() {
			return m_Stage;
		};
		Physics* GetPhys() {
			return &m_Physics;
		};
		void SetFreeCamera(bool set_to) {
			bFreeCamera = set_to; m_camera->bFreeCamera = set_to; cam_pos
				= *m_Stage->m_Banana->GetPosition();
		};

		void SetSnapGrid(bool set_snap_grid) {
			snap_grid = set_snap_grid;
		}

		void SetSpeedPos(float _speed_pos)
		{
			speed_pos = _speed_pos;
		}
	private:
		int l_delay;
		void Rotate(float radians);
		float roll_vol;

		bool bFreeCamera;

		bool snap_grid;

		MoveLookController^       m_controller;
		Simple3DGame::IGameUIControl^ m_uiControl;

		Windows::Foundation::Size outputSize;
		Windows::Foundation::Rect m_windowBounds;
		Camera^ m_camera;
	private:
		btVector3 last_pos;
		Stage* m_Stage;

		Physics m_Physics;

		btVector3 cam_pos, cam_rot;
		btMatrix3x3 cam_rot_mat;
		btVector3 cam_point_dir_x;
		btVector3 cam_point_dir_y;
		btVector3 cam_point_dir_z;

		float cam_at_y;

		int bInitialized;

		// Cached pointer to device resources.
		std::shared_ptr<DX::DeviceResources>		m_deviceResources;

		// Direct3D resources for cube geometry.
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_constantBuffer;

		// System resources for cube geometry.
		ModelViewProjectionConstantBuffer			m_constantBufferData;

		// Variables used with the rendering loop.
		bool	m_loadingComplete;
		float	m_degreesPerSecond;
		bool	m_tracking;


		float speed_pos;
	};
}

