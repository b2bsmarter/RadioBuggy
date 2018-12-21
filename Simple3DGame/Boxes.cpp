#include "pch.h"
#include "Boxes.h"

using namespace Simple3DGame;
using namespace DirectX;
using namespace DX;
using namespace std;
using namespace concurrency;

int BoxSortAlphCB(const VOID* arg1, const VOID* arg2)
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


int BoxSortCB(const VOID* arg1, const VOID* arg2)
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



Boxes::Boxes(std::shared_ptr<DX::DeviceResources> pm_deviceResources, Level* pp_Level, Physics* pp_physics, Camera^ pp_Camera, Statics* pp_Statics, Textures* pp_Textures)
{
	m_phys = pp_physics;

	m_maxBoxes = 500;

	m_totalBoxes = 0;

	m_deviceResources = pm_deviceResources;

	p_Camera = pp_Camera;

	p_level = pp_Level;

	p_Statics = pp_Statics;

	p_Textures = pp_Textures;

	statics = new static_t[500];
	p_index = new part_index[500];

	current_item_id = 0;

	bHidePlacements = true;

	for (int i = 0; i < 500; i++)
	{
		m_box[i] = new WorldObject(m_deviceResources, m_phys);
		statics[i].bActive = false;
	}

	

}




void Boxes::SaveBinary(int level)
{
	char local_file_folder[140];
	level = current_level;
	Windows::Storage::StorageFolder^ folder = Windows::Storage::ApplicationData::Current->LocalFolder;
	wcstombs(local_file_folder, folder->Path->Data(), sizeof(local_file_folder));
	// update local crazy
	//sprintf(local_file_folder, "C:\\Users\\craig\\Desktop\\Simple3DGame3\\Simple3DGame\\Assets");

	// Load level scores
	char info_filename[140];
	wchar_t w_info_filename[140];
	sprintf(info_filename, "%s\\LevelBinary\\BoxData", local_file_folder, level);
	_swprintf(w_info_filename, L"%hs", info_filename);
	CreateDirectory(w_info_filename, NULL);

	sprintf(info_filename, "%s\\LevelBinary\\BoxData\\%d.bmp", local_file_folder, level);

	FILE* pFile = fopen(info_filename, "wb");
	if (pFile != NULL)
	{
		fwrite(statics, sizeof(static_t), m_maxBoxes, pFile);

		fclose(pFile);
		//fwrite(&game_settings, sizeof(game_settings_t) * 1, 1, pFile);
		//fclose(pFile);
	}
}





task<void> Boxes::LoadModels()
{
///	LoadModel(0, "Assets/skull.obj", L"Assets/skull.dds", 2, -0.8f);

	std::vector<task<void>> tasks;

	LoadModel(0, "Assets/Models/globe2.obj", "beachball.dds",2, -1.1f, 0.1f, 0.3f, 0.3f,false);
	LoadModel(1, "Assets/Models/barrel.obj", "barrelColor.dds",3, -0.5f, 3.0f, 0.5f, 0.5f, false);
	LoadModel(2, "Assets/Models/cube_round.obj", "ice.dds",1, -0.4f,1.1f, 0.3f, 0.1f, true);
	LoadModel(3, "Assets/Models/box_sci.obj", "box_sci.dds",1, -1.0f, 1.2f, 0.3f, 0.3f, false);
	LoadModel(4, "Assets/Models/cube_round.obj", "crate.dds", 1, -0.6f, 0.1f, 0.3f, 0.3f, false);
	LoadModel(5, "Assets/Models/oilbarrel.obj", "barrel.dds", 3, -0.6f, 1.4f, 0.5f, 0.5f, false);
	LoadModel(6, "Assets/Models/column2.obj", "Granite_Dark.dds", 1, -1.0f, 2.3f, 0.9f, 1.0f, false);
	LoadModel(7, "Assets/Models/cone.obj", "cone.dds", 4, -1.0f, 0.07f, 0.5f, 0.3f, false);
	LoadModel(8, "Assets/Models/dice.obj", "dice.dds", 1, -1.0f, 0.07f, 0.5f, 0.3f, false);
	LoadModel(9, "Assets/Models/skull.obj", "skull.dds", 2, -0.7f, 0.7f, 0.5f, 0.3f, false, 0.8f);
	LoadModel(10, "Assets/Models/cat.obj", "cat.dds", 1, -5.0f, 0.1f, 0.5f, 0.3f, false , 0.8f);
	LoadModel(11, "Assets/Models/duck.obj", "duck.dds", 1, -7.0f, 0.8f, 0.5f, 0.3f, false,0.9f);
	LoadModel(12, "Assets/Models/log.obj", "log.dds", 3, 1.0f, 3.1f, 0.5f, 0.3f, false,0.9f);
	LoadModel(13, "Assets/Models/goat.obj", "goat.dds", 1, 1.5f, 1.1f, 0.5f, 0.3f, false, 0.9f);

	phobject[7].m_model->UpsideDown();
	phobject[7].m_model->FlipVertDir();

	phobject[13].m_model->FlipVertDir();
	phobject[13].m_model->FlipNorms();


	phobject[5].m_model->SetModelSpecular(0.3f);

	phobject[11].m_model->SetModelSpecular(0.0f);

	phobject[8].m_model->SetModelSpecular(1.0f);

	phobject[2].m_model->SetModelSpecular(1.0f);

	phobject[6].m_model->SetModelSpecular(0.7f);

	//phobject[8].m_model->SetModelColourWeight(0.0f, -0.6f, -0.6f, 0.0f);
	/*
	int i0 = phobject[0].m_model->m_indicesCount;
	int i1 = phobject[1].m_model->m_indicesCount;
	int i2 = phobject[2].m_model->m_indicesCount;
	int i3 = phobject[3].m_model->m_indicesCount;
	int i4 = phobject[4].m_model->m_indicesCount;
	int i5 = phobject[5].m_model->m_indicesCount;
	int i6 = phobject[6].m_model->m_indicesCount;
	int i7 = phobject[7].m_model->m_indicesCount;
	*/
	return when_all(tasks.begin(), tasks.end());
}


void Boxes::LoadModel(int index, char* obj_filename, char* tex_filename, int shape, float scale, float mass, float restitution, float friction, bool _bAlpha, float _physics_size)
{
	phobject[index].m_model = new ModelLoader(m_deviceResources);
	phobject[index].m_model->Load(obj_filename, scale,1);
	phobject[index].shape = shape;
	phobject[index].mass = mass;
	phobject[index].restitution = restitution;
	phobject[index].friction = friction;
	phobject[index].bAlpha = _bAlpha;
	phobject[index].physics_size = _physics_size;
	phobject[index].m_Texture = p_Textures->LoadTexture(tex_filename);
	//CreateDDSTextureFromFile((ID3D11Device *)m_d3dDevice.Get(), tex_filename, nullptr, &phobject[index].m_Texture, MAXSIZE_T);
}


bool Boxes::LoadLevel(int level, int location)
{
	current_level = level;
	for (int i = 0; i < m_maxBoxes; i++)
	{
		statics[i].bActive = 0;
	}

	if (LoadBinary(level, location) == false)
	{
		return true;// LoadLocations(level);
	}
	else
	{
		return true;
	}

	//update_frame = 0;
}


void Boxes::LoadLevel(int level)
{
	m_totalBoxes = 0;

	LoadBoxLocations(level, 6, "columnmap.bmp", 2.3f, 0.9f, 1.0f, false);

	LoadBoxLocations(level, 7, "conemap.bmp", 0.07f, 0.5f, 0.3f, false);

	LoadBoxLocations(level, 0,"Ballmap.bmp", 0.1f, 0.3f, 0.3f, false);

	//LoadBoxLocations(level, "rampmap.bmp", "Assets/ramp.obj", L"Assets/crate.dds", -1.8f, 0.07f, 0.0f, 1.0f, 5,true);

//	LoadBoxLocations(level, "cylmap.bmp", "Assets/oilbarrel.obj", L"Assets/barrel.dds", -0.4f, 0.1f, 0.5f, 0.5f, 3, false);
	LoadBoxLocations(level, 1,"cylmap.bmp", 0.3f, 0.5f, 0.5f, false);

	LoadBoxLocations(level, 2,"wallmap.bmp", 0.3f, 0.1f, 1.0f, true);
	//LoadBoxLocations(level, "Boxmap.bmp", "Assets/cube_round.obj", L"Assets/crate.dds", -0.6f, 0.07f, 0.1f, 1.0f, 1, false);
	LoadBoxLocations(level, 3,"Boxmap.bmp", 0.3f, 0.1f, 1.0f, false);



	//LoadBoxLocations(level,4, "rampmap.bmp", -1.8f, 0.07f, 0.0f, true);

	//LoadBoxLocations(level, "Ballmap.bmp", "Assets/globe2.obj", L"Assets/beachball.dds", -1.4f, 0.1f, 0.3f, 0.3f, 2, false);

	//LoadBoxLocations(level,"conemap.bmp" ,"Assets/tall_cylinder.obj",L"Assets/marble.dds",0.2f,		0.1f,0.5f,0.2f,3,false);
	/*
	//LoadBoxLocations(level,"conemap.bmp" ,"Assets/tall_cylinder.obj",L"Assets/marble.dds",0.2f,		0.1f,0.5f,0.2f,3);


	//LoadBoxLocations(level, "Boxmap.bmp", "Assets/torus.obj", L"Assets/crate.dds", -1.0f, 0.07f, 0.1f, 1.0f, 3);

	//LoadBoxLocations("Assets\\cylmap.bmp","Assets\\cylinder.obj",L"Assets\\cokecan.dds",3.5f,		0.1f,0.5f,0.5f,3);
	//LoadBoxLocations("Assets\\cylmap.bmp","Assets\\cylinder.obj",L"Assets\\cokecan.dds",3.5f,		0.1f,0.5f,0.5f,3);

	//LoadBoxLocations("Assets\\Boxmap.bmp","Assets\\cube_round.obj",L"Assets\\crate.dds",10.0f,-2.6f,     0.07f,0.1f,6.0f,1);
	//LoadBoxLocations("Assets\\cylmap.bmp","Assets\\cylinder.obj",L"Assets\\cokecan.dds",3.5f,  0.1f,0.5f,0.5f,3);

	//LoadBoxLocations(level,"conemap.bmp"	,"Assets/coneshape.obj",L"Assets/earth_sphere.dds",-0.3f,  0.4f,0.3f,0.3f,4);
	*/
}


void Boxes::DeleteBoxes()
{
	/*
	for (int i = 0; i<m_totalBoxes; i++)
	{
		//m_box[i]->
		//m_box[i]->
		//m_box[i]->
		//delete m_box[i];//->Update(viewMatrix,timeTotal);
	}
	*/
	m_totalBoxes = 0;

}

Boxes::~Boxes(void)
{

}


void Boxes::UpdateProjectionMatrix(DirectX::XMMATRIX *projectionMatrix)
{
	int i;

	for (i = 0; i<m_totalBoxes; i++)
	{
		m_box[i]->UpdateProjectionMatrix(projectionMatrix);
	}
}

void Boxes::SetDesignPointer(float x, float y, float z, int type)
{
	if (bIgnoreHeight == true)
	{
		design_pointer.y = design_height;
	}
	else
	{
		design_pointer.y = y + design_height + phobject[type].m_model->total_height*0.5;
	}

	design_pointer.bActive = 1;
	design_pointer.x = x;

	design_pointer.z = z;
	design_pointer.type = type;
	bDesignPointer = true;

	//bIgnoreHeight = true;
}


void Boxes::ExplodeArea(float x, float y, float z, float radius, float strength)
{
	btVector3 explode_point = btVector3(x, y, z);
	btVector3 obj_pos;
	btVector3 obj_direction;
	btTransform worldTransform;
	float dist;
	int i;

	for (i = 0; i < m_totalBoxes; i++)
	{
		if (m_box[i]->bActivated == true && m_box[i]->bStatic == false)
		{
			obj_pos = m_box[i]->m_rigidbody->getWorldTransform().getOrigin();
			dist = explode_point.distance(obj_pos);
			obj_direction = explode_point - obj_pos;
			obj_direction.normalize();

			if (dist < radius)
			{
				m_box[i]->m_rigidbody->applyCentralImpulse(obj_direction*(1.0f - (radius / dist))*strength);
				//m_box[i]->m_rigidbody->setActivationState(DISABLE_DEACTIVATION);
				m_box[i]->bDeactivated = true;
			}
		}
	}

	// player
	obj_pos = p_player->getWorldTransform().getOrigin();
	dist = explode_point.distance(obj_pos);
	obj_direction = explode_point - obj_pos;
	obj_direction.normalize();

	if (dist < radius)
	{
		p_player->applyCentralImpulse(obj_direction*(1.0f - (radius / dist))*strength);
		//p_player->setActivationState(DISABLE_DEACTIVATION);
		//m_box[i]->bDeactivated = true;
	}

}


void Boxes::CreateOne(float x, float y, float z, int type)
{
	int i, l;
	bool bFound = false;
	int point_find = 0;
	DirectX::XMFLOAT4 col = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	ObjInfo info;

	// Check exists
	if (true)
	{
		if (type == -1)
		{
			bFound = true;
			for (i = 0; i < m_maxBoxes; i++)
			{
				if (statics[i].bActive == 1 && statics[i].x == x && statics[i].z == z)
				{
					statics[i].bActive = 0;
				}
			}
		}
	}

	if (bFound == true || type == -1)
		return;

	for (i = 0; i < m_maxBoxes; i++)
	{
		if (statics[i].bActive == 0)
		{
			statics[i].x = x;
			statics[i].z = z;
			if (true)// bIgnoreHeight==true)
			{
				statics[i].y = design_pointer.y;// 0.5f + design_height + p_Level->GetTerrainHeight((int)x, (int)z);
			}

			if (i<m_maxBoxes - 1 && statics[i].y > -990.0f)
			{
				//total_pills++;
				statics[i].bActive = 1;

				statics[i].type = type;

				//statics[i].angle = (rand() % 100)*0.01f*(2.0f*M_PI);
				statics[i].rot_x = design_pointer.rot_x;
				statics[i].rot_y = design_pointer.rot_y;
				statics[i].rot_z = design_pointer.rot_z;
				/*
				btTransform m_initialTransform = btTransform(btQuaternion(0.0f, 0.0f, 0.0f), btVector3(statics[i]->x, statics[i]->y + (tree_model[statics[i]->type].m_solid_model->total_height) + 1.0f, statics[i]->z));

				statics[i]->setWorldTransform(m_initialTransform);

				Makem_m_physicsBody(statics[i]);
				statics[i]->m_body->setWorldTransform(m_initialTransform);
				*/
				//statics[i]->m_body = nullptr;
				//float rand_scalea = 0.9f + (rand() % 100)*0.002f;
				//float rand_scaleb = 0.9f + (rand() % 100)*0.002f;
				float rand_scalea = 1.0f;
				float rand_scaleb = 1.0f;

				XMMATRIX scaleMatrix = XMMatrixScaling(rand_scaleb, rand_scalea, rand_scaleb);

				auto rotationMatrix = XMMatrixRotationRollPitchYaw(statics[i].rot_x, statics[i].rot_y, statics[i].rot_z);
				XMMATRIX scaleRotationMatrix = XMMatrixMultiply(scaleMatrix, rotationMatrix);
				auto translationMatrix = XMMatrixTranslation(statics[i].x, statics[i].y, statics[i].z);
				DirectX::XMStoreFloat4x4(&statics[i].model_matrix, XMMatrixTranspose(scaleRotationMatrix * translationMatrix));

				current_pill++;



				info.Mass = 0.0f;// phobject[statics[i].type].mass;
				info.Yaw = statics[i].rot_y;
				info.Pitch = statics[i].rot_y;
				info.Roll = statics[i].rot_x;
				info.Restitution = phobject[statics[i].type].restitution;
				info.Friction = phobject[statics[i].type].friction;

				info.PosX = statics[i].x;
				info.PosY = statics[i].y;
				info.PosZ = statics[i].z;

				info.item_id = statics[i].type;

				info.raise_by_height = true;
				//m_totalBoxes = 0;
				m_box[m_totalBoxes] = new WorldObject(m_deviceResources, m_phys);

				if (true)// (current_stack_item>0)
				{
					m_box[m_totalBoxes]->m_model = phobject[statics[i].type].m_model;
					m_box[m_totalBoxes]->bTexture = 1;// 1;
													  //info.PosY += m_box[m_totalBoxes]->m_model->total_height;

													  //m_box[m_totalBoxes]->SetTexturePointer(phobject[phob_index].m_Texture);
				}
				info.mask = (COL_BOXES);
				info.group = (COL_WORLDSTUFF | COL_CAR | COL_TREES | COL_WALLS);

				if (phobject[statics[i].type].shape == 1)
				{
					m_box[m_totalBoxes]->MakePhysicsBoxFromMesh(&info, phobject[statics[i].type].physics_size);
				}
				if (phobject[statics[i].type].shape == 2)
				{
					m_box[m_totalBoxes]->MakePhysicsSphereFromMesh(&info, phobject[statics[i].type].physics_size);
				}
				if (phobject[statics[i].type].shape == 3)
				{
					m_box[m_totalBoxes]->MakePhysicsCylinderFromMesh(&info, phobject[statics[i].type].physics_size);
				}
				if (phobject[statics[i].type].shape == 4)
				{
					m_box[m_totalBoxes]->MakePhysicsConeFromMesh(&info);
				}
				if (phobject[statics[i].type].shape == 5)
				{
					m_box[m_totalBoxes]->MakePhysicsConvexMesh(&info);
				}
				//m_box[m_totalBoxes]->m_rigidbody->setCollisionFlags(COL_WORLDSTUFF)

				int ypos = (int)(p_level->front_back_walls + 1.0f + statics[i].z);
				int xpos = (int)(p_level->left_right_walls + 1.0f + statics[i].x);

				p_level->stack_height[ypos][xpos] += (m_box[m_totalBoxes]->m_model->total_height*1.0f);

				m_box[m_totalBoxes]->SetPositionMatrix(statics[i].model_matrix);

				if (true)//(is_static == true)
				{
					//info.Mass = 0.0f;
					m_box[m_totalBoxes]->bStatic = true;
					//m_box[m_totalBoxes]->active_frames = 20;
				}
				//m_box[m_totalBoxes]->m_camera = p_Camera;
				m_totalBoxes++;

			}

			if (total_pills < i + 1)
				total_pills = i + 1;
			break;
		}
	}
}



void Boxes::Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> deviceContext, Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer, ModelViewProjectionConstantBuffer* constantBufferData, bool bRenderAlpha)
{
	int i;
	UINT stride = sizeof(VertexPositionTex);
	UINT offset = 0;
	int current_item = 999;


	btVector3 cam_pos = btVector3(p_Camera->EyeX(), p_Camera->EyeY(), p_Camera->EyeZ());
	current_index = 0;
	for (i = 0; i<m_totalBoxes; i++)
	{
		if (phobject[m_box[i]->item_id].bAlpha == bRenderAlpha)
		{
			btTransform tran = m_box[i]->m_rigidbody->getWorldTransform();
			if(p_Camera->CheckPoint(tran.getOrigin().getX(), tran.getOrigin().getY(), tran.getOrigin().getZ(), m_box[i]->m_model->height_highest) == true)
			{	
				p_index[current_index].dist = cam_pos.distance(btVector3(tran.getOrigin().getX(), tran.getOrigin().getY(), tran.getOrigin().getZ()));
				p_index[current_index].part_no = i;
				current_index++;

				m_box[i]->bActivated = 1;
				m_box[i]->m_rigidbody->setActivationState(1);
			}
			else
			{
				if (m_box[i]->bActivated == 1)
				{
					// circular area
					if(FAR_DIST<cam_pos.distance(btVector3(tran.getOrigin().getX(), tran.getOrigin().getY(), tran.getOrigin().getZ())))
					{
						m_box[i]->bActivated = 0;
						m_box[i]->m_rigidbody->clearForces();
						m_box[i]->m_rigidbody->setLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));
						m_box[i]->ResetTransform();
						//m_box[i]->m_rigidbody->setActivationState(DISABLE_SIMULATION);
						m_box[i]->m_rigidbody->setActivationState(0);
						m_box[i]->bDeactivated = true;
					}
				}
			}
		}
	}

	if (current_index>0)
	{
		if (bRenderAlpha == true)
		{
			qsort(p_index, current_index, sizeof(part_index), BoxSortAlphCB);
		}
		else
		{
			qsort(p_index, current_index, sizeof(part_index), BoxSortCB);
		}
	}







	for (i = 0; i<current_index; i++)
	{

		if (true) //(phobject[m_box[i]->item_id].bAlpha == bRenderAlpha)
		{
			if (m_box[p_index[i].part_no]->bStatic == true)
			{
				if (m_box[p_index[i].part_no]->active_frames == 20)
				{
					m_box[p_index[i].part_no]->m_rigidbody->activate(true);
				}
				if (m_box[p_index[i].part_no]->active_frames > 0)
				{
					m_box[p_index[i].part_no]->active_frames--;
				}
				else
				{
					if (true) //(m_box[i]->m_rigidbody->getActivationState()==ISLAND_SLEEPING) //(m_box[i]->active_frames == 0)
					{
						btVector3 fallInertia(0, 0, 0);
						m_box[p_index[i].part_no]->m_rigidbody->setMassProps(0.0f, fallInertia);
						m_box[p_index[i].part_no]->m_rigidbody->setRestitution(1.9f);
						m_box[p_index[i].part_no]->active_frames = 0;
					}
				}
			}


			if (m_box[p_index[i].part_no]->bDeactivated == true)
			{
				m_box[p_index[i].part_no]->bDeactivated = false;
				m_box[p_index[i].part_no]->m_rigidbody->setActivationState(1);
			}


			if (current_item != m_box[p_index[i].part_no]->item_id)
			{
				current_item = m_box[p_index[i].part_no]->item_id;

				deviceContext->PSSetShaderResources(0, 1, &phobject[m_box[p_index[i].part_no]->item_id].m_Texture);

				//m_box[i]->SetTexture(deviceContext);
				deviceContext->IASetVertexBuffers(0, 1, m_box[p_index[i].part_no]->m_model->GetVertexBuffer().GetAddressOf(), &stride, &offset);
				deviceContext->IASetIndexBuffer(m_box[p_index[i].part_no]->m_model->GetIndexBuffer().Get(), DXGI_FORMAT_R16_UINT, 0);
			}
			m_box[p_index[i].part_no]->Render(deviceContext, constantBuffer, constantBufferData);

		
		}
	}



	// design pointer
	if (bDesignPointer == true)
	{


		if (bHidePlacements == false)
		{
			current_index = 0;
			for (i = 0; i < m_maxBoxes; i++)
			{
				if (statics[i].bActive == true && p_Camera->CheckPoint(statics[i].x, statics[i].y, statics[i].z, phobject[statics[i].type].m_model->furthest_point))
				{
					//p_index[current_index].dist = cam_pos.distance(btVector3(statics[i].x, statics[i].y, statics[i].z));
					p_index[current_index].part_no = i;

					current_index++;
				}
				else
				{

				}
			}

			for (int i = 0; i < current_index; i++)
			{
				if (phobject[statics[p_index[i].part_no].type].bAlpha == bRenderAlpha)
				{
					if (current_item != statics[p_index[i].part_no].type)
					{
						current_item = statics[p_index[i].part_no].type;

						deviceContext->IASetVertexBuffers(0, 1, phobject[current_item].m_model->GetVertexBuffer().GetAddressOf(), &stride, &offset);
						deviceContext->IASetIndexBuffer(phobject[current_item].m_model->GetIndexBuffer().Get(), DXGI_FORMAT_R16_UINT, 0);
						deviceContext->PSSetShaderResources(0, 1, &phobject[current_item].m_Texture);

					}

					if (true)//(part_to_render==0)
					{
						constantBufferData->model = statics[p_index[i].part_no].model_matrix;
						deviceContext->UpdateSubresource(constantBuffer.Get(), 0, NULL, constantBufferData, 0, 0);
						deviceContext->DrawIndexed(
							phobject[current_item].m_model->m_indicesCount,
							0,
							0
							);
					}
				}
			}
		}







		// draw design pointer

		if (phobject[design_pointer.type].bAlpha == bRenderAlpha)
		{

			if (current_item != design_pointer.type)
			{
				current_item = design_pointer.type;

				deviceContext->PSSetShaderResources(0, 1, &phobject[current_item].m_Texture);

				//m_box[i]->SetTexture(deviceContext);
				deviceContext->IASetVertexBuffers(0, 1, phobject[current_item].m_model->GetVertexBuffer().GetAddressOf(), &stride, &offset);
				deviceContext->IASetIndexBuffer(phobject[current_item].m_model->GetIndexBuffer().Get(), DXGI_FORMAT_R16_UINT, 0);
			}

			if (true)//(part_to_render==0)
			{
				//MVPConstantBuffer m_constantBuffer = m_constantBufferB;

				auto rotationMatrix = XMMatrixRotationRollPitchYaw(design_pointer.rot_x, design_pointer.rot_y, design_pointer.rot_z);
				auto translationMatrix = XMMatrixTranslation(design_pointer.x, design_pointer.y, design_pointer.z);
				XMStoreFloat4x4(&design_pointer.model_matrix, XMMatrixTranspose(rotationMatrix * translationMatrix));
				constantBufferData->model = design_pointer.model_matrix;
				deviceContext->UpdateSubresource(constantBuffer.Get(), 0, NULL, constantBufferData, 0, 0);

				deviceContext->DrawIndexed(
					phobject[current_item].m_model->m_indicesCount,
					0,
					0
					);
			}
		}
	}



}


void Boxes::Update()
{
	int i;
	/*
	for (i = 0; i < m_totalBoxes; i++)
	{
		if (m_box[i]->bStatic == true)
		{
			if (m_box[i]->active_frames > 0)
			{
				m_box[i]->active_frames--;
				if (m_box[i]->active_frames == 0)
				{
					btVector3 fallInertia(0, 0, 0);
					m_box[i]->m_rigidbody->setMassProps(0.0f, fallInertia);
					m_box[i]->m_rigidbody->setRestitution(1.9f);
				}
			}
		}
	}
	*/
}


float Boxes::GetWorldHeight(float posx, float posz)
{
	btVector3 vec_from = btVector3(posx, 50.0f, posz);
	btVector3 vec_to = btVector3(posx, -5.0f, posz);

	btCollisionWorld::ClosestRayResultCallback RayCallback(vec_from, vec_to);

	// Perform raycast
	m_phys->m_dynamicsWorld->rayTest(vec_from, vec_to, RayCallback);

	if (RayCallback.hasHit())
	{
		return RayCallback.m_hitPointWorld.getY();
	}
	else
	{
		return -10.0f;
	}

}


bool Boxes::LoadBinary(int level, int location)
{
	int i, j;
	char local_file_folder[140];
	current_level = level;
	Windows::Storage::StorageFolder^ folder = Windows::Storage::ApplicationData::Current->LocalFolder;
	wcstombs(local_file_folder, folder->Path->Data(), sizeof(local_file_folder));

	// update local crazy
	//sprintf(local_file_folder, "C:\\Users\\craig\\Desktop\\Simple3DGame3\\Simple3DGame\\Assets");

	// Load level scores
	char info_filename[140];

	if (location == 0)
	{
		sprintf(info_filename, "%s\\LevelBinary\\BoxData\\%d.bmp", local_file_folder, level);
	}
	else
	{
		sprintf(info_filename, "Assets\\LevelBinary\\BoxData\\%d.bmp", level);
	}




	// LoadLevelData
	FILE * pFile;
	current_pill = 0;
	pFile = fopen(info_filename, "r");
	if (pFile != NULL)
	{
		fread(statics, sizeof(static_t), m_maxBoxes, pFile);

		fclose(pFile);

	}
	else
	{
		return false;
	}
	current_pill = 0;
	for (int i = 0; i < m_maxBoxes; i++)
	{
		if (statics[i].bActive == 1)
		{
			//statics[i].y = p_Level->GetTerrainHeight(statics[i].x, statics[i].z);
			//auto rotationMatrix = XMMatrixRotationRollPitchYaw(statics[i].rot_x, statics[i].rot_y, statics[i].rot_z);
			//auto translationMatrix = XMMatrixTranslation(statics[i].x, statics[i].y, statics[i].z);
			//DirectX::XMStoreFloat4x4(&statics[i].model_matrix, XMMatrixTranspose(rotationMatrix * translationMatrix));

			if (current_pill < i + 1)
			{
				current_pill = i + 1;
			}

			//info.PosX-(-((float)p_level->left_right_walls) - 1.0f) =  ((float)i*1.0f);
			//info.PosZ+p_level->front_back_walls =  ((float)j*1.0f);
			//p_Statics->MakeLocal(info.PosX, info.PosZ);
			/*
			int ypos = (int)(p_level->front_back_walls + statics[i].z);
			int xpos = (int)(p_level->left_right_walls + 1.0f + statics[i].x);

			//p_Level->stack_height[ypos][xpos] = 10.0f;
			float hh = p_level->stack_height[ypos][xpos];
			if (p_level->stack_height[ypos][xpos] < statics[i].y)
			{
			p_level->stack_height[ypos][xpos] = statics[i].y + 5.0f;
			}
			*/
		}
	}
	total_pills = current_pill;

	MakeObjects();

	return true;
}

void Boxes::MakeObjects()
{
	int i;

	ObjInfo info;

	m_totalBoxes = 0;

	for (i = 0; i < total_pills;i++)
	{
		if (statics[i].bActive == true)
		{
			info.Mass = phobject[statics[i].type].mass;
			info.Yaw = statics[i].rot_x;
			info.Pitch = statics[i].rot_y;
			info.Roll = statics[i].rot_z;
			info.Restitution = phobject[statics[i].type].restitution;
			info.Friction = phobject[statics[i].type].friction;

			info.PosX = statics[i].x;
			info.PosY = statics[i].y;
			info.PosZ = statics[i].z;

			info.item_id = statics[i].type;

			info.raise_by_height = true;
			//m_totalBoxes = 0;
			m_box[m_totalBoxes] = new WorldObject(m_deviceResources, m_phys);

			if (true)// (current_stack_item>0)
			{
				m_box[m_totalBoxes]->m_model = phobject[statics[i].type].m_model;
				m_box[m_totalBoxes]->bTexture = 1;// 1;
												  //info.PosY += m_box[m_totalBoxes]->m_model->total_height;

												  //m_box[m_totalBoxes]->SetTexturePointer(phobject[phob_index].m_Texture);
			}
			info.mask = (COL_BOXES);
			info.group = (COL_WORLDSTUFF | COL_CAR | COL_TREES | COL_WALLS | COL_CARBODY);

			if (phobject[statics[i].type].shape == 1)
			{
				m_box[m_totalBoxes]->MakePhysicsBoxFromMesh(&info, phobject[statics[i].type].physics_size);
			}
			if (phobject[statics[i].type].shape == 2)
			{
				m_box[m_totalBoxes]->MakePhysicsSphereFromMesh(&info, phobject[statics[i].type].physics_size);
			}
			if (phobject[statics[i].type].shape == 3)
			{
				m_box[m_totalBoxes]->MakePhysicsCylinderFromMesh(&info, phobject[statics[i].type].physics_size);
			}
			if (phobject[statics[i].type].shape == 4)
			{
				m_box[m_totalBoxes]->MakePhysicsConeFromMesh(&info);
			}
			if (phobject[statics[i].type].shape == 5)
			{
				m_box[m_totalBoxes]->MakePhysicsConvexMesh(&info);
			}
			//m_box[m_totalBoxes]->m_rigidbody->setCollisionFlags(COL_WORLDSTUFF)

			m_box[m_totalBoxes]->SetPositionMatrix(statics[i].model_matrix);

			int ypos = (int)(p_level->front_back_walls + statics[i].z);
			int xpos = (int)(p_level->left_right_walls + 1.0f + statics[i].x);

			p_level->stack_height[ypos][xpos] += (m_box[m_totalBoxes]->m_model->total_height*1.0f) + 0.1f;

			if (false)//(is_static == true)
			{
				//info.Mass = 0.0f;
				m_box[m_totalBoxes]->bStatic = true;
				m_box[m_totalBoxes]->active_frames = 20;
			}
			//m_box[m_totalBoxes]->m_camera = p_Camera;
			m_totalBoxes++;
		}
	}

}


bool Boxes::LoadBoxLocations(int level, int phob_index, char* map_filename, float mass, float restitution, float friction, bool is_static)
{
	FILE *filePtr;							// Point to the current position in the file
	BITMAPFILEHEADER bitmapFileHeader;		// Structure which stores information about file
	BITMAPINFOHEADER bitmapInfoHeader;		// Structure which stores information about image
	int imageSize, index;
	unsigned char height,height2,height3;
	float item_height = 10.0f;

	HeightMapInfo hminfo;

	char file_folder[60];

	sprintf(file_folder, "Assets\\LevelMaps\\%d\\%s", level, map_filename);

	// Open the file
	filePtr = fopen(file_folder, "rb");
	if (filePtr == NULL)
		return 0;

	// Read bitmaps header
	fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);

	// Read the info header
	fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);

	// Get the width and height (width and length) of the image
	hminfo.terrainWidth = bitmapInfoHeader.biWidth;
	hminfo.terrainHeight = bitmapInfoHeader.biHeight;

	// Size of the image in bytes. the 3 represents RBG (byte, byte, byte) for each pixel
	imageSize = hminfo.terrainWidth * hminfo.terrainHeight * 3;
	/*
	unsigned char* bitmapImage = (unsigned char*)malloc(imageSize);
	hminfo.heightMap = (XMFLOAT3 *)malloc(imageSize*sizeof(XMFLOAT3));
	hminfo.normal = (XMFLOAT3 *)malloc(imageSize*sizeof(XMFLOAT3));
	*/

	unsigned char* bitmapImage = new unsigned char[imageSize];
	hminfo.heightMap = new DirectX::XMFLOAT3[imageSize*sizeof(DirectX::XMFLOAT3)];
	hminfo.normal = new DirectX::XMFLOAT3[imageSize*sizeof(DirectX::XMFLOAT3)];


	//ZeroMemory(&bitmapImage, sizeof(imageSize));
	//ZeroMemory(&hminfo.heightMap, imageSize*sizeof(XMFLOAT3));
	//ZeroMemory(&hminfo.normal, imageSize*sizeof(XMFLOAT3));

	// Set the file pointer to the beginning of the image data
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	// Store image data in bitmapImage
	fread(bitmapImage, 1, imageSize, filePtr);

	// Close file
	fclose(filePtr);


	ObjInfo info;


	// We use a greyscale image, so all 3 rgb values are the same, but we only need one for the height
	// So we use this counter to skip the next two components in the image data (we read R, then skip BG)
	int k = 0;
	int j2;
	int i2;
	int point_type;
	int current_stack_item = 0;
	int pos;

	// We divide the height by this number to "water down" the terrains height, otherwise the terrain will
	// appear to be "spikey" and not so smooth.
	// Read the image data into our heightMap array
	for (int j = 0; j< hminfo.terrainHeight; j++)
	{
		for (int i = 0; i<hminfo.terrainWidth-3; i++)
		{
			pos = (j*(hminfo.terrainWidth * 3)) + ((hminfo.terrainWidth - 1) * 3) - ((i * 3));
			
			height = bitmapImage[pos];
			height2 = bitmapImage[pos+1];
			height3 = bitmapImage[pos+2];

			point_type = 0;

			if ((height == height2) && height2 == height3 && height>0)
			{
				point_type = 1; // white
			}
			else
			{
				if ((height > height2) && height > height3)
				{
					point_type = 4; // blue
				}
				else
				{
					if ((height2 > height) && height2 > height3)
					{
						point_type = 3; // green
					}
					else
					{
						if ((height3 > height) && height3 > height2)
						{
							point_type = 2; // red
						}
						else
						{

						}
					}
				}
			}

			if (height>0)
			{
				info.PosX = -((float)p_level->left_right_walls) - 1.0f + ((float)i*1.0f);
				info.PosZ = -p_level->front_back_walls + ((float)j*1.0f);
				p_Statics->MakeLocal(info.PosX, info.PosZ);
				p_level->stack_height[j][i] = GetWorldHeight(info.PosX, info.PosZ);
				//p_level->stack_height[j][i] = 10.0f;
				for (int tower = 0; tower<point_type; tower++)
				{
					j2 = j;
					i2 = i;

					
					
					info.PosY = p_level->stack_height[j][i] + (phobject[phob_index].m_model->total_height*0.5f);// +0.01f;
					

					if (m_totalBoxes<m_maxBoxes && info.PosY > -990.0f) //( m_totalBoxes<m_maxBoxes-1 )
					{
						//m_box[m_totalBoxes] = new Planet();
						//m_box[m_totalBoxes]->Initialize(pm_d3dDevice , 

						j2 = j;
						i2 = i;

						//info.ObjFilename = obj_filename;
						//info.TextureFilename = tex_filename; //L"Assets\\trans_marble.dds";
						//info.Scale = phobject[phob_index].;
						info.Mass = mass;
						info.Yaw = 0.0f;
						info.Pitch = 0.0f;
						info.Roll = 0.0f;
						info.Restitution = restitution;
						info.Friction = friction;
						
						info.item_id = phob_index;

						info.raise_by_height = true;

						m_box[m_totalBoxes] = new WorldObject(m_deviceResources, m_phys);

						if (is_static == true)
						{
							//info.Mass = 0.0f;
							m_box[m_totalBoxes]->bStatic = true;
							m_box[m_totalBoxes]->active_frames = 20;
							//m_box[m_totalBoxes]->m_rigidbody->setActivationState(WANTS_DEACTIVATION);
						}
						else
						{
							m_box[m_totalBoxes]->bStatic = false;

						}

						if (true)// (current_stack_item>0)
						{
							m_box[m_totalBoxes]->m_model = phobject[phob_index].m_model;
							m_box[m_totalBoxes]->bTexture = 1;// 1;
							//info.PosY += m_box[m_totalBoxes]->m_model->total_height;

							//m_box[m_totalBoxes]->SetTexturePointer(phobject[phob_index].m_Texture);
						}
						info.mask = (COL_BOXES);
						info.group = (COL_WORLDSTUFF|COL_CAR| COL_TREES| COL_WALLS);

						if (phobject[phob_index].shape == 1)
						{
							m_box[m_totalBoxes]->MakePhysicsBoxFromMesh(&info);
						}
						if (phobject[phob_index].shape == 2)
						{
							m_box[m_totalBoxes]->MakePhysicsSphereFromMesh(&info);
						}
						if (phobject[phob_index].shape == 3)
						{
							m_box[m_totalBoxes]->MakePhysicsCylinderFromMesh(&info);
						}
						if (phobject[phob_index].shape == 4)
						{
							m_box[m_totalBoxes]->MakePhysicsConeFromMesh(&info);
						}
						if (phobject[phob_index].shape == 5)
						{
							m_box[m_totalBoxes]->MakePhysicsConvexMesh(&info);
						}
						//m_box[m_totalBoxes]->m_rigidbody->setCollisionFlags(COL_WORLDSTUFF)
						

						p_level->stack_height[j][i] += (m_box[m_totalBoxes]->m_model->total_height*1.0f)+0.1f;
						if (is_static == true)
						{
							//info.Mass = 0.0f;
							m_box[m_totalBoxes]->bStatic = true;
							m_box[m_totalBoxes]->active_frames = 20;
							
						}
						//m_box[m_totalBoxes]->m_camera = p_Camera;
						m_totalBoxes++;
						current_stack_item++;
					}
				}
			}
			k += 3;

			/*
			index = ( hminfo.terrainHeight * j) + i;

			hminfo.heightMap[index].x = (float)i;
			hminfo.heightMap[index].y = (float)height * height_scale;
			hminfo.heightMap[index].z = (float)j;
			*/

		}
	}

	current_item_id++;

	/*
	for(int j=0; j< hminfo.terrainHeight; j++)
	{
	for(int i=0; i< hminfo.terrainWidth; i++)
	{
	index = ( hminfo.terrainHeight * j) + i;

	float tl = hminfo.heightMap[clampB(i-1,hminfo.terrainWidth)+(clampB((j-1),hminfo.terrainHeight)*hminfo.terrainWidth)].y;
	float t = hminfo.heightMap[clampB(i,hminfo.terrainWidth)+(clampB((j-1),hminfo.terrainHeight)*hminfo.terrainWidth)].y;
	float tr = hminfo.heightMap[clampB(i+1,hminfo.terrainWidth)+(clampB((j-1),hminfo.terrainHeight)*hminfo.terrainWidth)].y;
	float r = hminfo.heightMap[clampB(i+1,hminfo.terrainWidth)+(clampB((j),hminfo.terrainHeight)*hminfo.terrainWidth)].y;
	float br = hminfo.heightMap[clampB(i+1,hminfo.terrainWidth)+(clampB((j+1),hminfo.terrainHeight)*hminfo.terrainWidth)].y;
	float b = hminfo.heightMap[clampB(i,hminfo.terrainWidth)+(clampB((j+1),hminfo.terrainHeight)*hminfo.terrainWidth)].y;
	float bl = hminfo.heightMap[clampB(i-1,hminfo.terrainWidth)+(clampB((j+1),hminfo.terrainHeight)*hminfo.terrainWidth)].y;
	float l = hminfo.heightMap[clampB(i-1,hminfo.terrainWidth)+(clampB((j),hminfo.terrainHeight)*hminfo.terrainWidth)].y;

	float dZ = -((tr + (2.0 * r) + br) - (tl + (2.0 * l) + bl));
	float dX = (tl + (2.0 * t) + tr) - (bl + (2.0 * b) + br);
	float dY = 0.5;// / pStrength;

	btVector3 v(dX,dY,dZ);

	v.normalize();

	hminfo.normal[index].x=v.getX();
	hminfo.normal[index].y=v.getY();
	hminfo.normal[index].z=v.getZ();
	}
	}
	*/

	delete[] bitmapImage;
	bitmapImage = 0;

	delete[] hminfo.heightMap;
	delete[] hminfo.normal;

	return true;
}

