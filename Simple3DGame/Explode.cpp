#include "pch.h"
#include "Explode.h"

using namespace Simple3DGame;
using namespace DX;
using namespace DirectX;



int ExplodeSortCB(const VOID* arg1, const VOID* arg2)
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

void Explode::CheckCollision()
{
	int i;
	/*
	btVector3 player_pos(p_Player->m_constantBuffer.model._14,p_Player->m_constantBuffer.model._24,p_Player->m_constantBuffer.model._34);
	btVector3 pill_pos;

	for( i=0;i<m_maxParticles;i++ )
	{
	if(explodes[i].bActive==1)
	{
	total_collected++;

	pill_pos.setX(pills[i].x);
	pill_pos.setY(pills[i].y);
	pill_pos.setZ(pills[i].z);

	pill_pos=pill_pos-player_pos;
	if(pill_pos.length()<18.0f)
	{
	pills[i].bActive=0;
	bUpdate=true;


	//p_particles->ActivateNext(pills[i].x,pills[i].y,pills[i].z);
	p_points->CreateOne(pills[i].x,pills[i].y+10.0f,pills[i].z);
	}
	}
	}
	*/
}

void Explode::CreateOne(float x,float y,float z)
{
	if (current_pill < m_maxParticles - 1)
	{
		
		explodes[current_pill].bActive = 1;
		explodes[current_pill].x = x;
		explodes[current_pill].y = y;
		explodes[current_pill].z = z;
		explodes[current_pill].frame = 0;
	}


	current_pill++;
	if (current_pill > m_maxParticles - 1)
	{
		current_pill = 0;
	}
}


Explode::~Explode(void)
{
	delete[] explodes;
	delete[] p_index;

}


Explode::Explode(Microsoft::WRL::ComPtr<ID3D11Device1> pm_d3dDevice, Level* pp_Level, Player* pp_Player, Camera^ pp_Camera)
{
	m_d3dDevice = pm_d3dDevice;

	p_Level = pp_Level;

	p_Player = pp_Player;

	explodes = new explode_t[300];

	p_index = new part_index[300];

	bUpdate = true;

	current_particle_que = 0;

	m_particleSize = 2.0f;

	m_maxParticles = 300;

	current_index = 0;

	total_collected = 0;


	InitializeBuffers();

	p_Camera = pp_Camera;

	CreateDDSTextureFromFile((ID3D11Device *)pm_d3dDevice.Get(), L"Assets/Textures/explosion.dds", nullptr, &m_Texture, MAXSIZE_T);

	for (int i = 0; i<m_maxParticles; i++)
	{
		explodes[i].bActive = 0;
	}
	/*
	auto pointVSTask = DX::ReadDataAsync("PointVertexShader.cso");
	auto pointPSTask = DX::ReadDataAsync("PointPixelShader.cso");

	auto createpointVSTask = pointVSTask.then([this](Platform::Array<byte>^ fileData) {
	DX::ThrowIfFailed(
	m_d3dDevice->CreateVertexShader(
	fileData->Data,
	fileData->Length,
	nullptr,
	&m_point_vertexShader
	)
	);

	const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
	{ "POSITION", 0,   DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0,    DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR", 0,   DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	DX::ThrowIfFailed(
	m_d3dDevice->CreateInputLayout(
	vertexDesc,
	ARRAYSIZE(vertexDesc),
	fileData->Data,
	fileData->Length,
	&m_point_inputLayout
	)
	);
	});
	*//*

	auto createpointPSTask = pointPSTask.then([this](Platform::Array<byte>^ fileData) {
	DX::ThrowIfFailed(
	m_d3dDevice->CreatePixelShader(
	fileData->Data,
	fileData->Length,
	nullptr,
	&m_point_pixelShader
	)
	);
	});
	*/


	XMMATRIX scaleMatrix = XMMatrixScaling(1.0f, 1.0f, 1.0f);

	auto rotationMatrix = XMMatrixRotationRollPitchYaw(0.0f, 0.0f, 0.0f);
	auto translationMatrix = XMMatrixTranslation(0.0f, 0.0f, 0.0f);

	XMStoreFloat4x4(&m_constantBufferB.model, XMMatrixTranspose(rotationMatrix * translationMatrix));

	//XMStoreFloat4x4( &m_constantBuffer.model,modelMatrix);


}




bool Explode::InitializeBuffers()
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





void Explode::Update(float timeTotal, float timeDelta)
{
	int i;

	total_lights = 0;

	for (i = 0; i < m_maxParticles; i++)
	{
		if (explodes[i].bActive == 1)
		{
			//if (rand() % 2 == 1)
			//{
				explodes[i].frame+= timeDelta*60.0f;
				if (explodes[i].frame > 47)
				{
					explodes[i].bActive = 0;
				}
				else
				{
					if (total_lights < MAX_POINT_LIGHTS - 1)
					{
						light[total_lights].x = explodes[i].x;
						light[total_lights].y = explodes[i].y;
						light[total_lights].z = explodes[i].z;
						light[total_lights].strength = explodes[i].frame/47.0f;
						total_lights++;
					}
				}
			//}
		}
	}

}




void Explode::UpdateProjectionMatrix(DirectX::XMMATRIX *projectionMatrix)
{
	XMStoreFloat4x4(&m_constantBufferB.projection, *projectionMatrix);
}



void Explode::Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> deviceContext, Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer, ModelViewProjectionConstantBuffer* constantBufferData)
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


	deviceContext->UpdateSubresource(constantBuffer.Get(), 0, NULL, &m_constantBufferB, 0, 0);
	*/

	/*
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
	*/

	deviceContext->PSSetShaderResources(0, 1, &m_Texture);

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	constantBufferData->model = m_constantBufferB.model;
	deviceContext->UpdateSubresource(constantBuffer.Get(), 0, NULL, constantBufferData, 0, 0);
	// 
	deviceContext->DrawIndexed(
		m_indexCount,
		0,
		0
		);

}





bool Explode::UpdateVertecies(ID3D11DeviceContext* deviceContext)
{
	int index, i;
	int particle_count = 0;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT result;
	ParticleVertexType* verticesPtr;
	btVector3 eye;

	current_index = 0;

	if (false)// (p_Camera == NULL)
	{
		eye = btVector3(0.0f, 0.0f, 0.0f);
	}
	else
	{
		eye = btVector3(p_Camera->EyeX(), p_Camera->EyeY(), p_Camera->EyeZ());
	}

	//CheckCollision();

	for (i = 0; i<m_maxParticles; i++)
	{
		if (explodes[i].bActive == 1)
		{
			float val = DX::Min_dist(explodes[i].x, explodes[i].z, p_Camera->EyeX(), p_Camera->EyeZ(), p_Camera->EyeX() + p_Camera->LookingTanX(), p_Camera->EyeZ() + p_Camera->LookingTanZ());

			if (true)//(p_Camera->CheckPoint(explodes[i].x, explodes[i].y, explodes[i].z,1.0f)) //(val>-200.0f && val<650.0f)
			{
				//btVector3 part_pos(explodes[i].x, explodes[i].y, explodes[i].z);

				//char str[40];
				//sprintf_s(str , "%f %f \n" , p_Camera->eyez , pills[i].z  );

				//OutputDebugStringA(str);

				p_index[current_index].part_no = i;
				//p_index[current_index].dist = part_pos.distance(eye);
				current_index++;
				particle_count++;
			}
		}
	}


	if (particle_count>0)
	{
		//qsort( p_index , particle_count , sizeof( part_index ) , PillsSortCB );
	}

	//particle_count=1;
	memset(m_vertices, 0, (sizeof(ParticleVertexType) * (m_vertexCount)));

	// Now build the vertex array from the particle list array.  Each particle is a quad made out of two triangles.
	index = 0;
	int total_active = 0;
	float px;
	float py;
	float pz;
	XMFLOAT4 col;
	float size = 1.8f;
	float xsize = p_Camera->LookingTanX()*1.8f;
	float zsize = p_Camera->LookingTanZ()*1.8f;

	//particle_count=1;
	if (particle_count>0) //(total_active<(m_maxParticles*20)-1)
	{
		//bUpdate=false;
		for (i = 0; i<particle_count; i++)
		{
			float t = (1.0f / 6.0f)*float(int(explodes[p_index[i].part_no].frame) / 8);
			float l = (1.0f / 8.0f)*float(int(explodes[p_index[i].part_no].frame) % 8);
			float r = l + (1.0f / 8.0f);//explodes[p_index[i].part_no].frame%12;
			float b = t + (1.0f / 6.0f);

			px = explodes[p_index[i].part_no].x;
			py = explodes[p_index[i].part_no].y;
			pz = explodes[p_index[i].part_no].z;

			col = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f);

			//size = 14.0f;//  + (40.0f * explodes[p_index[i].part_no].size);
			total_active++;




			// Top left.
			m_vertices[index].pos = XMFLOAT3(px - (xsize), py + size, pz - (zsize));
			m_vertices[index].tex = XMFLOAT2(l, t);
			m_vertices[index].color = col;
			index++;
			// Bottom right.
			m_vertices[index].pos = XMFLOAT3(px + (xsize), py - size, pz + (zsize));
			m_vertices[index].tex = XMFLOAT2(r, b);
			m_vertices[index].color = col;
			index++;
			// Bottom left.
			m_vertices[index].pos = XMFLOAT3(px - (xsize), py - size, pz - (zsize));
			m_vertices[index].tex = XMFLOAT2(l, b);
			m_vertices[index].color = col;
			index++;
			// Top left.
			m_vertices[index].pos = XMFLOAT3(px - (xsize), py + size, pz - (zsize));
			m_vertices[index].tex = XMFLOAT2(l, t);
			m_vertices[index].color = col;
			index++;
			// Top right.
			m_vertices[index].pos = XMFLOAT3(px + (xsize), py + size, pz + (zsize));
			m_vertices[index].tex = XMFLOAT2(r, t);
			m_vertices[index].color = col;
			index++;
			// Bottom right.
			m_vertices[index].pos = XMFLOAT3(px + (xsize), py - size, pz + (zsize));
			m_vertices[index].tex = XMFLOAT2(r, b);
			m_vertices[index].color = col;
			index++;






			/*
			// Top left.
			m_vertices[index].pos = XMFLOAT3(px - (size), py+ size,  pz );
			m_vertices[index].tex = XMFLOAT2(l, t);
			m_vertices[index].color = col;
			index++;
			// Bottom right.
			m_vertices[index].pos = XMFLOAT3( px+ (size),  py- size, pz);
			m_vertices[index].tex = XMFLOAT2(r, b);
			m_vertices[index].color = col;
			index++;
			// Bottom left.
			m_vertices[index].pos = XMFLOAT3(px- (size), py - size , pz);
			m_vertices[index].tex = XMFLOAT2(l, b);
			m_vertices[index].color = col;
			index++;
			// Top left.
			m_vertices[index].pos = XMFLOAT3(px- (size),py + size,  pz);
			m_vertices[index].tex = XMFLOAT2(l, t);
			m_vertices[index].color = col;
			index++;
			// Top right.
			m_vertices[index].pos = XMFLOAT3(px+ (size), py + size,  pz);
			m_vertices[index].tex = XMFLOAT2(r, t);
			m_vertices[index].color = col;
			index++;
			// Bottom right.
			m_vertices[index].pos = XMFLOAT3( px+ (size), py - size, pz);
			m_vertices[index].tex = XMFLOAT2(r, b);
			m_vertices[index].color = col;
			index++;

			*/
		}

		m_vertexCount = index;
		m_indexCount = index;
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