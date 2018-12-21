#include "pch.h"
#include "Physics.h"

#include <algorithm>

using namespace Simple3DGame;
using namespace std;

// Physics Class
Physics::Physics(void) :
	m_broadphase(new btDbvtBroadphase()),
	m_collisionConfiguration(new btDefaultCollisionConfiguration()),
	m_solver(new btSequentialImpulseConstraintSolver)
{
	m_dispatcher = std::unique_ptr<btCollisionDispatcher>(new btCollisionDispatcher(m_collisionConfiguration.get()));

	m_dynamicsWorld = std::unique_ptr<btDiscreteDynamicsWorld>(new btDiscreteDynamicsWorld(m_dispatcher.get(), m_broadphase.get(), m_solver.get(), m_collisionConfiguration.get()));

	m_dynamicsWorld->setGravity(btVector3(0.0f, -18.0f, 0.0f));

	//sm_dynamicsWorld->set
	//m_dynamicsWorld->
	cur_unique_id = 0;

	//m_broadphase.
}

Physics::~Physics(void)
{
//	for_each(begin(m_rigidBodies), end(m_rigidBodies), [&](const unique_ptr<btRigidBody>& rigidBody)
//	{
//		m_dynamicsWorld->removeRigidBody(rigidBody.get());
//		delete rigidBody->getMotionState();
//	});

//	m_rigidBodies.clear();
}

void Physics::Update(float timeDelta)
{
	high_impulse = 0;
	bCollideTest = 0;
	//m_dynamicsWorld->stepSimulation(1/20.f,10);
	m_dynamicsWorld->stepSimulation(1.5f*timeDelta,  5/*,1/1256.0f*/);
	//CheckForCollisions();

}

void Physics::ExplodeArea(float x, float y, float z, float radius, float strength)
{


}

void Physics::RemoveAllObjects(int offset)
{
	int i=0;

	/*
	for_each(begin(m_rigidBodies), end(m_rigidBodies), [&](const unique_ptr<btRigidBody>& rigidBody)
	{
		if( i>offset)
		{
			m_dynamicsWorld->get
			m_dynamicsWorld->removeRigidBody(rigidBody.get());
			delete rigidBody->getMotionState();
		}
	});
	*/
	for (i = m_dynamicsWorld->getNumCollisionObjects() - 1; i >= offset; i--)
	{


		btCollisionObject* obj = m_dynamicsWorld->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body && body->getMotionState())
		{
			delete body->getMotionState();
		}
		m_dynamicsWorld->removeCollisionObject(obj);
		delete obj;

		m_rigidBodies.at(i).release();
		//body
		//delete body;

		/*
		m_dynamicsWorld->removeCollisionObject(obj);
		delete obj;
		m_rigidBodies.at(i).release();
		*/
	}
	//m_dynamicsWorld->
	//m_rigidBodies.clear();
	/*
	for_each( begin(m_rigidBodies), end(m_rigidBodies), [&]( const unique_ptr<btRigidBody>& rigidBody )
	{
	m_dynamicsWorld->removeRigidBody(rigidBody.get());
	delete rigidBody->getMotionState();
	} );
	*/
	//m_rigidBodies.clear();

	cur_unique_id = 0;
}


btRigidBody* Physics::AddPhysicalObject(btCollisionShape* collisionShape, btMotionState* motionState, btScalar mass, const btVector3& inertia, float rest, float fric, int* ob_id, ObjInfo* info)
{
	btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(mass, motionState, collisionShape, inertia);
	groundRigidBodyCI.m_restitution = rest;
	groundRigidBodyCI.m_friction = fric;

	auto groundRigidBody = new btRigidBody(groundRigidBodyCI);

	groundRigidBody->item_id = cur_unique_id++;

	m_dynamicsWorld->addRigidBody(groundRigidBody, info->group, info->mask);

	//m_rigidBodies.

	m_shapes.push_back(std::unique_ptr<btCollisionShape>(collisionShape));
	m_rigidBodies.push_back(std::unique_ptr<btRigidBody>(groundRigidBody));

	return groundRigidBody;
}


void Physics::CheckForCollisions()
{
	int numManifolds = m_dynamicsWorld->getDispatcher()->getNumManifolds();
	for (int i = 0; i<numManifolds; i++)
	{
		btPersistentManifold* contactManifold = m_dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
		btCollisionObject* obA = (btCollisionObject*)(contactManifold->getBody0());
		btCollisionObject* obB = (btCollisionObject*)(contactManifold->getBody1());

		int numContacts = contactManifold->getNumContacts();
		for (int j = 0; j<numContacts; j++)
		{
			btManifoldPoint& pt = contactManifold->getContactPoint(j);
			if (pt.getDistance()<0.f)
			{
				bCollideTest++;
				if (pt.getAppliedImpulse()>high_impulse)
				{
					high_impulse = pt.getAppliedImpulse();
				}

				const btVector3& ptA = pt.getPositionWorldOnA();
				const btVector3& ptB = pt.getPositionWorldOnB();
				const btVector3& normalOnB = pt.m_normalWorldOnB;
			}
		}
	}
}