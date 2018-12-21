#pragma once

#include "../DirectXTK/Inc/DDSTextureLoader.h"
#include "../Bullet/src/btBulletDynamicsCommon.h"

#include "ModelLoader.h"
#include "Physics.h"


namespace Simple3DGame
{

	class WorldObject : public btMotionState
	{
	public:
		WorldObject(std::shared_ptr<DX::DeviceResources> pm_deviceResources, Physics* phys);
		~WorldObject();

		char bActivated;

		// Constant buffer
		btTransform m_initialTransform;

		ModelViewProjectionConstantBuffer	m_constantBufferData;

		int item_id;
		int obj_id;

		int active_frames;
		bool bStatic;

		bool bDeactivated;

		DirectX::XMFLOAT4X4* root_model;

		btRigidBody* m_rigidbody;



		Physics* m_phys;

		int bTexture;

		void getWorldTransform(btTransform &worldTransform) const;
		void setWorldTransform(const btTransform &worldTransform);

		void SetTexture(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> deviceContext);

		void SetTransform(const Transform^ transform);
		virtual void SetPosition(float x, float y, float z, float yaw, float pitch, float roll);

		void SetPositionMatrix(DirectX::XMFLOAT4X4 _model_matrix);

		btRigidBody*  MakePhysicsConvexRotation(ObjInfo* info);
		btRigidBody*  MakePhysicsConvexMesh(ObjInfo* info);
		btRigidBody*  MakePhysicsBoxFromMesh(ObjInfo* info, float _scale = 1.0f);
		btRigidBody*  MakePhysicsSphereFromMesh(ObjInfo* info, float _scale = 1.0f);
		btRigidBody*  MakePhysicsSphereFromMeshRadius(ObjInfo* info, float radius);
		btRigidBody*  MakePhysicsCylinderFromMesh(ObjInfo* info, float _scale = 1.0f);
		btRigidBody*  MakePhysicsConeFromMesh(ObjInfo* info);
		btRigidBody*  MakeObject(ObjInfo* info, int type);

		btRigidBody*  MakePhysicsBoxFromMeshLoopSpecial(ObjInfo* info);

		btRigidBody*  MakeTreeTrunk(ObjInfo* info, ModelLoader* model);

		btRigidBody*  MakePhysicsCompoundBoxFromMesh(ObjInfo* info, float x, float y, float z);
		ID3D11ShaderResourceView* GetTexture();

		// Initializes the asteroid by setting a location (X, Y, Z coordinates) and rotation speeds
		virtual void Initialize(char* model_filename, wchar_t* texture_filename, ObjInfo* info);

		void SetBuffers(Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer, Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer, UINT indexCount);

		void Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> deviceContext, Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer, ModelViewProjectionConstantBuffer* constantBufferData);
		void RenderYaw(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> deviceContext, Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer, ModelViewProjectionConstantBuffer* constantBufferData, float yaw);
		void RenderOffset(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> deviceContext, Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer, ModelViewProjectionConstantBuffer* constantBufferData, float x, float y, float z);

		void UpdateProjectionMatrix(DirectX::XMMATRIX *projectionMatrix);

		void ResetTransform();

		void SetTexturePointer(ID3D11ShaderResourceView* p_Tex);


		float initial_z;

		ModelLoader* m_model;

	private:
		ID3D11ShaderResourceView* m_Texture;
		std::shared_ptr<DX::DeviceResources> m_deviceResources;
	protected:

		// Vertex and index buffers
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;

		// A number of indices
		UINT m_indexCount;

	};
}
