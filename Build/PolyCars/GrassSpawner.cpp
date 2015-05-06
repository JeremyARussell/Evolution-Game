#include "GrassSpawner.h"
#include "Build\PolyCars\collision_enums.h"


GrassSpawner::GrassSpawner(b2World *m_world, float32 x,float32 y) : Grass(m_world, x,y) {

	float32 spx; float32 spy;
	spx = x; spy = y;

	b2BodyDef bd;
	b2Body* spawnerS1 = m_world->CreateBody(&bd);

	b2Vec2 vsS1[3];
	vsS1[0].Set(spx, spy);
	vsS1[1].Set(spx - 1, spy - 2);
	vsS1[2].Set(spx + 1, spy - 2);

	b2PolygonShape loopS1;
	loopS1.Set(vsS1, 3);

	b2FixtureDef fdS1;//This is the triangular base of the spawner...
	fdS1.shape = &loopS1;
	fdS1.density = 0.0f;
	fdS1.filter.categoryBits = GRASS_SPAWNER_BASE;

	spawnerS1->CreateFixture(&fdS1);
}

GrassSpawner::~GrassSpawner(void)
{
}
