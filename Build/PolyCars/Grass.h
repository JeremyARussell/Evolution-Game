#ifndef GRASS_H
#define GRASS_H	
#pragma once

using namespace std;

#include <vector>
#include <ctime>
#include "Box2D\Box2D.h"

class Grass {
public:
	b2Body* stalk;

	b2World *myWorld;
	bool beingEaten;

	Grass(b2World *m_world, float32 x,float32 y);
	void die();
	~Grass(void);
};

#endif