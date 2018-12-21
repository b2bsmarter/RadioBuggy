#include "pch.h"
#include "dots.h"

using namespace Simple3DGame;
using namespace DX;
using namespace DirectX;

int DotsSortCB(const VOID* arg1, const VOID* arg2)
{
	part_index* p1 = (part_index*)arg1;
	part_index* p2 = (part_index*)arg2;

	if (p1->dist == p2->dist)
	{
		return 0;
	}
	else
	{
		if (p1->dist<p2->dist)
		{
			return 1;
		}
		else
		{
			return -1;
		}
	}
}

void Dots::Reset()
{
	
	for (int i = 0; i<m_maxParticles; i++)
	{
		dott[i].bActive = 0;
	}
	current_point = 0;
}


void Dots::Initialize(Level* pp_Level, Player* pp_Player, Camera^ pp_Camera)
{
	p_Level = pp_Level;

	p_Player = pp_Player;

	dott = new dot_t[4000];

	p_index = new part_index[4000];

	bUpdate = true;

	current_particle_que = 0;

	m_particleSize = 1.0f;

	m_maxParticles = 4000;

	current_index = 0;

	current_point = 0;

	total_collected = 0;

	InitializeBuffers();

	p_Camera = pp_Camera;

	CreateDDSTextureFromFile((ID3D11Device *)m_deviceResources->GetD3DDevice(), L"Assets/Textures/spot.dds", nullptr, &m_Texture, MAXSIZE_T);

	for (int i = 0; i<m_maxParticles; i++)
	{
		dott[i].bActive = 0;
	}

}



void Dots::CreateOne(float x, float y, float z, DirectX::XMFLOAT4 col, int _type)
{
	int i;
	bool bFound = false;
	int point_find = 0;


	for (i = 0; i < m_maxParticles; i++)
	{
		if (dott[i].bActive == 0)
		{
			dott[i].angle = 0.0f;
			dott[i].bActive = 1;
			dott[i].vel_y = 0.0f;
			dott[i].x = x;
			dott[i].y = y;
			dott[i].z = z;
			dott[i].life = ((rand() % 100)*0.01f)*(M_PI*2.0f);
			dott[i].type = _type;
			dott[i].stopped = 0;
			dott[i].col = col;

			if (current_point < i + 1)
				current_point = i + 1;
			break;
		}
	}





}




Dots::~Dots(void)
{


}


void Dots::Update(float timeTotal, float timeDelta)
{
	int i;
	//timeDelta *= 1.1f;

	//angle=angle+0.01f;

	for (i = 0; i<m_maxParticles; i++)
	{
		dott[i].life += 3.9f * timeDelta;
		if (dott[i].life>M_PI*2.0f)
		{
			dott[i].life -= M_PI*2.0f;
		}

		dott[i].col = DirectX::XMFLOAT4(dott[i].col.x, dott[i].col.y, dott[i].col.z, (sin(dott[i].life))*0.5f);

	}

	//p_Level->


	/*
	CreateOne(-p_Level->left_right_walls + 2.0f + (float(rand() % (int)(p_Level->left_right_walls * 2.0f))), p_Player->GetPosition()->getY() + 30.0f, p_Player->GetPosition()->getZ() - FAR_DIST + float(float(rand() % int(FAR_DIST*20.0f)) / 10.0f));
	CreateOne(-p_Level->left_right_walls + 2.0f + (float(rand() % (int)(p_Level->left_right_walls * 2.0f))), p_Player->GetPosition()->getY() + 30.0f, p_Player->GetPosition()->getZ() - FAR_DIST + float(float(rand() % int(FAR_DIST*20.0f)) / 10.0f));
	CreateOne(-p_Level->left_right_walls + 2.0f + (float(rand() % (int)(p_Level->left_right_walls * 2.0f))), p_Player->GetPosition()->getY() + 30.0f, p_Player->GetPosition()->getZ() - FAR_DIST + float(float(rand() % int(FAR_DIST*20.0f)) / 10.0f));
	CreateOne(-p_Level->left_right_walls + 2.0f + (float(rand() % (int)(p_Level->left_right_walls * 2.0f))), p_Player->GetPosition()->getY() + 30.0f, p_Player->GetPosition()->getZ() - FAR_DIST + float(float(rand() % int(FAR_DIST*20.0f)) / 10.0f));
	CreateOne(-p_Level->left_right_walls + 2.0f + (float(rand() % (int)(p_Level->left_right_walls * 2.0f))), p_Player->GetPosition()->getY() + 30.0f, p_Player->GetPosition()->getZ() - FAR_DIST + float(float(rand() % int(FAR_DIST*20.0f)) / 10.0f));
	CreateOne(-p_Level->left_right_walls + 2.0f + (float(rand() % (int)(p_Level->left_right_walls * 2.0f))), p_Player->GetPosition()->getY() + 30.0f, p_Player->GetPosition()->getZ() - FAR_DIST + float(float(rand() % int(FAR_DIST*20.0f)) / 10.0f));
	CreateOne(-p_Level->left_right_walls + 2.0f + (float(rand() % (int)(p_Level->left_right_walls * 2.0f))), p_Player->GetPosition()->getY() + 30.0f, p_Player->GetPosition()->getZ() - FAR_DIST + float(float(rand() % int(FAR_DIST*20.0f)) / 10.0f));
	CreateOne(-p_Level->left_right_walls + 2.0f + (float(rand() % (int)(p_Level->left_right_walls * 2.0f))), p_Player->GetPosition()->getY() + 30.0f, p_Player->GetPosition()->getZ() - FAR_DIST + float(float(rand() % int(FAR_DIST*20.0f)) / 10.0f));
	CreateOne(-p_Level->left_right_walls + 2.0f + (float(rand() % (int)(p_Level->left_right_walls * 2.0f))), p_Player->GetPosition()->getY() + 30.0f, p_Player->GetPosition()->getZ() - FAR_DIST + float(float(rand() % int(FAR_DIST*20.0f)) / 10.0f));
	CreateOne(-p_Level->left_right_walls + 2.0f + (float(rand() % (int)(p_Level->left_right_walls * 2.0f))), p_Player->GetPosition()->getY() + 30.0f, p_Player->GetPosition()->getZ() - FAR_DIST + float(float(rand() % int(FAR_DIST*20.0f)) / 10.0f));
	CreateOne(-p_Level->left_right_walls + 2.0f + (float(rand() % (int)(p_Level->left_right_walls * 2.0f))), p_Player->GetPosition()->getY() + 30.0f, p_Player->GetPosition()->getZ() - FAR_DIST + float(float(rand() % int(FAR_DIST*20.0f)) / 10.0f));
	CreateOne(-p_Level->left_right_walls + 2.0f + (float(rand() % (int)(p_Level->left_right_walls * 2.0f))), p_Player->GetPosition()->getY() + 30.0f, p_Player->GetPosition()->getZ() - FAR_DIST + float(float(rand() % int(FAR_DIST*20.0f)) / 10.0f));
	CreateOne(-p_Level->left_right_walls + 2.0f + (float(rand() % (int)(p_Level->left_right_walls * 2.0f))), p_Player->GetPosition()->getY() + 30.0f, p_Player->GetPosition()->getZ() - FAR_DIST + float(float(rand() % int(FAR_DIST*20.0f)) / 10.0f));
	CreateOne(-p_Level->left_right_walls + 2.0f + (float(rand() % (int)(p_Level->left_right_walls * 2.0f))), p_Player->GetPosition()->getY() + 30.0f, p_Player->GetPosition()->getZ() - FAR_DIST + float(float(rand() % int(FAR_DIST*20.0f)) / 10.0f));
	CreateOne(-p_Level->left_right_walls + 2.0f + (float(rand() % (int)(p_Level->left_right_walls * 2.0f))), p_Player->GetPosition()->getY() + 30.0f, p_Player->GetPosition()->getZ() - FAR_DIST + float(float(rand() % int(FAR_DIST*20.0f)) / 10.0f));
	CreateOne(-p_Level->left_right_walls + 2.0f + (float(rand() % (int)(p_Level->left_right_walls * 2.0f))), p_Player->GetPosition()->getY() + 30.0f, p_Player->GetPosition()->getZ() - FAR_DIST + float(float(rand() % int(FAR_DIST*20.0f)) / 10.0f));
	CreateOne(-p_Level->left_right_walls + 2.0f + (float(rand() % (int)(p_Level->left_right_walls * 2.0f))), p_Player->GetPosition()->getY() + 30.0f, p_Player->GetPosition()->getZ() - FAR_DIST + float(float(rand() % int(FAR_DIST*20.0f)) / 10.0f));
	CreateOne(-p_Level->left_right_walls + 2.0f + (float(rand() % (int)(p_Level->left_right_walls * 2.0f))), p_Player->GetPosition()->getY() + 30.0f, p_Player->GetPosition()->getZ() - FAR_DIST + float(float(rand() % int(FAR_DIST*20.0f)) / 10.0f));
	*/
	CreateVerticies();

}


void Dots::UpdateProjectionMatrix(DirectX::XMMATRIX *projectionMatrix)
{
	XMStoreFloat4x4(&m_constantBufferB.projection, *projectionMatrix);
}


void Dots::Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> deviceContext, Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer, ModelViewProjectionConstantBuffer* constantBufferData)
{
	UpdateVertecies(deviceContext.Get());

	Sparticle::Render(deviceContext, constantBuffer, constantBufferData);
}


bool Dots::CreateVerticies()
{
	int index, i;
	int particle_count = 0;

	btVector3 eye;

	current_index = 0;

	

	for (i = 0; i<m_maxParticles; i++)
	{
		if (dott[i].bActive == 1 && dott[i].z<p_Level->front_back_walls && dott[i].z > -p_Level->front_back_walls && p_Camera->CheckPoint(dott[i].x, dott[i].y, dott[i].z, 1.0f) == true)
		{
			//btVector3 part_pos(dott[i].x, dott[i].y, dott[i].z);

			p_index[current_index].part_no = i;
			//p_index[current_index].dist = part_pos.distance(eye);
			current_index++;
			particle_count++;
		}
	}

	if (particle_count>0)
	{
		qsort(p_index, particle_count, sizeof(part_index), DotsSortCB);
	}

	//particle_count=1;
	//memset(m_vertices, 0, (sizeof(VertexPositionTex) * (m_vertexCount)));

	// Now build the vertex array from the particle list array.  Each particle is a quad made out of two triangles.
	index = 0;
	int total_active = 0;
	float px;
	float py;
	float pz;
	DirectX::XMFLOAT4 col;
	float size = 0.1f;
	float sizeB = 0.8f;


	btVector3 otop_right = btVector3(size, size, 0.0f);
	btVector3 otop_left = btVector3(-size, size, 0.0f);
	btVector3 obottom_right = btVector3(size, -size, 0.0f);
	btVector3 obottom_left = btVector3(-size, -size, 0.0f);
	btVector3 top_right;// = btVector3(1.0f, 1.0f, 0.0f);
	btVector3 top_left;// = btVector3(-1.0f, 1.0f, 0.0f);;
	btVector3 bottom_right;// = btVector3(1.0f, -1.0f, 0.0f);;
	btVector3 bottom_left;// = btVector3(-1.0f, -1.0f, 0.0f);;


	XMFLOAT4X4 x_mat = *p_Camera->GetViewMatrix();

	btVector3 tran;
	btQuaternion quat_rot;
	//btMatrix3x3 rot_mat = btMatrix3x3(mats._11, mats._12, mats._13, mats._21, mats._22, mats._23, mats._31, mats._32, mats._33);
	//btMatrix3x3 rot_matb
	//particle_count=1;
	btMatrix3x3 rot_mat = btMatrix3x3(x_mat._11, x_mat._12, x_mat._13, x_mat._21, x_mat._22, x_mat._23, x_mat._31, x_mat._32, x_mat._33);


	m_vertexCount = 0;
	m_indexCount = 0;

	if (true) //(total_active<(m_maxParticles*20)-1)
	{
		//bUpdate=false;
		for (i = 0; i<particle_count; i++)
		{
			//=m_particleSize;

			if (dott[p_index[i].part_no].z>p_Level->front_back_walls || dott[p_index[i].part_no].z < -p_Level->front_back_walls)
				continue;

			tran = btVector3(dott[p_index[i].part_no].x, dott[p_index[i].part_no].y, dott[p_index[i].part_no].z);
			//dott[p_index[i].part_no].angle += 0.01f;
			if (true) //(dott[p_index[i].part_no].life < 10.0f)
			{
				col = DirectX::XMFLOAT4(dott[p_index[i].part_no].col.x, dott[p_index[i].part_no].col.y, dott[p_index[i].part_no].col.z, sin(dott[p_index[i].part_no].life)-1.0f);
			}
			else
			{
				//col = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 0.0f);
				col = dott[p_index[i].part_no].col;
			}

			btQuaternion quat_rot = btQuaternion(btVector3(0.0f, 0.0f, 1.0f), dott[p_index[i].part_no].angle);

			btMatrix3x3 rot_matb = btMatrix3x3(quat_rot);
			rot_matb = rot_matb * rot_mat;

			top_right = (otop_right * rot_matb) + tran;
			top_left = (otop_left * rot_matb) + tran;
			bottom_right = (obottom_right * rot_matb) + tran;
			bottom_left = (obottom_left * rot_matb) + tran;

			index = m_vertexCount;

			// Top left.
			m_vertices[m_vertexCount].pos = DirectX::XMFLOAT3(top_left.getX(), top_left.getY(), top_left.getZ());
			m_vertices[m_vertexCount].tex = DirectX::XMFLOAT2(0.0f, 0.0f );
			m_vertices[m_vertexCount].color = col;
			m_vertexCount++;
			// Bottom right.
			m_vertices[m_vertexCount].pos = DirectX::XMFLOAT3(bottom_right.getX(), bottom_right.getY(), bottom_right.getZ());
			m_vertices[m_vertexCount].tex = DirectX::XMFLOAT2(1.0f, 1.0f );
			m_vertices[m_vertexCount].color = col;
			m_vertexCount++;
			// Bottom left.
			m_vertices[m_vertexCount].pos = DirectX::XMFLOAT3(bottom_left.getX(), bottom_left.getY(), bottom_left.getZ());
			m_vertices[m_vertexCount].tex = DirectX::XMFLOAT2(0.0f, 1.0f );
			m_vertices[m_vertexCount].color = col;
			m_vertexCount++;
			// Top right.
			m_vertices[m_vertexCount].pos = DirectX::XMFLOAT3(top_right.getX(), top_right.getY(), top_right.getZ());
			m_vertices[m_vertexCount].tex = DirectX::XMFLOAT2(1.0f, 0.0f );
			m_vertices[m_vertexCount].color = col;
			m_vertexCount++;



			//// Top left.
			//m_vertices[m_vertexCount].pos = DirectX::XMFLOAT3(px - (xsize), py + sizeB, pz - (zsize));
			//m_vertices[m_vertexCount].tex = DirectX::XMFLOAT2(0.5f + (ang_cos - ang_sin), point1 + (dott[p_index[i].part_no].v1)/*spark[p_index[i].part_no].v1*/);
			//m_vertices[m_vertexCount].color = col;
			//m_vertexCount++;
			//// Bottom right.
			//m_vertices[m_vertexCount].pos = DirectX::XMFLOAT3(px + (xsize), py - sizeB, pz + (zsize));
			//m_vertices[m_vertexCount].tex = DirectX::XMFLOAT2(0.5f + (ang_cos2 - ang_sin2), point2 + (dott[p_index[i].part_no].v1)/*spark[p_index[i].part_no].v2*/);
			//m_vertices[m_vertexCount].color = col;
			//m_vertexCount++;
			//// Bottom left.
			//m_vertices[m_vertexCount].pos = DirectX::XMFLOAT3(px - (xsize), py - sizeB, pz - (zsize));
			//m_vertices[m_vertexCount].tex = DirectX::XMFLOAT2(0.5f + (ang_cos - ang_sin2), point3 + (dott[p_index[i].part_no].v1)/*spark[p_index[i].part_no].v2*/);
			//m_vertices[m_vertexCount].color = col;
			//m_vertexCount++;
			//// Top right.
			//m_vertices[m_vertexCount].pos = DirectX::XMFLOAT3(px + (xsize), py + sizeB, pz + (zsize));
			//m_vertices[m_vertexCount].tex = DirectX::XMFLOAT2(0.5f + (ang_cos2 - ang_sin), point4 + (dott[p_index[i].part_no].v1)/*spark[p_index[i].part_no].v1*/);
			//m_vertices[m_vertexCount].color = col;
			//m_vertexCount++;

			m_indices[m_indexCount++] = index;
			m_indices[m_indexCount++] = index + 1;
			m_indices[m_indexCount++] = index + 2;
			m_indices[m_indexCount++] = index;
			m_indices[m_indexCount++] = index + 3;
			m_indices[m_indexCount++] = index + 1;

		}
	}

	return true;
}
