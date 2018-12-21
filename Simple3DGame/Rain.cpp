#include "pch.h"
#include "rain.h"



using namespace Simple3DGame;
using namespace DX;
using namespace DirectX;

int RainSortCB(const VOID* arg1, const VOID* arg2)
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

void Rain::Reset()
{
	for (int i = 0; i<m_maxParticles; i++)
	{
		snow[i].bActive = 0;
	}

}


void Rain::Initialize(Level* pp_Level, Player* pp_Player, Camera^ pp_Camera)
{
	p_Level = pp_Level;

	p_Player = pp_Player;

	snow = new rain_t[1000];

	p_index = new part_index[1000];

	bUpdate = true;

	current_particle_que = 0;

	m_particleSize = 1.0f;

	m_maxParticles = 1000;

	current_index = 0;

	current_point = 0;

	total_collected = 0;

	noise_position = 0.0f;

	InitializeBuffers();

	p_Camera = pp_Camera;

	pn = new PerlinNoise(237);

	CreateDDSTextureFromFile((ID3D11Device *)m_deviceResources->GetD3DDevice(), L"Assets/Textures/rain_drop.dds", nullptr, &m_Texture, MAXSIZE_T);

	for (int i = 0; i<m_maxParticles; i++)
	{
		snow[i].bActive = 0;
	}

}



void Rain::CreateOne(float x, float y, float z)
{
	int point_find = 0;

	do
	{
		point_find++;

		if (snow[current_point].bActive == 0)
		{
			snow[current_point].bActive = 1;
			snow[current_point].vel_y = 2.0f+(rand() % 50) / 50.0f;
			snow[current_point].x = x;
			snow[current_point].y = y;
			snow[current_point].z = z;
			snow[current_point].v1 = 0.125f*(float)(rand() % 8);
			snow[current_point].v2 = snow[current_point].v1 + 0.125f;
			snow[current_point].life = 100.0f;
			snow[current_point].stopped = 0;
			if (abs(snow[current_point].z) < 4.0f)
			{
				snow[current_point].floor_height = 0.0f;
			}
			else
			{
				snow[current_point].floor_height = p_Level->RayGetTerrainHeight(snow[current_point].x, snow[current_point].z);
			}
			break;
		}
		else
		{
			current_point++;
			if (current_point>m_maxParticles - 2)
				current_point = 0;
		}

	} while (point_find < m_maxParticles - 2);
	//snow[current_point].v1 = 0.125f*(float)num;
	//snow[current_point].v2 = snow[current_point].v1 + 0.125f;

	current_point++;
	if (current_point>m_maxParticles - 2)
		current_point = 0;
}




Rain::~Rain(void)
{


}


void Rain::Update(float timeTotal, float timeDelta)
{
	int i,j;
	//timeDelta *= 1.1f;

	//angle=angle+0.01f;
	XMFLOAT4 col = DirectX::XMFLOAT4(0.8f, 0.8f, 1.0f, 1.0f);

	for (i = 0; i<m_maxParticles; i++)
	{
		if (snow[i].bActive == 1)
		{
			
			if (snow[i].y <= snow[i].floor_height)
			{
				//	snow[i].vel_y = 0.0f;
				snow[i].bActive = 0;
				p_Splash->CreateOne(snow[i].x, snow[i].floor_height, snow[i].z, col, 2);
				p_Splash->CreateOne(snow[i].x, snow[i].floor_height, snow[i].z, col, 2);
				//snow[i].y = p_Level->GetTerrainHeight(snow[i].x, snow[i].z);
			}
			else
			{
				snow[i].y -= snow[i].vel_y*timeDelta*35.0f;

				if (snow[i].z < p_Player->GetPosition()->getZ() - FAR_DIST)// && (p_Player->GetPosition()->getZ() - FAR_DIST) > -p_Level->front_back_walls)
				{
					snow[i].z += FAR_DIST * 2.0f;
					if (snow[i].stopped == 1)
					{
						snow[i].y = snow[i].floor_height;
					}
				}
				else
				{
					if (snow[i].z > p_Player->GetPosition()->getZ() + FAR_DIST)// && (p_Player->GetPosition()->getZ() + FAR_DIST) < p_Level->front_back_walls)
					{
						snow[i].z -=  FAR_DIST * 2.0f;
						if (snow[i].stopped == 1)
						{
							snow[i].y = snow[i].floor_height;
						}
					}
				}
			}
		}
	}

	/*
	btVector3 player_direct = *p_Camera->GetLookingDirection();
	//btVector3 player_position = p_Camera->Eye();
	btVector3 rand_point;// = btVector3(((float)(rand() % 50))) - 25.0f, 20.0f, ((float)(rand() % 50)) - 25.0f);
	XMFLOAT4X4 x_mat = *p_Camera->GetViewMatrix();
	btVector3 tran;
	btQuaternion quat_rot;
	btMatrix3x3 rot_mat = btMatrix3x3(x_mat._11, x_mat._12, x_mat._13, x_mat._21, x_mat._22, x_mat._23, x_mat._31, x_mat._32, x_mat._33);
	for (i = 0; i < 30; i++)
	{
		rand_point = btVector3(((float)(rand() % 50)) - 25.0f, 20.0f, ((float)(rand() % 50)) - 25.0f);
		rand_point = rand_point*rot_mat;
		CreateOne(x_mat._14 + rand_point.getX(), 20.0f, x_mat._34 + rand_point.getZ());
	}
	*/




	float noise_scale = 0.01f;

	float noises = pn->noise(noise_position, 0.0f, 0.8)*100.0f;
	int num_noise = (int)pn->noise(noise_position, 0.0f , 0.8)*100.0f;


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

		float noise = pn->noise(((float)randx*noise_scale) + noise_position, ((float)randz *noise_scale)+noise_position, 0.8);

		noise = (noise - 3.0f)*30.0f;

		if ((rand() % (int)noise)>39)
		{
			CreateOne(randx, 20.0f, randz);
		}
	}

	/*
	CreateOne(-p_Level->left_right_walls + 2.0f + (float(rand() % (int)(p_Level->left_right_walls * 2.0f))), 20.0f, p_Player->GetPosition()->getZ() - FAR_DIST + float(float(rand() % int(FAR_DIST*20.0f)) / 10.0f));
	CreateOne(-p_Level->left_right_walls + 2.0f + (float(rand() % (int)(p_Level->left_right_walls * 2.0f))), 20.0f, p_Player->GetPosition()->getZ() - FAR_DIST + float(float(rand() % int(FAR_DIST*20.0f)) / 10.0f));
	CreateOne(-p_Level->left_right_walls + 2.0f + (float(rand() % (int)(p_Level->left_right_walls * 2.0f))), 20.0f, p_Player->GetPosition()->getZ() - FAR_DIST + float(float(rand() % int(FAR_DIST*20.0f)) / 10.0f));
	CreateOne(-p_Level->left_right_walls + 2.0f + (float(rand() % (int)(p_Level->left_right_walls * 2.0f))), 20.0f, p_Player->GetPosition()->getZ() - FAR_DIST + float(float(rand() % int(FAR_DIST*20.0f)) / 10.0f));
	CreateOne(-p_Level->left_right_walls + 2.0f + (float(rand() % (int)(p_Level->left_right_walls * 2.0f))), 20.0f, p_Player->GetPosition()->getZ() - FAR_DIST + float(float(rand() % int(FAR_DIST*20.0f)) / 10.0f));
	CreateOne(-p_Level->left_right_walls + 2.0f + (float(rand() % (int)(p_Level->left_right_walls * 2.0f))), 20.0f, p_Player->GetPosition()->getZ() - FAR_DIST + float(float(rand() % int(FAR_DIST*20.0f)) / 10.0f));
	CreateOne(-p_Level->left_right_walls + 2.0f + (float(rand() % (int)(p_Level->left_right_walls * 2.0f))), 20.0f, p_Player->GetPosition()->getZ() - FAR_DIST + float(float(rand() % int(FAR_DIST*20.0f)) / 10.0f));
	CreateOne(-p_Level->left_right_walls + 2.0f + (float(rand() % (int)(p_Level->left_right_walls * 2.0f))), 20.0f, p_Player->GetPosition()->getZ() - FAR_DIST + float(float(rand() % int(FAR_DIST*20.0f)) / 10.0f));
	CreateOne(-p_Level->left_right_walls + 2.0f + (float(rand() % (int)(p_Level->left_right_walls * 2.0f))), 20.0f, p_Player->GetPosition()->getZ() - FAR_DIST + float(float(rand() % int(FAR_DIST*20.0f)) / 10.0f));
	CreateOne(-p_Level->left_right_walls + 2.0f + (float(rand() % (int)(p_Level->left_right_walls * 2.0f))), 20.0f, p_Player->GetPosition()->getZ() - FAR_DIST + float(float(rand() % int(FAR_DIST*20.0f)) / 10.0f));
	CreateOne(-p_Level->left_right_walls + 2.0f + (float(rand() % (int)(p_Level->left_right_walls * 2.0f))), 20.0f, p_Player->GetPosition()->getZ() - FAR_DIST + float(float(rand() % int(FAR_DIST*20.0f)) / 10.0f));
	CreateOne(-p_Level->left_right_walls + 2.0f + (float(rand() % (int)(p_Level->left_right_walls * 2.0f))), 20.0f, p_Player->GetPosition()->getZ() - FAR_DIST + float(float(rand() % int(FAR_DIST*20.0f)) / 10.0f));
	CreateOne(-p_Level->left_right_walls + 2.0f + (float(rand() % (int)(p_Level->left_right_walls * 2.0f))), 20.0f, p_Player->GetPosition()->getZ() - FAR_DIST + float(float(rand() % int(FAR_DIST*20.0f)) / 10.0f));
	CreateOne(-p_Level->left_right_walls + 2.0f + (float(rand() % (int)(p_Level->left_right_walls * 2.0f))), 20.0f, p_Player->GetPosition()->getZ() - FAR_DIST + float(float(rand() % int(FAR_DIST*20.0f)) / 10.0f));
	CreateOne(-p_Level->left_right_walls + 2.0f + (float(rand() % (int)(p_Level->left_right_walls * 2.0f))), 20.0f, p_Player->GetPosition()->getZ() - FAR_DIST + float(float(rand() % int(FAR_DIST*20.0f)) / 10.0f));
	CreateOne(-p_Level->left_right_walls + 2.0f + (float(rand() % (int)(p_Level->left_right_walls * 2.0f))), 20.0f, p_Player->GetPosition()->getZ() - FAR_DIST + float(float(rand() % int(FAR_DIST*20.0f)) / 10.0f));
	CreateOne(-p_Level->left_right_walls + 2.0f + (float(rand() % (int)(p_Level->left_right_walls * 2.0f))), 20.0f, p_Player->GetPosition()->getZ() - FAR_DIST + float(float(rand() % int(FAR_DIST*20.0f)) / 10.0f));
	CreateOne(-p_Level->left_right_walls + 2.0f + (float(rand() % (int)(p_Level->left_right_walls * 2.0f))), 20.0f, p_Player->GetPosition()->getZ() - FAR_DIST + float(float(rand() % int(FAR_DIST*20.0f)) / 10.0f));
	*/


	CreateVerticies();

}




void Rain::UpdateProjectionMatrix(DirectX::XMMATRIX *projectionMatrix)
{
	XMStoreFloat4x4(&m_constantBufferB.projection, *projectionMatrix);
}


void Rain::Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> deviceContext, Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer, ModelViewProjectionConstantBuffer* constantBufferData)
{
	UpdateVertecies(deviceContext.Get());

	Sparticle::Render(deviceContext, constantBuffer, constantBufferData);
}

void Rain::SetRainLigtness(float _rain_lightness)
{
	rain_lightness = _rain_lightness;

}

bool Rain::CreateVerticies()
{
	int index, i;
	int particle_count = 0;

	btVector3 eye;

	current_index = 0;

	eye = btVector3(p_Camera->EyeX(), p_Camera->EyeY(), p_Camera->EyeZ());


	for (i = 0; i<m_maxParticles; i++)
	{
		if (snow[i].bActive == 1)
		{
			if( p_Camera->CheckPoint(snow[i].x, snow[i].y, snow[i].z, 1.0f) == true )
			{
				if (snow[i].z<p_Level->front_back_walls && snow[i].z > -p_Level->front_back_walls)
				{
					//btVector3 part_pos(snow[i].x, snow[i].y, snow[i].z);

					p_index[current_index].part_no = i;
					//p_index[current_index].dist = part_pos.distance(eye);
					current_index++;
					particle_count++;
				}
			}
			else
			{
				snow[i].z = -snow[i].z;
				snow[i].x = -snow[i].x;

				if (snow[i].z<p_Level->front_back_walls && snow[i].z > -p_Level->front_back_walls)
				{
					p_index[current_index].part_no = i;
					//p_index[current_index].dist = part_pos.distance(eye);
					current_index++;
					particle_count++;
				}
			}
		}
	}

	if (particle_count>0)
	{
		qsort(p_index, particle_count, sizeof(part_index), RainSortCB);
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
	float size = 0.4f;
	float sizeB = 0.8f;

	float xsize = p_Camera->LookingTanX() *0.04f;
	float zsize = p_Camera->LookingTanZ()*0.04f;

	m_vertexCount = 0;
	m_indexCount = 0;

	//particle_count=1;
	if (true) //(total_active<(m_maxParticles*20)-1)
	{
		//bUpdate=false;
		for (i = 0; i<particle_count; i++)
		{
			//=m_particleSize;

			//if ()
				//continue;

			px = snow[p_index[i].part_no].x;
			py = snow[p_index[i].part_no].y;
			pz = snow[p_index[i].part_no].z;

			if (false ) //snow[p_index[i].part_no].life < 10.0f)
			{
				col = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, snow[p_index[i].part_no].life*0.1f);
			}
			else
			{
				col = DirectX::XMFLOAT4(rain_lightness, rain_lightness, rain_lightness, 0.0f);
			}

			index = m_vertexCount;

			// Top left.
			m_vertices[m_vertexCount].pos = DirectX::XMFLOAT3(px - (xsize), py + sizeB, pz - (zsize));
			m_vertices[m_vertexCount].tex = DirectX::XMFLOAT2(0.0f, 0.0f);
			m_vertices[m_vertexCount].color = col;
			m_vertices[m_vertexCount].norm = DirectX::XMFLOAT3(0.0f,-1.0f,0.0f);
			m_vertexCount++;
			// Bottom right.
			m_vertices[m_vertexCount].pos = DirectX::XMFLOAT3(px + (xsize), py - sizeB, pz + (zsize));
			m_vertices[m_vertexCount].tex = DirectX::XMFLOAT2(1.0f, 1.0f);
			m_vertices[m_vertexCount].color = col;
			m_vertices[m_vertexCount].norm = DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f);
			m_vertexCount++;
			// Bottom left.
			m_vertices[m_vertexCount].pos = DirectX::XMFLOAT3(px - (xsize), py - sizeB, pz - (zsize));
			m_vertices[m_vertexCount].tex = DirectX::XMFLOAT2(0.0f, 1.0f);
			m_vertices[m_vertexCount].color = col;
			m_vertices[m_vertexCount].norm = DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f);
			m_vertexCount++;

			// Top right.
			m_vertices[m_vertexCount].pos = DirectX::XMFLOAT3(px + (xsize), py + sizeB, pz + (zsize));
			m_vertices[m_vertexCount].tex = DirectX::XMFLOAT2(1.0f, 0.0f);
			m_vertices[m_vertexCount].color = col;
			m_vertices[m_vertexCount].norm = DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f);
			m_vertexCount++;

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
