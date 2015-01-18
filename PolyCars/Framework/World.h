/*
* Copyright (c) 2006-2009 Erin Catto http://www.box2d.org
*
* This software is provided 'as-is', without any express or implied
* warranty.  In no event will the authors be held liable for any damages
* arising from the use of this software.
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*/

#ifndef WORLD_H
#define WORLD_H

#include <Box2D/Box2D.h>
#include "Render.h"
#include "Build\PolyCars\Wheeler.h"

#include <cstdlib>

class World;
struct Settings;

typedef World* WorldCreateFcn();

#define	RAND_LIMIT	32767

/// Random number in range [-1,1]
inline float32 RandomFloat() {
	float32 r = (float32)(std::rand() & (RAND_LIMIT));
	r /= RAND_LIMIT;
	r = 2.0f * r - 1.0f;
	return r;
}

/// Random floating point number in range [lo, hi]
inline float32 RandomFloat(float32 lo, float32 hi) {
	float32 r = (float32)(std::rand() & (RAND_LIMIT));
	r /= RAND_LIMIT;
	r = (hi - lo) * r + lo;
	return r;
}

/// World settings. Some can be controlled in the GUI.
struct Settings {
	Settings() :
		viewCenter(0.0f, 20.0f),
		//zoomLevel(3.5f),
		hz(60.0f),
		grassSpawnRate(5),
		velocityIterations(8),
		positionIterations(3),
		drawShapes(1),
		drawJoints(0),
		drawAABBs(0),
		drawPairs(0),
		drawContactPoints(0),
		drawContactNormals(0),
		drawContactForces(0),
		drawFrictionForces(0),
		drawCOMs(0),
		drawStats(0),
		drawProfile(0),
		enableWarmStarting(1),
		enableContinuous(1),
		enableSubStepping(0),
		pause(0),
		singleStep(0),
		followCreature(0)
		{}

	b2Vec2 viewCenter;
	//float32 zoomLevel;
	float32 hz;
	int grassSpawnRate;
	int32 velocityIterations;
	int32 positionIterations;
	int32 drawShapes;
	int32 drawJoints;
	int32 drawAABBs;
	int32 drawPairs;
	int32 drawContactPoints;
	int32 drawContactNormals;
	int32 drawContactForces;
	int32 drawFrictionForces;
	int32 drawCOMs;
	int32 drawStats;
	int32 drawProfile;
	int32 enableWarmStarting;
	int32 enableContinuous;
	int32 enableSubStepping;
	int32 pause;
	int32 singleStep;
	int32 followCreature;
};

struct WorldEntry {
	const char *name;
	WorldCreateFcn *createFcn;
	//vector<Wheeler> worldWheelers;
};

extern WorldEntry g_worldEntries[];
// This is called when a joint in the world is implicitly destroyed
// because an attached body is destroyed. This gives us a chance to
// nullify the mouse joint.
class DestructionListener : public b2DestructionListener {
public:
	void SayGoodbye(b2Fixture* fixture) { B2_NOT_USED(fixture); }
	void SayGoodbye(b2Joint* joint);

	World* world;
};

const int32 k_maxContactPoints = 2048;

struct ContactPoint {
	b2Fixture* fixtureA;
	b2Fixture* fixtureB;
	b2Vec2 normal;
	b2Vec2 position;
	b2PointState state;
};

class World : public b2ContactListener {
public:

	virtual void nextWheeler() = 0;
	virtual void previusWheeler() = 0;
	virtual void destroyCreature() = 0;
	virtual void saveWorld() = 0;
	virtual void loadWorld() = 0;
	virtual void exportCreature() = 0;
	virtual void importCreature() = 0;

	Wheeler* activeWheeler;

	World();
	virtual ~World();

	void SetTextLine(int32 line) { m_textLine = line; }
    void DrawTitle(int x, int y, const char *string);
	virtual void Step(Settings* settings);
	virtual void Keyboard(unsigned char key) { B2_NOT_USED(key); }
	virtual void KeyboardUp(unsigned char key) { B2_NOT_USED(key); }
	void ShiftMouseDown(const b2Vec2& p);
	virtual void MouseDown(const b2Vec2& p);
	virtual void MouseUp(const b2Vec2& p);
	void MouseMove(const b2Vec2& p);
	
	// Let derived worlds know that a joint was destroyed.
	virtual void JointDestroyed(b2Joint* joint) { B2_NOT_USED(joint); }

	// Callbacks for derived classes.
	virtual void BeginContact(b2Contact* contact) { B2_NOT_USED(contact); }
	virtual void EndContact(b2Contact* contact) { B2_NOT_USED(contact); }
	virtual void PreSolve(b2Contact* contact, const b2Manifold* oldManifold);
	virtual void PostSolve(const b2Contact* contact, const b2ContactImpulse* impulse) {
		B2_NOT_USED(contact);
		B2_NOT_USED(impulse);
	}

protected:
	friend class DestructionListener;
	friend class BoundaryListener;
	friend class ContactListener;

	b2Body* m_groundBody;
	b2AABB m_worldAABB;
	ContactPoint m_points[k_maxContactPoints];
	int32 m_pointCount;
	DestructionListener m_destructionListener;
	DebugDraw m_debugDraw;
	int32 m_textLine;
	b2World* m_world;
	b2MouseJoint* m_mouseJoint;
	b2Vec2 m_mouseWorld;
	int32 m_stepCount;

	b2Profile m_maxProfile;
	b2Profile m_totalProfile;
};

#endif
