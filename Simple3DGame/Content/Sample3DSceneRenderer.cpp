#include "pch.h"
#include "Sample3DSceneRenderer.h"

//#include "MoveLookControllerXaml.h"

using namespace Simple3DGame;

using namespace DirectX;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Display;
using namespace Windows::UI::Core;
using namespace Windows::UI::Input;
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Media;


// Loads vertex and pixel shaders from files and instantiates the cube geometry.
Sample3DSceneRenderer::Sample3DSceneRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources, MoveLookController^	controller, Simple3DGame::IGameUIControl^ UIControl) :
	m_loadingComplete(false),
	m_degreesPerSecond(45),
	m_uiControl(UIControl),

	m_tracking(false),
	m_deviceResources(deviceResources),
	m_controller(controller)
{
	m_camera = ref new Camera;
	m_Stage = new Stage(m_deviceResources, &m_Physics, m_camera, m_uiControl);
	bInitialized = 0;
	CreateWindowSizeDependentResources();
	CreateDeviceDependentResources();
	
	cam_pos = btVector3(0.0f, 0.0f, 0.0f);
	cam_rot = btVector3(0.0f, 0.0f, 0.0f);

	cam_at_y = 0.0f;
	
	speed_pos = -1.0f; // stays like this for keys


	snap_grid = false;
}



// Initializes view parameters when the window size changes.
void Sample3DSceneRenderer::CreateWindowSizeDependentResources()
{
	XMFLOAT4X4 orientation = m_deviceResources->GetOrientationTransform3D();
	//orientation.
	XMMATRIX orientationMatrix = XMLoadFloat4x4(&orientation);

	outputSize = m_deviceResources->GetOutputSize();
	float aspectRatio;
	
	if(true) //orientation._11==1.0f)
	{
		aspectRatio = outputSize.Width / outputSize.Height;
	}
	else
	{
		aspectRatio = outputSize.Height / outputSize.Width;
	}
	float fovAngleY = 60.0f * XM_PI / 180.0f;
	//aspectRatio = 1.0f;

	//m_deviceResources->

	m_Stage->m_camera->SetWindowsSize(outputSize);
	m_controller->SetWindowsSize(outputSize);

	if ((outputSize.Height < 481 || outputSize.Width < 481) && outputSize.Height>1.0f)
	{
		m_Stage->m_uiControl->SmallEverything();
	}
	// This is a simple example of change that can be made when the app is in
	// portrait or snapped view.
	if (aspectRatio < 1.0f)
	{
		fovAngleY *= 1.3f;
	}

	// This sample makes use of a right-handed coordinate system using row-major matrices.
	XMMATRIX perspectiveMatrix = XMMatrixPerspectiveFovRH(
		fovAngleY,
		aspectRatio,
		1.0f,
		300.0f
		);


	XMStoreFloat4x4(
		&m_constantBufferData.projection,
		XMMatrixTranspose(perspectiveMatrix * orientationMatrix)
		);


	m_camera->SetProjectionMatrix(m_constantBufferData.projection, orientation);

	roll_vol = 0.0f;
	l_delay = 10;



	// Eye is at (0,0.7,1.5), looking at point (0,-0.1,0) with the up-vector along the y-axis.
	//static const XMVECTORF32 eye = { 0.0f, 10.7f, 10.5f, 0.0f };
	//static const XMVECTORF32 at = { 0.0f, -0.1f, 0.0f, 0.0f };
	//static const XMVECTORF32 up = { 0.0f, 1.0f, 0.0f, 0.0f };

	//XMStoreFloat4x4(&m_constantBufferData.view, XMMatrixTranspose(XMMatrixLookAtRH(eye, at, up)));
	//int tid = 0;

	//btDefaultMotionState* floor = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, -3.0f, 0)));
	//auto floorShape = new btStaticPlaneShape(btVector3(0, 1, 0), 1);//left
	//m_Physics.AddPhysicalObject(floorShape, floor, 0, btVector3(0, 0, 0), 0.9f, 0.5f, &tid);

	//btVector3 grav(0.0f, -80.0f, 0.0f);
	//m_Physics.m_dynamicsWorld->setGravity(grav);
}





// Called once per frame, rotates the cube and calculates the model an d view matrices.
void Sample3DSceneRenderer::Update(DX::StepTimer const& timer)
{

	if (m_Stage->bResetLevelUI == true)
	{
		m_Stage->bResetLevelUI = false;
		m_Stage->SetLevelUI();
	}

	// Start delay must go here
	if (m_Stage->state_to > -1)
	{
		if (m_Stage->start_state_to_val > 0.0f)
		{
			
			m_Stage->start_state_to_val -= timer.GetElapsedSeconds();
			float fade_val = 1.0f - ((1.0f / m_Stage->start_state_to_delay)* m_Stage->start_state_to_val);
			m_Stage->m_audio.SetMusicVolume((1.0f-fade_val)*m_Stage->music_volume);
			m_uiControl->SetFadeOp(fade_val);
		}
		else
		{
			int temp_state = m_Stage->state_to;
			m_Stage->state_to = -1;
			m_Stage->SetGameState(temp_state);
			return;
		}
	}

	if (m_Stage->bFadeIn == true)
	{
		if (m_Stage->fade_in_to_val > 0.0f )
		{
			if (m_Stage->render_delay < 0.01f)
			{
				m_Stage->fade_in_to_val -= timer.GetElapsedSeconds();
				float fade_val = ((1.0f / m_Stage->fade_in_to_delay)* m_Stage->fade_in_to_val);
				m_Stage->m_audio.SetMusicVolume((1.0f - fade_val)*m_Stage->music_volume);
				m_uiControl->SetFadeOp(fade_val);
			}
		}
		else
		{
			m_uiControl->StopFade();
			m_Stage->bFadeIn = false;
		}
	}


	if (m_Stage->bPaused == true)
		return;

	if (!m_tracking)
	{
		// Convert degrees to radians, then convert seconds to rotation angle
		float radiansPerSecond = XMConvertToRadians(m_degreesPerSecond);
		double totalRotation = timer.GetTotalSeconds() * radiansPerSecond;
		float radians = static_cast<float>(fmod(totalRotation, XM_2PI));

		Rotate(radians);
	}

	if (!m_loadingComplete)
	{
		return;
	}


	float velocity1 = m_Stage->m_Banana->wheel[0]->m_rigidbody->getAngularVelocity().length();
	float velocity2 = m_Stage->m_Banana->wheel[1]->m_rigidbody->getAngularVelocity().length();
	float velocity3 = m_Stage->m_Banana->wheel[2]->m_rigidbody->getAngularVelocity().length();
	float velocity4 = m_Stage->m_Banana->wheel[3]->m_rigidbody->getAngularVelocity().length();

	float use_vel = 900.0f;

	if (velocity1 < use_vel)
		use_vel = velocity1;

	if (velocity2 < use_vel)
		use_vel = velocity2;

	if (velocity3 < use_vel)
		use_vel = velocity3;

	if (velocity4 < use_vel)
		use_vel = velocity4;

	

	//btTransform ball_tran = m_Stage->m_Banana->m_rigidbody->getWorldTransform();

	btVector3 eye_pos = *m_Stage->m_Banana->GetPosition();
	btVector3 at_pos = *m_Stage->m_Banana->GetPosition();

	btVector3 direct = *m_Stage->m_Banana->GetDirection();

	eye_pos.setY(eye_pos.getY() + 2.0f);
	at_pos.setY(at_pos.getY() + 2.0f);


	/*
	m_camera->SetViewParams(
		DirectX::XMFLOAT3(eye_pos.getX(), eye_pos.getY(), eye_pos.getZ()),            // Eye point in world coordinates.
		DirectX::XMFLOAT3(0.0f, 0.7f, 0.0f),     // Look at point in world coordinates.
		DirectX::XMFLOAT3(0.0f, 1.01f, 0.01f)      // The Up vector for the camera.
		);
		*/
	//m_camera->SetProjectionMatrix(m_constantBufferData.projection);
	/*
	if (outputSize.Width < outputSize.Height)
	{
		m_camera->SetEyeAt(eye_pos.getX()+ m_Stage->m_Banana->car_point.getX(), eye_pos.getY() + 2.5f, eye_pos.getZ() + m_Stage->m_Banana->car_point.getZ(), eye_pos.getX(), eye_pos.getY(), eye_pos.getZ());


	}
	else
	{
		m_camera->SetEyeAt(eye_pos.getX() + m_Stage->m_Banana->car_point.getX(), eye_pos.getY() + 4.0f, eye_pos.getZ() + m_Stage->m_Banana->car_point.getZ(), eye_pos.getX(), eye_pos.getY(), eye_pos.getZ());
		//m_camera->SetEyeAt(eye_pos.getX() - (direct.getX()*2.0f), eye_pos.getY() + 16.0f, eye_pos.getZ() - (direct.getZ()*2.0f), eye_pos.getX(), eye_pos.getY(), eye_pos.getZ());
	}
	*/

	//XMStoreFloat4x4(&R, XMMatrixRotationRollPitchYaw(pitch, yaw, roll));

	if (bFreeCamera == true)
	{
		btQuaternion quat = btQuaternion(cam_rot.getX(), cam_rot.getY(), cam_rot.getZ());

		cam_rot_mat = btMatrix3x3(quat);

		cam_point_dir_x = btVector3(1.0f, 0.0f, 0.0f);
		cam_point_dir_y = btVector3(0.0f, 1.0f, 0.0f);
		cam_point_dir_z = btVector3(0.0f, 0.0f, 1.0f);

		cam_point_dir_x = cam_point_dir_x * cam_rot_mat;
		cam_point_dir_y = cam_point_dir_y * cam_rot_mat;
		cam_point_dir_z = cam_point_dir_z * cam_rot_mat;

		if (outputSize.Width < outputSize.Height)
		{
			m_camera->SetEyeAt(cam_pos.getX(), cam_pos.getY(), cam_pos.getZ(), cam_pos.getX() + cam_point_dir_z.getX(), cam_pos.getY() + cam_at_y, cam_pos.getZ() + cam_point_dir_z.getZ());
		}
		else
		{
			m_camera->SetEyeAt(cam_pos.getX(), cam_pos.getY(), cam_pos.getZ(), cam_pos.getX() + cam_point_dir_z.getX(), cam_pos.getY() + cam_at_y, cam_pos.getZ() + cam_point_dir_z.getZ());

			//float zzzz = m_Stage->m_Banana->car_point.getZ();
			//m_camera->SetEyeAt(eye_pos.getX() + (-m_Stage->m_Banana->car_point.getX()*7.0f), eye_pos.getY() + 3.0f, eye_pos.getZ() + (-m_Stage->m_Banana->car_point.getZ()*7.0f), eye_pos.getX(), eye_pos.getY(), eye_pos.getZ());
			//m_camera->SetEyeAt(eye_pos.getX() - (direct.getX()*2.0f), eye_pos.getY() + 16.0f, eye_pos.getZ() - (direct.getZ()*2.0f), eye_pos.getX(), eye_pos.getY(), eye_pos.getZ());
		}
	}
	else
	{

		if (outputSize.Width < outputSize.Height)
		{
			m_camera->SetEyeAt(eye_pos.getX() + (-m_Stage->m_Banana->car_point.getX()*5.0f), eye_pos.getY() + 2.5f, eye_pos.getZ() + (-m_Stage->m_Banana->car_point.getZ()*5.0f), eye_pos.getX(), eye_pos.getY(), eye_pos.getZ());
		}
		else
		{
			//float zzzz = m_Stage->m_Banana->car_point.getZ();
			m_camera->SetEyeAt(eye_pos.getX() + (-m_Stage->m_Banana->car_point.getX()*7.0f), eye_pos.getY() + 3.0f, eye_pos.getZ() + (-m_Stage->m_Banana->car_point.getZ()*7.0f), eye_pos.getX(), eye_pos.getY(), eye_pos.getZ());
			//m_camera->SetEyeAt(eye_pos.getX() - (direct.getX()*2.0f), eye_pos.getY() + 16.0f, eye_pos.getZ() - (direct.getZ()*2.0f), eye_pos.getX(), eye_pos.getY(), eye_pos.getZ());
		}
	}

	
	//m_Stage->m_Banana->
	m_camera->Update(timer.GetElapsedSeconds());



	m_Physics.Update(timer.GetElapsedSeconds());
	XMFLOAT4X4 float_matrix;
	
	//m_camera->SetProjectionMatrix(m_constantBufferData.projection);
	//m_constantBufferData.projection=*m_camera->GetProjectionMatrix();



	

	m_controller->Update(timer.GetElapsedSeconds());

	if (m_Stage->bDesignMode == true)
	{
		XMFLOAT2* mouse_pixel = m_controller->GetMousePixel();
		Windows::Foundation::Point mouse_point = m_controller->GetPointerPos();

		if ((UINT)mouse_point.X < m_deviceResources->GetOutputSize().Width-130)
		{
			XMFLOAT3 pos, dir;

			//Stage->m_camera->outpu

			m_Stage->m_camera->GetWorldLine((UINT)mouse_point.X, (UINT)mouse_point.Y, &pos, &dir);
			//m_Stage->m_camera->GetWorldLine((UINT)mouse_pixel->x, (UINT)mouse_pixel->y, &pos, &dir);
			//m_Stage->m_camera->GetWorldLine((UINT)pointer_pos.X, (UINT)pointer_pos.Y, &pos, &dir);
			pos.x = m_Stage->m_camera->EyeX();
			pos.y = m_Stage->m_camera->EyeY();
			pos.z = m_Stage->m_camera->EyeZ();

			////dir.x = 0.0f;
			//dir.y = -0.5f;
			//dir.z = 1.0f;

			dir.x = (dir.x*500.0f);
			dir.y = (dir.y*500.0f);
			dir.z = (dir.z*500.0f);

			btVector3 vec = btVector3(dir.x, dir.y, dir.z);
			vec.normalize();
			dir.x = vec.getX();
			dir.y = vec.getY();
			dir.z = vec.getZ();
			/*
			dir.x += pos.x;
			dir.y += pos.y;
			dir.z += pos.z;
			*/
			
			m_Stage->LazerTest(&pos, &dir, (mouse_pixel != NULL));

			//dir.x = -dir.x;
			//dir.y = -dir.y;
			//dir.z = -dir.z;

			//m_Stage->LazerTest(&pos, &dir);
		}

		{
			btVector3 sideways = cam_point_dir_x;
			sideways.setY(0.0f);
			sideways.normalize();
			btVector3 forwards = cam_point_dir_z;
			forwards.setY(0.0f);
			forwards.normalize();

			float movespeed;

			if (m_controller->CamFast() == 1)
			{
				movespeed = 1.0f;
			}
			else
			{
				movespeed = 0.2f;
			}

			// Camera movement
			if (m_controller->CamMoveX() == 1)
			{
				cam_pos.setX(cam_pos.getX() - (movespeed*sideways.getX()));
				cam_pos.setZ(cam_pos.getZ() - (movespeed*sideways.getZ()));
			}
			if (m_controller->CamMoveX() == -1)
			{
				cam_pos.setX(cam_pos.getX() + (movespeed*sideways.getX()));
				cam_pos.setZ(cam_pos.getZ() + (movespeed*sideways.getZ()));
			}

			if (m_controller->CamMoveY() == 1)
				cam_pos.setY(cam_pos.getY() + movespeed);
			if (m_controller->CamMoveY() == -1)
				cam_pos.setY(cam_pos.getY() - movespeed);


			if (m_controller->CamMoveZ() == 1)
			{
				cam_pos.setX(cam_pos.getX() + (movespeed*forwards.getX()));
				cam_pos.setZ(cam_pos.getZ() + (movespeed*forwards.getZ()));
			}

			if (m_controller->CamMoveZ() == -1)
			{
				cam_pos.setX(cam_pos.getX() - (movespeed*forwards.getX()));
				cam_pos.setZ(cam_pos.getZ() - (movespeed*forwards.getZ()));
			}


			if (m_controller->CamYaw() == 1)
				cam_rot.setX(cam_rot.getX() + 0.02f);
			if (m_controller->CamYaw() == -1)
				cam_rot.setX(cam_rot.getX() - 0.02f);

			
			if (m_controller->CamPitch() == 1)
			{
				cam_at_y += 0.02f;
			}
			if (m_controller->CamPitch() == -1)
			{
				cam_at_y -= 0.02f;
			}
			/*
			if (m_controller->CamRoll() == 1)
				cam_rot.setZ(cam_rot.getZ() + 0.02f);
			if (m_controller->CamRoll() == -1)
				cam_rot.setZ(cam_rot.getZ() - 0.02f);
				*/
			if (cam_rot.getX() > M_PI*2.0f)
				cam_rot.setX( cam_rot.getX() - M_PI*2.0f);
			if (cam_rot.getY() > M_PI*2.0f)
				cam_rot.setY( cam_rot.getY() - M_PI*2.0f);
			if (cam_rot.getZ() > M_PI*2.0f)
				cam_rot.setZ( cam_rot.getZ() - M_PI*2.0f);

			if (cam_rot.getX() < 0.0f)
				cam_rot.setX(cam_rot.getX() + M_PI*2.0f);
			if (cam_rot.getY() < 0.0f)
				cam_rot.setY(cam_rot.getY() + M_PI*2.0f);
			if (cam_rot.getZ() < 0.0f)
				cam_rot.setZ(cam_rot.getZ() + M_PI*2.0f);

		}

		// static rotation
		if (m_controller->KeyYaw() == 1)
		{
			m_Stage->m_Statics->design_pointer.rot_y += 0.05f;
			m_Stage->m_Boxes->design_pointer.rot_y += 0.05f;
		}
		if (m_controller->KeyYaw() == -1)
		{
			m_Stage->m_Statics->design_pointer.rot_y -= 0.05f;
			m_Stage->m_Boxes->design_pointer.rot_y -= 0.05f;
		}
		if (m_controller->KeyPitch() == 1)
		{
			m_Stage->m_Statics->design_pointer.rot_x += 0.05f;
			m_Stage->m_Boxes->design_pointer.rot_x += 0.05f;
		}
		if (m_controller->KeyPitch() == -1)
		{
			m_Stage->m_Statics->design_pointer.rot_x -= 0.05f;
			m_Stage->m_Boxes->design_pointer.rot_x -= 0.05f;
		}

		if (m_controller->KeyRoll() == 1)
		{
			m_Stage->m_Statics->design_pointer.rot_z += 0.05f;
			m_Stage->m_Boxes->design_pointer.rot_z += 0.05f;
		}
		if (m_controller->KeyRoll() == -1)
		{
			m_Stage->m_Statics->design_pointer.rot_z -= 0.05f;
			m_Stage->m_Boxes->design_pointer.rot_z -= 0.05f;
		}

		if (m_controller->KeyHeight() == 1)
		{
			m_Stage->m_Statics->design_height_f += 0.05f;
			m_Stage->m_Boxes->design_height_f += 0.05f;
		}
		if (m_controller->KeyHeight() == -1)
		{
			m_Stage->m_Statics->design_height_f -= 0.05f;
			m_Stage->m_Boxes->design_height_f -= 0.05f;
		}
		if(snap_grid==true)
		{
			m_Stage->m_Statics->design_height = (int)m_Stage->m_Statics->design_height_f;
			m_Stage->m_Boxes->design_height = (int)m_Stage->m_Boxes->design_height_f;
		}
		else
		{
			m_Stage->m_Statics->design_height = m_Stage->m_Statics->design_height_f;
			m_Stage->m_Boxes->design_height = m_Stage->m_Boxes->design_height_f;
		}

		if (m_controller->KeyReset() == 1)
		{
			m_Stage->m_Statics->design_height = 0.0f;
			m_Stage->m_Statics->design_pointer.rot_x = 0.0f;
			m_Stage->m_Statics->design_pointer.rot_y = 0.0f;
			m_Stage->m_Statics->design_pointer.rot_z = 0.0f;

			m_Stage->m_Boxes->design_height = 0.0f;
			m_Stage->m_Boxes->design_pointer.rot_x = 0.0f;
			m_Stage->m_Boxes->design_pointer.rot_y = 0.0f;
			m_Stage->m_Boxes->design_pointer.rot_z = 0.0f;
		}

		if (m_controller->IsFreeze() == true)
		{
			m_Stage->m_Banana->m_rigidbody->setLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));
		}
	}

	if (m_controller->IsPauseRequested() == true)
	{
		//m_Stage->m_uiControl->SetPause();
	}

	m_Stage->m_audio.SetSoundEffectVolume(SFXRollingEvent, roll_vol*0.3f);
	m_Stage->m_audio.SetSoundEffectPitch(SFXRollingEvent, (use_vel*0.025f) + (roll_vol*0.032f));



	if(m_Stage->m_Banana->carspeed>0.0f) // spoiler pressure
	{
		float speed_mul = m_Stage->m_Banana->carspeed * 0.5f;
		m_Stage->m_Banana->m_rigidbody->applyCentralImpulse(btVector3(-m_Stage->m_Banana->y_offset.getX()*speed_mul, -m_Stage->m_Banana->y_offset.getY()*speed_mul, -m_Stage->m_Banana->y_offset.getZ()*speed_mul));
	}


	// set wheels
	//m_controller->
	if (m_controller->IsKeyDown())
	{
		bool b_motorkey = false;

		XMFLOAT2 move_ment = *m_controller->GetKeyDirection();;
		if (move_ment.x == 0.0f)
		{
			m_Stage->m_Banana->SetWheelTurn(0);
		}
		if (move_ment.x > 0.0f)
		{
			m_Stage->m_Banana->SetWheelTurn(1);
		}
		if (move_ment.x < 0.0f)
		{
			m_Stage->m_Banana->SetWheelTurn(-1);
		}

		if (move_ment.x < 0.0f)
		{
			m_Stage->m_Banana->SetWheelTurn(-1);
		}

		if (move_ment.y > 1.0f)
		{
			b_motorkey = true;
			m_Stage->m_Banana->SetMotors(-2);
		}
		else
		{
			if (move_ment.y > 0.0f)
			{
				b_motorkey = true;
				if (speed_pos > -1.0f)
				{
					if (speed_pos > 120.0f)
						speed_pos = 120.0f;
					if (speed_pos < 20.0f)
						speed_pos = 20.0f;

					m_Stage->m_Banana->SetMotors(20.0f+(speed_pos*0.45f), 2.6f);
				}
				else
				{
					m_Stage->m_Banana->SetMotors(-1);
				}
			}
		}

		if (move_ment.y < 0.0f)
		{
			b_motorkey = true;
			m_Stage->m_Banana->SetMotors(1);
		}

		if (move_ment.y == 0.0f)
		{
			//b_motorkey = true;
			m_Stage->m_Banana->SetMotors(0);
		}
		if (b_motorkey == true)
		{
			if (roll_vol < 1.0f)
				roll_vol += 0.1f;
		}
	}
	else
	{
		if (roll_vol > 0.0f)
			roll_vol -= 0.05f;
		m_Stage->m_Banana->SetWheelTurn(0);
		m_Stage->m_Banana->SetMotors(false);
	}


	switch (m_controller->IsShooting())
	{
	case 1:m_Stage->m_Banana->GrabPressed(); break;
	case -1:m_Stage->m_Banana->Grabreleased(); break;
	}



	if (false)//(m_controller->IsReleased() == true)
	{
		DirectX::XMFLOAT2 hit_dir = *m_controller->GetHitDirectionB();

		if(hit_dir.x!=0.0f || hit_dir.y != 0.0f)
		{
			XMFLOAT2 movement = *m_controller->GetHitDirectionB();
			btVector3 vec3;
			btVector3 vec4;
			vec3.setX(movement.x*10.0f);
			vec3.setZ(-movement.y*10.0f);
			vec3.setY(0.0f);

			//vec3.normalize();
			vec3.setX(-vec3.getX()*2.0f);
			vec3.setZ(-vec3.getZ()*2.0f);

			//m_Stage->PlaySound(1);

			btVector3 normdir;  // normal direction
			normdir.setX(0.01f);
			normdir.setY(0.01f);
			normdir.setZ(1.0f);

			btVector3 cam_point = *m_camera->GetLookingDirection();
			cam_point.setY(0.0f);
			cam_point = cam_point.normalize();

			float diff_angle = cam_point.angle(normdir);

			//btQuaternion rotqu = btQuaternion(btVector3(0.0f, 1.0f, 0.0f), diff_angle);

			/*
			if (diff_angle > M_PI)
			{

			diff_angle = diff_angle - M_PI;// (M_PI + M_PI - diff_angle);

			}
			*/

			//m_Stage->m_fpsTextRenderer->SetFloatNum(0, diff_angle);
			//m_Stage->m_fpsTextRenderer->SetFloatNum(1, diff_angle);
			//m_Stage->m_fpsTextRenderer->SetFloatNum(2, diff_angle);

			float cs = cos(diff_angle);
			float sn = sin(diff_angle);

			float x = vec3.getX();
			float y = vec3.getZ();

			if (cam_point.getX()<0.0f)
			{
				vec3.setX(x * cs - y * sn);
				vec3.setZ(x * sn + y * cs);
				//vec3 = vec3 * rotqu;
			}
			else
			{
				vec3 = vec3.rotate(btVector3(0.0f, 1.0f, 0.0f), diff_angle);
			}


			btVector3 imp = btVector3(0.0f,0.0f,0.0f);
			//m_Stage->m_Banana->m_rigidbody->setLinearVelocity(imp);
			m_Stage->m_Banana->m_rigidbody->setAngularVelocity(imp);

			m_Stage->m_Banana->m_rigidbody->applyCentralImpulse(vec3);
		}
	}

	if (m_Stage->bLevelComplete == true)
	{
		m_Stage->m_Banana->SetWheelTurn(1);
		m_Stage->m_Banana->SetMotors(2); // break

	}


	m_Stage->Update(&m_constantBufferData.view, timer.GetElapsedSeconds(), timer.GetElapsedSeconds(), timer.GetFramesPerSecond());// = m_constantBufferData.view;// Update(m_constantBufferData.view, timer.GetTotalTicks(), timer.GetElapsedTicks());

		/*																							  /*
	XMFLOAT4X4 mod_matrix = m_Stage->m_Banana->m_constantBufferData.model;

	mod_matrix._14 += m_Stage->m_Banana->y_offset.getX()*9.0f;
	mod_matrix._24 += m_Stage->m_Banana->y_offset.getY()*9.0f;
	mod_matrix._34 += m_Stage->m_Banana->y_offset.getZ()*9.0f;
	*/
	//XMStoreFloat4x4(&m_constantBufferData.view, *m_camera->GetMatrix(&mod_matrix));
	XMStoreFloat4x4(&m_constantBufferData.view, *m_camera->GetMatrix());
}

// Rotate the 3D cube model a set amount of radians.
void Sample3DSceneRenderer::Rotate(float radians)
{
	// Prepare to pass the updated model matrix to the shader
	XMStoreFloat4x4(&m_constantBufferData.model, XMMatrixTranspose(XMMatrixRotationY(radians)));
}

void Sample3DSceneRenderer::StartTracking()
{
	m_tracking = true;
}

// When tracking, the 3D cube can be rotated around its Y axis by tracking pointer position relative to the output screen width.
void Sample3DSceneRenderer::TrackingUpdate(float positionX)
{
	if (m_tracking)
	{
		float radians = XM_2PI * 2.0f * positionX / m_deviceResources->GetOutputSize().Width;
		Rotate(radians);
	}
}

void Sample3DSceneRenderer::StopTracking()
{
	m_tracking = false;
}

// Renders one frame using the vertex and pixel shaders.
void Sample3DSceneRenderer::Render()
{
	// Loading is asynchronous. Only draw geometry after it's loaded.
	if (!m_loadingComplete)
	{
		return;
	}

	auto context = m_deviceResources->GetD3DDeviceContext();

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	context->VSSetConstantBuffers(
		0,
		1,
		m_constantBuffer.GetAddressOf()
		);

	m_Stage->Render(m_constantBuffer, &m_constantBufferData);
}


void Sample3DSceneRenderer::CreateDeviceDependentResources()
{
	outputSize = m_deviceResources->GetOutputSize();
	float aspectRatio = outputSize.Width / outputSize.Height;
	float fovAngleY = 70.0f * XM_PI / 180.0f;


	m_camera->SetProjParams(fovAngleY, aspectRatio, 0.01f, 100.0f);

	m_Stage->LoadDeviceResources();


	// Make the constant buffer here	
	CD3D11_BUFFER_DESC constantBufferDesc(sizeof(ModelViewProjectionConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
	DX::ThrowIfFailed(
		m_deviceResources->GetD3DDevice()->CreateBuffer(
			&constantBufferDesc,
			nullptr,
			&m_constantBuffer
			)
		);

	m_loadingComplete = true;
}

void Sample3DSceneRenderer::ReleaseDeviceDependentResources()
{
	m_loadingComplete = false;

	m_constantBuffer.Reset();

}