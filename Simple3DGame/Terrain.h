#pragma once
#include <fstream>
#include <string>
#include <vector>
//#include "DirectXHelper.h"
//#include "VertexData.h"
//#include "MVPConstantBuffer.h"

#include "../DirectXTK/Inc/DDSTextureLoader.h"
#include "../Bullet/src/btBulletDynamicsCommon.h"


//#include "DDSTextureLoader.h"
#include "Physics.h"
//#include "Object3D.h"

using namespace std;
//using namespace DirectX;
//using namespace DX;
//using namespace Microsoft::WRL;

namespace Simple3DGame
{
	class SimpleMotion : btMotionState
	{
	public:
		SimpleMotion();
		~SimpleMotion();

		void getWorldTransform(btTransform &worldTransform) const;
		void setWorldTransform(const btTransform &worldTransform);

		btTransform m_initialTransform;
	};



	class Terrain 
	{
	public:
		Terrain(std::shared_ptr<DX::DeviceResources> pm_deviceResources);
		~Terrain(void);

		// Constant buffer
		btTransform m_initialTransform;
		ModelViewProjectionConstantBuffer	m_constantBufferData;

		int xcoords, ycoords;

		int xblockpos, yblockpos;

		bool bLoaded;

		int start_id, end_id;

		// Loads data of vertices and indices from the .obj file
		bool Load(int xnum, int ynum, float scale, int xplane);

		float** height_map;
		DirectX::XMFLOAT3** normals;
		DirectX::XMFLOAT4** cols;

		float* height_fields;

		SimpleMotion* m_box;

		void InitTerrain(HeightMapInfo* phm_info, int xbp, int ybp);

		float zpos;
		float xpos;

		btRigidBody* m_rigidbody;

		float* cam_z;

		HeightMapInfo* hm_info;

		part_index* p_index;

		float m_scale;

		VertexType* m_phy_vertices;
		int no_phy_verticies;
		int count_no_phy_verticies;
		Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer() { return m_vertexBuffer; }
		Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer() { return m_indexBuffer; }
		Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBufferSimple() { return m_indexBuffer_simple; }

		void SetVertexBuffer();
		void SetIndexBuffer();
		void SetIndexBufferSimple();
		void ClearMemory();

		void getWorldTransform(btTransform &worldTransform) const;
		void setWorldTransform(const btTransform &worldTransform);

		void SetTransform(const Transform^ transform);

		void SetPosition(float x, float y, float z);

		vector<VertexData> m_vertices;
		vector<unsigned short> m_indices;
		vector<unsigned short> m_indices_simple;

		void AddVertexTexNorm(float x, float y, float z, float u, float v, float nx, float ny, float nz);
		void AddVertexTexNormCol(float x, float y, float z, float u, float v, float nx, float ny, float nz, float r, float g, float b, float a);
		void Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> deviceContext, Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer, ModelViewProjectionConstantBuffer* constantBufferData, int complexity);

		void UpdateProjectionMatrix(DirectX::XMMATRIX *projectionMatrix);
		void Update(DirectX::XMMATRIX *viewMatrix, float timeTotal);
		void MakePhysicsConvexTerrain(Physics* phys);
		void MakePhysicsConvexTriangleTerrain(Physics* phys);

		//void LoadTexture(Microsoft::WRL::ComPtr<ID3D11Device1> pm_d3dDevice, wchar_t* texture_filename);


		int m_verticesCount;
		int m_indicesCount;
		int m_indicesCount_simple;

		Microsoft::WRL::ComPtr<ID3D11Device1> m_d3dDevice;

		ID3D11ShaderResourceView* m_Texture;

		// Vertex and index buffers
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer_simple;
	private:
		std::shared_ptr<DX::DeviceResources> m_deviceResources;

	};

}

