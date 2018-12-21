#include "pch.h"
#include "Terrain.h"

//#include "DirectXHelper.h"

using namespace Simple3DGame;
using namespace DirectX;
using namespace DX;



void SimpleMotion::getWorldTransform(btTransform &worldTransform) const
{
	worldTransform = m_initialTransform;
}

void SimpleMotion::setWorldTransform(const btTransform &worldTransform)
{

}

SimpleMotion::SimpleMotion()
{

}

SimpleMotion::~SimpleMotion()
{


}

Terrain::Terrain(std::shared_ptr<DX::DeviceResources> pm_deviceResources)
{
	m_deviceResources = pm_deviceResources;
}

void Terrain::InitTerrain(HeightMapInfo* phm_info, int xbp, int ybp)
{
	xblockpos = xbp;
	yblockpos = ybp;

	start_id = 9999;
	end_id = 0;
	bLoaded = false;

	hm_info = phm_info;

	m_box = new SimpleMotion();

	m_initialTransform = btTransform(btQuaternion(0.0f, 0.0f, 0.0f), btVector3(0.0f, 0.0f, 0.0f));
}

void Terrain::getWorldTransform(btTransform &worldTransform) const
{
	worldTransform = m_initialTransform;
}

void Terrain::setWorldTransform(const btTransform &worldTransform)
{
	auto rotation = worldTransform.getRotation();

	auto transform = ref new Transform(worldTransform.getOrigin().getX(),
		worldTransform.getOrigin().getY(),
		worldTransform.getOrigin().getZ(),
		rotation.getX(), rotation.getY(), rotation.getZ(), rotation.getW());

	SetTransform(transform);
}

void Terrain::SetTransform(const Transform^ transform)
{
	XMVECTOR data;
	XMVECTORF32 floatingVector = { transform->quatX, transform->quatY, transform->quatZ, transform->quatW };
	data = floatingVector;

	//	XMMATRIX scaleMatrix = XMMatrixScaling(1.0f , 1.0f, 1.0f);

	auto rotationMatrix = XMMatrixRotationQuaternion(data);
	auto translationMatrix = XMMatrixTranslation(transform->locX, transform->locY, transform->locZ);

	//XMMATRIX scaleRotationMatrix = XMMatrixMultiply(scaleMatrix, rotationMatrix);
	//XMMATRIX modelMatrix = XMMatrixMultiplyTranspose(scaleRotationMatrix, translationMatrix);
	//m_constantBuffer.model = 
	XMStoreFloat4x4(&m_constantBufferData.model, XMMatrixTranspose(rotationMatrix * translationMatrix));
	//XMStoreFloat4x4( &m_constantBuffer.model,modelMatrix);
}


void Terrain::Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> deviceContext, Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer, ModelViewProjectionConstantBuffer* constantBufferData, int complexity )
{
	UINT stride = sizeof(VertexPositionTex);
	UINT offset = 0;

	//XMMATRIX m_modelMatrix;
	///XMVECTOR data;
	//XMMATRIX m_InvMatrix = XMMatrixScaling(&data, XMLoadFloat4x4(&m_constantBuffer.model));

	//m_constantBuffer.model.m[3][0]=0.0f;
	//m_constantBuffer.model.m[3][1]=0.0f;
	//m_constantBuffer.model.m[3][2]=0.0f;
	//m_constantBuffer.model.m[3][3]=0.0f;
	//XMMatrixReflect(FMVECTOR3
	//XMMatrixTransformation

	//XMStoreFloat4x4(&m_constantBuffer.model, m_InvMatrix);


	deviceContext->IASetVertexBuffers(0, 1, GetVertexBuffer().GetAddressOf(), &stride, &offset);
	if (complexity == 1)
	{
		deviceContext->IASetIndexBuffer(GetIndexBufferSimple().Get(), DXGI_FORMAT_R16_UINT, 0);
	}
	else
	{
		deviceContext->IASetIndexBuffer(GetIndexBuffer().Get(), DXGI_FORMAT_R16_UINT, 0);
	}

	constantBufferData->model = m_constantBufferData.model;
	deviceContext->UpdateSubresource(constantBuffer.Get(), 0, NULL, constantBufferData, 0, 0);


	//m_indicesCount=300;
	deviceContext->DrawIndexed(m_indicesCount, 0, 0);
}

void Terrain::MakePhysicsConvexTriangleTerrain(Physics* phys)
{
	//item->Initialize(m_d3dDevice, filename , texture_filename,scale,x,y,z,yaw,pitch,roll);
	int ob_id;
	int triangles = 0;

	ObjInfo info = { "Assets/Models/board2.obj"
		, L"Assets/barrelColor.dds"
		//		, L"Assets/foot_ball.dds"
		//, L"Assets/sphere.dds"
		, 1.0f		// Scale
		, 10.0f		// X,y,z
		, 50.0f
		, 0.0f
		, 0.0f		// mass
		, 0.0f		// yaw,pitch,roll
		, 0.0f
		, 0.0f
		, 0.5f		// restitution
		, 0.5f
		,0,false
		,(COL_CAR | COL_BOXES)
		,(COL_WORLDSTUFF) };
	//info.mask = (COL_BOXES);
	//info.group = (COL_WORLDSTUFF | COL_CAR | COL_TREES | COL_WALLS);
	//static_ob[1][i] = new WorldObject();
	//static_ob[1][i]->MakePhysicsBoxFromMesh(m_d3dDevice, p_physics, &info);
	//static_ob[1][i]->m_rigidbody->setActivationState(1);
	//total_static_physical++;






	btVector3 A;
	btVector3 B;
	btVector3 C;
	btTriangleMesh* data = new btTriangleMesh();

	for (int i = 0; i<no_phy_verticies; i += 3)
	{
		if (false) //(m_phy_vertices[i].y == 0.0f && m_phy_vertices[i + 1].y == 0.0f && m_phy_vertices[i + 2].y == 0.0f)
		{

		}
		else
		{
			A = btVector3(m_phy_vertices[i].x, m_phy_vertices[i].y, m_phy_vertices[i].z);
			B = btVector3(m_phy_vertices[i + 1].x, m_phy_vertices[i + 1].y, m_phy_vertices[i + 1].z);
			C = btVector3(m_phy_vertices[i + 2].x, m_phy_vertices[i + 2].y, m_phy_vertices[i + 2].z);
			data->addTriangle(A, B, C, false);
		}
		triangles++;
	}

	m_box->setWorldTransform(m_initialTransform);
	m_box->m_initialTransform = m_initialTransform;

	//btDefaultMotionState* lMotionState = new btDefaultMotionState(tr);

	if(true) //triangles>0)
	{
	//btBvhTriangleMeshShape* fallShape=new btBvhTriangleMeshShape(data,true,true);
	auto fallShape = new btBvhTriangleMeshShape(data, true);

	//auto fallShape = new btCylinderShape(btCylinderShape(btVector3(1.2,1.2,6.0)));
	//auto fallShape = new btSphereShape(btSphereShape(1.5f));
	//auto fallShape = new btTriangleMesh (btSphereShape(1.5f));
	//btMotionState* fallMotionState = (btMotionState*)&m_box;//(cube, btTransform(btQuaternion(0,0,0,1),btVector3(x,y,z)));
	ObjInfo dum_objInfo;	
	dum_objInfo.group = (COL_CAR|COL_BOXES);
	dum_objInfo.mask = (COL_WORLDSTUFF);
	//btScalar mass = 0.2;
	btVector3 fallInertia(0, 0, 0);
	fallShape->calculateLocalInertia(0.0f, fallInertia);
	m_rigidbody = phys->AddPhysicalObject(fallShape, (btMotionState *)m_box, 0.0f, fallInertia, 0.3f, 0.4f, &ob_id,&dum_objInfo);
	m_rigidbody->setFriction(0.8f);

	if (ob_id<start_id)
		ob_id = start_id;

	if (ob_id>end_id)
		ob_id = end_id;


	//delete fallShape;
	}
	else
	{
		m_rigidbody = nullptr;
	}

	//delete data;

	//btHeightfieldTerrainShape* terrain;

}



void Terrain::MakePhysicsConvexTerrain(Physics* phys)
{
	//item->Initialize(m_d3dDevice, filename , texture_filename,scale,x,y,z,yaw,pitch,roll);
	int ob_id;

	btScalar maxheight = 200.0f;

	btHeightfieldTerrainShape* heightFieldShape = new btHeightfieldTerrainShape(xcoords, ycoords, (void *)&height_fields, 20.0f, 1, true, true);

	btCollisionShape* lColShape = heightFieldShape;

	btTransform tr;
	tr.setIdentity();
	tr.setOrigin(btVector3(0.0f, 0.0f, 0.0f));

	btScalar lMass(0.0f);
	btVector3 lLocalInertia(0, 0, 0);

	//heightFieldShape->

	//heightFieldShape->setUseDiamondSubdivision(true);

	auto fallShape = heightFieldShape;

	btVector3 sc_cec = btVector3(m_scale, 1.0f, m_scale);

	heightFieldShape->setLocalScaling(sc_cec);


	btDefaultMotionState* lMotionState = new btDefaultMotionState(tr);

	//auto fallShape = new btCylinderShape(btCylinderShape(btVector3(1.2,1.2,6.0)));
	//auto fallShape = new btSphereShape(btSphereShape(1.5f));
	//auto fallShape = new btTriangleMesh (btSphereShape(1.5f));
	btMotionState* fallMotionState = (btMotionState*)this;//(cube, btTransform(btQuaternion(0,0,0,1),btVector3(x,y,z)));
	ObjInfo dum_objInfo;				  //btScalar mass = 0.2;
	btVector3 fallInertia(0, 0, 0);
	fallShape->calculateLocalInertia(0.0f, fallInertia);
	btRigidBody* res = phys->AddPhysicalObject(lColShape, lMotionState, 0.0f, fallInertia, 0.0f, 0.0f, &ob_id,&dum_objInfo);

	if (ob_id<start_id)
		ob_id = start_id;

	if (ob_id>end_id)
		ob_id = end_id;


	res->setFriction(1.0f);
	res->setActivationState(WANTS_DEACTIVATION);
	/*
	btVector3 A;
	btVector3 B;
	btVector3 C;
	btTriangleMesh* data = new btTriangleMesh();

	for(int i=0;i<item->no_phy_verticies;i+=3)
	{
	A=btVector3(item->m_phy_vertices[i].x,item->m_phy_vertices[i].y,item->m_phy_vertices[i].z);
	B=btVector3(item->m_phy_vertices[i+1].x,item->m_phy_vertices[i+1].y,item->m_phy_vertices[i+1].z);
	C=btVector3(item->m_phy_vertices[i+2].x,item->m_phy_vertices[i+2].y,item->m_phy_vertices[i+2].z);
	data->addTriangle(A,B,C,false);
	}

	//btBvhTriangleMeshShape* fallShape=new btBvhTriangleMeshShape(data,true,true);
	auto fallShape = new btConvexTriangleMeshShape(data,true);

	//auto fallShape = new btCylinderShape(btCylinderShape(btVector3(1.2,1.2,6.0)));
	//auto fallShape = new btSphereShape(btSphereShape(1.5f));
	//auto fallShape = new btTriangleMesh (btSphereShape(1.5f));
	//btMotionState* fallMotionState = (btMotionState*)item;//(cube, btTransform(btQuaternion(0,0,0,1),btVector3(x,y,z)));
	//btScalar mass = 0.2;
	btVector3 fallInertia(0,0,0);
	fallShape->calculateLocalInertia(0.0f,fallInertia);
	phys->AddPhysicalObject(fallShape, fallMotionState, 0.0f, fallInertia,0.0f,0.0f);

	btHeightfieldTerrainShape* terrain;
	*/



}


/*
void Terrain::LoadTexture(Microsoft::WRL::ComPtr<ID3D11Device1> pm_d3dDevice, wchar_t* texture_filename)
{
	CreateDDSTextureFromFile((ID3D11Device *)pm_d3dDevice.Get(), texture_filename, nullptr, &m_Texture, MAXSIZE_T);

}
*/


void Terrain::UpdateProjectionMatrix(XMMATRIX *projectionMatrix)
{
	//XMStoreFloat4x4(&m_constantBufferData.projection, *projectionMatrix);
}

void Terrain::Update(XMMATRIX *viewMatrix, float timeTotal)
{
	XMStoreFloat4x4(&m_constantBufferData.view, *viewMatrix);
}


void Terrain::SetPosition(float x, float y, float z)
{
	XMMATRIX m_modelMatrix;

	auto rotationMatrix = XMMatrixRotationRollPitchYaw(0.0f, 0.0f, 0.0f);
	auto translationMatrix = XMMatrixTranslation(x, y, z);
	auto scalingMatrix = XMMatrixScaling(1.0f, 1.0f, 1.0f);
	m_modelMatrix = XMMatrixTranspose((translationMatrix *  rotationMatrix)*scalingMatrix);

	XMStoreFloat4x4(&m_constantBufferData.model, m_modelMatrix);

	m_initialTransform = btTransform(btQuaternion(0.0f, 0.0f, 0.0f), btVector3(x, y, z));

}



bool Terrain::Load(int xnum, int ynum, float scale, int xplane)
{
	int i, j;

	//scale = scale*1.1f;

	m_scale = scale;


	if (bLoaded == true)
	{
		//ClearMemory();
		start_id = 9999;
		end_id = 0;
	}

	bLoaded = true;

	xcoords = xnum;
	ycoords = ynum;

	XMMATRIX m_modelMatrix;
	XMVECTOR data;
	XMVECTORF32 floatingVector = { 0.0f, 0.0f, 0.0f, 1.0f };
	data = floatingVector;

	no_phy_verticies = ((xnum)*(ynum)) * 8;
	height_fields = new float[xnum*ynum];

	//no_phy_verticies = ((xnum/2)*(ynum/2))*8;

	m_phy_vertices = new VertexType[no_phy_verticies];
	/*
	auto rotationMatrix = XMMatrixRotationRollPitchYaw(0.0f,0.0f,0.0f);
	auto translationMatrix = XMMatrixTranslation(0.0f, 0.0f, 0.0f);
	auto scalingMatrix = XMMatrixScaling(1.0f, 1.0f, 1.0f);
	m_modelMatrix = XMMatrixTranspose((translationMatrix *  rotationMatrix)*scalingMatrix);
	XMStoreFloat4x4(&m_constantBuffer.model, m_modelMatrix);
	*/

	m_initialTransform = btTransform(btQuaternion(0.0f, 0.0f, 0.0f), btVector3(0.0f, 0.0f, 0.0f));

	height_map = new float*[ynum];
	normals = new XMFLOAT3*[ynum];
	cols = new XMFLOAT4*[ynum];

	for (i = 0; i < ynum; ++i) {
		height_map[i] = new float[xnum];
		normals[i] = new XMFLOAT3[xnum];
		cols[i] = new XMFLOAT4[xnum];
	}

	int height_total_size = hm_info->terrainHeight * hm_info->terrainWidth;

	//xblockpos++;
	for (i = 0; i<ynum; i++)
	{
		for (j = 0; j<xnum; j++)
		{
			//char str[40];
			//sprintf_s(str , "%d  %d \n" , i+yblockpos , j+yblockpos );

			//OutputDebugStringA(str);

			//float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			if (true)///((i<xnum && j<ynum)) //&& (i!=0 && j!=0) )
			{
				if (true) //(j + xblockpos == hm_info->terrainWidth)
				{
					height_map[j][i] = hm_info->heightMap[((j) + xblockpos +  (((i) + yblockpos) * hm_info->terrainWidth))].y;
					normals[j][i] = hm_info->normal[((j) + xblockpos + (((i) + yblockpos)*hm_info->terrainWidth))];
					cols[j][i] = hm_info->colour[((j)+xblockpos + (((i)+yblockpos)*hm_info->terrainWidth))];
					//height_map[y][x] = hm_info->heightMap[j + xblockpos - 1 + ((i + yblockpos) * (hm_info->terrainWidth+2))].y;
					//normals[y][x] = hm_info->normal[j + xblockpos - 1 + ((i + yblockpos)*(hm_info->terrainWidth + 2))];
				}
				else
				{
					//height_map[y][x] = hm_info->heightMap[j + xblockpos + ((i + yblockpos) * hm_info->terrainWidth)].y;
					//normals[y][x] = hm_info->normal[j + xblockpos + ((i + yblockpos)*hm_info->terrainWidth)];

					//height_map[y][x] = hm_info->heightMap[j + xblockpos + ((i + yblockpos) * (hm_info->terrainWidth + 2))].y;
					//normals[y][x] = hm_info->normal[j + xblockpos + ((i + yblockpos)*(hm_info->terrainWidth + 2))];
				}
			}
			else
			{
				height_map[i][j] = 10.0f;// hm_info->heightMap[i+xblockpos+((j+yblockpos)*hm_info->terrainWidth)].y; 
			}
			//height_map[i][j]=hm_info->heightMap[i+xblockpos+((j+yblockpos)*hm_info->terrainWidth)].y; 

			//(sin( ((float)i*3.0f/(float)xnum) )+(cos( ( (float)i*3.0f/(float)xnum ) )))*4.0f;
			height_fields[j + (i*xnum)] = height_map[i][j];
			/*
			if( i<2 )
			{
			height_map[i][j]=30.0f;
			}
			else
			{
			if( i>xnum-2 )
			{
			height_map[i][j]=30.0f;
			}
			else
			{

			}
			}
			*/

			//height_map[i][j]=0.0f;
		}
	}
	//scale=scale-(scale-((scale/4.0f)*(float)xnum));

	//scale=scale+(scale-((scale/4.0f)*(float)xnum));
	// create verticies

	int end_dist;

	if (false) // size 16
	{
		ynum = ynum / 2;
		xnum = xnum / 2;
		for (j = 0; j<ynum + 2; j++)
		{
			for (i = 0; i<xnum + 2; i++)
			{
				if (false)// j>ynum || i>xnum )
				{
					AddVertexTexNorm(
						0.0f, 0.0f, 0.0f,
						0.0f, 0.0f,
						0.0f, 0.0f, 0.0f);
				}
				else
				{
					if (ynum == 10 && xplane == 2)
					{
						AddVertexTexNormCol(
							(float(i * 2)*scale) - ((((float)(xnum * 2) - 2)*0.5f)*scale) + 100.0f, height_map[i * 2][j * 2], (float(j * 2)*scale) - ((((float)(xnum * 2) - 2)*0.5f)*scale),
							((1.0f / float((xnum * 2) - 1))*float(i * 2)) - 1.0f, (1.0f / float((ynum * 2) - 1))*float(j * 2),
							normals[i * 2][j * 2].x, normals[i * 2][j * 2].y, normals[i * 2][j * 2].z, cols[i][j].x, cols[i][j].y, cols[i][j].z, cols[i][j].w);
					}
					else
					{
						if (ynum == 0 && xplane == 0)
						{
							AddVertexTexNormCol(
								(float(i * 2)*scale) - ((((float)(xnum * 2) - 2)*0.5f)*scale) - 100.0f, height_map[i * 2][j * 2], (float(j * 2)*scale) - ((((float)(xnum * 2) - 2)*0.5f)*scale),
								((1.0f / float((xnum * 2) - 1))*float(i * 2)) + 1.0f, (1.0f / float((ynum * 2) - 1))*float(j * 2),
								normals[i * 2][j * 2].x, normals[i * 2][j * 2].y, normals[i * 2][j * 2].z, cols[i][j].x, cols[i][j].y, cols[i][j].z, cols[i][j].w);
						}
						else
						{
							AddVertexTexNormCol(
								(float(i * 2)*scale) - ((((float)(xnum * 2) - 2)*0.5f)*scale), height_map[i * 2][j * 2], (float(j * 2)*scale) - ((((float)(xnum * 2) - 2)*0.5f)*scale),
								(1.0f / float((xnum * 2) - 1))*float(i * 2), (1.0f / float((ynum * 2) - 1))*float(j * 2),
								normals[i * 2][j * 2].x, normals[i * 2][j * 2].y, normals[i * 2][j * 2].z, cols[i][j].x, cols[i][j].y, cols[i][j].z, cols[i][j].w);
						}
					}
				}
			}
		}
		end_dist = 0;
	}
	else
	{
		
		for (j = 0; j<ynum; j++)
		{
			for (i = 0; i<xnum; i++)
			{
				if (false)//(i == 10 && xplane == 2)
				{
					AddVertexTexNormCol(
						(float(i)*scale) - ((((float)xnum - 2)*0.5f)*scale) + 500.0f, height_map[i][j], (float(j)*scale) - ((((float)xnum - 2)*0.5f)*scale),
						((1.0f / float(xnum - 1))*float(i)) - 5.0f, (1.0f / float(ynum - 1))*float(j),
						normals[i][j].x, normals[i][j].y, normals[i][j].z,cols[i][j].x, cols[i][j].y, cols[i][j].z, cols[i][j].w);
				}
				else
				{
					if (false) // (i == 0 && xplane == 0)
					{
						AddVertexTexNormCol(
							(float(i)*scale) - ((((float)xnum - 2)*0.5f)*scale) - 500.0f, height_map[i][j], (float(j)*scale) - ((((float)xnum - 2)*0.5f)*scale),
							((1.0f / float(xnum - 1))*float(i)) + 5.0f, (1.0f / float(ynum - 1))*float(j),
							normals[i][j].x, normals[i][j].y, normals[i][j].z, cols[i][j].x, cols[i][j].y, cols[i][j].z, cols[i][j].w);
					}
					else
					{
						AddVertexTexNormCol(
							(float(i)*scale) - ((((float)xnum - 2)*0.5f)*scale), height_map[i][j], (float(j)*scale) - ((((float)xnum - 2)*0.5f)*scale),
							(1.0f / float(xnum - 1))*float(i), (1.0f / float(ynum - 1))*float(j),
							normals[i][j].x, normals[i][j].y, normals[i][j].z, cols[i][j].x, cols[i][j].y, cols[i][j].z, cols[i][j].w);
					}
				}
			}
		}		
		end_dist = 2;

	}
	int cur_phy = 0;
	//Opt
	//	ynum=ynum/2;
	//	xnum=xnum/2;



	// create indicies
	//D3DXOptimizeFaces

	bool bHoles=false;
	// make simple indicies for distant terrain
	for (j = 0; j<ynum - end_dist; j += 4)
	{
		for (i = 0; i<xnum - end_dist; i += 4)
		{
			if (bHoles == false || !(m_vertices[i + (j*xnum)].position.y == 0.0f && m_vertices[(i + 2) + (j*xnum)].position.y == 0.0f && m_vertices[(i + 2) + ((j + 2)*xnum)].position.y == 0.0f))
			{
				if (j == 0)
				{
					// A
					m_indices_simple.push_back(i + (j*xnum));
					m_indices_simple.push_back((i + 1) + (j*xnum));
					m_indices_simple.push_back((i + 2) + ((j + 2)*xnum));

					// B
					m_indices_simple.push_back((i + 1)+ (j*xnum));
					m_indices_simple.push_back((i + 2 ) + (j*xnum));
					m_indices_simple.push_back((i + 2) + ((j + 2)*xnum));

				}
				else
				{
					m_indices_simple.push_back(i + (j*xnum));
					m_indices_simple.push_back((i + 2) + (j*xnum));
					m_indices_simple.push_back((i + 2) + ((j + 2)*xnum));
				}
			}

			if (bHoles == false || !(m_vertices[i + (j*xnum)].position.y == 0.0f && m_vertices[(i + 2) + ((j + 2)*xnum)].position.y == 0.0f && m_vertices[i + ((j + 2)*xnum)].position.y == 0.0f))
			{
				// i == full
				if (i == 0)
				{
					// E
					m_indices_simple.push_back(i + (j*xnum));
					m_indices_simple.push_back((i + 2) + ((j + 2)*xnum));
					m_indices_simple.push_back(i + ((j + 1)*xnum));

					// F
					m_indices_simple.push_back(i + ((j+1)*xnum));
					m_indices_simple.push_back((i + 2) + ((j + 2)*xnum));
					m_indices_simple.push_back(i + ((j + 2)*xnum));

					//m_indices_simple.push_back(i + (j*xnum));
					//m_indices_simple.push_back((i + 2) + ((j + 1)*xnum));
					//m_indices_simple.push_back(i + ((j + 2)*xnum));

				}
				else
				{
					m_indices_simple.push_back(i + (j*xnum));
					m_indices_simple.push_back((i + 2) + ((j + 2)*xnum));
					m_indices_simple.push_back(i + ((j + 2)*xnum));
				}
			}


			if (bHoles == false || !(m_vertices[(i + 2) + (j*xnum)].position.y == 0.0f && m_vertices[(i + 4) + (j*xnum)].position.y == 0.0f && m_vertices[(i + 2) + ((j + 2)*xnum)].position.y == 0.0f))
			{
				if (j == 0)
				{
					// C
					m_indices_simple.push_back((i + 3) + (j*xnum));
					m_indices_simple.push_back((i + 4) + (j*xnum));
					m_indices_simple.push_back((i + 2) + ((j + 2)*xnum));

					// D
					m_indices_simple.push_back((i + 2) + (j*xnum));
					m_indices_simple.push_back((i + 3) + (j*xnum));
					m_indices_simple.push_back((i + 2) + ((j + 2)*xnum));
				}
				else
				{
					m_indices_simple.push_back((i + 2) + (j*xnum));
					m_indices_simple.push_back((i + 4) + (j*xnum));
					m_indices_simple.push_back((i + 2) + ((j + 2)*xnum));

				}
			}

			if (bHoles == false || !(m_vertices[(i + 2) + ((j + 2)*xnum)].position.y == 0.0f && m_vertices[(i + 4) + (j*xnum)].position.y == 0.0f && m_vertices[(i + 4) + ((j + 2)*xnum)].position.y == 0.0f))
			{
				// i = full
				if (i < (xnum - end_dist) - 4)
				{
					m_indices_simple.push_back((i + 2) + ((j + 2)*xnum));
					m_indices_simple.push_back((i + 4) + (j*xnum));
					m_indices_simple.push_back((i + 4) + ((j + 2)*xnum));
				}
				else
				{
					// I
					m_indices_simple.push_back((i + 2) + ((j + 2)*xnum));
					m_indices_simple.push_back((i + 4) + (j*xnum));
					m_indices_simple.push_back((i + 4) + ((j + 1)*xnum));

					// J
					m_indices_simple.push_back((i + 2) + ((j + 2)*xnum));
					m_indices_simple.push_back((i + 4) + ((j+1)*xnum));
					m_indices_simple.push_back((i + 4) + ((j + 2)*xnum));
			
				}

			}


			if (bHoles == false || !(m_vertices[i + ((j + 2)*xnum)].position.y == 0.0f && m_vertices[(i + 2) + ((j + 2)*xnum)].position.y == 0.0f && m_vertices[i + ((j + 4)*xnum)].position.y == 0.0f))
			{
				if (i == 0)
				{
					// G
					m_indices_simple.push_back(i + ((j + 2)*xnum));
					m_indices_simple.push_back((i + 2) + ((j + 2)*xnum));
					m_indices_simple.push_back(i + ((j + 3)*xnum));

					// H
					m_indices_simple.push_back(i + ((j + 3)*xnum));
					m_indices_simple.push_back((i + 2) + ((j + 2)*xnum));
					m_indices_simple.push_back(i + ((j + 4)*xnum));
				}
				else
				{
					m_indices_simple.push_back(i + ((j + 2)*xnum));
					m_indices_simple.push_back((i + 2) + ((j + 2)*xnum));
					m_indices_simple.push_back(i + ((j + 4)*xnum));
				}
			}

			if (bHoles == false || !(m_vertices[(i + 2) + ((j + 2)*xnum)].position.y == 0.0f && m_vertices[(i + 2) + ((j + 4)*xnum)].position.y == 0.0f && m_vertices[i + ((j + 4)*xnum)].position.y == 0.0f))
			{
				
				if (j < (ynum - end_dist) - 4)
				{
					m_indices_simple.push_back((i + 2) + ((j + 2)*xnum));
					m_indices_simple.push_back((i + 2) + ((j + 4)*xnum));
					m_indices_simple.push_back(i + ((j + 4)*xnum));
				}
				else
				{
					// K
					m_indices_simple.push_back((i + 2) + ((j + 2)*xnum));
					m_indices_simple.push_back((i + 2) + ((j + 4)*xnum));
					m_indices_simple.push_back((i + 1) + ((j + 4)*xnum));

					// L
					m_indices_simple.push_back((i + 2) + ((j + 2)*xnum));
					m_indices_simple.push_back((i + 1) + ((j + 4)*xnum));
					m_indices_simple.push_back(i + ((j + 4)*xnum));

				}
				
			}
			if (bHoles == false || !(m_vertices[(i + 2) + ((j + 2)*xnum)].position.y == 0.0f && m_vertices[(i + 4) + ((j + 2)*xnum)].position.y == 0.0f && m_vertices[(i + 4) + ((j + 4)*xnum)].position.y == 0.0f))
			{
				if (i < (xnum - end_dist) - 4)
				{
					m_indices_simple.push_back((i + 2) + ((j + 2)*xnum));
					m_indices_simple.push_back((i + 4) + ((j + 2)*xnum));
					m_indices_simple.push_back((i + 4) + ((j + 4)*xnum));
				}
				else
				{
					// M
					m_indices_simple.push_back((i + 2) + ((j + 2)*xnum));
					m_indices_simple.push_back((i + 4) + ((j + 2)*xnum));
					m_indices_simple.push_back((i + 4) + ((j + 3)*xnum));

					// N
					m_indices_simple.push_back((i + 2) + ((j + 2)*xnum));
					m_indices_simple.push_back((i + 4) + ((j + 3)*xnum));
					m_indices_simple.push_back((i + 4) + ((j + 4)*xnum));

					//m_indices_simple.push_back((i + 2) + ((j + 2)*xnum));
					//m_indices_simple.push_back((i + 4) + ((j + 2)*xnum));
					//m_indices_simple.push_back((i + 4) + ((j + 4)*xnum));

				}
			}
			if (bHoles == false || !(m_vertices[(i + 2) + ((j + 2)*xnum)].position.y == 0.0f && m_vertices[(i + 4) + ((j + 4)*xnum)].position.y == 0.0f && m_vertices[(i + 2) + ((j + 4)*xnum)].position.y == 0.0f))
			{
				if (j < (ynum - end_dist) - 4)
				{
					m_indices_simple.push_back((i + 2) + ((j + 2)*xnum));
					m_indices_simple.push_back((i + 4) + ((j + 4)*xnum));
					m_indices_simple.push_back((i + 2) + ((j + 4)*xnum));
				}
				else
				{
					// O
					m_indices_simple.push_back((i + 2) + ((j + 2)*xnum));
					m_indices_simple.push_back((i + 4) + ((j + 4)*xnum));
					m_indices_simple.push_back((i + 3) + ((j + 4)*xnum));

					// P
					m_indices_simple.push_back((i + 2) + ((j + 2)*xnum));
					m_indices_simple.push_back((i + 3) + ((j + 4)*xnum));
					m_indices_simple.push_back((i + 2) + ((j + 4)*xnum));
				}
			}
		}
	}

	bHoles = false;


	for (j = 0; j<ynum - end_dist; j += 2)
	{
		for (i = 0; i<xnum - end_dist; i += 2)
		{
			if(bHoles==false || !(m_vertices[i + (j*xnum)].position.y==0.0f && m_vertices[(i + 1) + (j*xnum)].position.y==0.0f && m_vertices[(i + 1) + ((j + 1)*xnum)].position.y==0.0f))
			{
				m_indices.push_back(i + (j*xnum));
				m_indices.push_back((i + 1) + (j*xnum));
				m_indices.push_back((i + 1) + ((j + 1)*xnum));
			}

			if (bHoles == false || !(m_vertices[i + (j*xnum)].position.y == 0.0f && m_vertices[(i + 1) + ((j + 1)*xnum)].position.y == 0.0f && m_vertices[i + ((j + 1)*xnum)].position.y == 0.0f))
			{
				m_indices.push_back(i + (j*xnum));
				m_indices.push_back((i + 1) + ((j + 1)*xnum));
				m_indices.push_back(i + ((j + 1)*xnum));
			}


			if (bHoles == false || !(m_vertices[(i + 1) + (j*xnum)].position.y == 0.0f && m_vertices[(i + 2) + (j*xnum)].position.y == 0.0f && m_vertices[(i + 1) + ((j + 1)*xnum)].position.y == 0.0f))
			{
				m_indices.push_back((i + 1) + (j*xnum));
				m_indices.push_back((i + 2) + (j*xnum));
				m_indices.push_back((i + 1) + ((j + 1)*xnum));
			}

			if (bHoles == false || !(m_vertices[(i + 1) + ((j + 1)*xnum)].position.y == 0.0f && m_vertices[(i + 2) + (j*xnum)].position.y == 0.0f && m_vertices[(i + 2) + ((j + 1)*xnum)].position.y == 0.0f))
			{
				m_indices.push_back((i + 1) + ((j + 1)*xnum));
				m_indices.push_back((i + 2) + (j*xnum));
				m_indices.push_back((i + 2) + ((j + 1)*xnum));
			}


			if (bHoles == false || !(m_vertices[i + ((j + 1)*xnum)].position.y == 0.0f && m_vertices[(i + 1) + ((j + 1)*xnum)].position.y == 0.0f && m_vertices[i + ((j + 2)*xnum)].position.y == 0.0f))
			{
				m_indices.push_back(i + ((j + 1)*xnum));
				m_indices.push_back((i + 1) + ((j + 1)*xnum));
				m_indices.push_back(i + ((j + 2)*xnum));
			}

			if (bHoles == false || !(m_vertices[(i + 1) + ((j + 1)*xnum)].position.y == 0.0f && m_vertices[(i + 1) + ((j + 2)*xnum)].position.y == 0.0f && m_vertices[i + ((j + 2)*xnum)].position.y == 0.0f))
			{
				m_indices.push_back((i + 1) + ((j + 1)*xnum));
				m_indices.push_back((i + 1) + ((j + 2)*xnum));
				m_indices.push_back(i + ((j + 2)*xnum));
			}
			if (bHoles == false || !(m_vertices[(i + 1) + ((j + 1)*xnum)].position.y == 0.0f && m_vertices[(i + 2) + ((j + 1)*xnum)].position.y == 0.0f && m_vertices[(i + 2) + ((j + 2)*xnum)].position.y == 0.0f))
			{
				m_indices.push_back((i + 1) + ((j + 1)*xnum));
				m_indices.push_back((i + 2) + ((j + 1)*xnum));
				m_indices.push_back((i + 2) + ((j + 2)*xnum));
			}
			if (bHoles == false || !(m_vertices[(i + 1) + ((j + 1)*xnum)].position.y == 0.0f && m_vertices[(i + 2) + ((j + 2)*xnum)].position.y == 0.0f && m_vertices[(i + 1) + ((j + 2)*xnum)].position.y == 0.0f))
			{
				m_indices.push_back((i + 1) + ((j + 1)*xnum));
				m_indices.push_back((i + 2) + ((j + 2)*xnum));
				m_indices.push_back((i + 1) + ((j + 2)*xnum));
			}
		}
	}

	
	if (true)
	{
		// simple
		for (i = 0; i < m_indices_simple.size(); i++)
		{
			m_phy_vertices[cur_phy].x = m_vertices.at(m_indices_simple.at(i)).position.x;
			m_phy_vertices[cur_phy].y = m_vertices.at(m_indices_simple.at(i)).position.y;
			m_phy_vertices[cur_phy++].z = m_vertices.at(m_indices_simple.at(i)).position.z;
		}
	}
	else
	{
		for (i = 0; i < m_indices.size(); i++)
		{
			m_phy_vertices[cur_phy].x = m_vertices.at(m_indices.at(i)).position.x;
			m_phy_vertices[cur_phy].y = m_vertices.at(m_indices.at(i)).position.y;
			m_phy_vertices[cur_phy++].z = m_vertices.at(m_indices.at(i)).position.z;
		}
	}
	
	no_phy_verticies = cur_phy - 1;

	m_verticesCount = m_vertices.size();
	m_indicesCount = m_indices.size();
	m_indicesCount_simple = m_indices_simple.size();

	SetVertexBuffer();
	SetIndexBuffer();
	SetIndexBufferSimple();

	return true;
}

void Terrain::AddVertexTexNormCol(float x, float y, float z, float u, float v, float nx, float ny, float nz,float r,float g,float b, float a)
{
	unique_ptr<VertexData> vd(new VertexData());
	//float rand_brightness = float(rand() % 100)*0.01f;
	vd->SetPosition(x, y, z);
	vd->SetTex(u, v);
	vd->SetNormal(nx, ny, nz);

	vd->SetColor(DirectX::XMFLOAT4(r, g, b, 0.0f));
	vd->SetBlend(a);
	vd->SetSpecular(0.0f);
	m_vertices.push_back(*vd);
}

void Terrain::AddVertexTexNorm(float x, float y, float z, float u, float v, float nx, float ny, float nz)
{
	unique_ptr<VertexData> vd(new VertexData());
	//float rand_brightness = float(rand() % 100)*0.01f;
	vd->SetPosition(x, y, z);
	vd->SetTex(u, v);
	vd->SetNormal(nx, ny, nz);

	//vd->SetColor(DirectX::XMFLOAT4(rand_brightness, rand_brightness, rand_brightness, 1.0f));
	m_vertices.push_back(*vd);
}


void Terrain::SetVertexBuffer()
{
	D3D11_SUBRESOURCE_DATA bufferData = { m_vertices.data(), 0, 0 };
	UINT bytes = sizeof(VertexData) * m_verticesCount;
	CD3D11_BUFFER_DESC bufferDesc = CD3D11_BUFFER_DESC(bytes, D3D11_BIND_VERTEX_BUFFER);
	DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateBuffer(&bufferDesc, &bufferData, &m_vertexBuffer));
}


void Terrain::SetIndexBuffer()
{
	D3D11_SUBRESOURCE_DATA bufferData = { m_indices.data(), 0, 0 };
	UINT bytes = sizeof(unsigned short) * m_indicesCount;
	CD3D11_BUFFER_DESC bufferDesc = CD3D11_BUFFER_DESC(bytes, D3D11_BIND_INDEX_BUFFER);
	ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateBuffer(&bufferDesc, &bufferData, &m_indexBuffer));
}

void Terrain::SetIndexBufferSimple()
{
	D3D11_SUBRESOURCE_DATA bufferData = { m_indices_simple.data(), 0, 0 };
	UINT bytes = sizeof(unsigned short) * m_indicesCount_simple;
	CD3D11_BUFFER_DESC bufferDesc = CD3D11_BUFFER_DESC(bytes, D3D11_BIND_INDEX_BUFFER);
	ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateBuffer(&bufferDesc, &bufferData, &m_indexBuffer_simple));
}

Terrain::~Terrain(void)
{
	ClearMemory();
}


void Terrain::ClearMemory()
{

	for (int i = 0; i < ycoords; ++i) {
		delete[] height_map[i];
	}
	delete[] height_map;
	//delete m_Texture;


	//m_box->m_model->Clearmemory();
	m_vertices.clear();
	m_indices.clear();
	m_indices_simple.clear();
	
	//m_rigidbody->~btRigidBody();
	//delete m_vertexBuffer.;
	//delete m_indexBuffer;
	
	//m_vertexBuffer->Release();
	//m_indexBuffer->Release();
	//m_indexBuffer_simple->Release();
	
	delete[] m_phy_vertices;

	//m_vertices.
}
