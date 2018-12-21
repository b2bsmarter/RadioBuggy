#include "pch.h"
#include "Points.h"

using namespace Simple3DGame;
using namespace DX;
using namespace DirectX;

int PointsSortCB(const VOID* arg1, const VOID* arg2)
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


void Points::CreateOne(float x, float y, float z, int num)
{
	points[current_point].bActive = 1;
	points[current_point].life = 10.0f;
	points[current_point].x = x;
	points[current_point].y = y;
	points[current_point].z = z;

	points[current_point].v1 = 0.125f*(float)num;
	points[current_point].v2 = points[current_point].v1 + 0.125f;

	switch (num)
	{
	case 0:p_Player->points += 10; break;
	case 1:p_Player->points += 50; break;
	case 2:p_Player->points += 100; break;
	case 3:p_Player->points += 200; break;
	case 4:p_Player->points += 350; break;
	case 5:p_Player->points += 500; break;
	case 6:p_Player->points += 1000; break;
	case 7:p_Player->points += 2000; break;
	}

	current_point++;
	if (current_point>m_maxParticles - 2)
		current_point = 0;
}


Points::Points(Microsoft::WRL::ComPtr<ID3D11Device1> pm_d3dDevice, Level* pp_Level, Player* pp_Player, Camera^ pp_Camera)
{
	m_d3dDevice = pm_d3dDevice;

	p_Level = pp_Level;

	p_Player = pp_Player;

	points = new point_t[50];

	p_index = new part_index[50];

	bUpdate = true;

	current_particle_que = 0;

	m_particleSize = 2.0f;

	m_maxParticles = 50;

	current_index = 0;

	current_point = 0;

	total_collected = 0;

	InitializeBuffers();

	p_Camera = pp_Camera;

	CreateDDSTextureFromFile((ID3D11Device *)pm_d3dDevice.Get(), L"Assets/Textures/points.dds", nullptr, &m_Texture, MAXSIZE_T);

	for (int i = 0; i<m_maxParticles; i++)
	{
		points[i].bActive = 0;
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




bool Points::InitializeBuffers()
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
	result = m_d3dDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
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
	result = m_d3dDevice->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Release the index array since it is no longer needed.
	delete[] indices;
	indices = 0;

	return true;
}


Points::~Points(void)
{


}


void Points::Update(float timeTotal, float timeDelta)
{
	int i;
	//timeDelta *= 1.1f;

	//angle=angle+0.01f;

	for (i = 0; i<m_maxParticles; i++)
	{
		if (points[i].bActive == 1)
		{
			points[i].life -= 17.0f*timeDelta;
			points[i].y += 1.0f*timeDelta;
			if (points[i].life<0.0f)
			{
				points[i].bActive = 0;
			}
		}
	}

	/*
	auto rotationMatrix = XMMatrixRotationRollPitchYaw(0.0f,0.0f,0.0f);

	XMMATRIX scaleMatrix = XMMatrixScaling(1.0f , 1.0f, 1.0f);

	XMMATRIX translationMatrix = XMMatrixTranslation(0.0f,0.0f,0.0f);

	XMMATRIX scaleRotationMatrix = XMMatrixMultiply(scaleMatrix, rotationMatrix);

	XMMATRIX modelMatrix = XMMatrixMultiply(scaleRotationMatrix, translationMatrix);

	XMStoreFloat4x4( &m_constantBufferB.model,modelMatrix);
	*/
}




void Points::UpdateProjectionMatrix(DirectX::XMMATRIX *projectionMatrix)
{
	XMStoreFloat4x4(&m_constantBufferB.projection, *projectionMatrix);
}


void Points::Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> deviceContext, Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer, ModelViewProjectionConstantBuffer* constantBufferData)
{
	unsigned int stride;
	unsigned int offset;

	// Set vertex buffer stride and offset.
	stride = sizeof(ParticleVertexType);
	offset = 0;

	UpdateVertecies(deviceContext.Get());

	/*
	deviceContext->IASetInputLayout(m_point_inputLayout.Get());

	deviceContext->VSSetShader(
	m_point_vertexShader.Get(),
	nullptr,
	0
	);

	deviceContext->VSSetConstantBuffers(
	0,
	1,
	constantBuffer.GetAddressOf()
	);

	deviceContext->PSSetShader(
	m_point_pixelShader.Get(),
	nullptr,
	0
	);
	*/

	constantBufferData->model = m_constantBufferB.model;
	deviceContext->UpdateSubresource(constantBuffer.Get(), 0, NULL, constantBufferData, 0, 0);

	deviceContext->PSSetShaderResources(0, 1, &m_Texture);

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	deviceContext->DrawIndexed(
		m_indexCount,
		0,
		0
		);
}


bool Points::UpdateVertecies(ID3D11DeviceContext* deviceContext)
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
		if (points[i].bActive == 1)
		{
			btVector3 part_pos(points[i].x, points[i].y, points[i].z);

			p_index[current_index].part_no = i;
			p_index[current_index].dist = part_pos.distance(eye);
			current_index++;
			particle_count++;
		}
	}

	if (particle_count>0)
	{
		qsort(p_index, particle_count, sizeof(part_index), PointsSortCB);
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
	float size = 0.1f;
	float sizeB = 0.3f;

	float xsize = p_Camera->LookingTanX() *1.0f;
	float zsize = p_Camera->LookingTanZ()*1.0f;

	//particle_count=1;
	if (true) //(total_active<(m_maxParticles*20)-1)
	{
		//bUpdate=false;
		for (i = 0; i<particle_count; i++)
		{
			//=m_particleSize;

			px = points[p_index[i].part_no].x;
			py = points[p_index[i].part_no].y;
			pz = points[p_index[i].part_no].z;

			if (points[p_index[i].part_no].life<10.0f)
			{
				col = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, points[p_index[i].part_no].life*0.1f);
			}
			else
			{
				col = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			}


			total_active++;

			// Top left.
			m_vertices[index].pos = DirectX::XMFLOAT3(px - (xsize), py + sizeB, pz - (zsize));
			m_vertices[index].tex = DirectX::XMFLOAT2(0.0f, points[p_index[i].part_no].v1);
			m_vertices[index].color = col;
			index++;
			// Bottom right.
			m_vertices[index].pos = DirectX::XMFLOAT3(px + (xsize), py - sizeB, pz + (zsize));
			m_vertices[index].tex = DirectX::XMFLOAT2(1.0f, points[p_index[i].part_no].v2);
			m_vertices[index].color = col;
			index++;
			// Bottom left.
			m_vertices[index].pos = DirectX::XMFLOAT3(px - (xsize), py - sizeB, pz - (zsize));
			m_vertices[index].tex = DirectX::XMFLOAT2(0.0f, points[p_index[i].part_no].v2);
			m_vertices[index].color = col;
			index++;
			// Top left.
			m_vertices[index].pos = DirectX::XMFLOAT3(px - (xsize), py + sizeB, pz - (zsize));
			m_vertices[index].tex = DirectX::XMFLOAT2(0.0f, points[p_index[i].part_no].v1);
			m_vertices[index].color = col;
			index++;
			// Top right.
			m_vertices[index].pos = DirectX::XMFLOAT3(px + (xsize), py + sizeB, pz + (zsize));
			m_vertices[index].tex = DirectX::XMFLOAT2(1.0f, points[p_index[i].part_no].v1);
			m_vertices[index].color = col;
			index++;
			// Bottom right.
			m_vertices[index].pos = DirectX::XMFLOAT3(px + (xsize), py - sizeB, pz + (zsize));
			m_vertices[index].tex = DirectX::XMFLOAT2(1.0f, points[p_index[i].part_no].v2);
			m_vertices[index].color = col;
			index++;

			/*
			// Top left.
			m_vertices[index].pos = XMFLOAT3(px - (size), py+ sizeB,  pz );
			m_vertices[index].tex = XMFLOAT2(0.0f, points[p_index[i].part_no].v1);
			m_vertices[index].color = col;
			index++;
			// Bottom right.
			m_vertices[index].pos = XMFLOAT3( px+ (size),  py- sizeB, pz);
			m_vertices[index].tex = XMFLOAT2(1.0f, points[p_index[i].part_no].v2);
			m_vertices[index].color = col;
			index++;
			// Bottom left.
			m_vertices[index].pos = XMFLOAT3( px- (size), py - sizeB , pz);
			m_vertices[index].tex = XMFLOAT2(0.0f, points[p_index[i].part_no].v2);
			m_vertices[index].color = col;
			index++;
			// Top left.
			m_vertices[index].pos = XMFLOAT3(px- (size),py + sizeB,  pz);
			m_vertices[index].tex = XMFLOAT2(0.0f, points[p_index[i].part_no].v1);
			m_vertices[index].color = col;
			index++;
			// Top right.
			m_vertices[index].pos = XMFLOAT3(px+ (size), py + sizeB,  pz);
			m_vertices[index].tex = XMFLOAT2(1.0f, points[p_index[i].part_no].v1);
			m_vertices[index].color = col;
			index++;
			// Bottom right.
			m_vertices[index].pos = XMFLOAT3( px+ (size), py - sizeB, pz);
			m_vertices[index].tex = XMFLOAT2(1.0f, points[p_index[i].part_no].v2);
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