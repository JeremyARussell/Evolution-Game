#ifndef GROUND_H
#define GROUND_H

#include "Box2D\Box2D.h"

#pragma once
class Ground
{
public:
	Ground(b2World *m_world, b2Vec2 pa, b2Vec2 pb);
	~Ground(void);

	b2Vec2 posA, posB;

	b2Body* ground;

	void destroy();

private:

	b2World *myWorld;
	b2BodyDef groundBodyDef;
    b2PolygonShape  groundShape;
    b2FixtureDef groundFixDef;
	b2Fixture* groundFix;

};

#endif