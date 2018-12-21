#include "pch.h"
#include "Snow.h"

using namespace Simple3DGame;
using namespace DX;
using namespace DirectX;

int SnowSortCB(const VOID* arg1, const VOID* arg2)
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


void Snow::CreateOne(float x, float y, float z)
{
	int point_find = 0;

	do
	{
		point_find++;
	
		if (snow[current_point].bActive == 0 ) //&& z < p_Level->front_back_walls && z > -p_Level->front_back_walls)
		{
			snow[current_point].bActive = 1;
			snow[current_point].vel_y = ((rand() % 50) / 50.0f)*0.5f;
			snow[current_point].vel_x = ((rand() % 50) / 50.0f)*0.2f;
			snow[current_point].x = x;
			snow[current_point].y = y;
			snow[current_point].z = z;
			snow[current_point].v1 = 0.125f*(float)(rand() % 6);
			snow[current_point].v2 = snow[current_point].v1 + 0.0625f;
			snow[current_point].life = 10.0f;
			snow[current_point].stopped = 0;
			snow[current_point].angle = 0.0f;
			snow[current_point].angle_mom = (((rand() % 50) / 50.0f)-0.5f)*0.1f;

			break;
		}
		else
		{
			current_point++;
			if (current_point>m_maxParticles - 2)
				current_point = 0;
		}

	} while (point_find < m_maxParticles -2);
	//snow[current_point].v1 = 0.125f*(float)num;
	//snow[current_point].v2 = snow[current_point].v1 + 0.125f;
	noise_position = 0.0f;

	current_point++;
	if (current_point>m_maxParticles - 2)
		current_point = 0;
} 

void Snow::Initialize(Level* pp_Level, Player* pp_Player, Camera^ pp_Camera)
{
	p_Level = pp_Level;

	p_Player = pp_Player;

	snow = new snow_t[2000];

	p_index = new part_index[2000];

	bUpdate = true;

	current_particle_que = 0;

	m_particleSize = 1.0f;

	m_maxParticles = 2000;

	current_index = 0;

	current_point = 0;

	total_collected = 0;

	InitializeBuffers();

	p_Camera = pp_Camera;

	pn = new PerlinNoise(237);

	CreateDDSTextureFromFile((ID3D11Device *)m_deviceResources->GetD3DDevice(), L"Assets/Textures/snow.dds", nullptr, &m_Texture, MAXSIZE_T);

	for (int i = 0; i<m_maxParticles; i++)
	{
		snow[i].bActive = 0;
	}

}


void Snow::Reset()
{
	for (int i = 0; i<m_maxParticles; i++)
	{
		snow[i].bActive = 0;
	}
	for (int i = 0; i < 120; i++)
	{
		Update(0.0f, 0.1f);
	}
}



Snow::~Snow(void)
{


}


void Snow::Update(float timeTotal, float timeDelta)
{
	int i;
	//timeDelta *= 1.1f;

	//angle=angle+0.01f;

	for (i = 0; i<m_maxParticles; i++)
	{
		if (snow[i].bActive == 1)
		{

			if (snow[i].stopped == 1 && snow[i].bActive == 1)
			{
				snow[i].life -= 17.0f*timeDelta;
				if (snow[i].life < 0)
				{
					snow[i].bActive = 0;
				}
			}
			if(snow[i].stopped==0)
			{
				snow[i].angle += snow[i].angle_mom;
				if (snow[i].angle > M_PI*2.0f)
				{
					snow[i].angle -= M_PI*2.0f;
				}
				if (snow[i].angle <0.0f)
				{
					snow[i].angle += M_PI*2.0f;
				}

				if (snow[i].x >p_Level->left_right_walls)
				{
					snow[i].x -= p_Level->left_right_walls*2.0f;
				}
				else
				{
					if (snow[i].x <-p_Level->left_right_walls)
					{
						snow[i].x += p_Level->left_right_walls*2.0f;
					}
				}
			}

			//snow[i].life -= 17.0f*timeDelta;
			
			if( snow[i].bActive==true)
			{
				float terrain_height = p_Level->GetTerrainHeight(snow[i].x, snow[i].z);
				if (snow[i].z < p_Player->GetPosition()->getZ() - FAR_DIST)// && (p_Player->GetPosition()->getZ() - FAR_DIST) > -p_Level->front_back_walls)
				{
					snow[i].z += FAR_DIST * 2.0f;
					if (snow[i].stopped==1)
					{
						snow[i].y = terrain_height;
					}
				}
				else
				{
					if (snow[i].z > p_Player->GetPosition()->getZ() + FAR_DIST)// && (p_Player->GetPosition()->getZ() + FAR_DIST) < p_Level->front_back_walls)
					{
						snow[i].z -= FAR_DIST * 2.0f;
						if (snow[i].stopped == 1)
						{
							snow[i].y = terrain_height;
						}
					}
				}

				if (snow[i].y <= terrain_height)
				{
					snow[i].vel_y = 0.0f;
					snow[i].stopped = 1;
					//snow[current_point].life = 1.0f;
					snow[i].y = terrain_height;
				}
				else
				{
					snow[i].y -= snow[i].vel_y*timeDelta*6.0f;
					snow[i].x += snow[i].vel_x*timeDelta*1.2f;
				}
			}
			if (snow[i].y<-10.0f)
			{
				snow[i].bActive = 0;
			}
		}
	}
	//p_Level->

	float noise_scale = 0.01f;

	float noises = pn->noise(noise_position, 0.0f, 0.8)*100.0f;
	int num_noise = (int)pn->noise(noise_position, 0.0f, 0.8)*100.0f;


	noise_position += 0.1f;
	if (noise_position > 100.0f)
	{
		noise_position = 0.0f;
	}

	for (i = 0; i < (int)noises; i++)
	{
		float randx;
		float randz;

		randx = -p_Level->left_right_walls + 2.0f + (float(rand() % (int)(p_Level->left_right_walls * 2.0f)));
		randz = p_Player->GetPosition()->getZ() - FAR_DIST + float(float(rand() % int(FAR_DIST*20.0f)) / 10.0f);

		float noise = pn->noise(((float)randx*noise_scale) + noise_position, ((float)randz *noise_scale) + noise_position, 0.8);

		noise = (noise - 3.0f)*30.0f;

		if ((rand() % (int)noise)>39)
		{
			CreateOne(randx, 20.0f, randz);
		}
	}

	/*
	for (i = 0; i < 24; i++)
	{
		CreateOne(-p_Level->left_right_walls + 2.0f + (float(rand() % (int)(p_Level->left_right_walls * 2.0f))), 20.0f, p_Player->GetPosition()->getZ() - FAR_DIST + float(float(rand() % int(FAR_DIST*20.0f)) / 10.0f));
	}
	*/

	CreateVerticies();
}


void Snow::UpdateProjectionMatrix(DirectX::XMMATRIX *projectionMatrix)
{
	XMStoreFloat4x4(&m_constantBufferB.projection, *projectionMatrix);
}


void Snow::Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> deviceContext, Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer, ModelViewProjectionConstantBuffer* constantBufferData)
{
	UpdateVertecies(deviceContext.Get());

	Sparticle::Render(deviceContext, constantBuffer, constantBufferData);
}

void Snow::CreateVerticies()
{
	unsigned long index, i;
	int particle_count = 0;

	current_index = 0;
	
	for (i = 0; i<m_maxParticles; i++)
	{
		if (snow[i].bActive == 1 && p_Camera->CheckPoint(snow[i].x, snow[i].y, snow[i].z,1.0f) == true)
		{
			p_index[current_index].part_no = i;
			current_index++;
			particle_count++;
		}
	}

	if (particle_count>0)
	{
		qsort(p_index, particle_count, sizeof(part_index), SnowSortCB);
	}

	m_vertexCount = 0;
	m_indexCount = 0;

	index = 0;
	int total_active = 0;
	float px;
	float py;
	float pz;
	DirectX::XMFLOAT4 col;
	float size = 0.2f;
	float snow_edge = 0.2f;

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

	if (true) //(total_active<(m_maxParticles*20)-1)
	{
		//bUpdate=false;
		for (i = 0; i<particle_count; i++)
		{
			//=m_particleSize;

			if (snow[p_index[i].part_no].z>p_Level->front_back_walls || snow[p_index[i].part_no].z < -p_Level->front_back_walls)
				continue;

			tran = btVector3(snow[p_index[i].part_no].x, snow[p_index[i].part_no].y, snow[p_index[i].part_no].z);

			if (snow[p_index[i].part_no].life < 10.0f)
			{
				col = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, -1+(snow[p_index[i].part_no].life*0.1f));
			}
			else
			{
				col = DirectX::XMFLOAT4(2.0f, 2.0f, 2.0f, 0.0f);
			}

			btQuaternion quat_rot = btQuaternion(btVector3(0.0f, 0.0f, 1.0f), snow[p_index[i].part_no].angle);

			btMatrix3x3 rot_matb = btMatrix3x3(quat_rot);
			rot_matb = rot_matb * rot_mat;

			top_right = (otop_right * rot_matb) + tran;
			top_left = (otop_left * rot_matb) + tran;
			bottom_right = (obottom_right * rot_matb) + tran;
			bottom_left = (obottom_left * rot_matb) + tran;

			float height = 0.0625f;

			index = m_vertexCount;

			// Top left.
			m_vertices[m_vertexCount].pos = DirectX::XMFLOAT3(top_left.getX(), top_left.getY(), top_left.getZ());
			m_vertices[m_vertexCount].tex = DirectX::XMFLOAT2(0.0f, 0.0f + (snow[p_index[i].part_no].v1)/*spark[p_index[i].part_no].v1*/);
			m_vertices[m_vertexCount].color = col;
			m_vertexCount++;
			// Bottom right.
			m_vertices[m_vertexCount].pos = DirectX::XMFLOAT3(bottom_right.getX(), bottom_right.getY(), bottom_right.getZ());
			m_vertices[m_vertexCount].tex = DirectX::XMFLOAT2(1.0f , height + (snow[p_index[i].part_no].v1)/*spark[p_index[i].part_no].v2*/);
			m_vertices[m_vertexCount].color = col;
			m_vertexCount++;
			// Bottom left.
			m_vertices[m_vertexCount].pos = DirectX::XMFLOAT3(bottom_left.getX(), bottom_left.getY(), bottom_left.getZ());
			m_vertices[m_vertexCount].tex = DirectX::XMFLOAT2(0.0f, height + (snow[p_index[i].part_no].v1)/*spark[p_index[i].part_no].v2*/);
			m_vertices[m_vertexCount].color = col;
			m_vertexCount++;
			// Top right.
			m_vertices[m_vertexCount].pos = DirectX::XMFLOAT3(top_right.getX(), top_right.getY(), top_right.getZ());
			m_vertices[m_vertexCount].tex = DirectX::XMFLOAT2(1.0f, 0.0f + (snow[p_index[i].part_no].v1)/*spark[p_index[i].part_no].v1*/);
			m_vertices[m_vertexCount].color = col;
			m_vertexCount++;



			//// Top left.
			//m_vertices[m_vertexCount].pos = DirectX::XMFLOAT3(px - (xsize), py + sizeB, pz - (zsize));
			//m_vertices[m_vertexCount].tex = DirectX::XMFLOAT2(0.5f + (ang_cos - ang_sin), point1 + (snow[p_index[i].part_no].v1)/*spark[p_index[i].part_no].v1*/);
			//m_vertices[m_vertexCount].color = col;
			//m_vertexCount++;
			//// Bottom right.
			//m_vertices[m_vertexCount].pos = DirectX::XMFLOAT3(px + (xsize), py - sizeB, pz + (zsize));
			//m_vertices[m_vertexCount].tex = DirectX::XMFLOAT2(0.5f + (ang_cos2 - ang_sin2), point2 + (snow[p_index[i].part_no].v1)/*spark[p_index[i].part_no].v2*/);
			//m_vertices[m_vertexCount].color = col;
			//m_vertexCount++;
			//// Bottom left.
			//m_vertices[m_vertexCount].pos = DirectX::XMFLOAT3(px - (xsize), py - sizeB, pz - (zsize));
			//m_vertices[m_vertexCount].tex = DirectX::XMFLOAT2(0.5f + (ang_cos - ang_sin2), point3 + (snow[p_index[i].part_no].v1)/*spark[p_index[i].part_no].v2*/);
			//m_vertices[m_vertexCount].color = col;
			//m_vertexCount++;
			//// Top right.
			//m_vertices[m_vertexCount].pos = DirectX::XMFLOAT3(px + (xsize), py + sizeB, pz + (zsize));
			//m_vertices[m_vertexCount].tex = DirectX::XMFLOAT2(0.5f + (ang_cos2 - ang_sin), point4 + (snow[p_index[i].part_no].v1)/*spark[p_index[i].part_no].v1*/);
			//m_vertices[m_vertexCount].color = col;
			//m_vertexCount++;

			m_indices[m_indexCount++] = index;
			m_indices[m_indexCount++] = index+1;
			m_indices[m_indexCount++] = index+2;
			m_indices[m_indexCount++] = index;
			m_indices[m_indexCount++] = index+3;
			m_indices[m_indexCount++] = index+1;
			
		}
	}
}


