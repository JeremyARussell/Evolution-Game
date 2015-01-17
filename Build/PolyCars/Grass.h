#pragma once

using namespace std;

#include <vector>
#include <ctime>
#include "Box2D\Box2D.h"

class Grass
{
public:

	//Just a rectangle, doesn't even need to grow yet, shoots out a circle seed.

	b2Body* stalk;

	b2World *myWorld;
	bool beingEaten;

	Grass(b2World *m_world, float32 x,float32 y);
	//Grass(Grass parent, b2World *m_world, float32 x,float32 y);
	void die();
	~Grass(void);
};

