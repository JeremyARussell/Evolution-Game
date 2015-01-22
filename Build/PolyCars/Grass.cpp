#include "Grass.h"

//Use collision of seeds to find the ground anchor point to grow the new grass.

Grass::Grass(b2World *m_world, float32 x,float32 y) {
	prep();

	myWorld = m_world;

	beingEaten = false;

	xp = x;
	yp = y;

    m_stalkBody.position.Set(xp, yp);
    m_stalkBody.angle = 0;
	m_stalkBody.type = b2_staticBody;

	stalkShape.SetAsBox(0.4f, 0.2f);//0.2 per hp

	stalkFixture.filter.categoryBits = 0x0002;
	stalkFixture.filter.maskBits = 0x0004;
	stalkFixture.isSensor = true;
    stalkFixture.shape = &stalkShape;
    stalkFixture.density = 2.0f;
    stalkFixture.friction = 10.0f;
    stalkFixture.restitution = 0.05f;

    stalk = myWorld->CreateBody(&m_stalkBody);
	stalkFix = stalk->CreateFixture(&stalkFixture);
	stalk->SetUserData( this );

}

void Grass::prep() {//Later 0'd out and then a randomizer function will be born... maybe
	second = 0;
	maxEn = 20.0f;
	en = 1;
	growthPoint = 5.0f;
	maxHp = 10.0f;
	hp = 1;
}

void Grass::step() {
	if (second < 60) {
		second++;
	} else {//A second went by

		if (en > (int)growthPoint && hp != (int)maxHp) {
			grow(hp);
			hp++;
		}

		if (en > (int)(maxEn - 1)) {
			//seed(myWorld);
		}

		en++;
		second = 0;
	}
}

void Grass::grow(int) {
	stalk->DestroyFixture(stalkFix);

	stalk->SetTransform(b2Vec2(xp, yp + (0.2f * hp)), 0);

	stalkShape.SetAsBox(0.4f, 0.2f + (0.2f * hp));//0.2 per hp
    stalkFixture.shape = &stalkShape;

	stalkFix = stalk->CreateFixture(&stalkFixture);
}

int Grass::bitten(int) {
	//Take hp to remove

	//return how much was given, not always the hp removed if the grass's hp was less than 
	//what was removed.



	///Eruekeka alsk nalkdjchnzsldkjhvbs kj
	///If the grass returns less HP than the Wheeler bit, it's added to the dead list. :)
	return 1;
}

void Grass::seed(b2World*) {
	//Generate a seed and shoot it out. 
	//- --- Seeds generate with an angle, a force, and a parents gene.
}

void Grass::die() {
	myWorld->DestroyBody(stalk);
}

Grass::~Grass(void) {

}
