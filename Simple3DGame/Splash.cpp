#include "pch.h"
#include "Splash.h"

using namespace Simple3DGame;
using namespace DX;
using namespace DirectX;

int SplashSortCB(const VOID* arg1, const VOID* arg2)
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


void Splash::CreateOne(float x, float y, float z, DirectX::XMFLOAT4 col, int start_delay)
{
	int point_find = 0;

	do
	{
		point_find++;

		if (true)//(spark[current_point].bActive == 0)
		{
			spark[current_point].bActive = 1;
			spark[current_point].mom.setX(1.5f - (rand() % 150) / 50.0f);
			spark[current_point].mom.setY(1.5f + (rand() % 150) / 30.0f);
			spark[current_point].mom.setZ(1.5f - (rand() % 150) / 50.0f);
			spark[current_point].pos.setX(x);
			spark[current_point].pos.setY(y);
			spark[current_point].pos.setZ(z);
			spark[current_point].v1 = 0.125f*(float)(rand() % 8);
			spark[current_point].v2 = spark[current_point].v1 + 0.125f;
			spark[current_point].life = 10.0f;
			spark[current_point].stopped = 0;
			spark[current_point].angle = 0.0f;
			spark[current_point].angle_mom = 0.1f - (float)((rand() % 20) / 100.0f);
			spark[current_point].size = 2.0f;

			spark[current_point].nx = 0.0f;
			spark[current_point].ny = -1.0f;
			spark[current_point].nz = 0.0f;


			spark[current_point].col = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);

			spark[current_point].start_delay = start_delay;

			break;
		}
		else
		{
			current_point++;
			if (current_point>m_maxParticles - 2)
				current_point = 0;
		}

	} while (point_find < m_maxParticles - 2);
	//Splash[current_point].v1 = 0.125f*(float)num;
	//Splash[current_point].v2 = Splash[current_point].v1 + 0.125f;

	current_point++;
	if (current_point>m_maxParticles - 2)
		current_point = 0;
}

void Splash::Initialize(Level* pp_Level, Player* pp_Player, Camera^ pp_Camera)
{
	p_Level = pp_Level;

	p_Player = pp_Player;

	spark = new Splash_t[10000];

	p_index = new part_index[10000];

	bUpdate = true;

	current_particle_que = 0;

	m_particleSize = 1.0f;

	m_maxParticles = 10000;

	current_index = 0;

	current_point = 0;

	total_collected = 0;

	InitializeBuffers();

	p_Camera = pp_Camera;

	CreateDDSTextureFromFile((ID3D11Device *)m_deviceResources->GetD3DDevice(), L"Assets/Textures/spark.dds", nullptr, &m_Texture, MAXSIZE_T);

	for (int i = 0; i<m_maxParticles; i++)
	{
		spark[i].bActive = 0;
	}

}





Splash::~Splash(void)
{


}


void Splash::Update(float timeTotal, float timeDelta)
{
	int i;
	//timeDelta *= 1.1f;

	//angle=angle+0.01f;

	for (i = 0; i<m_maxParticles; i++)
	{
		if (spark[i].start_delay > 0)
			spark[i].start_delay--;

		if (spark[i].bActive == 1 && spark[i].start_delay == 0)
		{
			//spark[i].angle += (float(rand()%100)/100.0f)*M_PI*2.0f;
			spark[i].angle += spark[i].angle_mom;
			if (spark[i].angle > M_PI*2.0f)
			{
				spark[i].angle -= M_PI*2.0f;
			}
			else
			{
				if (spark[i].angle < 0.0f)
				{
					spark[i].angle += M_PI*2.0f;
				}
			}

			if (true) //(spark[i].stopped == 1 && spark[i].bActive == 1)
			{
				spark[i].life -= 10.0f*timeDelta;
				if (spark[i].life < 0)
				{
					spark[i].bActive = 0;
				}
			}

			if(spark[i].pos.getY() <= p_Level->RayGetTerrainHeight(spark[i].pos.getX(), spark[i].pos.getZ()))
			{
				//spark[i].

				if (spark[i].mom.getY()<1.5f && spark[i].mom.getY()>-1.5f)
				{
					spark[i].stopped = 1;
					spark[i].mom.setX(0.0f);
					spark[i].mom.setY(0.0f);
					spark[i].mom.setZ(0.0f);
				}
				spark[i].pos.setY(p_Level->RayGetTerrainHeight(spark[i].pos.getX(), spark[i].pos.getZ()));
				spark[i].mom.setY(-spark[i].mom.getY());
				spark[i].mom.setY(spark[i].mom.getY() * 0.8f);
			}
			else
			{

			}
			if(true)
			{
			spark[i].pos.setX(spark[i].pos.getX() + spark[i].mom.getX()*timeDelta);
			spark[i].pos.setZ(spark[i].pos.getZ() + spark[i].mom.getZ()*timeDelta);

			spark[i].pos.setY(spark[i].pos.getY() + spark[i].mom.getY()*timeDelta);


			spark[i].mom.setY(spark[i].mom.getY() - 25.0f*timeDelta);

			if (spark[i].pos.getY()<-10.0f)
			{
				spark[i].bActive = 0;
			}
			}
		}
	}
	//p_Level->
	btVector3 player_pos = *p_Player->GetPosition();

	CreateVerticies();

}



void Splash::Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> deviceContext, Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer, ModelViewProjectionConstantBuffer* constantBufferData)
{
	UpdateVertecies(deviceContext.Get());

	Sparticle::Render(deviceContext, constantBuffer, constantBufferData);
}

void Splash::SetSplashLigtness(float _splash_lightness)
{
	splash_lightness = _splash_lightness;

}


void Splash::CreateVerticies()
{
	int index, i;
	int particle_count = 0;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT result;
	VertexPositionTex* verticesPtr;
	btVector3 eye;

	current_index = 0;

	eye = btVector3(p_Camera->EyeX(), p_Camera->EyeY(), p_Camera->EyeZ());


	for (i = 0; i<m_maxParticles; i++)
	{
		if (spark[i].bActive == 1 && p_Camera->CheckPoint(spark[i].pos.getX(), spark[i].pos.getY(), spark[i].pos.getZ(), 1.0f) == true
			&& spark[i].pos.getZ()<p_Level->front_back_walls && spark[i].pos.getZ() > -p_Level->front_back_walls)
		{
			//btVector3 part_pos(spark[i].pos.getX(), spark[i].pos.getY(), spark[i].pos.getZ());

			p_index[current_index].part_no = i;
			//p_index[current_index].dist = part_pos.distance(eye);
			current_index++;
			particle_count++;
		}
	}

	if (particle_count>0)
	{
		qsort(p_index, particle_count, sizeof(part_index), SplashSortCB);
	}

	//particle_count=1;
	memset(m_vertices, 0, (sizeof(VertexPositionTex) * (m_vertexCount)));

	m_vertexCount = 0;
	m_indexCount = 0;

	// Now build the vertex array from the particle list array.  Each particle is a quad made out of two triangles.
	index = 0;
	int total_active = 0;
	float px;
	float py;
	float pz;
	DirectX::XMFLOAT4 col;
	float size = 0.2f;
	float sizeB = 0.1f;

	float xsize = p_Camera->LookingTanX() * size;
	float zsize = p_Camera->LookingTanZ() * size;

	DirectX::XMFLOAT3 norm = DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f);

	//particle_count=1;
	if (true) //(total_active<(m_maxParticles*20)-1)
	{
		//bUpdate=false;
		for (i = 0; i<particle_count; i++)
		{
			if (spark[p_index[i].part_no].size>0.1f)
				spark[p_index[i].part_no].size -= 0.005f;

			size = spark[p_index[i].part_no].size*0.3f;
			sizeB = size*0.5f;

			xsize = p_Camera->LookingTanX() * size;
			zsize = p_Camera->LookingTanZ() * size;


			//=m_particleSize;
			float ang_cos = -0.5f * cos(spark[p_index[i].part_no].angle);
			float ang_sin = -0.5f * sin(spark[p_index[i].part_no].angle);

			float ang_cos2 = 0.5f * cos(spark[p_index[i].part_no].angle);
			float ang_sin2 = 0.5f * sin(spark[p_index[i].part_no].angle);

			px = spark[p_index[i].part_no].pos.getX();
			py = spark[p_index[i].part_no].pos.getY();
			pz = spark[p_index[i].part_no].pos.getZ();

			
			col = DirectX::XMFLOAT4(splash_lightness , splash_lightness, splash_lightness, -0.3f);

			if(spark[p_index[i].part_no].life<10.0f)
			{
				xsize *= (spark[p_index[i].part_no].life*0.1f);
				zsize *= (spark[p_index[i].part_no].life*0.1f);
				sizeB *= (spark[p_index[i].part_no].life*0.1f);
				col = DirectX::XMFLOAT4(0.5f, 0.5f, 1.0f, -1.5f+(spark[p_index[i].part_no].life*0.1f));
				//col = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, spark[p_index[i].part_no].life*0.1f);
				//spark[p_index[i].part_no].col.w = spark[p_index[i].part_no].life*0.1f;
			}


			total_active++;
			index = m_vertexCount;

			// Top left.
			m_vertices[m_vertexCount].pos = DirectX::XMFLOAT3(px - (xsize), py + sizeB, pz - (zsize));
			m_vertices[m_vertexCount].tex = DirectX::XMFLOAT2(0.5f + (ang_cos - ang_sin), 0.5f + (ang_sin + ang_cos)/*spark[p_index[i].part_no].v1*/);
			m_vertices[m_vertexCount].color = col;
			m_vertices[m_vertexCount].norm = norm;
			m_vertexCount++;
			// Bottom right.
			m_vertices[m_vertexCount].pos = DirectX::XMFLOAT3(px + (xsize), py - sizeB, pz + (zsize));
			m_vertices[m_vertexCount].tex = DirectX::XMFLOAT2(0.5f + (ang_cos2 - ang_sin2), 0.5f + (ang_sin2 + ang_cos2)/*spark[p_index[i].part_no].v2*/);
			m_vertices[m_vertexCount].color = col;
			m_vertices[m_vertexCount].norm = norm;
			m_vertexCount++;
			// Bottom left.
			m_vertices[m_vertexCount].pos = DirectX::XMFLOAT3(px - (xsize), py - sizeB, pz - (zsize));
			m_vertices[m_vertexCount].tex = DirectX::XMFLOAT2(0.5f + (ang_cos - ang_sin2), 0.5f + (ang_sin + ang_cos2)/*spark[p_index[i].part_no].v2*/);
			m_vertices[m_vertexCount].color = col;
			m_vertices[m_vertexCount].norm = norm;
			m_vertexCount++;
			// Top right.
			m_vertices[m_vertexCount].pos = DirectX::XMFLOAT3(px + (xsize), py + sizeB, pz + (zsize));
			m_vertices[m_vertexCount].tex = DirectX::XMFLOAT2(0.5f + (ang_cos2 - ang_sin), 0.5f + (ang_sin2 + ang_cos)/*spark[p_index[i].part_no].v1*/);
			m_vertices[m_vertexCount].color = col;
			m_vertices[m_vertexCount].norm = norm;
			m_vertexCount++;

			m_indices[m_indexCount++] = index;
			m_indices[m_indexCount++] = index + 1;
			m_indices[m_indexCount++] = index + 2;
			m_indices[m_indexCount++] = index;
			m_indices[m_indexCount++] = index + 3;
			m_indices[m_indexCount++] = index + 1;

		}

	}


	return ;
}