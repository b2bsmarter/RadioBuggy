//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

#include "pch.h"
#include "Level.h"
#include "Camera.h"
#include "StereoProjection.h"
#include <DirectXMath.h>



using namespace DirectX;
using namespace Simple3DGame;

#undef min // Use __min instead.
#undef max // Use __max instead.


//--------------------------------------------------------------------------------------

Camera::Camera()
{
	// Setup the view matrix.
	SetViewParams(
		XMFLOAT3(0.0f, 0.0f, 0.0f),   // Default eye position.
		XMFLOAT3(0.0f, 0.0f, 1.0f),   // Default look at position.
		XMFLOAT3(0.0f, 1.0f, 0.0f)    // Default up vector.
		);

	up = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);

	bFreeCamera = false;
	// Setup the projection matrix.
	//SetProjParams(XM_PI / 1, 1.0f, 1.0f, 10.0f);
}


void Camera::buildWorldFrustumPlanes()
{
	//SetProjParams(XM_PI / 8, 1.0f, 1.0f, 10.0f);
	//projectionMatrix = XMMatrixInverse(nullptr, projectionMatrix);
	XMMATRIX identityMatrix = XMMatrixIdentity();
	XMMATRIX projectionMatrix = XMLoadFloat4x4(&m_projectionMatrix);
	XMMATRIX viewMatrix = XMLoadFloat4x4(&m_viewMatrix);
	//projectionMatrix = XMMatrixInverse(nullptr, projectionMatrix);
	//XMMATRIX tran = XMMatrixTranslation(vEye.getX(), vEye.getY(), vEye.getZ());
	viewMatrix = viewMatrix * XMLoadFloat4x4(&m_Orientation);

	//projectionMatrix = XMMatrixMultiply(identityMatrix,projectionMatrix);

	// Note: Extract the frustum planes in world space.
	XMMATRIX VP = XMMatrixMultiply(viewMatrix, projectionMatrix);
	//XMMATRIX tran = XMMatrixTranslation(vEye.getX(), vEye.getY(), vEye.getZ());
	//VP = XMMatrixMultiply(VP, tran);


	//XMMatrixTranslation()
	//VP = XMMatrixInverse(nullptr, VP);
	XMFLOAT4X4 VPvalues;
	XMStoreFloat4x4(&VPvalues, VP);

	XMVECTOR col0 = XMVectorSet(VPvalues(0, 0), VPvalues(1, 0), VPvalues(2, 0), VPvalues(3, 0));
	XMVECTOR col1 = XMVectorSet(VPvalues(0, 1), VPvalues(1, 1), VPvalues(2, 1), VPvalues(3, 1));
	XMVECTOR col2 = XMVectorSet(VPvalues(0, 2), VPvalues(1, 2), VPvalues(2, 2), VPvalues(3, 2));
	XMVECTOR col3 = XMVectorSet(VPvalues(0, 3), VPvalues(1, 3), VPvalues(2, 3), VPvalues(3, 3));

	// Planes face inward.
	XMStoreFloat4(&mFrustumPlanes[0], col2);        // near
	XMStoreFloat4(&mFrustumPlanes[1], col3 - col2); // far
	XMStoreFloat4(&mFrustumPlanes[2], col3 + col0); // left
	XMStoreFloat4(&mFrustumPlanes[3], col3 - col0); // right
	XMStoreFloat4(&mFrustumPlanes[4], col3 - col1); // top
	XMStoreFloat4(&mFrustumPlanes[5], col3 + col1); // bottom

	//mFrustumPlanes[0] = mFrustumPlanes[0] * viewMatrix;

	for (int i = 0; i < 6; i++) // Parallel Potential
		XMStoreFloat4(&mFrustumPlanes[i], XMPlaneNormalize(XMLoadFloat4(&mFrustumPlanes[i])));

}

bool Camera::CheckPlanes(float x, float y, float z,float radius)
{
	int i;
	XMVECTOR dist;
	float f_dist;
	//return true;
	//for (i = 0; i < 6;i++)
	//{vEye.getX(), vEye.getY(), vEye.getZ()
	for (i = 2; i < 4;i++) // 2 to 4 only does left to right
	{
		XMStoreFloat(&f_dist, XMVector3Dot(XMLoadFloat4(&mFrustumPlanes[i]), XMLoadFloat3(&XMFLOAT3(x - vEye.getX(), y - vEye.getY(), z - vEye.getZ()))));
		//XMStoreFloat(&f_dist_right, dist_right);

		if (f_dist >radius)
		{
			return false;
		}
	}
	return true;
}

float Camera::LookingTanX()
{
	return vec_looking_tan.getX();
}

float Camera::LookingTanZ()
{
	return vec_looking_tan.getZ();
}

float Camera::EyeX()
{
	return vEye.getX();
}

float Camera::EyeZ()
{
	return vEye.getZ();
}

float Camera::EyeY()
{
	return vEye.getY();
}



DirectX::XMFLOAT4X4* Camera::GetProjectionMatrix()
{
	return &m_projectionMatrix;
}

btVector3* Camera::GetLookingDirection()
{
	return &vec_looking;

}

void Camera::SetEyeAtThere(float ex, float ey, float ez, float ax, float ay, float az)
{
	vEyeTo.setX(ex);
	vEyeTo.setY(ey);
	vEyeTo.setZ(ez);

	vAtTo.setX(ax);
	vAtTo.setY(ay);
	vAtTo.setZ(az);

	vEye.setX(ex);
	vEye.setY(ey);
	vEye.setZ(ez);

	vAt.setX(ax);
	vAt.setY(ay);
	vAt.setZ(az);

	/*
	eyex = ex;
	eyey = ey;
	eyez = ez;

	eye_to = XMVectorSet(ex, ey, ez, 1.0f);		//      =(XMVECTOR*)eye_new;at=at_new;};
	at_to = XMVectorSet(ax, ay, az, 1.0f);		//      =(XMVECTOR*)eye_new;at=at_new;};
	*/
}

void Camera::SetEyeAt(float ex, float ey, float ez, float ax, float ay, float az)
{
	vEyeTo.setX(ex);
	vEyeTo.setY(ey);
	vEyeTo.setZ(ez);

	vAtTo.setX(ax);
	vAtTo.setY(ay);
	vAtTo.setZ(az);
	/*
	eyex = ex;
	eyey = ey;
	eyez = ez;

	eye_to = XMVectorSet(ex, ey, ez, 1.0f);		//      =(XMVECTOR*)eye_new;at=at_new;};
	at_to = XMVectorSet(ax, ay, az, 1.0f);		//      =(XMVECTOR*)eye_new;at=at_new;};
	*/
}

bool Camera::CheckPointDist(FLOAT x, FLOAT y, FLOAT z)
{
	if (vEye.distance(btVector3(x, y, z)) < FAR_DIST)
	{
		return true;
	}
	else
	{
		return false;
	}

}

bool Camera::CheckPoint(FLOAT x, FLOAT y, FLOAT z, FLOAT radius, float dist)
{
	if (CheckPlanes(x, y, z, radius) == true && vEye.distance(btVector3(x, y, z)) < dist)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool Camera::CheckPoint(FLOAT x, FLOAT y, FLOAT z,FLOAT radius)
{
	if (CheckPlanes(x,y,z, radius)==true && vEye.distance(btVector3(x, y, z)) < FAR_DIST)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool Camera::CheckSphere(FLOAT xCenter, FLOAT yCenter, FLOAT zCenter, FLOAT radius)
{
	// Check if the radius of the sphere is inside the view frustum.
	for (INT i = 0; i < 6; i++)
	{
		if (XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&m_planes[i]), XMLoadFloat4(&XMFLOAT4(xCenter, yCenter, zCenter, 1.0f)))) < -radius)
		//if (true)
		{
			return false;
		}
	}

	return true;
}

DirectX::XMFLOAT4X4* Camera::GetViewMatrix()
{
	return &m_viewMatrix3;

}


void Camera::SetViewParams(
	_In_ XMFLOAT3 eye,
	_In_ XMFLOAT3 lookAt,
	_In_ XMFLOAT3 up
	)
{
	m_eye = eye;
	m_lookAt = lookAt;
	m_up = up;

	// Calculate the view matrix.
	XMMATRIX view = XMMatrixLookAtLH(
		XMLoadFloat3(&m_eye),
		XMLoadFloat3(&m_lookAt),
		XMLoadFloat3(&m_up)
		);

	XMVECTOR det;
	XMMATRIX inverseView = XMMatrixInverse(&det, view);
	XMStoreFloat4x4(&m_viewMatrix, view);
	XMStoreFloat4x4(&m_inverseView, inverseView);

	// The axis basis vectors and camera position are stored inside the
	// position matrix in the 4 rows of the camera's world matrix.
	// To figure out the yaw/pitch of the camera, we just need the Z basis vector.
	XMFLOAT3 zBasis;
	XMStoreFloat3(&zBasis, inverseView.r[2]);

	m_cameraYawAngle = atan2f(zBasis.x, zBasis.z);

	float len = sqrtf(zBasis.z * zBasis.z + zBasis.x * zBasis.x);
	m_cameraPitchAngle = atan2f(zBasis.y, len);
}

DirectX::XMMATRIX* Camera::GetMatrix(DirectX::XMFLOAT4X4* car_view)
{
	SetViewParams(
		XMFLOAT3(vEye.getX(), vEye.getY(), vEye.getZ()),   // Default eye position.
		XMFLOAT3(vAt.getX(), vAt.getY(), vAt.getZ()),   // Default look at position.
		XMFLOAT3(0.0f, 1.0f, 0.01f)    // Default up vector.
		);
	XMVECTOR det;
	XMMATRIX view_temp = XMMatrixInverse(&det, XMLoadFloat4x4(car_view));
	view_temp = XMMatrixRotationRollPitchYaw(M_PI, 0.0f, M_PI) * view_temp;

	XMStoreFloat4x4(&m_viewMatrix3, view_temp);
	//XMStoreFloat4x4(&m_viewMatrix, view_temp);
	
	XMMATRIX inverseView = XMMatrixInverse(&det, view_temp);
	XMStoreFloat4x4(&m_viewMatrix2, inverseView);

	XMMATRIX prjident = XMMatrixIdentity();

	view_temp = XMMatrixMultiply(prjident, view_temp);

	buildWorldFrustumPlanes();
	return &view_temp;
}

DirectX::XMMATRIX* Camera::GetMatrix()
{
	SetViewParams(
		XMFLOAT3(vEye.getX(), vEye.getY(), vEye.getZ()),   // Default eye position.
		XMFLOAT3(vAt.getX(), vAt.getY(), vAt.getZ()),   // Default look at position.
		XMFLOAT3(0.0f, 1.0f, 0.01f)    // Default up vector.
		);
		
	XMMATRIX view_temp = XMMatrixTranspose(XMMatrixLookAtRH(XMVectorSet(vEye.getX(), vEye.getY(), vEye.getZ(), 1.0f), XMVectorSet(vAt.getX(), vAt.getY(), vAt.getZ(), 1.0f), up));
	XMStoreFloat4x4(&m_viewMatrix3, view_temp);
	//XMStoreFloat4x4(&m_viewMatrix, view_temp);
	XMVECTOR det;
	XMMATRIX inverseView = XMMatrixInverse(&det, view_temp);
	XMStoreFloat4x4(&m_viewMatrix2, inverseView);

	XMMATRIX prjident = XMMatrixIdentity();

	view_temp = XMMatrixMultiply(prjident, view_temp);

	buildWorldFrustumPlanes();
	return &view_temp;
}



void Camera::GetWorldLine(UINT pixelX, UINT pixelY, DirectX::XMFLOAT3* outPoint, DirectX::XMFLOAT3* outDir)
{
	//pixelY = (float)outputSize.Height - pixelY;
	//SetProjParams(XM_PI / 8, 1.0f, 1.0f, 10.0f);
	//projectionMatrix = XMMatrixInverse(nullptr, projectionMatrix);
	XMMATRIX identityMatrix = XMMatrixIdentity();
	XMMATRIX projectionMatrix = XMLoadFloat4x4(&m_projectionMatrix);
	XMMATRIX viewMatrix = XMLoadFloat4x4(&m_viewMatrix2);
	//viewMatrix = XMMatrixInverse(nullptr, viewMatrix);
	//XMMATRIX tran = XMMatrixTranslation(vEye.getX(), vEye.getY(), vEye.getZ());
	viewMatrix = viewMatrix * XMLoadFloat4x4(&m_Orientation);

	projectionMatrix = XMMatrixMultiply(identityMatrix, projectionMatrix);
	viewMatrix = XMMatrixMultiply(identityMatrix, viewMatrix);

	DirectX::XMFLOAT4 p0 = DirectX::XMFLOAT4((float)pixelX, (float)pixelY, 0, 1);
	DirectX::XMFLOAT4 p1 = DirectX::XMFLOAT4((float)pixelX, (float)pixelY, 1, 1);

	DirectX::XMVECTOR screen0 = DirectX::XMLoadFloat4(&p0);
	DirectX::XMVECTOR screen1 = DirectX::XMLoadFloat4(&p1);

	DirectX::XMMATRIX projMat = projectionMatrix;
	DirectX::XMMATRIX viewMat = viewMatrix;

	DirectX::XMVECTOR pp0 = DirectX::XMVector3Unproject(screen0, 0, 0, (float)outputSize.Width, (float)outputSize.Height, 0, 1, projMat, viewMat, DirectX::XMMatrixIdentity());
	DirectX::XMVECTOR pp1 = DirectX::XMVector3Unproject(screen1, 0, 0, (float)outputSize.Width, (float)outputSize.Height, 0, 1, projMat, viewMat, DirectX::XMMatrixIdentity());

	DirectX::XMStoreFloat3(outPoint, pp0);
	DirectX::XMStoreFloat3(outDir, pp1);

	outDir->x -= outPoint->x;
	outDir->y -= outPoint->y;
	outDir->z -= outPoint->z;
}



void Camera::ConstructFrustum(FLOAT screenDepth)
{

	XMMATRIX projectionMatrix = XMLoadFloat4x4(&m_projectionMatrix);
	XMMATRIX viewMatrix = XMLoadFloat4x4(&m_viewMatrix);
	XMMATRIX prjident = XMMatrixIdentity();
	projectionMatrix = XMMatrixMultiply(prjident, projectionMatrix);


	//projectionMatrix = XMMatrixInverse(nullptr, projectionMatrix);
	
	XMMATRIX viewProjection = XMMatrixMultiply(viewMatrix, projectionMatrix);
	//viewProjection = XMMatrixInverse(nullptr, viewProjection);
	//BoundingFrustum::CreateFromMatrix(view_clip, viewProjection);

	BoundingFrustum view_clip = BoundingFrustum();
	view_clip.CreateFromMatrix(view_clip, viewProjection);
	//view_clip.Transform(view_clip, viewMatrix);

	//view_clip.Transform(view_clip, viewMatrix);

	return;
}

void Camera::CheckTerrain(float height)
{
	
	if (height > vEyeTo.getY())
		vEyeTo.setY(height);
}


void Camera::Update(float timeDelta)
{
	//eye = eye_to;
	//at = at_to;    
	//XMVectorSetX(at,XMVectorGetX(at_to));
	btVector3 hvEyeMove;
	btVector3 hvAtMove;

	btVector3 hvEye = btVector3(vEyeTo.getX(), 0.0f, vEyeTo.getZ());
	btVector3 hvAt = btVector3(vAtTo.getX(), 0.0f, vAtTo.getZ());
	btVector3 vDir = hvAt - hvEye;
	
	/*
	if (vDir.length() > 1.0f)
	{
		vDir.normalize();
		hvEye = hvAt - (vDir*1.0f);
		vEyeTo.setX(hvEye.getX());
		vEyeTo.setZ(hvEye.getZ());
	}
	*/

	if (bFreeCamera == true)
	{

		vEye = vEyeTo;
		vAt = vAtTo;

	}
	else
	{
		hvEyeMove = (vEyeTo - vEye)*(timeDelta*4.5);
		hvAtMove = (vAtTo - vAt)*(timeDelta*6.5);

		vEye += hvEyeMove;
		vAt += hvAtMove;

	}
	/*
	eye += (eye_to - eye)*(timeDelta*3.5);
	at += (at_to - at)*(timeDelta*3.5);

	eye += (eye_to - eye)*(timeDelta*3.5);
	*/
	//at=at_to;
	//XMMATRIX mat = XMMatrixTranspose(XMMatrixLookAtRH(eye, at, up));
	/*
	eyex = XMVectorGetX(eye);
	eyey = XMVectorGetY(eye);
	eyez = XMVectorGetZ(eye);
	*/
	// Create tangent for point sprites
	vec_looking = vAt - vEye;
	vec_looking.setY(0.0f);
	vec_looking.normalize();
	vec_looking_tan = vec_looking.rotate(btVector3(0.0f, 1.0f, 0.0f), -3.14159*0.5f);

	/*
	char str[40];
	sprintf_s(str , "%f  %f  %f\n" , eyex , eyey , eyez );

	OutputDebugStringA(str);
	*/

}


//--------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------




//--------------------------------------------------------------------------------------

void Camera::SetProjParams(
	_In_ float fieldOfView,
	_In_ float aspectRatio,
	_In_ float nearPlane,
	_In_ float farPlane
	)
{
	return;
	// Set attributes for the projection matrix.
	m_fieldOfView = fieldOfView;
	m_aspectRatio = aspectRatio;
	m_nearPlane = nearPlane;
	m_farPlane = farPlane;
	
	XMStoreFloat4x4(
		&m_projectionMatrix,
		XMMatrixPerspectiveFovLH(
			m_fieldOfView,
			m_aspectRatio,
			m_nearPlane,
			m_farPlane
			)
		);
		
	STEREO_PARAMETERS* stereoParams = nullptr;
	// Update the projection matrix.
	XMStoreFloat4x4(
		&m_projectionMatrixLeft,
		MatrixStereoProjectionFovLH(
			stereoParams,
			STEREO_CHANNEL::LEFT,
			m_fieldOfView,
			m_aspectRatio,
			m_nearPlane,
			m_farPlane,
			STEREO_MODE::NORMAL
			)
		);

	XMStoreFloat4x4(
		&m_projectionMatrixRight,
		MatrixStereoProjectionFovLH(
			stereoParams,
			STEREO_CHANNEL::RIGHT,
			m_fieldOfView,
			m_aspectRatio,
			m_nearPlane,
			m_farPlane,
			STEREO_MODE::NORMAL
			)
		);
}

//--------------------------------------------------------------------------------------

DirectX::XMMATRIX Camera::View()
{
	return XMLoadFloat4x4(&m_viewMatrix);
}

//--------------------------------------------------------------------------------------

DirectX::XMMATRIX Camera::Projection()
{
	return XMLoadFloat4x4(&m_projectionMatrix);
}

//--------------------------------------------------------------------------------------

DirectX::XMMATRIX Camera::LeftEyeProjection()
{
	return XMLoadFloat4x4(&m_projectionMatrixLeft);
}

//--------------------------------------------------------------------------------------

DirectX::XMMATRIX Camera::RightEyeProjection()
{
	return XMLoadFloat4x4(&m_projectionMatrixRight);
}

//--------------------------------------------------------------------------------------

DirectX::XMMATRIX Camera::World()
{
	return XMLoadFloat4x4(&m_inverseView);
}

//--------------------------------------------------------------------------------------

DirectX::XMFLOAT3 Camera::Eye()
{
	return m_eye;
}

//--------------------------------------------------------------------------------------

DirectX::XMFLOAT3 Camera::LookAt()
{
	return m_lookAt;
}

//--------------------------------------------------------------------------------------

float Camera::NearClipPlane()
{
	return m_nearPlane;
}

//--------------------------------------------------------------------------------------

float Camera::FarClipPlane()
{
	return m_farPlane;
}

//--------------------------------------------------------------------------------------

float Camera::Pitch()
{
	return m_cameraPitchAngle;
}

//--------------------------------------------------------------------------------------

float Camera::Yaw()
{
	return m_cameraYawAngle;
}

//--------------------------------------------------------------------------------------