#pragma once
#include <fstream>
#include <string>
#include <vector>
#include "common/DirectXHelper.h"
#include "Content/ShaderStructures.h"
//#include "VertexData.h"
//#include "GameHelpers.h"



namespace Simple3DGame
{

	// The class used for loading models
	class ModelLoader
	{
	public:
		ModelLoader(std::shared_ptr<DX::DeviceResources> pm_deviceResources, bool _bUpdateable = false, int _updateLevel  = 0);
		~ModelLoader();
		// Loads data of vertices and indices from the .obj file
		bool Load(char *filename, float scale, int optimize=0);

		// Sets available colors for vertices
		//void SetColors(std::vector<DirectX::XMFLOAT3> colors) { m_colors = colors; }

		void SetModelColourWeight(float _r, float _g, float _b, float _a);
		void SetModelNormals(float _x, float _y, float _z);

		void TransOrigin(float x, float y, float z);
		void RotateY(float angle);
		void FlipNorms();
		void Clearmemory();

		bool CreateUpdatebleBuffers();

		void SetColourWeights(DirectX::XMFLOAT4* _cols);

		// Returns a number of indices
		int GetIndicesCount() { return m_indicesCount; }
		int GetVerticesCount() { return m_verticesCount; }
		void GetModelFilename(char* filename);
		bool ReadFileCounts(char* filename, int& vertexCount, int& textureCount, int& normalCount, int& faceCount);
		bool LoadDataStructures(char* filename, int vertexCount, int textureCount, int normalCount, int faceCount, float m_scale);

		float RayHit(float x_point, float y_point, float z_point, float x_dir, float y_dir, float z_dir, DirectX::XMFLOAT4X4 _model_matrix);

		void UpsideDown();

		void SetScale(float x, float y, float z);

		bool UpdateVerticies();

		void SetModelSpecular(float _s);

		void Sort(float x, float y, float z); // sort by camera position


		part_index* p_index;

		bool bCleared;

		bool bUpdateable;
		int iUpdateLevel; // what to update  0 = vertex only     1 = index only      2 = both

		// Methods returning vertex and index buffers
		Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer() { return m_vertexBuffer; }
		Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer() { return m_indexBuffer; }

		// Methods creating vertex and index buffers
		void SetVertexBuffer();
		void SetIndexBuffer();

		std::vector<btVector3> m_central;


		std::vector<VertexData> m_vertices;
		VertexType* m_phy_vertices;
		int no_phy_verticies;
		std::vector<unsigned short> m_indices;
		std::vector<unsigned short> m_indices_back;


		btVector3 ray_normal;

		void FlipVertDir();
		void Optimize();

		float height_highest;
		float height_lowest;
		float total_height;
		float furthest_point;

		float object_scale;


		float rot_x, rot_y, rot_z;

		int current_normal;
		int current_texture;

		// Fields storing vertices, indices, as well as their numbers

		int m_verticesCount;
		int m_indicesCount;

	private:
		std::shared_ptr<DX::DeviceResources> m_deviceResources;
		// Available colors for vertices




		// Vertex and index buffers
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;
	};

}