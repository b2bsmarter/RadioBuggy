//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

#pragma once

// Camera:
// This class defines the position, orientation and viewing frustum of a camera looking into
// a 3D world.  It will generate both the View matrix and Projection matrix.  It can also
// provide a pair of Projection matrices to be used when stereoscopic 3D is used.
#include <DirectXCollision.h>



namespace Simple3DGame
{
	ref class Camera
	{
	internal:
		Camera();

		void SetViewParams(_In_ DirectX::XMFLOAT3 eye, _In_ DirectX::XMFLOAT3 lookAt, _In_ DirectX::XMFLOAT3 up);
		void SetProjParams(_In_ float fieldOfView, _In_ float aspectRatio, _In_ float nearPlane, _In_ float farPlane);

		void SetWindowsSize(Windows::Foundation::Size p_outputSize) { outputSize = p_outputSize; };

		DirectX::XMMATRIX View();
		DirectX::XMMATRIX Projection();
		DirectX::XMMATRIX LeftEyeProjection();
		DirectX::XMMATRIX RightEyeProjection();
		DirectX::XMMATRIX World();
		DirectX::XMFLOAT3 Eye();
		DirectX::XMFLOAT3 LookAt();
		DirectX::XMFLOAT3 Up();
		float NearClipPlane();
		float FarClipPlane();
		float Pitch();
		float Yaw();

		bool bFreeCamera;

		btVector3* GetLookingDirection();

		void SetEyeAtThere(float ex, float ey, float ez, float ax, float ay, float az);
		void GetWorldLine(UINT pixelX, UINT pixelY, DirectX::XMFLOAT3* outPoint, DirectX::XMFLOAT3* outDir);

		DirectX::XMMATRIX* GetMatrix();
		DirectX::XMFLOAT4X4* GetProjectionMatrix();
		DirectX::XMFLOAT4X4* GetViewMatrix();
		void Update(float timeDelta);
		void SetEyeAt(float ex, float ey, float ez, float ax, float ay, float az);
		float EyeX();
		float EyeZ();
		float EyeY();
		float LookingTanX();
		float LookingTanZ();
		bool objectVisible(float *MVPMatrix, const float &xPos, const float &yPos, const float &zPos, const float &radius);
	
		bool CheckPlanes(float x, float y, float z,float radius);

		void ConstructFrustum(FLOAT screenDepth);
		bool CheckPoint(FLOAT x, FLOAT y, FLOAT z, FLOAT radius);
		bool CheckPoint(FLOAT x, FLOAT y, FLOAT z, FLOAT radius, float dist);

		bool CheckPointDist(FLOAT x, FLOAT y, FLOAT z); // just distance check

		bool CheckSphere(FLOAT xCenter, FLOAT yCenter, FLOAT zCenter, FLOAT radius);

		DirectX::XMMATRIX* GetMatrix(DirectX::XMFLOAT4X4* car_view);



		void SetProjectionMatrix(DirectX::XMFLOAT4X4 matr, DirectX::XMFLOAT4X4 orientation)
		{
			m_Orientation = orientation;
			m_projectionMatrix = matr;
		}

		void buildWorldFrustumPlanes();

		float GetX() { return vEyeTo.getX(); }
		float GetZ() { return vEyeTo.getZ(); }
		void CheckTerrain(float height);
	
	protected private:
		DirectX::BoundingFrustum view_clip;
	
		DirectX::XMFLOAT4	mFrustumPlanes[6]; 	// [0] = near, [1] = far, [2] = left, [3] = right, [4] = top, [5] = bottom
		DirectX::XMFLOAT4X4 m_viewMatrix;
		DirectX::XMFLOAT4X4 m_viewMatrix2;
		DirectX::XMFLOAT4X4 m_viewMatrix3; // non inverted
		DirectX::XMFLOAT4X4 m_projectionMatrix;
		DirectX::XMFLOAT4X4 m_projectionMatrixLeft;
		DirectX::XMFLOAT4X4 m_projectionMatrixRight;

		DirectX::XMFLOAT4X4 m_Orientation;

		DirectX::XMFLOAT4X4 m_inverseView;

		Windows::Foundation::Size outputSize;

		DirectX::XMFLOAT3 m_eye;
		DirectX::XMFLOAT3 m_lookAt;
		DirectX::XMFLOAT3 m_up;
		float             m_cameraYawAngle;
		float             m_cameraPitchAngle;

		float             m_fieldOfView;
		float             m_aspectRatio;
		float             m_nearPlane;
		float             m_farPlane;

		DirectX::XMFLOAT4 m_planes[6];

		DirectX::XMVECTOR up;
		/*
		btVector3 EyeAt;
		btVector3 EyeTo;
		btVector3 LookAt;
		btVector3 LookTo;
		*/
		btVector3 vEye, vAt, vEyeTo, vAtTo;
		btVector3 vec_looking_tan;
		btVector3 vec_looking;

	};
}
