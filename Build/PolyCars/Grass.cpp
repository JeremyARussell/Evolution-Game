#include "Grass.h"


//Use collision of seeds to find the ground anchor point to grow the new grass.



Grass::Grass(b2World *m_world, float32 x,float32 y)
{
	myWorld = m_world;

	beingEaten = false;

	b2BodyDef m_stalkBody;
    m_stalkBody.position.Set(x, y);
    m_stalkBody.angle = 0;
	m_stalkBody.type = b2_staticBody;

    b2PolygonShape stalkShape;
	stalkShape.SetAsBox(0.4f, 1.6f);

    b2FixtureDef stalkFixture;
	stalkFixture.filter.categoryBits = 0x0002;
	stalkFixture.filter.maskBits = 0x0004;
	stalkFixture.isSensor = true;

    stalkFixture.shape = &stalkShape;
    stalkFixture.density = 2.0f;
    stalkFixture.friction = 10.0f;
    stalkFixture.restitution = 0.05f;
    //stalkFixture.filter.groupIndex = -1;

    stalk = myWorld->CreateBody(&m_stalkBody);
	stalk->CreateFixture(&stalkFixture);
	stalk->SetUserData( this );

}

void Grass::die()
{
	myWorld->DestroyBody(stalk);
}

Grass::~Grass(void)
{
	//myWorld->DestroyBody(stalk);
}
