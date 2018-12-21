#include "pch.h"
#include "Player.h"

using namespace Simple3DGame;
using namespace DirectX;
using namespace DX;
using namespace std;

Player::Player(std::shared_ptr<DX::DeviceResources> pm_deviceResources, Physics* phys) : WorldObject(pm_deviceResources, phys)
{
	angle = 0.0f;
	points = 0;

	m_deviceResources = pm_deviceResources;
	m_phys = phys;

	dir_position = btVector3(0.0f,0.0f,0.0f);
	dir_last_osition = btVector3(0.0f, 0.0f, 0.0f);

	point_angle_to = 0.0f;
	point_angle = 0.0f;

	bIsHolding = false;
	bReleaseForce = false;

	bShooting = false;
	bShoot = false;

	Wheelposition = 0;

	b_show_rockets = 0;

	carspeed = 0.0f;

	break_value = 400.0f;

	rockets = 0;

	//respot_delay = 3.0f;

	last_floor_norm = btVector3(0.0f, 1.0f, 0.0f);

	shoot_delay_time = 0.3f;
	bAB = false;
}



btVector3* Player::GetDirection()
{
	static btVector3 dir;
	static btVector3 lastdir;
	
	dir = dir_last_osition - dir_position;
	dir.setY(0.0f);
	dir.normalize();

	if (dir.getX() == 0 && dir.getZ() == 0)
	{
		lastdir = dir;
	}
	else
	{

	}

	return &dir;
	//dir_position = *GetPosition();
}

bool Player::DoShoot()
{
	if (bShoot == true)
	{
		bShoot = false;
		return true;
	}
	else
	{
		return false;
	}
	
}

void Player::ABPressed()
{
	bAB = true;

}

void Player::ABReleased()
{
	bAB = false;

}

void Player::GrabPressed()
{
	/*
	if (bIsHolding == true)
	{
		release_force = 0.0f;
		bReleaseForce = true;
	}
	else
	{


	}
	*/
	bShooting = true;
	//bShoot = true;
	//shooting_delay = shoot_delay_time;
}

void Player::Grabreleased()
{
	//bReleaseForce = false;
	bShooting = false;
}

void Player::Update(float timeDelta, float timeTotal)
{
	int i;
	float point_before = point_angle_to;
	float ang1 = point_angle_to;// +4 * M_PI;
	float ang2 = point_angle;// +4 * M_PI;

	float wheel_height[4];
	float highest=-100.0f;
	float lowest=100.0f;
	float highest_space = -100.0f;
	float lowest_space = 100.0f;

	float spacing = 0.0f;



	if (true)
	{
		player_x = m_constantBufferData.model._14;
		player_y = m_constantBufferData.model._24;
		player_z = m_constantBufferData.model._34;
	}
	else
	{
		btTransform tran = m_rigidbody->getWorldTransform();
		player_x = tran.getOrigin().getX();
		player_y = tran.getOrigin().getY();
		player_z = tran.getOrigin().getZ();
	}

	rot_mat = btMatrix3x3(m_constantBufferData.model._11, m_constantBufferData.model._12, m_constantBufferData.model._13, m_constantBufferData.model._21, m_constantBufferData.model._22, m_constantBufferData.model._23, m_constantBufferData.model._31, m_constantBufferData.model._32, m_constantBufferData.model._33);

	x_offset = btVector3(1.0f, 0.0f, 0.0f);
	x_offset = rot_mat* x_offset;

	x_offset1 = btVector3(0.7f, 0.0f, -0.0f);
	x_offset1 = rot_mat* x_offset1;

	x_offset2 = btVector3(-0.7f, 0.0f, -0.0f);
	x_offset2 = rot_mat* x_offset2;

	y_offset = btVector3(0.0f, 0.2f, 0.0f);
	y_offset = rot_mat* y_offset;

	down_offset = btVector3(0.0f, -0.3f, 0.0f);
	down_offset = rot_mat* down_offset;



	for (i = 0; i < 4; i++)
	{
		spacing = wheel[i]->m_rigidbody->getWorldTransform().getOrigin().getY();
		wheel_height[i] = p_Level->GetTerrainHeight(wheel[i]->m_rigidbody->getWorldTransform().getOrigin().getX(), wheel[i]->m_rigidbody->getWorldTransform().getOrigin().getZ());
		spacing = spacing - wheel_height[i];

		if (wheel_height[i] < lowest)
			lowest = wheel_height[i];
		if (wheel_height[i] >highest)
			highest = wheel_height[i];

		if (spacing < lowest_space)
			lowest_space = spacing;
		if (spacing >highest_space)
			highest_space = spacing;

	}
	

	btTransform ground_tran = p_Level->m_FloorPlane->getWorldTransform();

	float floor_height = p_Level->GetTerrainHeight(player_x+ down_offset.getX(), player_z+ down_offset.getZ());


	btVector3 floor_norm = p_Level->GetNormal(player_x + down_offset.getX(), player_z + down_offset.getZ());
	
	float heigh_diff = 1.5f;

	if(((highest-lowest)<3.0f) && (highest_space<2.0f))//(ball_tran.getOrigin().getY() - heigh_diff && wheel_tran0.getOrigin().getY() - floor_height<heigh_diff && wheel_tran1.getOrigin().getY() - floor_height<heigh_diff && wheel_tran2.getOrigin().getY() - floor_height<heigh_diff && wheel_tran3.getOrigin().getY() - floor_height<heigh_diff) // ( btVector3(0.0f, 1.0f, 0.0f).distance(floor_norm)<0.4f)
	{
		
		//hvEyeMove = (vEyeTo - vEye)*(timeDelta*2.5);
		//hvAtMove = (vAtTo - vAt)*(timeDelta*4.5);

		/*
		if (hvEyeMove.length() > 1.0f)
		{
		hvEyeMove.normalize();
		}
		*/

		//vEye += hvEyeMove;
		// Smooth transition
		last_floor_norm += (floor_norm - last_floor_norm)*(timeDelta*4.5);

		// try to interpolate the floor plane movement
		/*
		if (last_floor_norm.distance(floor_norm) < 0.1f)
		{
			last_floor_norm = floor_norm;
		}
		else
		{
			btVector3 floor_travel = last_floor_norm - floor_norm;
			floor_travel.normalize();
			last_floor_norm = last_floor_norm - floor_travel*0.1f;
		}
		*/
		btQuaternion floor_rot = btQuaternion(last_floor_norm.getX(), last_floor_norm.getY(), last_floor_norm.getZ(), 0.0f);
		//	btQuaternion floor_rot = btQuaternion(0.0f, 1.0f, 0.0f, 1.0f);
		
		ground_tran.setOrigin(btVector3(player_x, floor_height - 0.7f, player_z));


		ground_tran.setRotation(floor_rot);

		if (floor_norm.getX() != -999.0f)
		{
			p_Level->m_FloorPlane->setWorldTransform(ground_tran);
		}
	}
	else
	{
		ground_tran.setOrigin(btVector3(0.0f, 0.0f, 0.0f));

		btQuaternion floor_rot = btQuaternion(0.0f, 1.0f, 0.0f, 0.0f);
		ground_tran.setRotation(floor_rot);

		if (floor_norm.getX() != -999.0f)
		{
			p_Level->m_FloorPlane->setWorldTransform(ground_tran);
		}
	}

	/*
	if ((ball_tran.getOrigin().getY() - floor_height) < 2.0f)
	{
		btVector3 floor_norm = p_Level->GetNormal(ball_tran.getOrigin().getX(), ball_tran.getOrigin().getZ());

		btQuaternion floor_rot = btQuaternion(floor_norm.getX(), floor_norm.getY(), floor_norm.getZ(), 0.0f);
		//	btQuaternion floor_rot = btQuaternion(0.0f, 1.0f, 0.0f, 1.0f);
		ground_tran.setOrigin(btVector3(ball_tran.getOrigin().getX(), floor_height - 0.5f, ball_tran.getOrigin().getZ()));

		ground_tran.setRotation(floor_rot);

		if (floor_norm.getX() != -999.0f)
		{
			p_Level->m_FloorPlane->setWorldTransform(ground_tran);
		}
	}
	else
	{
		btVector3 floor_norm = btVector3(0.0f,1.0f,0.0f);

		btQuaternion floor_rot = btQuaternion(floor_norm.getX(), floor_norm.getY(), floor_norm.getZ(), 0.0f);
		//	btQuaternion floor_rot = btQuaternion(0.0f, 1.0f, 0.0f, 1.0f);
		ground_tran.setOrigin(btVector3(0.0f,0.0f,0.0f));

		ground_tran.setRotation(floor_rot);

		if (floor_norm.getX() != -999.0f)
		{
			p_Level->m_FloorPlane->setWorldTransform(ground_tran);
		}

	}
	*/


	if (shooting_delay > 0)
	{
		shooting_delay -= timeDelta;

	}
	else
	{
		if (bShooting == true)
		{
			shooting_delay = shoot_delay_time;
			bShoot = true;
		}
	}



	if (ang1 - ang2 < -M_PI)//(point_angle_to-point_angle_to>M_PI)
	{
		//ang2 = point_angle;
		point_angle_to -= ((ang1 + M_PI*2.0f) - (ang2))*(timeDelta*5.5);
	}
	else
	{
		if (ang1 - ang2 > M_PI)
		{
			point_angle_to -= ((ang1 - M_PI*2.0f) - (ang2))*(timeDelta*5.5);
		}
		else
		{
			point_angle_to -= (ang1 - ang2)*(timeDelta*5.5);
		}
	}

	// find the forwards vectors


	btVector3 lin_vel = m_rigidbody->getLinearVelocity();


	car_point = btVector3(0.0f, 0.0f, 1.0f);
	car_point = rot_mat* car_point;
	
	carspeed = lin_vel.dot(car_point);

	// copy rotation matrix
	for (int row = 0; row<3; ++row)
		for (int column = 0; column<3; ++column)
			point_martix.m[row][column] = rot_mat[column][row];

	// copy position
	for (int column = 0; column<3; ++column)
		point_martix.m[3][column] = 0.0f;





	/*
	btTransform trans;
	this->getWorldTransform(trans);
	trans = m_rigidbody->getWorldTransform();
	//trans.getRotation().
	WorldObject::SetPosition(trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ(), point_angle_to, trans.getRotation().getY(), trans.getRotation().getZ());
	*/
	//point_angle_to = point_angle;

}




btVector3* Player::GetPosition()
{
	static btVector3 player_pos;

	player_pos = btVector3(m_constantBufferData.model._14, m_constantBufferData.model._24, m_constantBufferData.model._34);
	/*
	btTransform ball_tran = m_rigidbody->getWorldTransform();

	player_pos = ball_tran.getOrigin();

	if (player_pos.getY() < 0.0f)
	{
		player_pos.setY(20.0f);
		player_pos.setX(player_pos.getX());
		player_pos.setZ(player_pos.getZ());
		//ball_tran.setOrigin(player_pos);

		setWorldTransform(ball_tran);
		//m_rigidbody->setLinearVelocity(btVector3(0.0f, 10.0f, 0.0f));

	}
	*/
	return &player_pos;
}

btVector3 Player::GetRealPosition()
{
	return btVector3(m_constantBufferData.model._14, m_constantBufferData.model._24, m_constantBufferData.model._34);
}


void Player::Breaking()
{
	wheel[0]->m_rigidbody->setDamping(0.0f, break_value);
	wheel[1]->m_rigidbody->setDamping(0.0f, break_value);
	wheel[2]->m_rigidbody->setDamping(0.0f, break_value);
	wheel[3]->m_rigidbody->setDamping(0.0f, break_value);

}


void Player::SetMotors(float targetVelocity, float maxMotorImpulse)
{
	//targetVelocity = 60.0f;
	//maxMotorImpulse = 2.6f;
	if (wheel[0]->m_rigidbody->getAngularVelocity().length() < targetVelocity)
	{
		wheel[0]->m_rigidbody->applyTorqueImpulse(x_offset*maxMotorImpulse);
	}
	if (wheel[1]->m_rigidbody->getAngularVelocity().length() < targetVelocity)
	{
		wheel[1]->m_rigidbody->applyTorqueImpulse(x_offset*maxMotorImpulse);
	}
	if (wheel[2]->m_rigidbody->getAngularVelocity().length() < targetVelocity)
	{
		wheel[2]->m_rigidbody->applyTorqueImpulse(x_offset*maxMotorImpulse);
	}
	if (wheel[3]->m_rigidbody->getAngularVelocity().length() < targetVelocity)
	{
		wheel[3]->m_rigidbody->applyTorqueImpulse(x_offset*maxMotorImpulse);
	}

}

void Player::SetMotors(int b_on)
{
	//return;


	// nonoe of this is working

	float	targetVelocity = 30.0f;
	float	maxMotorImpulse = 1.3f;

	if(b_on == 1)
	{
		//targetVelocity = -targetVelocity;
		//maxMotorImpulse = -maxMotorImpulse;
	}
	wheel[0]->m_rigidbody->setDamping(0.0f, angular_damping);
	wheel[1]->m_rigidbody->setDamping(0.0f, angular_damping);
	wheel[2]->m_rigidbody->setDamping(0.0f, angular_damping);
	wheel[3]->m_rigidbody->setDamping(0.0f, angular_damping);
	
	if (b_on == 0)
	{
		
		//hinge0->enableAngularMotor(false, targetVelocity, maxMotorImpulse);
		//hinge1->enableAngularMotor(false, targetVelocity, maxMotorImpulse);
		//hinge2->enableAngularMotor(false, targetVelocity, maxMotorImpulse);
		//hinge3->enableAngularMotor(false, targetVelocity, maxMotorImpulse);
	}
	else
	{
		if (b_on == -1)
		{
			if (carspeed < -0.5f)
			{
				Breaking();
			}
			else
			{
				if (wheel[0]->m_rigidbody->getAngularVelocity().length() < targetVelocity)
				{
					wheel[0]->m_rigidbody->applyTorqueImpulse(x_offset*maxMotorImpulse);
				}
				if (wheel[1]->m_rigidbody->getAngularVelocity().length() < targetVelocity)
				{
					wheel[1]->m_rigidbody->applyTorqueImpulse(x_offset*maxMotorImpulse);
				}
				if (wheel[2]->m_rigidbody->getAngularVelocity().length() < targetVelocity)
				{
					wheel[2]->m_rigidbody->applyTorqueImpulse(x_offset*maxMotorImpulse);
				}
				if (wheel[3]->m_rigidbody->getAngularVelocity().length() < targetVelocity)
				{
					wheel[3]->m_rigidbody->applyTorqueImpulse(x_offset*maxMotorImpulse);
				}
			}
		}
		else
		{
			if (b_on == 2)// break
			{
				Breaking();
			}
			else
			{
				if (b_on == -2) 
				{
					if (carspeed < -0.5f)
					{
						Breaking();
					}
					else
					{
						targetVelocity = 60.0f;
						maxMotorImpulse = 2.6f;
						if (wheel[0]->m_rigidbody->getAngularVelocity().length() < targetVelocity)
						{
							wheel[0]->m_rigidbody->applyTorqueImpulse(x_offset*maxMotorImpulse);
						}
						if (wheel[1]->m_rigidbody->getAngularVelocity().length() < targetVelocity)
						{
							wheel[1]->m_rigidbody->applyTorqueImpulse(x_offset*maxMotorImpulse);
						}
						if (wheel[2]->m_rigidbody->getAngularVelocity().length() < targetVelocity)
						{
							wheel[2]->m_rigidbody->applyTorqueImpulse(x_offset*maxMotorImpulse);
						}
						if (wheel[3]->m_rigidbody->getAngularVelocity().length() < targetVelocity)
						{
							wheel[3]->m_rigidbody->applyTorqueImpulse(x_offset*maxMotorImpulse);
						}
					}
				}
				else
				{
					if (carspeed > 0.5f)
					{
						Breaking();
					}
					else
					{
						if (wheel[0]->m_rigidbody->getAngularVelocity().length() < targetVelocity)
						{
							wheel[0]->m_rigidbody->applyTorqueImpulse(-x_offset*maxMotorImpulse);
						}
						if (wheel[1]->m_rigidbody->getAngularVelocity().length() < targetVelocity)
						{
							wheel[1]->m_rigidbody->applyTorqueImpulse(-x_offset*maxMotorImpulse);
						}
						if (wheel[2]->m_rigidbody->getAngularVelocity().length() < targetVelocity)
						{
							wheel[2]->m_rigidbody->applyTorqueImpulse(-x_offset*maxMotorImpulse);
						}
						if (wheel[3]->m_rigidbody->getAngularVelocity().length() < targetVelocity)
						{
							wheel[3]->m_rigidbody->applyTorqueImpulse(-x_offset*maxMotorImpulse);
						}
					}
				}
			}
		}
		//wheel[0]->m_rigidbody->applyTorqueImpulse(btVector3(0.0f, 0.0f, maxMotorImpulse));
		//wheel[1]->m_rigidbody->applyTorqueImpulse(btVector3(0.0f, 0.0f, maxMotorImpulse));
		//wheel[2]->m_rigidbody->applyTorqueImpulse(btVector3(0.0f, 0.0f, maxMotorImpulse));
		//wheel[3]->m_rigidbody->applyTorqueImpulse(btVector3(0.0f, 0.0f, maxMotorImpulse));

   		//hinge0->enableAngularMotor(true, targetVelocity, maxMotorImpulse);
		//hinge1->enableAngularMotor(true, targetVelocity, maxMotorImpulse);
		//hinge2->enableAngularMotor(true, targetVelocity, maxMotorImpulse);
		//hinge3->enableAngularMotor(true, targetVelocity, maxMotorImpulse);
	}
	
}

void Player::SetWheelTurn(float _wheel_turn)
{
	wheel_turn = _wheel_turn;

	wheel_turn = -1.0f;

	btVector3 turn = btVector3(0.0f, 0.0f, 1.0f);
	//turn.normalize();
	hinge0->setAxis(turn);
	//hinge0->egt
	//hinge0->setEnabled(false);
}

void Player::SetWheelTurn(int turn_to)
{
	switch (turn_to)
	{
	case 0:if (Wheelposition == 1)
			{
				hinge1right->setEnabled(false);
				hinge3right->setEnabled(false);
				hinge1->setEnabled(true);
				hinge3->setEnabled(true);

				//hinge1->e
			}
		   if (Wheelposition == -1)
		   {
			   hinge1left->setEnabled(false);
			   hinge3left->setEnabled(false);
			   hinge1->setEnabled(true);
			   hinge3->setEnabled(true);
		   }
		   Wheelposition = 0;
		   break;

	case 1:if (Wheelposition == 0)
			{
				hinge1->setEnabled(false);
				hinge3->setEnabled(false);
				hinge1right->setEnabled(true);
				hinge3right->setEnabled(true);
			}
		   if (Wheelposition == -1)
		   {
			   hinge1left->setEnabled(false);
			   hinge3left->setEnabled(false);
			   hinge1right->setEnabled(true);
			   hinge3right->setEnabled(true);
		   }
		   Wheelposition = 1;
		   break;

	case -1:if (Wheelposition == 1)
			{
				hinge1right->setEnabled(false);
				hinge3right->setEnabled(false);
				hinge1left->setEnabled(true);
				hinge3left->setEnabled(true);
			}
		   if (Wheelposition == 0)
		   {
			   hinge1->setEnabled(false);
			   hinge3->setEnabled(false);
			   hinge1left->setEnabled(true);
			   hinge3left->setEnabled(true);
		   }
		   Wheelposition = -1;
		   break;

	}

}


void Player::MakeWheels()
{
	bool bPoint2Point=false;

	float softness = 0.001f;
	float bias = 0.001f;
	float relaxation = 0.001f;
	float wheelheight = -0.22f;
	float linear_damping = 0.0f;
	angular_damping = 0.1f;
	float hit_fraction = 0.0f;

	float front_friction_diff = 0.3f;

	float steer_ammount = 0.3f;
	float wheel_shine = 0.1f;

	ObjInfo info = { "Assets/Models/wheel.obj"
		, L"Assets/Textures/wheel_tex2.dds"
		//		, L"Assets/foot_ball.dds"
		//, L"Assets/sphere.dds"
		, -0.4f		// Scale
		, local_info.PosX+2.0f		// X,y,z
		, local_info.PosY
		, local_info.PosZ-2.0f
		, 1.1f		// mass
		, 0.0f		// yaw,pitch,roll
		, M_PI*0.5f
		, 0.0f
		, 0.1f		// restitution
		, 1.3f
	,0
	,false
	,(COL_FLOORPLANE | COL_BOXES | COL_TREES | COL_WALLS | COL_WORLDSTUFF)
	,(COL_CAR)
	,1};	// friction
					/*
					info.ObjFilename = "Assets/Orange.obj";
					info.TextureFilename = L"Assets/Orange.dds";
					info.Scale = -0.3f;
					info.PosX = 0.0f;
					info.PosY = 1.0f;
					info.PosZ = 0.0f; //m_Level->back_position+100.0f;
					info.Mass = 1.0f;
					info.Yaw = 0.0f;
					info.Pitch = 0.0f;
					info.Roll = 0.0f;
					info.Restitution = 0.8f;
					info.Friction = 0.5f;
					*/

					//m_Banana->MakePhysicsConvexMesh(m_deviceResources->GetD3DDevice(), m_Physics, &info);
	

	// Add a chasy mass between wheels
	// Add a low center of mass
	/*
	auto fallShape = new btSphereShape(btSphereShape(0.01f));
	//auto fallShape = new btSphereShape(btSphereShape(1.5f));
	//auto fallShape = new btTriangleMesh (btSphereShape(1.5f));
	btMotionState* fallMotionState = (btMotionState*)this;//(cube, btTransform(btQuaternion(0,0,0,1),btVector3(x,y,z)));
														  //btScalar mass = 0.2;
	btVector3 fallInertia(0, 0, 0);
	//fallMotionState->
	//btSphereShape::
	//m_rigidbody = null;
	fallShape->calculateLocalInertia(1.0f, fallInertia);
	m_rigidbody = phys->AddPhysicalObject(fallShape, fallMotionState, info->Mass, fallInertia, info->Restitution, info->Friction, &obj_id, info);
	*/






	

	wheel[0] = new WorldObject(m_deviceResources,m_phys);
	wheel[0]->MakePhysicsSphereFromMesh(&info);
	wheel[0]->m_model->SetModelSpecular(wheel_shine);
	wheel[0]->m_rigidbody->setActivationState(DISABLE_DEACTIVATION);
	wheel[0]->m_rigidbody->setDamping(linear_damping, angular_damping);
	wheel[0]->m_rigidbody->setHitFraction(hit_fraction);
	wheel_offset[0] = btVector3(0.8f, wheelheight, -1.2f);
	btVector3 pivotInA = wheel_offset[0];
	btVector3 pivotInB = btVector3(0.0f, 0.0f, 0.0f);
	btVector3 axisInA = btVector3(1.0f, 0.0f, 0.0f);
	btVector3 axisInB = btVector3(0.0f, 1.0f, 0.0f);
	

	if(bPoint2Point==true)
	{
		point0 = new btPoint2PointConstraint(*m_rigidbody, *wheel[0]->m_rigidbody,
			pivotInA,
			pivotInB);
		m_phys->m_dynamicsWorld->addConstraint(point0, true);
	}
	else
	{
		hinge0 = new btHingeConstraint(*m_rigidbody, *wheel[0]->m_rigidbody,
			pivotInA,
			pivotInB,
			axisInA,
			axisInB,
			false);


		//hinge0->enableAngularMotor(true, targetVelocity, maxMotorImpulse);

		hinge0->setLimit(0.0f,
			M_PI*2.0f,
			softness,
			bias,
			relaxation);
		//btVector3 axis = btVector3(-1.0f, 0.0f, 1.0f);
		//hinge0->setAxis(btVector3(-1.0f, 0.0f, 1.0f).normalize());

		m_phys->m_dynamicsWorld->addConstraint(hinge0, true);
	}

	
	info.PosX = local_info.PosX + 2.0f;		// X,y,z
	info.PosY = local_info.PosZ + 2.0f;


	info.Friction -= front_friction_diff;

	wheel[1] = new WorldObject(m_deviceResources, m_phys);
	wheel[1]->MakePhysicsSphereFromMesh(&info);
	wheel[1]->m_model->SetModelSpecular(wheel_shine);
	wheel[1]->m_rigidbody->setActivationState(DISABLE_DEACTIVATION);
	wheel[1]->m_rigidbody->setDamping(linear_damping, angular_damping);
	wheel[1]->m_rigidbody->setHitFraction(hit_fraction);
	wheel_offset[1] = btVector3(0.8f, wheelheight, 1.2f);
	pivotInA = wheel_offset[1]; // front left
	pivotInB = btVector3(0.0f, 0.0f, 0.0f);
	axisInA = btVector3(1.0f, 0.0f, 0.0f);
	//axisInA = btVector3(0.0f, 0.0f, 0.0f);
	axisInB = btVector3(0.0f, 1.0f, 0.0f);

	if (bPoint2Point == true)
	{
		point1 = new btPoint2PointConstraint(*m_rigidbody, *wheel[1]->m_rigidbody,
			pivotInA,
			pivotInB);
		m_phys->m_dynamicsWorld->addConstraint(point1, true);
	}
	else
	{
		hinge1 = new btHingeConstraint(*m_rigidbody, *wheel[1]->m_rigidbody,
			pivotInA,
			pivotInB,
			axisInA,
			axisInB,
			false);

		//hinge1->enableAngularMotor(true, targetVelocity, maxMotorImpulse);

		hinge1->setLimit(0.0f,
			M_PI*2.0f,
			softness,
			bias,
			relaxation);

		m_phys->m_dynamicsWorld->addConstraint(hinge1, true);

		axisInA = btVector3(1.0f, 0.0f, steer_ammount); // turn tight

		hinge1right = new btHingeConstraint(*m_rigidbody, *wheel[1]->m_rigidbody,
			pivotInA,
			pivotInB,
			axisInA,
			axisInB,
			false);

		m_phys->m_dynamicsWorld->addConstraint(hinge1right, true);
		hinge1right->setEnabled(false);


		axisInA = btVector3(1.0f, 0.0f, -steer_ammount); // turn left

		hinge1left = new btHingeConstraint(*m_rigidbody, *wheel[1]->m_rigidbody,
			pivotInA,
			pivotInB,
			axisInA,
			axisInB,
			false);

		m_phys->m_dynamicsWorld->addConstraint(hinge1left, true);
		hinge1left->setEnabled(false);

	}


	info.PosX = local_info.PosX - 2.0f;		// X,y,z
	info.PosY = local_info.PosZ - 2.0f;


	info.Friction += front_friction_diff;

	wheel[2] = new WorldObject(m_deviceResources, m_phys);
	wheel[2]->MakePhysicsSphereFromMesh(&info);
	wheel[2]->m_model->SetModelSpecular(wheel_shine);
	wheel[2]->m_rigidbody->setActivationState(DISABLE_DEACTIVATION);
	wheel[2]->m_rigidbody->setDamping(linear_damping, angular_damping);
	wheel[2]->m_rigidbody->setHitFraction(hit_fraction);
	wheel[2]->m_model->UpsideDown();
	wheel[2]->m_model->FlipVertDir();
	//wheel[2]->m_model->FlipNorms();
	wheel_offset[2] = btVector3(-0.8f, wheelheight, -1.2f);
	pivotInA = wheel_offset[2];
	pivotInB = btVector3(0.0f, 0.0f, 0.0f);
	axisInA = btVector3(1.0f, 0.0f, 0.0f);
	axisInB = btVector3(0.0f, 1.0f, 0.0f);


	if (bPoint2Point == true)
	{
		point2 = new btPoint2PointConstraint(*m_rigidbody, *wheel[2]->m_rigidbody,
			pivotInA,
			pivotInB);
		m_phys->m_dynamicsWorld->addConstraint(point2, true);
	}
	else
	{


		hinge2 = new btHingeConstraint(*m_rigidbody, *wheel[2]->m_rigidbody,
			pivotInA,
			pivotInB,
			axisInA,
			axisInB,
			false);

		//hinge2->enableAngularMotor(true, targetVelocity, maxMotorImpulse);

		hinge2->setLimit(0.0f,
			M_PI*2.0f,
			softness,
			bias,
			relaxation);

		m_phys->m_dynamicsWorld->addConstraint(hinge2, true);
	}

	info.PosX = local_info.PosX - 2.0f;		// X,y,z
	info.PosY = local_info.PosZ + 2.0f;


	info.Friction -= front_friction_diff;

	wheel[3] = new WorldObject(m_deviceResources, m_phys);
	wheel[3]->MakePhysicsSphereFromMesh(&info);
	wheel[3]->m_model->SetModelSpecular(wheel_shine);
	wheel[3]->m_rigidbody->setActivationState(DISABLE_DEACTIVATION);
	wheel[3]->m_rigidbody->setDamping(linear_damping, angular_damping);
	wheel[3]->m_rigidbody->setHitFraction(hit_fraction);
	wheel[3]->m_model->UpsideDown();
	wheel[3]->m_model->FlipVertDir();
	//wheel[3]->m_model->FlipNorms();
	wheel_offset[3] = btVector3(-0.8f, wheelheight, 1.2f);
	pivotInA = wheel_offset[3]; // front right
	pivotInB = btVector3(0.0f, 0.0f, 0.0f);
	axisInA = btVector3(1.0f, 0.0f, 0.0f);
	axisInB = btVector3(0.0f, 1.0f, 0.0f);

	if (bPoint2Point == true)
	{
		point3 = new btPoint2PointConstraint(*m_rigidbody, *wheel[3]->m_rigidbody,
			pivotInA,
			pivotInB);
		m_phys->m_dynamicsWorld->addConstraint(point3, true);
	}
	else
	{

		hinge3 = new btHingeConstraint(*m_rigidbody, *wheel[3]->m_rigidbody,
			pivotInA,
			pivotInB,
			axisInA,
			axisInB,
			false);

		//hinge3->enableAngularMotor(true, targetVelocity, maxMotorImpulse);


		hinge3->setLimit(0.0f,
			M_PI*2.0f,
			softness,
			bias,
			relaxation);

		m_phys->m_dynamicsWorld->addConstraint(hinge3, true);

		axisInA = btVector3(1.0f, 0.0f, steer_ammount); // turn tight

		hinge3right = new btHingeConstraint(*m_rigidbody, *wheel[3]->m_rigidbody,
			pivotInA,
			pivotInB,
			axisInA,
			axisInB,
			false);

		m_phys->m_dynamicsWorld->addConstraint(hinge3right, true);
		hinge3right->setEnabled(false);


		axisInA = btVector3(1.0f, 0.0f, -steer_ammount); // turn left

		hinge3left = new btHingeConstraint(*m_rigidbody, *wheel[3]->m_rigidbody,
			pivotInA,
			pivotInB,
			axisInA,
			axisInB,
			false);

		m_phys->m_dynamicsWorld->addConstraint(hinge3left, true);
		hinge3left->setEnabled(false);

	}


	//SetMotors();
	//SetWheelTurn(1.0f);

}


void Player::Initialize(char* model_filename, wchar_t* texture_filename, ObjInfo* info)
{
	CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), texture_filename, nullptr, &m_Texture, MAXSIZE_T);

	local_info = *info;
	//CreateVehicle((ID3D11Device *)pm_d3dDevice.Get(), )
	//setRPY(yaw, double pitch, double yaw)
	/*
	m_initialTransform = btTransform(btQuaternion(0.0f,0.0f,0.0f,1.0f),btVector3(x,y,z));

	XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(yaw, pitch, roll);

	XMMATRIX scaleMatrix = XMMatrixScaling(scale, scale, scale);

	XMMATRIX translationMatrix = XMMatrixTranslation(x, y, z);

	XMMATRIX scaleRotationMatrix = XMMatrixMultiply(scaleMatrix, rotationMatrix);
	XMMATRIX modelMatrix = XMMatrixMultiplyTranspose(scaleRotationMatrix, translationMatrix);
	*/
	//btQuaternion(
	m_initialTransform = btTransform(btQuaternion(info->Yaw, info->Pitch, info->Roll), btVector3(info->PosX, info->PosY, info->PosZ));
	//m_initialTransform = btTransform(btQuaternion(0.0f,0.0f,0.0f,1.0f),btVector3(x,y,z));

	/*
	XMMATRIX m_modelMatrix;
	XMVECTOR data;
	XMVECTORF32 floatingVector = { 0.0f, 0.0f, 0.0f, 1.0f };
	data = floatingVector;

	auto rotationMatrix = XMMatrixRotationRollPitchYaw(yaw,pitch,roll);
	auto translationMatrix = XMMatrixTranslation(x, y, z);
	auto scale_matrix = XMMatrixScaling(scale, scale, scale);

	m_modelMatrix = XMMatrixTranspose((scale_matrix * translationMatrix ) *  rotationMatrix);

	XMMATRIX m_InvMatrix = XMMatrixInverse(&data, m_modelMatrix);

	XMStoreFloat4x4(&m_constantBuffer.model, m_InvMatrix);
	*/
	ObjInfo info2 = *info;


	//info2.PosX += 3.0f;

	//wheel = new WorldObject();

	//wheel->MakePhysicsSphereFromMesh(pm_d3dDevice, m_phys, &info2);
	info2.Scale = -0.5f;
	m_model = new ModelLoader(m_deviceResources);
	m_model2 = new ModelLoader(m_deviceResources);
	//m_model->Load(model_filename, pm_d3dDevice, scale);
	m_model->Load("Assets/Models/car_cube.obj", info2.Scale,0);
	m_model2->Load("Assets/Models/car_cube2.obj", info2.Scale,1);

	//m_model->Load("Assets/ship.obj", pm_d3dDevice, info2.Scale);
	//m_model->RotateY(180.0f);
	m_model->UpsideDown();
	m_model->FlipVertDir();
	//m_model->FlipNorms();

	m_model2->UpsideDown();
	m_model2->FlipVertDir();
	//m_model2->FlipNorms();

	m_model->SetScale(1.45f, 1.0f, 0.7f);
	m_model2->SetScale(1.45f, 1.0f, 0.7f);

	m_model->TransOrigin(0.0f, 0.2f, 0.0f);
	m_model2->TransOrigin(0.0f, 0.2f, 0.0f);

	m_model->SetModelSpecular(0.7f);
	m_model2->SetModelSpecular(0.4f);

	CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), L"Assets/Textures/carshade3.dds", nullptr, &m_Texture2, MAXSIZE_T);







	/*
	m_ship_model = new ModelLoader();

	m_ship_model->Load("Assets/ship.obj", pm_d3dDevice, scale);

	SetBuffers(m_ship_model->GetVertexBuffer(), m_ship_model->GetIndexBuffer(), m_ship_model->GetIndicesCount());
	*/
	WorldObject::Initialize(model_filename, texture_filename, info);

}







void Player::Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> deviceContext, Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer, ModelViewProjectionConstantBuffer* constantBufferData)
{
	UINT stride = sizeof(VertexPositionTex);
	UINT offset = 0;


	if (b_show_rockets == 0)
	{
		if(rockets>0)
		{
			deviceContext->IASetVertexBuffers(0, 1, m_model_rocket->GetVertexBuffer().GetAddressOf(), &stride, &offset);
			deviceContext->IASetIndexBuffer(m_model_rocket->GetIndexBuffer().Get(), DXGI_FORMAT_R16_UINT, 0);

			deviceContext->PSSetShaderResources(0, 1, &m_Texture_rocket);

			auto translationMatrix = XMMatrixTranslation(player_x + x_offset1.getX(), player_y + x_offset1.getY(), player_z + x_offset1.getZ());
			XMStoreFloat4x4(&constantBufferData->model, XMMatrixTranspose(XMLoadFloat3x3(&point_martix) * translationMatrix));

			//constantBufferData->model = m_constantBuffer.model;
			//constantBufferData->model = m_constantBuffer.model;

			//m_constantBufferB.model = fruit[i].model;
			deviceContext->UpdateSubresource(constantBuffer.Get(), 0, NULL, constantBufferData, 0, 0);

			deviceContext->DrawIndexed(
				m_model_rocket->m_indicesCount,
				0,
				0
				);


			translationMatrix = XMMatrixTranslation(player_x + x_offset2.getX(), player_y + x_offset2.getY(), player_z + x_offset2.getZ());
			XMStoreFloat4x4(&constantBufferData->model, XMMatrixTranspose(XMLoadFloat3x3(&point_martix) * translationMatrix));

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
	else
	{
		b_show_rockets--;
	}
	/*
	btTransform worldTransform;
	SetTexture(deviceContext);
	UINT stride = sizeof(VertexPositionTex);
	UINT offset = 0;

	deviceContext->PSSetShaderResources(0, 1, &m_Texture);

	deviceContext->IASetVertexBuffers(0, 1, m_model->GetVertexBuffer().GetAddressOf(), &stride, &offset);
	deviceContext->IASetIndexBuffer(m_model->GetIndexBuffer().Get(), DXGI_FORMAT_R16_UINT, 0);

	constantBufferData->model = m_constantBufferData.model;

	deviceContext->UpdateSubresource(constantBuffer.Get(), 0, NULL, &constantBufferData, 0, 0);

	deviceContext->DrawIndexed(m_indexCount, 0, 0);
	*/
	/*
	deviceContext->PSSetShaderResources(0, 1, &m_Texture);
	btTransform trans;
	this->getWorldTransform(trans);
	trans = m_rigidbody->getWorldTransform();

	btQuaternion rotation = trans.getRotation();

	XMVECTOR data;
	XMVECTORF32 floatingVector = { rotation.getX(), rotation.getY(), rotation.getZ(), rotation.getW() };
	data = floatingVector;

	auto rotationMatrix = XMMatrixRotationQuaternion(data);
	auto translationMatrix = XMMatrixTranslation(trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ());

	//	auto rotationMatrix = XMMatrixRotationRollPitchYaw(0.0f, 0.0f, 0.0f);
	//	auto translationMatrix = XMMatrixTranslation(0.0f, 6.0f, 20.0f);

	XMStoreFloat4x4(&m_constantBufferData.model, XMMatrixTranspose(rotationMatrix * translationMatrix));
	constantBufferData->model = m_constantBufferData.model;

	
	dir_last_osition = dir_position;
	dir_position = *GetPosition();

	
	deviceContext->UpdateSubresource(constantBuffer.Get(), 0, NULL, &constantBufferData, 0, 0);

	SetBuffers(m_ship_model->GetVertexBuffer(), m_ship_model->GetIndexBuffer(), m_ship_model->GetIndicesCount());

	deviceContext->IASetVertexBuffers(0, 1, m_ship_model->GetVertexBuffer().GetAddressOf(), &stride, &offset);
	deviceContext->IASetIndexBuffer(m_ship_model->GetIndexBuffer().Get(), DXGI_FORMAT_R16_UINT, 0);

	deviceContext->DrawIndexed(m_ship_model->GetIndicesCount(), 0, 0);
	*/
	WorldObject::Render(deviceContext, constantBuffer, constantBufferData);
	//WorldObject::RenderYaw(deviceContext, constantBuffer, constantBufferData, point_angle_to);
	deviceContext->IASetVertexBuffers(0, 1, m_model2->GetVertexBuffer().GetAddressOf(), &stride, &offset);
	deviceContext->IASetIndexBuffer(m_model2->GetIndexBuffer().Get(), DXGI_FORMAT_R16_UINT, 0);
	deviceContext->PSSetShaderResources(0, 1, &m_Texture2);
	deviceContext->DrawIndexed(m_model2->m_indicesCount, 0, 0);

	float off_x = x_offset.getX();
	float off_y = x_offset.getY();
	float off_z = x_offset.getZ();
	// Offset the wheels slightly
	wheel[0]->RenderOffset(deviceContext, constantBuffer, constantBufferData, off_x*0.3f, off_y*0.3f, off_z*0.3f);
	wheel[1]->RenderOffset(deviceContext, constantBuffer, constantBufferData, off_x*0.3f, off_y*0.3f, off_z*0.3f);
	wheel[2]->RenderOffset(deviceContext, constantBuffer, constantBufferData, off_x*-0.3f, off_y*-0.3f, off_z*-0.3f);
	wheel[3]->RenderOffset(deviceContext, constantBuffer, constantBufferData, off_x*-0.3f, off_y*-0.3f, off_z*-0.3f);
	/*
	btTransform trans;
	this->getWorldTransform(trans);
	trans = m_vehicle->getChassisWorldTransform();


	btQuaternion rotation = trans.getRotation();

	XMVECTOR data;
	XMVECTORF32 floatingVector = { rotation.getX(), rotation.getY(), rotation.getZ(), rotation.getW() };
	data = floatingVector;

	auto rotationMatrix = XMMatrixRotationQuaternion(data);
	auto translationMatrix = XMMatrixTranslation(trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ());

	//	auto rotationMatrix = XMMatrixRotationRollPitchYaw(0.0f, 0.0f, 0.0f);
	//	auto translationMatrix = XMMatrixTranslation(0.0f, 6.0f, 20.0f);

	XMStoreFloat4x4(&m_constantBufferData.model, XMMatrixTranspose(rotationMatrix * translationMatrix));
	constantBufferData->model = m_constantBufferData.model;
	//constantBufferData->model = m_constantBuffer.model;

	deviceContext->UpdateSubresource(constantBuffer.Get(), 0, NULL, constantBufferData, 0, 0);

	deviceContext->DrawIndexed(m_indexCount, 0, 0);

	//SetPosition(trans.getOrigin().getX(), trans.getOrigin().getY()+10.0f, trans.getOrigin().getZ(), 0.0f, 0.0f, 0.0f);
	m_rigidbody->setWorldTransform(trans);
	*/

}

void Player::SetPosition(float x, float y, float z, float yaw, float pitch, float roll)
{
	//btTransform transform1 = btTransform(btQuaternion(yaw, pitch, roll), btVector3(x+2.0f, y /*+ (m_model->total_height*0.5f)*/, z));
	wheel[0]->SetPosition(x, y, z, yaw, pitch, roll - M_PI*0.5f);// setWorldTransform(transform1);
	wheel[1]->SetPosition(x, y, z, yaw, pitch, roll - M_PI*0.5f);
	wheel[2]->SetPosition(x, y, z, yaw, pitch, roll - M_PI*0.5f);
	wheel[3]->SetPosition(x, y, z, yaw, pitch, roll - M_PI*0.5f);

	//setWorldTransform(m_initialTransform);
	


	WorldObject::SetPosition(x, y, z, yaw, pitch, roll);
}

void Player::SetCarPosition(float x, float y, float z, float yaw, float pitch, float roll)
{
	btQuaternion rot_quat = btQuaternion(yaw, pitch, roll);

	btMatrix3x3 rotation = btMatrix3x3(rot_quat);

	btVector3 off0 = wheel_offset[0] * rotation;
	btVector3 off1 = wheel_offset[1] * rotation;
	btVector3 off2 = wheel_offset[2] * rotation;
	btVector3 off3 = wheel_offset[3] * rotation;

	//btTransform transform1 = btTransform(btQuaternion(yaw, pitch, roll), btVector3(x+2.0f, y /*+ (m_model->total_height*0.5f)*/, z));
	wheel[0]->SetPosition(x + off0.getX(), y + off0.getY(), z + off0.getZ(), yaw, pitch, roll - M_PI*0.5f);// setWorldTransform(transform1);
	wheel[1]->SetPosition(x + off1.getX(), y + off1.getY(), z + off1.getZ(), yaw, pitch, roll - M_PI*0.5f);
	wheel[2]->SetPosition(x + off2.getX(), y + off2.getY(), z + off2.getZ(), yaw, pitch, roll - M_PI*0.5f);
	wheel[3]->SetPosition(x + off3.getX(), y + off3.getY(), z + off3.getZ(), yaw, pitch, roll - M_PI*0.5f);

	//setWorldTransform(m_initialTransform);

	// clear all forces
	m_rigidbody->setLinearVelocity(btVector3(0.0f, 5.0f, 0.0f));
	m_rigidbody->setAngularVelocity(btVector3(0.0f, 0.0f, 0.0f));

	wheel[0]->m_rigidbody->setLinearVelocity(btVector3(0.0f, 5.0f, 0.0f));
	wheel[0]->m_rigidbody->setAngularVelocity(btVector3(0.0f, 0.0f, 0.0f));
	wheel[1]->m_rigidbody->setLinearVelocity(btVector3(0.0f, 5.0f, 0.0f));
	wheel[1]->m_rigidbody->setAngularVelocity(btVector3(0.0f, 0.0f, 0.0f));
	wheel[2]->m_rigidbody->setLinearVelocity(btVector3(0.0f, 5.0f, 0.0f));
	wheel[2]->m_rigidbody->setAngularVelocity(btVector3(0.0f, 0.0f, 0.0f));
	wheel[3]->m_rigidbody->setLinearVelocity(btVector3(0.0f, 5.0f, 0.0f));
	wheel[3]->m_rigidbody->setAngularVelocity(btVector3(0.0f, 0.0f, 0.0f));

	WorldObject::SetPosition(x, y, z, yaw, pitch, roll);
}



Player::~Player(void)
{


}
