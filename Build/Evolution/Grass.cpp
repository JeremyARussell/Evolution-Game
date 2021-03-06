#include "Grass.h"
#include "Build\Evolution\collision_enums.h"

//Use collision of seeds to find the ground anchor point to grow the new grass.

Grass::Grass(b2World *m_world, float32 x,float32 y, int hp) {
	prep(hp);

	myWorld = m_world;

	xp = x;
	yp = y;

    m_stalkBody.position.Set(xp, yp);
    m_stalkBody.angle = 0;
	m_stalkBody.type = b2_staticBody;

	stalkShape.SetAsBox(0.4f, 0.2f);//0.2 per hp

	stalkFixture.filter.categoryBits = GRASS;
	stalkFixture.filter.maskBits = GRASS | WHEELER;//Wheeler is really just the sensor, the parts are non_int
	//stalkFixture.isSensor = true;
    stalkFixture.shape = &stalkShape;
    stalkFixture.density = 2.0f;
    stalkFixture.friction = 10.0f;
    stalkFixture.restitution = 0.05f;

    stalk = myWorld->CreateBody(&m_stalkBody);
	stalkFix = stalk->CreateFixture(&stalkFixture);
	stalk->SetUserData( this );


	//////
    m_stalkSensorBody.position.Set(xp, yp);
    m_stalkSensorBody.angle = 0;
	m_stalkSensorBody.type = b2_dynamicBody;
	m_stalkSensorBody.gravityScale = 0;

	stalkSensorShape.SetAsBox(0.4f, 0.2f);//0.2 per hp

	stalkSensorFixture.filter.categoryBits = ROOT;
	stalkSensorFixture.filter.maskBits = ROOT;
	stalkSensorFixture.isSensor = true;
    stalkSensorFixture.shape = &stalkShape;
    stalkSensorFixture.density = 2.0f;
    stalkSensorFixture.friction = 10.0f;
    stalkSensorFixture.restitution = 0.05f;

    stalkSensor = myWorld->CreateBody(&m_stalkSensorBody);
	stalkSensorFix = stalkSensor->CreateFixture(&stalkSensorFixture);
	stalkSensor->SetUserData( this );

	testweld.bodyA = stalk;
	testweld.bodyB = stalkSensor;

	testweld.localAnchorA = b2Vec2(0.0, 0.0);
	testweld.localAnchorB = b2Vec2(0.0, 0.0);

	testweldJoint = (b2WeldJoint*)myWorld->CreateJoint(&testweld);

}

void Grass::prep(int _hp) {//Later 0'd out and then a randomizer function will be born... maybe
	second = 0;
	maxEn = 40.0f;
	en = 1;
	growthPoint = 0.0f;
	maxHp = 15.0f;
	hp = _hp;
	seeding = false;
	beingEaten = false;
	crowded = false;
	crowdTimer = 0;

	fresh = true;
}

void Grass::step() {
	if (second == 2) fresh = false;
	if (second < 120) {
		second++;
	} else {//Two seconds went by
		grow(hp);

		if (en > (int)growthPoint && hp != (int)maxHp && !crowded) {
			hp++;
		}

		if (en > (int)(maxEn - 1) && !crowded) {
			seed();
		}

		if (crowded) {
			if (crowdTimer < 11) {
				
				crowdTimer++;
			} else {
				crowded = false;
				crowdTimer = 0;
			}
		} else {
			en++;
		}

		
		second = 0;
	}
}

void Grass::grow(int) {
	fresh = true;
	stalkSensor->DestroyFixture(stalkSensorFix);

	//stalkSensor->SetTransform(b2Vec2(xp, yp + (0.2f * hp)), 0);
	//stalkSensorShape.SetAsBox(0.4f, 0.2f + (0.2f * hp));//0.2 per hp

	stalkSensor->SetTransform(b2Vec2(xp, yp ), 0);
	stalkSensorShape.SetAsBox(0.4f + (0.1f * hp), 0.2f + (0.03f * hp));//0.2 per hp


    stalkSensorFixture.shape = &stalkSensorShape;

	stalkSensorFix = stalkSensor->CreateFixture(&stalkSensorFixture);


	stalk->DestroyFixture(stalkFix);

	stalk->SetTransform(b2Vec2(xp, yp + (0.2f * hp)), 0);

	stalkShape.SetAsBox(0.4f, 0.2f + (0.2f * hp));//0.2 per hp
    stalkFixture.shape = &stalkShape;

	fresh = true;
	stalkFix = stalk->CreateFixture(&stalkFixture);

	myWorld->DestroyJoint(testweldJoint);

	//testweld.bodyA = stalk;
	//testweld.bodyB = stalkSensor;
	testweld.localAnchorA = b2Vec2(0.0, 0.0 - (0.22f * hp));
	//testweld.localAnchorB = b2Vec2(0.0, 0.0);

	testweldJoint = (b2WeldJoint*)myWorld->CreateJoint(&testweld);

}

int Grass::bitten(int x) {
	//Take hp to remove

	//return how much was given, not always the hp removed if the grass's hp was less than 
	//what was removed.

	int amountBitten = 0;

	

	if (hp >= x) {
		hp = hp - x;
		amountBitten = x;
	}
	else {
		amountBitten = hp;
		hp = 0;
	}

	fresh = true;

	if (hp < 3)
	{
		en = 0;
	}

	//grow(hp);
	//If the grass returns less HP than the Wheeler bit, it's added to the dead list. :)
	return amountBitten;
}

int Grass::HP() {
	return hp;
}

void Grass::seed() {
	//Generate a seed and shoot it out. 
	//- --- Seeds generate with an angle, a force, and a parents gene.
	en = en = 1;
	hp = hp = 1;
	grow(hp);
	seeding = true;
}

void Grass::die() {
	//myWorld->DestroyBody(stalk);
	//myWorld->DestroyBody(stalkSensor);
	en = en = 1;
	hp = hp = 1;
	grow(hp);
}

Grass::~Grass(void) {

}
