#include "pch.h"
#include "WorldObject.h"

using namespace Simple3DGame;
using namespace DirectX;
using namespace DX;
using namespace std;

WorldObject::WorldObject(std::shared_ptr<DX::DeviceResources> pm_deviceResources, Physics* phys)
{
	bDeactivated = false;
	m_phys = phys;
	m_deviceResources = pm_deviceResources;
}


WorldObject::~WorldObject()
{
	if (m_Texture!=nullptr)
	{
		m_Texture->Release();
	}
	if (m_model != NULL)
		m_model->Clearmemory();
}


void WorldObject::SetBuffers(Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer, Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer, UINT indexCount)
{
	m_vertexBuffer = vertexBuffer;
	m_indexBuffer = indexBuffer;
	m_indexCount = indexCount;
}

void WorldObject::Initialize(char* model_filename, wchar_t* texture_filename, ObjInfo* info)
{
	initial_z = info->PosX;

	if (bTexture == 0)
	{
		CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), texture_filename, nullptr, &m_Texture, MAXSIZE_T);
	}
	else
	{
		m_Texture = NULL;
	}

	if (m_model == NULL)
	{
		m_model = new ModelLoader(m_deviceResources);
		if (info->optimize == 1)
		{
			m_model->Load(model_filename, info->Scale,1);
		}
		else
		{
			m_model->Load(model_filename, info->Scale);
		}
		
	}
	if (false)//(info->raise_by_height == true)
	{
		info->PosY += m_model->total_height;
	}

	m_initialTransform = btTransform(btQuaternion(info->Yaw, info->Pitch, info->Roll), btVector3(info->PosX, info->PosY /*+ (m_model->total_height*0.5f)*/, info->PosZ));

	SetBuffers(m_model->GetVertexBuffer(), m_model->GetIndexBuffer(), m_model->GetIndicesCount());

	setWorldTransform(m_initialTransform);

}

void WorldObject::SetPositionMatrix(DirectX::XMFLOAT4X4 _model_matrix)
{
	btMatrix3x3 mat = btMatrix3x3(_model_matrix._11, _model_matrix._12, _model_matrix._13, _model_matrix._21, _model_matrix._22, _model_matrix._23, _model_matrix._31, _model_matrix._32, _model_matrix._33);

	//btQuaternion new_q = btQuaternion(yaw, pitch, roll);
	m_initialTransform = btTransform(mat, btVector3(_model_matrix._14, _model_matrix._24, _model_matrix._34));
	//m_initialTransform = btTransform(btQuaternion(yaw, pitch, roll), btVector3(x, y /*+ (m_model->total_height*0.5f)*/, z));
	m_rigidbody->setWorldTransform(m_initialTransform);
	setWorldTransform(m_initialTransform);
}

void WorldObject::SetPosition(float x, float y, float z, float yaw, float pitch, float roll)
{
	//btQuaternion new_q = btQuaternion(yaw, pitch, roll);
	
	m_initialTransform = btTransform(btQuaternion(yaw, pitch, roll), btVector3(x, y /*+ (m_model->total_height*0.5f)*/, z));
	m_rigidbody->setWorldTransform(m_initialTransform);
	setWorldTransform(m_initialTransform);
}


void WorldObject::getWorldTransform(btTransform &worldTransform) const
{
	worldTransform = m_initialTransform;
}

void WorldObject::setWorldTransform(const btTransform &worldTransform)
{
	auto rotation = worldTransform.getRotation();

	auto transform = ref new Transform(worldTransform.getOrigin().getX(),
		worldTransform.getOrigin().getY(),
		worldTransform.getOrigin().getZ(),
		rotation.getX(), rotation.getY(), rotation.getZ(), rotation.getW());

	SetTransform(transform);
}


void WorldObject::SetTransform(const Transform^ transform)
{
	XMVECTOR data;
	XMVECTORF32 floatingVector = { transform->quatX, transform->quatY, transform->quatZ, transform->quatW };
	data = floatingVector;

	//	XMMATRIX scaleMatrix = XMMatrixScaling(1.0f , 1.0f, 1.0f);

	auto rotationMatrix = XMMatrixRotationQuaternion(data);
	auto translationMatrix = XMMatrixTranslation(transform->locX, transform->locY, transform->locZ);

	//XMMATRIX scaleRotationMatrix = XMMatrixMultiply(scaleMatrix, rotationMatrix);
	//XMMATRIX modelMatrix = XMMatrixMultiplyTranspose(scaleRotationMatrix, translationMatrix);
	//m_constantBuffer.model = 
	XMStoreFloat4x4(&m_constantBufferData.model, XMMatrixTranspose(rotationMatrix * translationMatrix));
	//XMStoreFloat4x4( &m_constantBuffer.model,modelMatrix);
}


void WorldObject::UpdateProjectionMatrix(XMMATRIX *projectionMatrix)
{
	//	XMStoreFloat4x4(&m_constantBufferData.projection, *projectionMatrix);
}

void WorldObject::SetTexturePointer(ID3D11ShaderResourceView* p_Tex)
{
	m_Texture = p_Tex;
}

ID3D11ShaderResourceView* WorldObject::GetTexture()
{
	return m_Texture;
}

void WorldObject::SetTexture(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> deviceContext)
{
	deviceContext->PSSetShaderResources(0, 1, &m_Texture);


}

void WorldObject::ResetTransform()
{
	m_rigidbody->setWorldTransform(m_initialTransform);
}


void WorldObject::RenderYaw(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> deviceContext, Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer, ModelViewProjectionConstantBuffer* constantBufferData,float yaw)
{
	UINT stride = sizeof(VertexPositionTex);
	UINT offset = 0;

	//XMMATRIX m_modelMatrix;
	///XMVECTOR data;
	//XMMATRIX m_InvMatrix = XMMatrixScaling(&data, XMLoadFloat4x4(&m_constantBuffer.model));

	//m_constantBuffer.model.m[3][0]=0.0f;
	//m_constantBuffer.model.m[3][1]=0.0f;
	//m_constantBuffer.model.m[3][2]=0.0f;
	//m_constantBuffer.model.m[3][3]=0.0f;
	//XMMatrixReflect(FMVECTOR3
	//XMMatrixTransformation

	//XMStoreFloat4x4(&m_constantBuffer.model, m_InvMatrix);

	deviceContext->PSSetShaderResources(0, 1, &m_Texture);

	deviceContext->IASetVertexBuffers(0, 1, m_model->GetVertexBuffer().GetAddressOf(), &stride, &offset);
	deviceContext->IASetIndexBuffer(m_model->GetIndexBuffer().Get(), DXGI_FORMAT_R16_UINT, 0);


	btTransform trans;
	this->getWorldTransform(trans);
	trans = m_rigidbody->getWorldTransform();


	btQuaternion rotation = trans.getRotation();

	XMVECTOR data;
	XMVECTORF32 floatingVector = { rotation.getX(), rotation.getY(), rotation.getZ(), rotation.getW() };
	data = floatingVector;

	auto rotationMatrix = XMMatrixRotationRollPitchYaw(0.0f, yaw, 0.0f);

	auto translationMatrix = XMMatrixTranslation(trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ());

	//	auto rotationMatrix = XMMatrixRotationRollPitchYaw(0.0f, 0.0f, 0.0f);
	//	auto translationMatrix = XMMatrixTranslation(0.0f, 6.0f, 20.0f);

	XMStoreFloat4x4(&m_constantBufferData.model, XMMatrixTranspose(rotationMatrix * translationMatrix));
	constantBufferData->model = m_constantBufferData.model;
	//constantBufferData->model = m_constantBuffer.model;

	deviceContext->UpdateSubresource(constantBuffer.Get(), 0, NULL, constantBufferData, 0, 0);

	deviceContext->DrawIndexed(m_indexCount, 0, 0);
}


void WorldObject::Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> deviceContext, Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer, ModelViewProjectionConstantBuffer* constantBufferData)
{
	UINT stride = sizeof(VertexPositionTex);
	UINT offset = 0;

	//XMMATRIX m_modelMatrix;
	///XMVECTOR data;
	//XMMATRIX m_InvMatrix = XMMatrixScaling(&data, XMLoadFloat4x4(&m_constantBuffer.model));

	//m_constantBuffer.model.m[3][0]=0.0f;
	//m_constantBuffer.model.m[3][1]=0.0f;
	//m_constantBuffer.model.m[3][2]=0.0f;
	//m_constantBuffer.model.m[3][3]=0.0f;
	//XMMatrixReflect(FMVECTOR3
	//XMMatrixTransformation
	 
	//XMStoreFloat4x4(&m_constantBuffer.model, m_InvMatrix);
	if(m_Texture!=NULL)
	{
		deviceContext->PSSetShaderResources(0, 1, &m_Texture);
		deviceContext->IASetVertexBuffers(0, 1, m_model->GetVertexBuffer().GetAddressOf(), &stride, &offset);
		deviceContext->IASetIndexBuffer(m_model->GetIndexBuffer().Get(), DXGI_FORMAT_R16_UINT, 0);
	}

	btTransform trans;
	//this->getWorldTransform(trans);
	trans = m_rigidbody->getWorldTransform();


	btQuaternion rotation = trans.getRotation();

	XMVECTOR data;
	XMVECTORF32 floatingVector = { rotation.getX(), rotation.getY(), rotation.getZ(), rotation.getW() };
	data = floatingVector;
	
	auto rotationMatrix = XMMatrixRotationQuaternion(data);
	auto translationMatrix = XMMatrixTranslation(trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ());

	//	auto rotationMatrix = XMMatrixRotationRollPitchYaw(0.0f, 0.0f, 0.0f);
	//	auto translationMatrix = XMMatrixTranslation(0.0f, 6.0f, 20.0f);

	XMStoreFloat4x4(&m_constantBufferData.model, XMMatrixTranspose(rotationMatrix * translationMatrix));
	constantBufferData->model = m_constantBufferData.model;
	//constantBufferData->model = m_constantBuffer.model;

	deviceContext->UpdateSubresource(constantBuffer.Get(), 0, NULL, constantBufferData, 0, 0);

	deviceContext->DrawIndexed(m_indexCount, 0, 0);
}

void WorldObject::RenderOffset(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> deviceContext, Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer, ModelViewProjectionConstantBuffer* constantBufferData, float x,float y,float z)
{
	UINT stride = sizeof(VertexPositionTex);
	UINT offset = 0;

	//XMMATRIX m_modelMatrix;
	///XMVECTOR data;
	//XMMATRIX m_InvMatrix = XMMatrixScaling(&data, XMLoadFloat4x4(&m_constantBuffer.model));

	//m_constantBuffer.model.m[3][0]=0.0f;
	//m_constantBuffer.model.m[3][1]=0.0f;
	//m_constantBuffer.model.m[3][2]=0.0f;
	//m_constantBuffer.model.m[3][3]=0.0f;
	//XMMatrixReflect(FMVECTOR3
	//XMMatrixTransformation

	//XMStoreFloat4x4(&m_constantBuffer.model, m_InvMatrix);

	deviceContext->PSSetShaderResources(0, 1, &m_Texture);

	deviceContext->IASetVertexBuffers(0, 1, m_model->GetVertexBuffer().GetAddressOf(), &stride, &offset);
	deviceContext->IASetIndexBuffer(m_model->GetIndexBuffer().Get(), DXGI_FORMAT_R16_UINT, 0);


	btTransform trans;
	this->getWorldTransform(trans);
	trans = m_rigidbody->getWorldTransform();


	btQuaternion rotation = trans.getRotation();

	XMVECTOR data;
	XMVECTORF32 floatingVector = { rotation.getX(), rotation.getY(), rotation.getZ(), rotation.getW() };
	data = floatingVector;

	auto scaleMatrix = XMMatrixScaling(1.5f, 1.5f, 1.5f);

	auto rotationMatrix = XMMatrixRotationQuaternion(data);
	auto translationMatrix = XMMatrixTranslation(trans.getOrigin().getX()+x, trans.getOrigin().getY()+y, trans.getOrigin().getZ()+z);



	//	auto rotationMatrix = XMMatrixRotationRollPitchYaw(0.0f, 0.0f, 0.0f);
	//	auto translationMatrix = XMMatrixTranslation(0.0f, 6.0f, 20.0f);


	XMStoreFloat4x4(&m_constantBufferData.model, XMMatrixTranspose(scaleMatrix * rotationMatrix * translationMatrix ));
	constantBufferData->model = m_constantBufferData.model;
	//constantBufferData->model = m_constantBuffer.model;

	deviceContext->UpdateSubresource(constantBuffer.Get(), 0, NULL, constantBufferData, 0, 0);

	deviceContext->DrawIndexed(m_indexCount, 0, 0);
}







btRigidBody*  WorldObject::MakeObject(ObjInfo* info, int type)
{
	switch (type)
	{
	case 1:	return MakePhysicsBoxFromMesh(info); break;
	case 2:	return MakePhysicsSphereFromMesh(info); break;
	case 3: return MakePhysicsCylinderFromMesh(info); break;
	default:return NULL;
	}


}

btRigidBody*  WorldObject::MakePhysicsBoxFromMeshLoopSpecial(ObjInfo* info)
{
	Initialize(info->ObjFilename, info->TextureFilename, info);

	item_id = info->item_id;

	btVector3 A;
	btVector3 B;
	btVector3 C;

	float extent1 = 0.0f;
	float extent2 = 0.0f;
	float extent3 = 0.0f;

	//btTriangleMesh* data = new btTriangleMesh();

	for (int i = 0; i<m_model->GetVerticesCount() - 3; i += 3)
	{
		if (abs(m_model->m_phy_vertices[i].x) > extent1)
			extent1 = abs(m_model->m_phy_vertices[i].x);
		if (abs(m_model->m_phy_vertices[i + 1].x) > extent1)
			extent1 = abs(m_model->m_phy_vertices[i + 1].x);
		if (abs(m_model->m_phy_vertices[i + 2].x) > extent1)
			extent1 = abs(m_model->m_phy_vertices[i + 2].x);

		if (abs(m_model->m_phy_vertices[i].y) > extent2)
			extent2 = abs(m_model->m_phy_vertices[i].y);
		if (abs(m_model->m_phy_vertices[i + 1].y) > extent2)
			extent2 = abs(m_model->m_phy_vertices[i + 1].y);
		if (abs(m_model->m_phy_vertices[i + 2].y) > extent2)
			extent2 = abs(m_model->m_phy_vertices[i + 2].y);

		if (abs(m_model->m_phy_vertices[i].z) > extent3)
			extent3 = abs(m_model->m_phy_vertices[i].z);
		if (abs(m_model->m_phy_vertices[i + 1].z) > extent3)
			extent3 = abs(m_model->m_phy_vertices[i + 1].z);
		if (abs(m_model->m_phy_vertices[i + 2].z) > extent3)
			extent3 = abs(m_model->m_phy_vertices[i + 2].z);

	}
	extent1 = extent1 - 1.5f;
	auto fallShape = new btBoxShape(btBoxShape(btVector3(extent1, extent2, extent3)));
	//auto fallShape = new btSphereShape(btSphereShape(1.5f));
	//auto fallShape = new btTriangleMesh (btSphereShape(1.5f));


	btMotionState* fallMotionState = (btMotionState*)this;//(cube, btTransform(btQuaternion(0,0,0,1),btVector3(x,y,z)));
														  //btScalar mass = 0.2;
	btVector3 fallInertia(0, 0, 0);

	fallShape->calculateLocalInertia(info->Mass, fallInertia);
	m_rigidbody = m_phys->AddPhysicalObject(fallShape, fallMotionState, info->Mass, fallInertia, info->Restitution, info->Friction, &obj_id, info);
	m_rigidbody->setActivationState(WANTS_DEACTIVATION);
	return m_rigidbody;
}


btRigidBody*  WorldObject::MakePhysicsBoxFromMesh(ObjInfo* info,float _scale)
{
	Initialize(info->ObjFilename, info->TextureFilename, info);

	item_id = info->item_id;


	btVector3 A;
	btVector3 B;
	btVector3 C;

	float extent1 = 0.0f;
	float extent2 = 0.0f;
	float extent3 = 0.0f;

	//btTriangleMesh* data = new btTriangleMesh();

	for (int i = 0; i<m_model->GetVerticesCount() - 3; i += 3)
	{
		if (abs(m_model->m_phy_vertices[i].x) > extent1)
			extent1 = abs(m_model->m_phy_vertices[i].x);
		if (abs(m_model->m_phy_vertices[i + 1].x) > extent1)
			extent1 = abs(m_model->m_phy_vertices[i + 1].x);
		if (abs(m_model->m_phy_vertices[i + 2].x) > extent1)
			extent1 = abs(m_model->m_phy_vertices[i + 2].x);

		if (abs(m_model->m_phy_vertices[i].y) > extent2)
			extent2 = abs(m_model->m_phy_vertices[i].y);
		if (abs(m_model->m_phy_vertices[i + 1].y) > extent2)
			extent2 = abs(m_model->m_phy_vertices[i + 1].y);
		if (abs(m_model->m_phy_vertices[i + 2].y) > extent2)
			extent2 = abs(m_model->m_phy_vertices[i + 2].y);

		if (abs(m_model->m_phy_vertices[i].z) > extent3)
			extent3 = abs(m_model->m_phy_vertices[i].z);
		if (abs(m_model->m_phy_vertices[i + 1].z) > extent3)
			extent3 = abs(m_model->m_phy_vertices[i + 1].z);
		if (abs(m_model->m_phy_vertices[i + 2].z) > extent3)
			extent3 = abs(m_model->m_phy_vertices[i + 2].z);

	}
	extent1 = extent1*_scale;
	extent2 = extent2*_scale;
	extent3 = extent3*_scale;

	auto fallShape = new btBoxShape(btBoxShape(btVector3(extent1, extent2, extent3)));
	//auto fallShape = new btSphereShape(btSphereShape(1.5f));
	//auto fallShape = new btTriangleMesh (btSphereShape(1.5f));
	btMotionState* fallMotionState = (btMotionState*)this;//(cube, btTransform(btQuaternion(0,0,0,1),btVector3(x,y,z)));
														  //btScalar mass = 0.2;
	btVector3 fallInertia(0, 0, 0);

	fallShape->calculateLocalInertia(info->Mass, fallInertia);
	m_rigidbody = m_phys->AddPhysicalObject(fallShape, fallMotionState, info->Mass, fallInertia, info->Restitution, info->Friction, &obj_id, info);
	m_rigidbody->setActivationState(WANTS_DEACTIVATION);


	//delete data;

	return m_rigidbody;
}



btRigidBody*  WorldObject::MakePhysicsCompoundBoxFromMesh(ObjInfo* info, float x,float y,float z)
{
	Initialize(info->ObjFilename, info->TextureFilename, info);

	item_id = info->item_id;

	btVector3 A;
	btVector3 B;
	btVector3 C;

	float extent1 = 0.0f;
	float extent2 = 0.0f;
	float extent3 = 0.0f;

	//btTriangleMesh* data = new btTriangleMesh();

	for (int i = 0; i<m_model->GetVerticesCount() - 3; i += 3)
	{
		if (abs(m_model->m_phy_vertices[i].x) > extent1)
			extent1 = abs(m_model->m_phy_vertices[i].x);
		if (abs(m_model->m_phy_vertices[i + 1].x) > extent1)
			extent1 = abs(m_model->m_phy_vertices[i + 1].x);
		if (abs(m_model->m_phy_vertices[i + 2].x) > extent1)
			extent1 = abs(m_model->m_phy_vertices[i + 2].x);

		if (abs(m_model->m_phy_vertices[i].y) > extent2)
			extent2 = abs(m_model->m_phy_vertices[i].y);
		if (abs(m_model->m_phy_vertices[i + 1].y) > extent2)
			extent2 = abs(m_model->m_phy_vertices[i + 1].y);
		if (abs(m_model->m_phy_vertices[i + 2].y) > extent2)
			extent2 = abs(m_model->m_phy_vertices[i + 2].y);

		if (abs(m_model->m_phy_vertices[i].z) > extent3)
			extent3 = abs(m_model->m_phy_vertices[i].z);
		if (abs(m_model->m_phy_vertices[i + 1].z) > extent3)
			extent3 = abs(m_model->m_phy_vertices[i + 1].z);
		if (abs(m_model->m_phy_vertices[i + 2].z) > extent3)
			extent3 = abs(m_model->m_phy_vertices[i + 2].z);

	}
	extent2 -= abs(y)*0.5f;


	btCollisionShape* boxShape = new btBoxShape(btVector3(extent1, extent2, extent3));
	//btCollisionShape* boxShape = new btSphereShape(2.0f);
	btCompoundShape* compound = new btCompoundShape();
	btTransform localTrans;
	localTrans.setIdentity();
	//localTrans effectively shifts the center of mass with respect to the chassis
	localTrans.setOrigin(btVector3(x, y, z));
	compound->addChildShape(localTrans, boxShape);


	//auto fallShape = new btBoxShape(btBoxShape(btVector3(extent1, extent2, extent3)));
	//auto fallShape = new btSphereShape(btSphereShape(1.5f));
	//auto fallShape = new btTriangleMesh (btSphereShape(1.5f));
	btMotionState* fallMotionState = (btMotionState*)this;//(cube, btTransform(btQuaternion(0,0,0,1),btVector3(x,y,z)));
														  //btScalar mass = 0.2;
	btVector3 fallInertia(0, 0, 0);

	compound->calculateLocalInertia(info->Mass, fallInertia);
	m_rigidbody = m_phys->AddPhysicalObject(compound, fallMotionState, info->Mass, fallInertia, info->Restitution, info->Friction, &obj_id, info);
	m_rigidbody->setActivationState(WANTS_DEACTIVATION);

	//delete data;


	return m_rigidbody;
}



btRigidBody* WorldObject::MakePhysicsSphereFromMesh(ObjInfo* info, float _scale)
{
	Initialize(info->ObjFilename, info->TextureFilename, info);

	item_id = info->item_id;

	btVector3 A;
	btVector3 B;
	btVector3 C;

	float radius1 = 0.0f;

	//btTriangleMesh* data = new btTriangleMesh();

	for (int i = 0; i<m_model->GetVerticesCount() - 3; i += 3)
	{
		if (abs(m_model->m_phy_vertices[i].x) > radius1)
			radius1 = abs(m_model->m_phy_vertices[i].x);
		if (abs(m_model->m_phy_vertices[i + 1].x) > radius1)
			radius1 = abs(m_model->m_phy_vertices[i + 1].x);
		if (abs(m_model->m_phy_vertices[i + 2].x) > radius1)
			radius1 = abs(m_model->m_phy_vertices[i + 2].x);

		if (abs(m_model->m_phy_vertices[i].y) > radius1)
			radius1 = abs(m_model->m_phy_vertices[i].y);
		if (abs(m_model->m_phy_vertices[i + 1].y) > radius1)
			radius1 = abs(m_model->m_phy_vertices[i + 1].y);
		if (abs(m_model->m_phy_vertices[i + 2].y) > radius1)
			radius1 = abs(m_model->m_phy_vertices[i + 2].y);

		if (abs(m_model->m_phy_vertices[i].z) > radius1)
			radius1 = abs(m_model->m_phy_vertices[i].z);
		if (abs(m_model->m_phy_vertices[i + 1].z) > radius1)
			radius1 = abs(m_model->m_phy_vertices[i + 1].z);
		if (abs(m_model->m_phy_vertices[i + 2].z) > radius1)
			radius1 = abs(m_model->m_phy_vertices[i + 2].z);
	}
	radius1 = radius1 * _scale;

	//btMotionState ms;
	//ms.

	auto fallShape = new btSphereShape(btSphereShape(radius1));
	//auto fallShape = new btSphereShape(btSphereShape(1.5f));
	//auto fallShape = new btTriangleMesh (btSphereShape(1.5f));
	btMotionState* fallMotionState = (btMotionState*)this;//(cube, btTransform(btQuaternion(0,0,0,1),btVector3(x,y,z)));
														  //btScalar mass = 0.2;
	btVector3 fallInertia(0, 0, 0);
	//fallMotionState->
	//btSphereShape::
	//m_rigidbody = null;
	fallShape->calculateLocalInertia(info->Mass, fallInertia);
	m_rigidbody = m_phys->AddPhysicalObject(fallShape, fallMotionState, info->Mass, fallInertia, info->Restitution, info->Friction, &obj_id, info);
	//m_rigidbody->setActivationState(WANTS_DEACTIVATION);
	//delete data;

	return m_rigidbody;

}


// overloaded function specify radius
btRigidBody* WorldObject::MakePhysicsSphereFromMeshRadius(ObjInfo* info, float radius)
{
	Initialize(info->ObjFilename, info->TextureFilename, info);

	item_id = info->item_id;

	btVector3 A;
	btVector3 B;
	btVector3 C;

	float radius1 = 0.0f;

	//btTriangleMesh* data = new btTriangleMesh();

	auto fallShape = new btSphereShape(btSphereShape(radius));
	//auto fallShape = new btSphereShape(btSphereShape(1.5f));
	//auto fallShape = new btTriangleMesh (btSphereShape(1.5f));
	btMotionState* fallMotionState = (btMotionState*)this;//(cube, btTransform(btQuaternion(0,0,0,1),btVector3(x,y,z)));
														  //btScalar mass = 0.2;
	btVector3 fallInertia(0, 0, 0);
	//fallMotionState->
	//btSphereShape::

	fallShape->calculateLocalInertia(info->Mass, fallInertia);
	m_rigidbody = m_phys->AddPhysicalObject(fallShape, fallMotionState, info->Mass, fallInertia, info->Restitution, info->Friction, &obj_id, info);
	m_rigidbody->setActivationState(WANTS_DEACTIVATION);

	//delete data;

	return m_rigidbody;

}



//btRigidBody*  Planet::MakePhysicsConvexMesh(Microsoft::WRL::ComPtr<ID3D11Device1> pm_d3dDevice, Physics* phys, char* filename,  wchar_t* texture_filename, float scale, float x,float y,float z,btScalar mass,float yaw,float pitch,float roll,float rest,float fric)
btRigidBody*  WorldObject::MakePhysicsConvexMesh(ObjInfo* info)

{
	Initialize(info->ObjFilename, info->TextureFilename, info);

	item_id = info->item_id;

	btVector3 A;
	btVector3 B;
	btVector3 C;
	btTriangleMesh* data = new btTriangleMesh();

	for (int i = 0; i<m_model->GetVerticesCount() - 3; i += 3)
	{
		A = btVector3(m_model->m_phy_vertices[i].x, m_model->m_phy_vertices[i].y, m_model->m_phy_vertices[i].z);
		B = btVector3(m_model->m_phy_vertices[i + 1].x, m_model->m_phy_vertices[i + 1].y, m_model->m_phy_vertices[i + 1].z);
		C = btVector3(m_model->m_phy_vertices[i + 2].x, m_model->m_phy_vertices[i + 2].y, m_model->m_phy_vertices[i + 2].z);
		data->addTriangle(A, B, C, false);
	}

	//btBvhTriangleMeshShape* fallShape=new btBvhTriangleMeshShape(data,true,true);
	auto fallShape = new btConvexTriangleMeshShape(data, true);

	//auto fallShape = new btCylinderShape(btCylinderShape(btVector3(1.2,1.2,6.0)));
	//auto fallShape = new btSphereShape(btSphereShape(1.5f));
	//auto fallShape = new btTriangleMesh (btSphereShape(1.5f));
	btMotionState* fallMotionState = (btMotionState*)this;//(cube, btTransform(btQuaternion(0,0,0,1),btVector3(x,y,z)));
														  //btScalar mass = 0.2;
	btVector3 fallInertia(0, 0, 0);
	fallShape->calculateLocalInertia(info->Mass, fallInertia);

	m_rigidbody = m_phys->AddPhysicalObject(fallShape, fallMotionState, info->Mass, fallInertia, info->Restitution, info->Friction, &obj_id, info);
	//m_rigidbody->setActivationState(WANTS_DEACTIVATION);

	delete data;

	return m_rigidbody;

}


//btRigidBody*  Planet::MakePhysicsConvexMesh(Microsoft::WRL::ComPtr<ID3D11Device1> pm_d3dDevice, Physics* phys, char* filename,  wchar_t* texture_filename, float scale, float x,float y,float z,btScalar mass,float yaw,float pitch,float roll,float rest,float fric)
btRigidBody*  WorldObject::MakePhysicsConvexRotation(ObjInfo* info)

{


	item_id = info->item_id;

	btQuaternion rotate = btQuaternion(info->Yaw, info->Pitch, info->Roll);
	btMatrix3x3 rotmat = btMatrix3x3(rotate);

	info->Yaw = 0.0f;
	info->Pitch = 0.0f;
	info->Roll = 0.0f;
	Initialize(info->ObjFilename, info->TextureFilename, info);

	btVector3 A;
	btVector3 B;
	btVector3 C;
	btTriangleMesh* data = new btTriangleMesh();

	for (int i = 0; i<m_model->GetVerticesCount() - 3; i += 3)
	{
		A = btVector3(m_model->m_phy_vertices[i].x, m_model->m_phy_vertices[i].y, m_model->m_phy_vertices[i].z);
		B = btVector3(m_model->m_phy_vertices[i + 1].x, m_model->m_phy_vertices[i + 1].y, m_model->m_phy_vertices[i + 1].z);
		C = btVector3(m_model->m_phy_vertices[i + 2].x, m_model->m_phy_vertices[i + 2].y, m_model->m_phy_vertices[i + 2].z);

		A = rotmat*A;
		B = rotmat*B;
		C = rotmat*C;

		data->addTriangle(A, B, C, false);
	}

	//btBvhTriangleMeshShape* fallShape=new btBvhTriangleMeshShape(data,true,true);
	auto fallShape = new btConvexTriangleMeshShape(data, true);

	//auto fallShape = new btCylinderShape(btCylinderShape(btVector3(1.2,1.2,6.0)));
	//auto fallShape = new btSphereShape(btSphereShape(1.5f));
	//auto fallShape = new btTriangleMesh (btSphereShape(1.5f));
	btMotionState* fallMotionState = (btMotionState*)this;//(cube, btTransform(btQuaternion(0,0,0,1),btVector3(x,y,z)));
														  //btScalar mass = 0.2;
	btVector3 fallInertia(0, 0, 0);
	fallShape->calculateLocalInertia(info->Mass, fallInertia);

	m_rigidbody = m_phys->AddPhysicalObject(fallShape, fallMotionState, info->Mass, fallInertia, info->Restitution, info->Friction, &obj_id, info);
	//m_rigidbody->setActivationState(WANTS_DEACTIVATION);
	return m_rigidbody;

}


btRigidBody* WorldObject::MakeTreeTrunk(ObjInfo* info, ModelLoader* model)
{
	//Initialize(pm_d3dDevice, info->ObjFilename, info->TextureFilename, info->Scale, info->PosX, info->PosY, info->PosZ, info->Yaw, info->Pitch, info->Roll);

	item_id = info->item_id;

	btVector3 A;
	btVector3 B;
	btVector3 C;

	float radius1 = 0.0f;
	float radius2 = 0.0f;
	float length = 0.0f;

	//btTriangleMesh* data = new btTriangleMesh();

	for (int i = 0; i<model->GetVerticesCount() - 3; i += 3)
	{
		if (model->m_phy_vertices[i].x > radius1)
			radius1 = model->m_phy_vertices[i].x;
		if (model->m_phy_vertices[i + 1].x > radius1)
			radius1 = model->m_phy_vertices[i + 1].x;
		if (model->m_phy_vertices[i + 2].x > radius1)
			radius1 = model->m_phy_vertices[i + 2].x;

		if (model->m_phy_vertices[i].y > length)
			length = model->m_phy_vertices[i].y;
		if (model->m_phy_vertices[i + 1].y > length)
			length = model->m_phy_vertices[i + 1].y;
		if (model->m_phy_vertices[i + 2].y > length)
			length = model->m_phy_vertices[i + 2].y;
	}
	radius1 = 1.0f;
	length = 3.0f;

	//btBvhTriangleMeshShape* fallShape=new btBvhTriangleMeshShape(data,true,true);
	//auto fallShape = new btConvexTriangleMeshShape(data,true);

	auto fallShape = new btCylinderShape(btCylinderShape(btVector3(radius1, length, length)));
	//auto fallShape = new btSphereShape(btSphereShape(1.5f));
	//auto fallShape = new btTriangleMesh (btSphereShape(1.5f));
	btMotionState* fallMotionState = (btMotionState*)this;//(cube, btTransform(btQuaternion(0,0,0,1),btVector3(x,y,z)));
														  //btScalar mass = 0.2;
	btVector3 fallInertia(0, 0, 0);
	fallShape->calculateLocalInertia(info->Mass, fallInertia);
	m_rigidbody = m_phys->AddPhysicalObject(fallShape, fallMotionState, info->Mass, fallInertia, info->Restitution, info->Friction, &obj_id, info);
	//m_rigidbody->setActivationState(WANTS_DEACTIVATION);

	m_initialTransform = btTransform(btQuaternion(info->Yaw, info->Pitch, info->Roll), btVector3(info->PosX, info->PosY, info->PosZ));

	setWorldTransform(m_initialTransform);



	return m_rigidbody;
}




btRigidBody*  WorldObject::MakePhysicsCylinderFromMesh(ObjInfo* info, float _scale)
{
	Initialize(info->ObjFilename, info->TextureFilename, info);

	item_id = info->item_id;

	btVector3 A;
	btVector3 B;
	btVector3 C;

	float radius1 = 0.0f;
	float radius2 = 0.0f;
	float length = 0.0f;

	//btTriangleMesh* data = new btTriangleMesh();

	for (int i = 0; i<m_model->GetVerticesCount() - 3; i += 3)
	{
		if (m_model->m_phy_vertices[i].x > radius1)
			radius1 = m_model->m_phy_vertices[i].x;
		if (m_model->m_phy_vertices[i + 1].x > radius1)
			radius1 = m_model->m_phy_vertices[i + 1].x;
		if (m_model->m_phy_vertices[i + 2].x > radius1)
			radius1 = m_model->m_phy_vertices[i + 2].x;

		if (m_model->m_phy_vertices[i].y > length)
			length = m_model->m_phy_vertices[i].y;
		if (m_model->m_phy_vertices[i + 1].y > length)
			length = m_model->m_phy_vertices[i + 1].y;
		if (m_model->m_phy_vertices[i + 2].y > length)
			length = m_model->m_phy_vertices[i + 2].y;

	}

	radius1 = radius1*_scale;

	//btBvhTriangleMeshShape* fallShape=new btBvhTriangleMeshShape(data,true,true);
	//auto fallShape = new btConvexTriangleMeshShape(data,true);

	auto fallShape = new btCylinderShape(btCylinderShape(btVector3(radius1, length, length)));
	//auto fallShape = new btSphereShape(btSphereShape(1.5f));
	//auto fallShape = new btTriangleMesh (btSphereShape(1.5f));
	btMotionState* fallMotionState = (btMotionState*)this;//(cube, btTransform(btQuaternion(0,0,0,1),btVector3(x,y,z)));
														  //btScalar mass = 0.2;
	btVector3 fallInertia(0, 0, 0);
	fallShape->calculateLocalInertia(info->Mass, fallInertia);
	m_rigidbody = m_phys->AddPhysicalObject(fallShape, fallMotionState, info->Mass, fallInertia, info->Restitution, info->Friction, &obj_id, info);
	m_rigidbody->setActivationState(WANTS_DEACTIVATION);
	return m_rigidbody;
}



btRigidBody*  WorldObject::MakePhysicsConeFromMesh(ObjInfo* info)
{
	Initialize(info->ObjFilename, info->TextureFilename, info);

	item_id = info->item_id;

	btVector3 A;
	btVector3 B;
	btVector3 C;

	float radius1 = 0.0f;
	float radius2 = 0.0f;
	float length = 0.0f;

	//btTriangleMesh* data = new btTriangleMesh();

	for (int i = 0; i<m_model->GetVerticesCount() - 3; i += 3)
	{
		if (m_model->m_phy_vertices[i].x > length)
			length = m_model->m_phy_vertices[i].x;
		if (m_model->m_phy_vertices[i + 1].x > length)
			length = m_model->m_phy_vertices[i + 1].x;
		if (m_model->m_phy_vertices[i + 2].x > length)
			length = m_model->m_phy_vertices[i + 2].x;

		if (m_model->m_phy_vertices[i].y > radius1)
			radius1 = m_model->m_phy_vertices[i].y;
		if (m_model->m_phy_vertices[i + 1].y > radius1)
			radius1 = m_model->m_phy_vertices[i + 1].y;
		if (m_model->m_phy_vertices[i + 2].y > radius1)
			radius1 = m_model->m_phy_vertices[i + 2].y;
	}

	//btBvhTriangleMeshShape* fallShape=new btBvhTriangleMeshShape(data,true,true);
	//auto fallShape = new btConvexTriangleMeshShape(data,true);
	auto fallShape = new btConeShape(btConeShape(length*2.0f, radius1*2.0f));
	//auto fallShape = new btCylinderShape(btCylinderShape(btVector3(length,radius1,radius1)));
	//auto fallShape = new btSphereShape(btSphereShape(1.5f));
	//auto fallShape = new btTriangleMesh (btSphereShape(1.5f));
	btMotionState* fallMotionState = (btMotionState*)this;//(cube, btTransform(btQuaternion(0,0,0,1),btVector3(x,y,z)));
														  //btScalar mass = 0.2;
	btVector3 fallInertia(0, 0, 0);
	fallShape->calculateLocalInertia(info->Mass, fallInertia);
	m_rigidbody = m_phys->AddPhysicalObject(fallShape, fallMotionState, info->Mass, fallInertia, info->Restitution, info->Friction, &item_id, info);
	m_rigidbody->setActivationState(WANTS_DEACTIVATION);
	return m_rigidbody;
}
