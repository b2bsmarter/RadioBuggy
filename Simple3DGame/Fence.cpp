#include "pch.h"
#include "Fence.h"

using namespace Simple3DGame;
using namespace DX;
using namespace DirectX;


Fence::~Fence(void)
{
	delete[] pills;
	delete[] p_index;
}


Fence::Fence(Microsoft::WRL::ComPtr<ID3D11Device1> pm_d3dDevice, Level* pp_Level, Camera^ pp_Camera)
{
	m_d3dDevice = pm_d3dDevice;

	p_Level = pp_Level;

	p_Camera = pp_Camera;

	//p_particles = pp_particles;

	pills = new pill_t[1000];

	p_index = new part_index[1000];

	bUpdate = true;

	current_particle_que = 0;

	m_particleSize = 2.0f;

	m_maxParticles = 1000;

	current_index = 0;

	last_time = 0.0f;


	total_collected = 0;

	InitializeBuffers();

	cur_pop_sound = 0;

	CreateDDSTextureFromFile((ID3D11Device *)pm_d3dDevice.Get(), L"Assets/Textures/wooden-fence-hi.dds", nullptr, &m_Texture, MAXSIZE_T);

	/*
	for (int i = 0; i<m_maxParticles; i++)
	{
		pills[i].bActive = 0;
	}*/
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
	*/
	/*
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


	auto rotationMatrix = XMMatrixRotationRollPitchYaw(0.0f, 0.0f, 0.0f);

	XMMATRIX scaleMatrix = XMMatrixScaling(1.0f, 1.0f, 1.0f);

	XMMATRIX translationMatrix = XMMatrixTranslation(0.0f, 0.0f, 0.0f);

	XMMATRIX scaleRotationMatrix = XMMatrixMultiply(scaleMatrix, rotationMatrix);

	XMMATRIX modelMatrix = XMMatrixMultiply(scaleRotationMatrix, translationMatrix);

	XMStoreFloat4x4(&m_constantBufferB.model, modelMatrix);

	//XMStoreFloat4x4( &m_constantBuffer.model,modelMatrix);


}




bool Fence::InitializeBuffers()
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
	m_vertices = new VertexPositionTex[m_vertexCount];

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
	memset(m_vertices, 0, (sizeof(VertexPositionTex) * m_vertexCount));

	// Initialize the index array.
	for (i = 0; i<m_indexCount; i++)
	{
		indices[i] = i;
	}

	// Set up the description of the dynamic vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexPositionTex) * m_vertexCount;
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

bool Fence::LoadLevel(int level)
{
	for (int i = 0; i<m_maxParticles; i++)
	{
		pills[i].bActive = 0;
	}
	return LoadLocations(level);
}


bool Fence::LoadLocations(int level)
{
	FILE *filePtr;							// Point to the current position in the file
	BITMAPFILEHEADER bitmapFileHeader;		// Structure which stores information about file
	BITMAPINFOHEADER bitmapInfoHeader;		// Structure which stores information about image
	int imageSize, index;
	unsigned char height;

	HeightMapInfo hminfo;

	char file_folder[30];

	
	/*
	sprintf( file_folder , "Assets\\LevelMaps\\%d\\pillmap.bmp" , level );

	// Open the file
	filePtr = fopen(file_folder,"rb");
	if (filePtr == NULL)
	return 0;

	// Read bitmaps header
	fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1,filePtr);

	// Read the info header
	fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);

	// Get the width and height (width and length) of the image
	hminfo.terrainWidth = bitmapInfoHeader.biWidth;
	hminfo.terrainHeight = bitmapInfoHeader.biHeight;

	// Size of the image in bytes. the 3 represents RBG (byte, byte, byte) for each pixel
	imageSize = hminfo.terrainWidth * hminfo.terrainHeight * 3;
	*/

	/*
	unsigned char* bitmapImage = (unsigned char*)malloc(imageSize);
	hminfo.heightMap = (XMFLOAT3 *)malloc(imageSize*sizeof(XMFLOAT3));
	hminfo.normal = (XMFLOAT3 *)malloc(imageSize*sizeof(XMFLOAT3));
	*/
	/*
	unsigned char* bitmapImage = new unsigned char[imageSize];
	hminfo.heightMap = new XMFLOAT3[imageSize*sizeof(XMFLOAT3)];
	hminfo.normal = new XMFLOAT3[imageSize*sizeof(XMFLOAT3)];


	//ZeroMemory(&bitmapImage, sizeof(imageSize));
	//ZeroMemory(&hminfo.heightMap, imageSize*sizeof(XMFLOAT3));
	//ZeroMemory(&hminfo.normal, imageSize*sizeof(XMFLOAT3));

	// Set the file pointer to the beginning of the image data
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	// Store image data in bitmapImage
	fread(bitmapImage, 1, imageSize, filePtr);

	// Close file
	fclose(filePtr);
	*/



	// We use a greyscale image, so all 3 rgb values are the same, but we only need one for the height
	// So we use this counter to skip the next two components in the image data (we read R, then skip BG)
	int k = 0;
	int j2;
	int i2;

	int x_planes = ((p_Level->map_x_size) / 9);
	int y_planes = ((p_Level->map_y_size) / 9);

	int x_space = p_Level->map_x_size - (x_planes * 8);

	float left_right_walls = 8.0f * x_planes;
	float front_back_walls = 8.0f * y_planes;

	// We divide the height by this number to "water down" the terrains height, otherwise the terrain will
	// appear to be "spikey" and not so smooth.
	current_pill = 0;
	//p_Level->m_hminfo
	total_pills = 0;
	// Read the image data into our heightMap array
	for (int j = 0; j< (y_planes*8); j += 1)
	{
		for (int i = -1; i< 2; i += 2)
		{
			if (current_pill < m_maxParticles - 1)
			{
				total_pills++;
				pills[current_pill].bActive = 1;

				j2 = j;
				i2 = i;

				if (i == -1)
				{
					pills[current_pill].x = -left_right_walls + 1.0f;
					pills[current_pill].y = 0.0f + (p_Level->m_hminfo.heightMap[((j2)*(p_Level->m_hminfo.terrainWidth)) + (i2 / 2)].y*(p_Level->scale_height*2.0f));
					pills[current_pill].y2 = 0.0f + (p_Level->m_hminfo.heightMap[(((j2 + 1))*(p_Level->m_hminfo.terrainWidth)) + (i2 / 2)].y*(p_Level->scale_height*2.0f));
					pills[current_pill].z = -front_back_walls + ((float)j*2.0f)+2.0f;
					pills[current_pill].type = 0;
					pills[current_pill].nx = 1.0f;
					pills[current_pill].ny = 0.0f;
					pills[current_pill].nz = 0.0f;
					pills[current_pill].col = p_Level->m_hminfo.colour[((j2)*(p_Level->m_hminfo.terrainWidth)) + (i2 / 2)];
					pills[current_pill].col2 = p_Level->m_hminfo.colour[((j2+1)*(p_Level->m_hminfo.terrainWidth)) + (i2 / 2)];
				}
				else
				{
					pills[current_pill].x = left_right_walls + 1.0f;
					pills[current_pill].y = 0.0f + (p_Level->m_hminfo.heightMap[((j2)*(p_Level->m_hminfo.terrainWidth)) + (i2 / 2) + (x_planes*8)].y*(p_Level->scale_height*2.0f));
					pills[current_pill].y2 = 0.0f + (p_Level->m_hminfo.heightMap[(((j2 + 1))*(p_Level->m_hminfo.terrainWidth)) + (i2 / 2)+ (x_planes*8)].y*(p_Level->scale_height*2.0f));
					pills[current_pill].z = -front_back_walls + ((float)j*2.0f)+2.0f;
					pills[current_pill].type = 0;
					pills[current_pill].nx = -1.0f;
					pills[current_pill].ny = 0.0f;
					pills[current_pill].nz = 0.0f;
					pills[current_pill].col = p_Level->m_hminfo.colour[((j2)*(p_Level->m_hminfo.terrainWidth)) + (i2 / 2) + (x_planes * 8)];
					pills[current_pill].col2 = p_Level->m_hminfo.colour[((j2+1)*(p_Level->m_hminfo.terrainWidth)) + (i2 / 2) + (x_planes * 8)];
				}
				pills[current_pill].col.w = 0.0f;
				pills[current_pill].col2.w = 0.0f;
				current_pill++;
			}
			k += 3;
		}
	}

	for (int j = 0; j< (x_planes * 8); j += 1)
	{
		for (int i = -1; i< 2; i += 2)
		{
			if (current_pill < m_maxParticles - 1)
			{
				total_pills++;
				pills[current_pill].bActive = 1;

				j2 = j;
				i2 = i;

				if (i == -1)
				{
					pills[current_pill].x = -left_right_walls + ((float)j*2.0f)+2.0f;
					pills[current_pill].y = 0.0f + (p_Level->m_hminfo.heightMap[(j2)].y*(p_Level->scale_height*2.0f));
					pills[current_pill].y2 = 0.0f + (p_Level->m_hminfo.heightMap[(j2)+1].y*(p_Level->scale_height*2.0f));
					pills[current_pill].z = -front_back_walls + 1.0f;
					pills[current_pill].type = 1;
					pills[current_pill].nx = 0.0f;
					pills[current_pill].ny = 0.0f;
					pills[current_pill].nz = 1.0f;
					pills[current_pill].col = p_Level->m_hminfo.colour[(j2)];
					pills[current_pill].col2 = p_Level->m_hminfo.colour[(j2)+1];
				}
				else
				{
					pills[current_pill].x = -left_right_walls + ((float)j*2.0f)+2.0f;
					pills[current_pill].y = 0.0f + (p_Level->m_hminfo.heightMap[(p_Level->m_hminfo.terrainWidth*(y_planes * 8)) + (j2)].y*(p_Level->scale_height*2.0f));
					pills[current_pill].y2 = 0.0f + (p_Level->m_hminfo.heightMap[(p_Level->m_hminfo.terrainWidth*(y_planes*8))+(j2)+1].y*(p_Level->scale_height*2.0f));
					pills[current_pill].z = front_back_walls + 1.0f;
					pills[current_pill].type = 1;
					pills[current_pill].nx = 0.0f;
					pills[current_pill].ny = 0.0f;
					pills[current_pill].nz = -1.0f;
					pills[current_pill].col = p_Level->m_hminfo.colour[(p_Level->m_hminfo.terrainWidth*(y_planes * 8)) + (j2)];
					pills[current_pill].col2 = p_Level->m_hminfo.colour[(p_Level->m_hminfo.terrainWidth*(y_planes * 8)) + (j2)+1];
				}
				pills[current_pill].col.w = 0.0f;
				pills[current_pill].col2.w = 0.0f;

				current_pill++;
			}
			k += 3;
		}
	}

	/*
	for(int j=0; j< hminfo.terrainHeight; j++)
	{
	for(int i=0; i< hminfo.terrainWidth; i++)
	{
	index = ( hminfo.terrainHeight * j) + i;

	float tl = hminfo.heightMap[clampB(i-1,hminfo.terrainWidth)+(clampB((j-1),hminfo.terrainHeight)*hminfo.terrainWidth)].y;
	float t = hminfo.heightMap[clampB(i,hminfo.terrainWidth)+(clampB((j-1),hminfo.terrainHeight)*hminfo.terrainWidth)].y;
	float tr = hminfo.heightMap[clampB(i+1,hminfo.terrainWidth)+(clampB((j-1),hminfo.terrainHeight)*hminfo.terrainWidth)].y;

	float dZ = -((tr + (2.0 * r) + br) - (tl + (2.0 * l) + bl));
	float dX = (tl + (2.0 * t) + tr) - (bl + (2.0 * b) + br);
	float dY = 0.5;// / pStrength;	float r = hminfo.heightMap[clampB(i+1,hminfo.terrainWidth)+(clampB((j),hminfo.terrainHeight)*hminfo.terrainWidth)].y;
	float br = hminfo.heightMap[clampB(i+1,hminfo.terrainWidth)+(clampB((j+1),hminfo.terrainHeight)*hminfo.terrainWidth)].y;
	float b = hminfo.heightMap[clampB(i,hminfo.terrainWidth)+(clampB((j+1),hminfo.terrainHeight)*hminfo.terrainWidth)].y;
	float bl = hminfo.heightMap[clampB(i-1,hminfo.terrainWidth)+(clampB((j+1),hminfo.terrainHeight)*hminfo.terrainWidth)].y;
	float l = hminfo.heightMap[clampB(i-1,hminfo.terrainWidth)+(clampB((j),hminfo.terrainHeight)*hminfo.terrainWidth)].y;


	btVector3 v(dX,dY,dZ);

	v.normalize();

	hminfo.normal[index].x=v.getX();
	hminfo.normal[index].y=v.getY();
	hminfo.normal[index].z=v.getZ();
	}
	}
	*/

	//delete [] bitmapImage;
	//bitmapImage = 0;

	//delete [] hminfo.heightMap;
	//delete [] hminfo.normal;

	return true;
}







void Fence::Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> deviceContext, Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer, ModelViewProjectionConstantBuffer* constantBufferData)
{
	unsigned int stride;
	unsigned int offset;

	// Set vertex buffer stride and offset.
	stride = sizeof(VertexPositionTex);
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





bool Fence::UpdateVertecies(ID3D11DeviceContext* deviceContext)
{
	int index, i;
	int particle_count = 0;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT result;
	VertexPositionTex* verticesPtr;
	btVector3 eye;

	current_index = 0;

	eye = btVector3(p_Camera->EyeX(), p_Camera->EyeY(), p_Camera->EyeZ());

	//CheckCollision();

	//if( bUpdate==false )
	//	return true;

	for (i = 0; i<m_maxParticles; i++)
	{
		if (pills[i].bActive == 1)
		{
			//float val = DX::Min_dist(pills[i].x, pills[i].z, p_Camera->EyeX(), p_Camera->EyeZ(), p_Camera->EyeX() + p_Camera->LookingTanX(), p_Camera->EyeZ() + p_Camera->LookingTanZ());

			if (p_Camera->CheckPoint(pills[i].x, pills[i].y, pills[i].z,1.0f, FAR_DIST+60.0f)==true) /*(val>FIN_DIST && val<FAR_DIST)*/
			{
				//btVector3 part_pos(pills[i].x, pills[i].y - 15.0f, pills[i].z);

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

	memset(m_vertices, 0, (sizeof(ParticleVertexType) * (m_vertexCount)));

	index = 0;
	int total_active = 0;
	float px;
	float py;
	float pz;
	float py2;
	XMFLOAT4 col;
	float height = 2.0f;

	//particle_count=1;
	if (particle_count>0) //(total_active<(m_maxParticles*20)-1)
	{
		for (i = 0; i<particle_count; i++)
		{
			float size = m_particleSize;

			px = pills[p_index[i].part_no].x;
			py = pills[p_index[i].part_no].y;
			pz = pills[p_index[i].part_no].z;
			py2 = pills[p_index[i].part_no].y2;
			XMFLOAT4 col = pills[p_index[i].part_no].col;
			XMFLOAT4 col2 = pills[p_index[i].part_no].col2;
			/*
			int mod_z = 100 - ((((int)(pills[p_index[i].part_no].z)) % 600) / 6);
			int light = (int)light_position;

			float col_int = 0.01f*(mod_z - light);

			if (col_int<0.0f)
				col_int += 1.0f;

			if (col_int>1.0f)
				col_int -= 1.0f;
			col_int = 1.0f;

			float colr = cos(col_int);
			float colg = sin(col_int);
			float colb = tan(col_int);
			*/
			XMFLOAT3 norm = XMFLOAT3(pills[p_index[i].part_no].nx, pills[p_index[i].part_no].ny, pills[p_index[i].part_no].nz);

			//col_int=1.0f-col_int;

			//col = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
			
			size = 0.35f;//*col_int;
			total_active++;

			if (pills[p_index[i].part_no].type == 1)
			{

				// Bottom right.
				m_vertices[index].pos = XMFLOAT3(px + (size*3.0f), py2, pz);
				m_vertices[index].tex = XMFLOAT2(1.0f, 1.0f);
				m_vertices[index].color = col2;
				m_vertices[index].norm = norm;
				index++;


				m_vertices[index].pos = XMFLOAT3(px - (size*3.0f), py + height, pz );
				m_vertices[index].tex = XMFLOAT2(0.0f, 0.0f);
				m_vertices[index].color = col;
				m_vertices[index].norm = norm;
				index++;

				// Bottom left.
				m_vertices[index].pos = XMFLOAT3(px - (size*3.0f), py, pz );
				m_vertices[index].tex = XMFLOAT2(0.0f, 1.0f);
				m_vertices[index].color = col;
				m_vertices[index].norm = norm;
				index++;

				// Top right.
				m_vertices[index].pos = XMFLOAT3(px + (size*3.0f), py2 + height, pz);
				m_vertices[index].tex = XMFLOAT2(1.0f, 0.0f);
				m_vertices[index].color = col2;
				m_vertices[index].norm = norm;
				index++;


				// Top left.
				m_vertices[index].pos = XMFLOAT3(px - (size*3.0f), py + height, pz );
				m_vertices[index].tex = XMFLOAT2(0.0f, 0.0f);
				m_vertices[index].color = col;
				m_vertices[index].norm = norm;
				index++;

				// Bottom right.
				m_vertices[index].pos = XMFLOAT3(px + (size*3.0f) , py2, pz );
				m_vertices[index].tex = XMFLOAT2(1.0f, 1.0f);
				m_vertices[index].color = col2;
				m_vertices[index].norm = norm;
				index++;
			}
			else
			{
				if (px>0.0f)
				{

					// Top left.
					m_vertices[index].pos = XMFLOAT3(px, py + height, pz - (size*3.0f));
					m_vertices[index].tex = XMFLOAT2(0.0f, 0.0f);
					m_vertices[index].color = col;
					m_vertices[index].norm = norm;
					index++;
					// Bottom right.
					m_vertices[index].pos = XMFLOAT3(px, py2, pz + (size*3.0f));
					m_vertices[index].tex = XMFLOAT2(1.0f, 1.0f);
					m_vertices[index].color = col2;
					m_vertices[index].norm = norm;
					index++;
					// Bottom left.
					m_vertices[index].pos = XMFLOAT3(px, py, pz - (size*3.0f));
					m_vertices[index].tex = XMFLOAT2(0.0f, 1.0f);
					m_vertices[index].color = col;
					m_vertices[index].norm = norm;
					index++;
					// Top left.
					m_vertices[index].pos = XMFLOAT3(px, py + height, pz - (size*3.0f));
					m_vertices[index].tex = XMFLOAT2(0.0f, 0.0f);
					m_vertices[index].color = col;
					m_vertices[index].norm = norm;
					index++;
					// Top right.
					m_vertices[index].pos = XMFLOAT3(px, py2 + height, pz + (size*3.0f));
					m_vertices[index].tex = XMFLOAT2(1.0f, 0.0f);
					m_vertices[index].color = col2;
					m_vertices[index].norm = norm;
					index++;
					// Bottom right.
					m_vertices[index].pos = XMFLOAT3(px, py2, pz + (size*3.0f));
					m_vertices[index].tex = XMFLOAT2(1.0f, 1.0f);
					m_vertices[index].color = col2;
					m_vertices[index].norm = norm;
					index++;

				}
				else
				{
					// Top left.
					m_vertices[index].pos = XMFLOAT3(px, py + height, pz - (size*3.0f));
					m_vertices[index].tex = XMFLOAT2(0.0f, 0.0f);
					m_vertices[index].color = col;
					m_vertices[index].norm = norm;
					index++;
					// Bottom left.
					m_vertices[index].pos = XMFLOAT3(px, py, pz - (size*3.0f));
					m_vertices[index].tex = XMFLOAT2(0.0f, 1.0f);
					m_vertices[index].color = col;
					m_vertices[index].norm = norm;
					index++;
					// Bottom right.
					m_vertices[index].pos = XMFLOAT3(px, py2, pz + (size*3.0f));
					m_vertices[index].tex = XMFLOAT2(1.0f, 1.0f);
					m_vertices[index].color = col2;
					m_vertices[index].norm = norm;
					index++;

					// Top left.
					m_vertices[index].pos = XMFLOAT3(px, py + height, pz - (size*3.0f));
					m_vertices[index].tex = XMFLOAT2(0.0f, 0.0f);
					m_vertices[index].color = col;
					m_vertices[index].norm = norm;
					index++;
					// Bottom right.
					m_vertices[index].pos = XMFLOAT3(px, py2, pz + (size*3.0f));
					m_vertices[index].tex = XMFLOAT2(1.0f, 1.0f);
					m_vertices[index].color = col2;
					m_vertices[index].norm = norm;
					index++;
					// Top right.
					m_vertices[index].pos = XMFLOAT3(px, py2 + height, pz + (size*3.0f));
					m_vertices[index].tex = XMFLOAT2(1.0f, 0.0f);
					m_vertices[index].color = col2;
					m_vertices[index].norm = norm;
					index++;

				}
			}


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
		verticesPtr = (VertexPositionTex*)mappedResource.pData;

		// Copy the data into the vertex buffer.
		memcpy(verticesPtr, (void*)m_vertices, (sizeof(VertexPositionTex) * (m_vertexCount)));

		// Unlock the vertex buffer.
		deviceContext->Unmap(m_vertexBuffer, 0);
	}

	return true;
}