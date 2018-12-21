#pragma once
#include "../Bullet/src/btBulletDynamicsCommon.h"

#define MAX_POINT_LIGHTS 8

namespace Simple3DGame
{
	// Constant buffer used to send MVP matrices to the vertex shader.
	struct ModelViewProjectionConstantBuffer
	{
		DirectX::XMFLOAT4X4 identity;
		DirectX::XMFLOAT4X4 model;
		DirectX::XMFLOAT4X4 view;
		DirectX::XMFLOAT4X4 projection;
	};

	// Used to send per-vertex data to the vertex shader.
	struct VertexPositionColor
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 color;
	};

	struct VertexData
	{
	public:
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT2 tex;

		void SetNormal(float x, float y, float z) { normal = DirectX::XMFLOAT3(x, y, z); }
		void SetPosition(float x, float y, float z) { position = DirectX::XMFLOAT3(x, y, z); }
		void SetTex(float u, float v) { tex = DirectX::XMFLOAT2(u, v); }

		//void SetColor(XMFLOAT3 c) { color = c; }
	};

	typedef struct
	{
		float x, y, z;
	}VertexType;

	typedef struct
	{
		int part_no;
		int tail_no;
		float dist;
	} part_index;

	const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	struct ParticleVertexType
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT2 tex;
		DirectX::XMFLOAT4 color;
	};

	struct VertexTex
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT2 tex;
	};

	struct VertexPositionTex
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 norm;
		DirectX::XMFLOAT2 tex;
		DirectX::XMFLOAT4 color;
		float tex_blend;
		float specular;
	};

	typedef struct
	{
		int vIndex1, vIndex2, vIndex3;
		int tIndex1, tIndex2, tIndex3;
		int nIndex1, nIndex2, nIndex3;
	}FaceType;



	struct CameraBufferType
	{
		DirectX::XMFLOAT3 cameraPosition;
		float padding;
	};

	struct PointLightType
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT4 ambient;
		DirectX::XMFLOAT4 diffuse;
		DirectX::XMFLOAT4 specular;
		float radius;
	};

	struct LightBufferType
	{
		DirectX::XMFLOAT4 ambientColor;
		DirectX::XMFLOAT4 diffuseColor;
		DirectX::XMFLOAT3 lightDirection;
		float specularPower;
		DirectX::XMFLOAT4 specularColor;
		DirectX::XMFLOAT4 fogColor;
		int numLights;
		DirectX::XMFLOAT3 pos;
		PointLightType lights[MAX_POINT_LIGHTS];

	};



	ref class Transform
	{
	internal:
		Transform(float locX,
			float locY,
			float locZ,
			float quatX,
			float quatY,
			float quatZ,
			float quatW)
			:
			locX(locX),
			locY(locY),
			locZ(locZ),
			quatX(quatX),
			quatY(quatY),
			quatZ(quatZ),
			quatW(quatW)
		{
		}

		float locX;
		float locY;
		float locZ;
		float quatX;
		float quatY;
		float quatZ;
		float quatW;
	};


	struct ObjInfo
	{
		char* ObjFilename;
		wchar_t* TextureFilename;
		float Scale;
		float PosX;
		float PosY;
		float PosZ;
		btScalar Mass;
		float Yaw;
		float Pitch;
		float Roll;
		float Restitution;
		float Friction;
		int item_id;
		bool raise_by_height; // to initialize on terrain.
		int group;
		int mask;
	};

	struct HeightMapInfo {				// Heightmap structure
		int terrainWidth;				// Width of heightmap
		int terrainHeight;				// Height (Length) of heightmap
		DirectX::XMFLOAT3 *heightMap;	// Array to store terrain's vertex positions
		DirectX::XMFLOAT3 *normal;		// Array to store terrain's vertex positions
		DirectX::XMFLOAT4 *colour;		// Array to store terrain's vertex positions
	};

	struct MVPConstantBuffer
	{
		DirectX::XMFLOAT4X4 model;
		DirectX::XMFLOAT4X4 view;
		DirectX::XMFLOAT4X4 projection;
	};

	typedef struct
	{
		int bActive;
		float x, y, z;
		float y2;
	} pill_t;

}