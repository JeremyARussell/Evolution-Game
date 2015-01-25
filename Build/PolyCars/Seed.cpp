#include "Seed.h"


Seed::Seed(b2World* world, b2Vec2 pos, b2Vec2 impulse) {

	myWorld = world;
	timetoSeed = false;

    seedBodyDef.position.Set(pos.x, pos.y);
    seedBodyDef.angle = 0;
	seedBodyDef.type = b2_dynamicBody;

	seedShape.m_radius = 0.2f;

	seedFixDef.filter.categoryBits = 0x0001;
	seedFixDef.filter.maskBits = 0x0020;
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
