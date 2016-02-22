#ifndef SEED_H
#define SEED_H

#include "Box2D\Box2D.h"


#pragma once
class Seed {
public:
	Seed(b2World*, b2Vec2, b2Vec2);
	~Seed(void);

	void destroy();

	bool  timetoSeed;

	b2Body* seed;

private:


	b2World *myWorld;
	b2BodyDef seedBodyDef;
    b2CircleShape  seedShape;
    b2FixtureDef seedFixDef;

};

#endif