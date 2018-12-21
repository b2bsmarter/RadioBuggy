#pragma once

#include "WorldObject.h"

#include "../DirectXTK/Inc/DDSTextureLoader.h"



namespace Simple3DGame
{

	class Player : public WorldObject
	{
	public:
		Player(std::shared_ptr<DX::DeviceResources> pm_deviceResources, Physics* phys);
		~Player(void);

		Physics* m_phys;
		int points;

		float angle;
		void Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> deviceContext, Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer, ModelViewProjectionConstantBuffer* constantBufferData);
		
		void Initialize(char* model_filename, wchar_t* texture_filename, ObjInfo* info);

		void Update(float timeDelta, float timeTotal);

		float respot_delay;

		void MakeWheels();
		void SetMotors(int b_on);

		btVector3 GetRealPosition();

		void SetMotors(float targetVelocity, float maxMotorImpulse);

		void Breaking();

		void GrabPressed();
		void Grabreleased();

		void ABPressed();

		void ABReleased();

		bool DoShoot();


		bool bShooting;
		bool bShoot;
		bool bAB; // afterburner

		float shooting_delay;
		float shoot_delay_time;
		float angular_damping;
		float carspeed;

		bool bIsHolding;
		bool bReleaseForce;

		float release_force;

		float break_value;


		int rockets;


		btPoint2PointConstraint* hold_constraint;

		// Sets wheel position
		void SetPosition(float x, float y, float z, float yaw, float pitch, float roll);
		void SetCarPosition(float x, float y, float z, float yaw, float pitch, float roll);

		void SetWheelTurn(float _wheel_turn);
		void SetWheelTurn(int turn_to);

		int Wheelposition; // -1,0,1

		float point_angle;
		float point_angle_to;

		float gEngineForce;
		float gBreakingFrontForce;
		float gBreakingBackForce;

		float maxEngineForce;
		float minEngineForce;
		float maxBreakingFrontForce;
		float maxBreakingBackForce;

		float gVehicleSteering;
		float steeringIncrement;
		float steeringClamp;
		float wheelRadius;
		float wheelWidth;
		float wheelFriction;
		float suspensionStiffness;
		float suspensionDamping;
		float suspensionCompression;
		float rollInfluence;

		ObjInfo local_info;

		WorldObject* wheel[4];
		btVector3 wheel_offset[4];

		Level* p_Level;

		ModelLoader* m_model2;
		ID3D11ShaderResourceView* m_Texture2;

		int b_show_rockets;

		ModelLoader* m_model_rocket;
		ID3D11ShaderResourceView* m_Texture_rocket;

		btVector3 car_point;
		//DirectX::XMMATRIX point_martix;
		DirectX::XMFLOAT3X3 point_martix;
		btVector3 x_offset;
		btVector3 y_offset; // light
		btVector3 x_offset1;
		btVector3 x_offset2;
		btVector3 down_offset; // floor plane

		float player_x, player_y, player_z;
		btQuaternion rot;
		btMatrix3x3 rot_mat;



		float wheel_turn;

		btHingeConstraint* hinge0;
		btHingeConstraint* hinge1;
		btHingeConstraint* hinge2;
		btHingeConstraint* hinge3;


		btPoint2PointConstraint* point0;
		btPoint2PointConstraint* point1;
		btPoint2PointConstraint* point2;
		btPoint2PointConstraint* point3;

		btHingeConstraint* hinge1left; // front left
		btHingeConstraint* hinge3left; // front right

		btHingeConstraint* hinge1right; // front left
		btHingeConstraint* hinge3right; // front right

		btVector3* GetDirection();

		btVector3 dir_position;
		btVector3 dir_last_osition;

		btVector3 last_floor_norm;

		ModelLoader* m_ship_model;

		btCompoundShape* compound;
		btBoxShape* chassisShape;

		btDefaultVehicleRaycaster* m_vehicleRayCaster;

		btRaycastVehicle* m_vehicle;

		btRaycastVehicle::btVehicleTuning m_tuning;

		btCylinderShapeX* m_wheelShape;

		btRigidBody* m_carChassis;

		btCollisionShape* turretShape;

		btRigidBody* turretBody;

		btHingeConstraint* hinge;

		btVector3* GetPosition();

		WorldObject* compound_motion_state;
		WorldObject* turret_motion_state;


		float zworldpos;


		ID3D11ShaderResourceView* m_Texture;


			private:
				std::shared_ptr<DX::DeviceResources> m_deviceResources;

	};

}