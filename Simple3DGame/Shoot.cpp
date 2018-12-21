#include "pch.h"
#include "Shoot.h"

using namespace Simple3DGame;
using namespace DX;
using namespace DirectX;

int ShootSortCB(const VOID* arg1, const VOID* arg2)
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

void Shoot::CreateRocket(float x, float y, float z, float mom_x, float mom_y, float mom_z, int type, DirectX::XMFLOAT3X3 point_dir, float p_x, float p_y, float p_z)
{
	int point_find = 0;

	do
	{
		
		point_find++;

		if (arrow[current_arrow].bActive == 0)
		{
			arrow[current_arrow].p_dir = XMLoadFloat3x3(&point_dir);
			arrow[current_arrow].point = btVector3(p_x,p_y,p_z);
			arrow[current_arrow].bActive = 1;
			arrow[current_arrow].mom.setX(mom_x);
			arrow[current_arrow].mom.setY(mom_y);
			arrow[current_arrow].mom.setZ(mom_z);
			arrow[current_arrow].dir = arrow[current_arrow].mom;// .normalized()
			arrow[current_arrow].dir.normalize();
			arrow[current_arrow].bCollide = 0;

			arrow[current_arrow].pos.setX(x);
			arrow[current_arrow].pos.setY(y);
			arrow[current_arrow].pos.setZ(z);
			arrow[current_arrow].v1 = 0.125f*(float)(rand() % 8);
			arrow[current_arrow].v2 = spark[current_point].v1 + 0.125f;
			arrow[current_arrow].life = 100.0f;
			arrow[current_arrow].stopped = 0;
			arrow[current_arrow].angle = 0.0f;
			arrow[current_arrow].angle_mom = 0.1f - (float)((rand() % 20) / 100.0f);
			arrow[current_arrow].size = 2.0f;

			//arrow[current_arrow].type = type;

			arrow[current_arrow].col = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

			arrow[current_arrow].start_delay = 0;

			break;
		}
		else
		{
			current_arrow++;
			if (current_arrow>m_maxArrows - 2)
				current_arrow = 0;
		}

	} while (point_find < m_maxArrows - 2);
	//Shoot[current_point].v1 = 0.125f*(float)num;
	//Shoot[current_point].v2 = Shoot[current_point].v1 + 0.125f;

	current_arrow++;
	if (current_arrow>m_maxArrows - 2)
		current_arrow = 0;

}

void Shoot::CreateOne(float x, float y, float z, float mom_x, float mom_y, float mom_z, int type)
{
	int point_find = 0;

	do
	{
		point_find++;

		if (spark[current_point].bActive == 0)
		{
			spark[current_point].bActive = 1;
			spark[current_point].mom.setX(mom_x);
			spark[current_point].mom.setY(mom_y);
			spark[current_point].mom.setZ(mom_z);
			spark[current_point].pos.setX(x);
			spark[current_point].pos.setY(y);
			spark[current_point].pos.setZ(z);
			spark[current_point].v1 = 0.125f*(float)(rand() % 8);
			spark[current_point].v2 = spark[current_point].v1 + 0.125f;
			spark[current_point].life = 10.0f;
			spark[current_point].stopped = 0;
			spark[current_point].angle = 0.0f;
			spark[current_point].angle_mom = 0.1f - (float)((rand() % 20) / 100.0f);
			spark[current_point].size = 1.0f;
			
			spark[current_point].type = type;

			spark[current_point].col = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

			spark[current_point].start_delay = 0;

			break;
		}
		else
		{
			current_point++;
			if (current_point>m_maxParticles - 2)
				current_point = 0;
		}

	} while (point_find < m_maxParticles - 2);
	//Shoot[current_point].v1 = 0.125f*(float)num;
	//Shoot[current_point].v2 = Shoot[current_point].v1 + 0.125f;



	current_point++;
	if (current_point>m_maxParticles - 2)
		current_point = 0;
}


Shoot::Shoot(std::shared_ptr<DX::DeviceResources> pm_deviceResources, Level* pp_Level, Player* pp_Player, Camera^ pp_Camera)
{
	int i;

	m_deviceResources = pm_deviceResources;

	p_Level = pp_Level;

	p_Player = pp_Player;

	m_maxArrows = 50;
	m_maxParticles = 5000;

	arrow = new Arrow_t[50];
	spark = new Shoot_t[5000];
	
	p_index = new part_index[5000];

	bUpdate = true;

	current_particle_que = 0;

	m_particleSize = 1.0f;

	current_index = 0;

	current_point = 0;
	current_arrow = 0;

	total_collected = 0;

	InitializeBuffers();

	p_Camera = pp_Camera;

	CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), L"Assets/Textures/spark.dds", nullptr, &m_Texture, MAXSIZE_T);

	CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), L"Assets/Textures/poke.dds", nullptr, &m_Texture_rocket, MAXSIZE_T);

	m_model_rocket = new ModelLoader(m_deviceResources);

	m_model_rocket->Load("Assets/Models/rocket.obj",  -1.0f);


	for (i = 0; i<m_maxParticles; i++)
	{
		spark[i].bActive = 0;
	}

	for (i = 0; i<m_maxArrows; i++)
	{
		arrow[i].bActive = 0;
	}

	//DirectX::XMMATRIX scaleMatrix = XMMatrixScaling(1.0f, 1.0f, 1.0f);

	auto rotationMatrix = XMMatrixRotationRollPitchYaw(0.0f, 0.0f, 0.0f);
	auto translationMatrix = XMMatrixTranslation(0.0f, 0.0f, 0.0f);

	XMMATRIX scaleMatrix = XMMatrixScaling(1.0f, 1.0f, 1.0f);
	XMMATRIX scaleRotationMatrix = XMMatrixMultiply(scaleMatrix, rotationMatrix);
	XMMATRIX modelMatrix = XMMatrixMultiplyTranspose(scaleRotationMatrix, translationMatrix);



	//XMStoreFloat4x4(&m_constantBufferB.model, XMMatrixTranspose(rotationMatrix * translationMatrix));

	XMStoreFloat4x4(&m_constantBufferB.model, modelMatrix);

}




bool Shoot::InitializeBuffers()
{
	unsigned long* indices;
	int i;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	// Set the maximum number of vertices in the vertex array.
	m_vertexCount = m_maxParticles * 6;

	// Set the maximum number of indices in the index array.
	m_indexCount = m_vertexCount;

	// Create the vertex array for the particles that will be rendered.
	m_vertices = new ParticleVertexType[m_vertexCount];

	if (!m_vertices)
	{
		return false;
	}

	// Create the index array.
	indices = new unsigned long[m_indexCount];
	if (!indices)
	{
		return false;
	}

	// Initialize vertex array to zeros at first.
	memset(m_vertices, 0, (sizeof(ParticleVertexType) * m_vertexCount));

	// Initialize the index array.
	for (i = 0; i<m_indexCount; i++)
	{
		indices[i] = i;
	}

	// Set up the description of the dynamic vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(ParticleVertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = m_vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now finally create the vertex buffer.
	result = m_deviceResources->GetD3DDevice()->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = m_deviceResources->GetD3DDevice()->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Release the index array since it is no longer needed.
	delete[] indices;
	indices = 0;

	return true;
}


Shoot::~Shoot(void)
{


}


void Shoot::Update(float timeTotal, float timeDelta)
{
	int i;
	//timeDelta *= 1.1f;

	//angle=angle+0.01f;

	for (i = 0; i<m_maxArrows; i++)
	{
		if (arrow[i].start_delay > 0)
			arrow[i].start_delay--;

		if (arrow[i].bActive == 1 && arrow[i].start_delay == 0)
		{
			//arrow[i].angle += (float(rand()%100)/100.0f)*M_PI*2.0f;
			arrow[i].angle += arrow[i].angle_mom;
			if (arrow[i].angle > M_PI*2.0f)
			{
				arrow[i].angle -= M_PI*2.0f;
			}
			else
			{
				if (arrow[i].angle < 0.0f)
				{
					arrow[i].angle += M_PI*2.0f;
				}
			}

			if (true) //(arrow[i].stopped == 1 && arrow[i].bActive == 1)
			{
				arrow[i].life -= 53.0f*timeDelta;
				if (arrow[i].life < 0)
				{
					arrow[i].bActive = 0;
				}
			}


			//Shoot[i].life -= 17.0f*timeDelta;
			/*
			if (arrow[i].pos.getZ() < p_Player->GetPosition()->getZ() - FAR_DIST)
			{
			arrow[i].pos.setZ(p_Player->GetPosition()->getZ() + FAR_DIST);
			if (arrow[i].stopped == 1)
			{
			arrow[i].y = p_Level->GetTerrainHeight((arrow[i].z / 2.0f) + 4.5f, (arrow[i].x / 2.0f) + 19.0f);
			}
			}
			else
			{
			if (arrow[i].z > p_Player->GetPosition()->getZ() + FAR_DIST)
			{
			arrow[i].z = p_Player->GetPosition()->getZ() - FAR_DIST;
			if (arrow[i].stopped == 1)
			{
			arrow[i].y = p_Level->GetTerrainHeight((arrow[i].z / 2.0f) + 4.5f, (arrow[i].x / 2.0f) + 19.0f);
			}
			}
			}
			*/

			
			if (arrow[i].pos.getY() <= p_Level->GetTerrainHeight(arrow[i].pos.getX(), arrow[i].pos.getZ()) + 1.0f)
			{
				//arrow[i].

				//if (arrow[i].mom.getY()<1.5f && arrow[i].mom.getY()>-1.5f)
				//{
				//arrow[i].stopped = 1;
				//arrow[i].mom.setX(0.0f);
				//arrow[i].mom.setY(0.0f);
				//arrow[i].mom.setZ(0.0f);
				//}
				arrow[i].pos.setY(p_Level->GetTerrainHeight(arrow[i].pos.getX(), arrow[i].pos.getZ()) + 1.0f);
				//arrow[i].mom.setY(-arrow[i].mom.getY());
				//arrow[i].mom.setY(arrow[i].mom.getY() * 0.8f);
			}
			else
			{

			}
			
			float rnd = float((rand() % 100)*0.01f);
			CreateOne(arrow[i].pos.getX() + (arrow[i].point.getX()*rnd), arrow[i].pos.getY() + (arrow[i].point.getY()*rnd), arrow[i].pos.getZ() + (arrow[i].point.getZ()*rnd), arrow[i].mom.getX()*-0.3f, arrow[i].mom.getY()*-0.3f, arrow[i].mom.getZ()*-0.3f, 0);
			CreateOne(arrow[i].pos.getX() + (arrow[i].point.getX()*rnd), arrow[i].pos.getY() + (arrow[i].point.getY()*rnd), arrow[i].pos.getZ() + (arrow[i].point.getZ()*rnd), arrow[i].mom.getX()*-0.3f, arrow[i].mom.getY()*-0.3f, arrow[i].mom.getZ()*-0.3f, 0);
			CreateOne(arrow[i].pos.getX() + (arrow[i].point.getX()*rnd), arrow[i].pos.getY() + (arrow[i].point.getY()*rnd), arrow[i].pos.getZ() + (arrow[i].point.getZ()*rnd), arrow[i].mom.getX()*-0.3f, arrow[i].mom.getY()*-0.3f, arrow[i].mom.getZ()*-0.3f, 0);
			CreateOne(arrow[i].pos.getX() + (arrow[i].point.getX()*rnd), arrow[i].pos.getY() + (arrow[i].point.getY()*rnd), arrow[i].pos.getZ() + (arrow[i].point.getZ()*rnd), arrow[i].mom.getX()*-0.3f, arrow[i].mom.getY()*-0.3f, arrow[i].mom.getZ()*-0.3f, 0);


			arrow[i].pos.setX(arrow[i].pos.getX() + arrow[i].mom.getX()*timeDelta*50.0f);
			arrow[i].pos.setZ(arrow[i].pos.getZ() + arrow[i].mom.getZ()*timeDelta*50.0f);

			arrow[i].pos.setY(arrow[i].pos.getY() + arrow[i].mom.getY()*timeDelta*50.0f);

			arrow[i].mom*=(1.0f+(timeDelta*0.05f));

			//arrow[i].mom.setY(arrow[i].mom.getY() - 0.2f);
			/*
			if (arrow[i].pos.getY()<-10.0f)
			{
			arrow[i].bActive = 0;
			}
			*/
		}
	}

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
				spark[i].life -= 53.0f*timeDelta;
				if (spark[i].life < 0)
				{
					spark[i].bActive = 0;
				}
			}
			//Shoot[i].life -= 17.0f*timeDelta;
			/*
			if (spark[i].pos.getZ() < p_Player->GetPosition()->getZ() - FAR_DIST)
			{
			spark[i].pos.setZ(p_Player->GetPosition()->getZ() + FAR_DIST);
			if (spark[i].stopped == 1)
			{
			spark[i].y = p_Level->GetTerrainHeight((spark[i].z / 2.0f) + 4.5f, (spark[i].x / 2.0f) + 19.0f);
			}
			}
			else
			{
			if (spark[i].z > p_Player->GetPosition()->getZ() + FAR_DIST)
			{
			spark[i].z = p_Player->GetPosition()->getZ() - FAR_DIST;
			if (spark[i].stopped == 1)
			{
			spark[i].y = p_Level->GetTerrainHeight((spark[i].z / 2.0f) + 4.5f, (spark[i].x / 2.0f) + 19.0f);
			}
			}
			}
			*/

			/*
			if (spark[i].pos.getY() <= p_Level->GetTerrainHeight(spark[i].pos.getX(), spark[i].pos.getZ())+2.0f)
			{
				//spark[i].

				//if (spark[i].mom.getY()<1.5f && spark[i].mom.getY()>-1.5f)
				//{
					//spark[i].stopped = 1;
					//spark[i].mom.setX(0.0f);
					//spark[i].mom.setY(0.0f);
					//spark[i].mom.setZ(0.0f);
				//}
				spark[i].pos.setY(p_Level->GetTerrainHeight(spark[i].pos.getX(), spark[i].pos.getZ())+2.0f);
				//spark[i].mom.setY(-spark[i].mom.getY());
				//spark[i].mom.setY(spark[i].mom.getY() * 0.8f);
			}
			else
			{

			}
			*/
			spark[i].pos.setX(spark[i].pos.getX() + spark[i].mom.getX()*timeDelta*50.0f);
			spark[i].pos.setZ(spark[i].pos.getZ() + spark[i].mom.getZ()*timeDelta*50.0f);

			spark[i].pos.setY(spark[i].pos.getY() + spark[i].mom.getY()*timeDelta*50.0f);


			//spark[i].mom.setY(spark[i].mom.getY() - 0.2f);
			/*
			if (spark[i].pos.getY()<-10.0f)
			{
				spark[i].bActive = 0;
			}
			*/
		}
	}



}



void Shoot::UpdateProjectionMatrix(DirectX::XMMATRIX *projectionMatrix)
{
	XMStoreFloat4x4(&m_constantBufferB.projection, *projectionMatrix);
}


void Shoot::RenderRockets(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> deviceContext, Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer, ModelViewProjectionConstantBuffer* constantBufferData)
{
	unsigned int stride;
	unsigned int offset;
	stride = sizeof(VertexPositionTex);
	offset = 0;
	// Set vertex buffer stride and offset.


	// draw rocket then fire
	deviceContext->IASetVertexBuffers(0, 1, m_model_rocket->GetVertexBuffer().GetAddressOf(), &stride, &offset);
	deviceContext->IASetIndexBuffer(m_model_rocket->GetIndexBuffer().Get(), DXGI_FORMAT_R16_UINT, 0);

	deviceContext->PSSetShaderResources(0, 1, &m_Texture_rocket);

	for (int i = 0; i < m_maxArrows; i++)
	{
		if (arrow[i].bActive == true)
		{
			auto translationMatrix = XMMatrixTranslation(arrow[i].pos.getX(), arrow[i].pos.getY(), arrow[i].pos.getZ());
			XMStoreFloat4x4(&constantBufferData->model, XMMatrixTranspose(arrow[i].p_dir * translationMatrix));

			//constantBufferData->model = m_constantBuffer.model;
			//constantBufferData->model = m_constantBuffer.model;

			//m_constantBufferB.model = fruit[i].model;
			deviceContext->UpdateSubresource(constantBuffer.Get(), 0, NULL, constantBufferData, 0, 0);

			deviceContext->DrawIndexed(
				m_model_rocket->m_indicesCount,
				0,
				0
				);
		}
	}

}

void Shoot::Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> deviceContext, Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer, ModelViewProjectionConstantBuffer* constantBufferData)
{
	unsigned int stride;
	unsigned int offset;

	// Set vertex buffer stride and offset.
	stride = sizeof(ParticleVertexType);
	offset = 0;


	UpdateVertecies(deviceContext.Get());


	constantBufferData->model = m_constantBufferB.model;
	deviceContext->UpdateSubresource(constantBuffer.Get(), 0, NULL, constantBufferData, 0, 0);

	deviceContext->PSSetShaderResources(0, 1, &m_Texture);

	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	deviceContext->DrawIndexed(
		m_indexCount,
		0,
		0
		);
}


bool Shoot::UpdateVertecies(ID3D11DeviceContext* deviceContext)
{
	int index, i;
	int particle_count = 0;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT result;
	ParticleVertexType* verticesPtr;
	btVector3 eye;

	current_index = 0;

	eye = btVector3(p_Camera->EyeX(), p_Camera->EyeY(), p_Camera->EyeZ());


	for (i = 0; i<m_maxParticles; i++)
	{
		if (spark[i].bActive == 1 && spark[i].start_delay == 0)
		{
			btVector3 part_pos(spark[i].pos.getX(), spark[i].pos.getY(), spark[i].pos.getZ());

			p_index[current_index].part_no = i;
			p_index[current_index].dist = part_pos.distance(eye);
			current_index++;
			particle_count++;
		}
	}

	if (particle_count>0)
	{
		qsort(p_index, particle_count, sizeof(part_index), ShootSortCB);
	}

	//particle_count=1;
	memset(m_vertices, 0, (sizeof(ParticleVertexType) * (m_vertexCount)));

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

			if (spark[p_index[i].part_no].life<10.0f)
			{
				xsize *= (spark[p_index[i].part_no].life*0.1f);
				zsize *= (spark[p_index[i].part_no].life*0.1f);
				sizeB *= (spark[p_index[i].part_no].life*0.1f);
				spark[p_index[i].part_no].col = DirectX::XMFLOAT4(1.0f, size, size, 1.0f);
				//col = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, spark[p_index[i].part_no].life*0.1f);
				//spark[p_index[i].part_no].col.w = spark[p_index[i].part_no].life*0.1f;
			}
			else
			{
				//col = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
				//spark[p_index[i].part_no].col.w = 1.0f;
			}

			total_active++;


			// Top left.
			m_vertices[index].pos = DirectX::XMFLOAT3(px - (xsize), py + sizeB, pz - (zsize));
			m_vertices[index].tex = DirectX::XMFLOAT2(0.5f + (ang_cos - ang_sin), 0.5f + (ang_sin + ang_cos)/*spark[p_index[i].part_no].v1*/);
			m_vertices[index].color = spark[p_index[i].part_no].col;
			index++;
			// Bottom right.
			m_vertices[index].pos = DirectX::XMFLOAT3(px + (xsize), py - sizeB, pz + (zsize));
			m_vertices[index].tex = DirectX::XMFLOAT2(0.5f + (ang_cos2 - ang_sin2), 0.5f + (ang_sin2 + ang_cos2)/*spark[p_index[i].part_no].v2*/);
			m_vertices[index].color = spark[p_index[i].part_no].col;
			index++;
			// Bottom left.
			m_vertices[index].pos = DirectX::XMFLOAT3(px - (xsize), py - sizeB, pz - (zsize));
			m_vertices[index].tex = DirectX::XMFLOAT2(0.5f + (ang_cos - ang_sin2), 0.5f + (ang_sin + ang_cos2)/*spark[p_index[i].part_no].v2*/);
			m_vertices[index].color = spark[p_index[i].part_no].col;
			index++;
			// Top left.
			m_vertices[index].pos = DirectX::XMFLOAT3(px - (xsize), py + sizeB, pz - (zsize));
			m_vertices[index].tex = DirectX::XMFLOAT2(0.5f + (ang_cos - ang_sin), 0.5f + (ang_sin + ang_cos)/*spark[p_index[i].part_no].v1*/);
			m_vertices[index].color = spark[p_index[i].part_no].col;
			index++;
			// Top right.
			m_vertices[index].pos = DirectX::XMFLOAT3(px + (xsize), py + sizeB, pz + (zsize));
			m_vertices[index].tex = DirectX::XMFLOAT2(0.5f + (ang_cos2 - ang_sin), 0.5f + (ang_sin2 + ang_cos)/*spark[p_index[i].part_no].v1*/);
			m_vertices[index].color = spark[p_index[i].part_no].col;
			index++;
			// Bottom right.
			m_vertices[index].pos = DirectX::XMFLOAT3(px + (xsize), py - sizeB, pz + (zsize));
			m_vertices[index].tex = DirectX::XMFLOAT2(0.5f + (ang_cos2 - ang_sin2), 0.5f + (ang_sin2 + ang_cos2) /*spark[p_index[i].part_no].v2*/);
			m_vertices[index].color = spark[p_index[i].part_no].col;
			index++;


			/*
			// Top left.
			m_vertices[index].pos = DirectX::XMFLOAT3(px - (xsize), py + sizeB, pz - (zsize));
			m_vertices[index].tex = DirectX::XMFLOAT2(0.5f + (ang_cos - ang_sin), ((0.5 + (ang_sin + ang_cos))*0.125) + spark[p_index[i].part_no].v1);
			m_vertices[index].color = col;
			index++;
			// Bottom right.
			m_vertices[index].pos = DirectX::XMFLOAT3(px + (xsize), py - sizeB, pz + (zsize));
			m_vertices[index].tex = DirectX::XMFLOAT2(0.5f + (ang_cos2 - ang_sin2), ((0.5 + (ang_sin2 + ang_cos2))*0.125) + spark[p_index[i].part_no].v2);
			m_vertices[index].color = col;
			index++;
			// Bottom left.
			m_vertices[index].pos = DirectX::XMFLOAT3(px - (xsize), py - sizeB, pz - (zsize));
			m_vertices[index].tex = DirectX::XMFLOAT2(0.5f + (ang_cos - ang_sin2), ((0.5 + (ang_sin + ang_cos2))*0.125) + spark[p_index[i].part_no].v2);
			m_vertices[index].color = col;
			index++;
			// Top left.
			m_vertices[index].pos = DirectX::XMFLOAT3(px - (xsize), py + sizeB, pz - (zsize));
			m_vertices[index].tex = DirectX::XMFLOAT2(0.5f + (ang_cos - ang_sin), ((0.5 + (ang_sin + ang_cos))*0.125) + spark[p_index[i].part_no].v1);
			m_vertices[index].color = col;
			index++;
			// Top right.
			m_vertices[index].pos = DirectX::XMFLOAT3(px + (xsize), py + sizeB, pz + (zsize));
			m_vertices[index].tex = DirectX::XMFLOAT2(0.5f + (ang_cos2 - ang_sin), ((0.5 + (ang_sin2 + ang_cos))*0.125) + spark[p_index[i].part_no].v1);
			m_vertices[index].color = col;
			index++;
			// Bottom right.
			m_vertices[index].pos = DirectX::XMFLOAT3(px + (xsize), py - sizeB, pz + (zsize));
			m_vertices[index].tex = DirectX::XMFLOAT2(0.5f + (ang_cos2 - ang_sin2), ((0.5f + (ang_sin2 + ang_cos2))*0.125f) + spark[p_index[i].part_no].v2);
			m_vertices[index].color = col;
			index++;
			*/

			/*
			// Top left.
			m_vertices[index].pos = DirectX::XMFLOAT3(px - (xsize), py + sizeB, pz - (zsize));
			m_vertices[index].tex = DirectX::XMFLOAT2(0.0f, spark[p_index[i].part_no].v1);
			m_vertices[index].color = col;
			index++;
			// Bottom right.
			m_vertices[index].pos = DirectX::XMFLOAT3(px + (xsize), py - sizeB, pz + (zsize));
			m_vertices[index].tex = DirectX::XMFLOAT2(1.0f, spark[p_index[i].part_no].v2);
			m_vertices[index].color = col;
			index++;
			// Bottom left.
			m_vertices[index].pos = DirectX::XMFLOAT3(px - (xsize), py - sizeB, pz - (zsize));
			m_vertices[index].tex = DirectX::XMFLOAT2(0.0f, spark[p_index[i].part_no].v2);
			m_vertices[index].color = col;
			index++;
			// Top left.
			m_vertices[index].pos = DirectX::XMFLOAT3(px - (xsize), py + sizeB, pz - (zsize));
			m_vertices[index].tex = DirectX::XMFLOAT2(0.0f, spark[p_index[i].part_no].v1);
			m_vertices[index].color = col;
			index++;
			// Top right.
			m_vertices[index].pos = DirectX::XMFLOAT3(px + (xsize), py + sizeB, pz + (zsize));
			m_vertices[index].tex = DirectX::XMFLOAT2(1.0f, spark[p_index[i].part_no].v1);
			m_vertices[index].color = col;
			index++;
			// Bottom right.
			m_vertices[index].pos = DirectX::XMFLOAT3(px + (xsize), py - sizeB, pz + (zsize));
			m_vertices[index].tex = DirectX::XMFLOAT2(1.0f, spark[p_index[i].part_no].v2);
			m_vertices[index].color = col;
			index++;
			*/

		}

		// Lock the vertex buffer.
		result = deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (FAILED(result))
		{
			return false;
		}

		// Get a pointer to the data in the vertex buffer.
		verticesPtr = (ParticleVertexType*)mappedResource.pData;

		// Copy the data into the vertex buffer.
		memcpy(verticesPtr, (void*)m_vertices, (sizeof(ParticleVertexType) * (m_vertexCount)));

		// Unlock the vertex buffer.
		deviceContext->Unmap(m_vertexBuffer, 0);
	}


	return true;
}