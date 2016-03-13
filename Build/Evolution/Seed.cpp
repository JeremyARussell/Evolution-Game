#include "Seed.h"
#include "Build\Evolution\collision_enums.h"



Seed::Seed(b2World* world, b2Vec2 pos, b2Vec2 impulse) {

	myWorld = world;
	timetoSeed = false;

    seedBodyDef.position.Set(pos.x, pos.y);
    seedBodyDef.angle = 0;
	seedBodyDef.type = b2_dynamicBody;

	seedShape.m_radius = 0.2f;

	seedFixDef.filter.categoryBits = SEED;
	seedFixDef.filter.maskBits = GROUND | WALL | PERM_GROUND | PERM_WALL | WHEELER_BODY | GRASS_SPAWNER_BASE | SEED;
    seedFixDef.shape = &seedShape;
    seedFixDef.density = 2.0f;
    seedFixDef.friction = 10.0f;
    seedFixDef.restitution = 0.05f;

    seed = myWorld->CreateBody(&seedBodyDef);
	seed->CreateFixture(&seedFixDef);
	seed->SetUserData( this );

	seed->ApplyLinearImpulse( impulse, seed->GetWorldCenter() );

}


Seed::~Seed(void) {

}

void Seed::destroy() {
	myWorld->DestroyBody(seed);
}
