#pragma once

#include "../Bullet/src/btBulletDynamicsCommon.h"
#include "Content/ShaderStructures.h"

namespace Simple3DGame
{

	class Physics
	{
	public:
		Physics(void);
		~Physics(void);

		void Update(float timeDelta);
		btRigidBody* AddPhysicalObject(btCollisionShape* collisionShape, btMotionState* motionState, btScalar mass, const btVector3& inertia, float rest, float fric, int* ob_id, ObjInfo* info);

		void RemoveAllObjects(int offset);

		int cur_unique_id;

		void CheckForCollisions();

		void ExplodeArea(float x, float y, float z, float radius, float strength);
		//	void Pause();
		//	void Continue();

		int bCollideTest;
		float high_impulse;

		//private:
		Physics(const Physics&) {}
		Physics& operator=(const Physics&) { return *this; }

		std::unique_ptr<btBroadphaseInterface> m_broadphase;
		std::unique_ptr<btDefaultCollisionConfiguration> m_collisionConfiguration;
		std::unique_ptr<btCollisionDispatcher> m_dispatcher;
		std::unique_ptr<btSequentialImpulseConstraintSolver> m_solver;
		std::unique_ptr<btDiscreteDynamicsWorld> m_dynamicsWorld;

		std::vector<std::unique_ptr<btCollisionShape>> m_shapes;
		std::vector<std::unique_ptr<btRigidBody>> m_rigidBodies;

	};

}