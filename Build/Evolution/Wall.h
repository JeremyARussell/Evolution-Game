#ifndef WALL_H
#define WALL_H

#include "Box2D\Box2D.h"

#pragma once
class Wall
{
public:
	Wall(b2World *m_world, b2Vec2 pa, b2Vec2 pb);
	~Wall(void);

	b2Vec2 posA, posB;

	b2Body* wall;

	void destroy();

private:

	b2World *myWorld;
	b2BodyDef wallBodyDef;
    b2PolygonShape  wallShape;
    b2FixtureDef wallFixDef;
	b2Fixture* wallFix;

};

#endif