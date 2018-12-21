#pragma once

#include "Common\StepTimer.h"
#include "Common\DeviceResources.h"
#include "Content\Sample3DSceneRenderer.h"
#include "Content\SampleFpsTextRenderer.h"
#include "GameUIControl.h"
#include "MoveLookController.h"

// Renders Direct2D and 3D content on the screen.
namespace Simple3DGame
{
	class Simple3DGameMain : public DX::IDeviceNotify
	{
	public:
		Simple3DGameMain(const std::shared_ptr<DX::DeviceResources>& deviceResources, Simple3DGame::IGameUIControl^ UIControl);
		~Simple3DGameMain();
		void CreateWindowSizeDependentResources();
		void StartTracking() { m_sceneRenderer->StartTracking(); }
		void TrackingUpdate(float positionX) { m_pointerLocationX = positionX; }
		void StopTracking() { m_sceneRenderer->StopTracking(); }
		bool IsTracking() { return m_sceneRenderer->IsTracking(); }
		void StartRenderLoop();
		void StopRenderLoop();
		Concurrency::critical_section& GetCriticalSection() { return m_criticalSection; };

		/*
		std::shared_ptr<Sample3DSceneRenderer> GetSceneRenderer()
		{
			return m_sceneRenderer;
		}
		*/
		//std::unique_ptr<Sample3DSceneRenderer> GetSc

		// IDeviceNotify
		virtual void OnDeviceLost();
		virtual void OnDeviceRestored();

		MoveLookController^ GetMoveLook() {
			return m_controller
				;
		};
		std::unique_ptr<Sample3DSceneRenderer> m_sceneRenderer;
	private:
		void ProcessInput();
		void Update();
		bool Render();

		Simple3DGame::IGameUIControl^                   m_uiControl;
		// Cached pointer to device resources.
		std::shared_ptr<DX::DeviceResources> m_deviceResources;

		MoveLookController^                                 m_controller;

		// TODO: Replace with your own content renderers.
		

		Windows::Foundation::IAsyncAction^ m_renderLoopWorker;
		Concurrency::critical_section m_criticalSection;

		// Rendering loop timer.
		DX::StepTimer m_timer;

		// Track current input pointer position.
		float m_pointerLocationX;
	};
}