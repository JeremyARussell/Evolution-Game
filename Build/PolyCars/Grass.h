#ifndef GRASS_H
#define GRASS_H	
#pragma once

using namespace std;

#include <vector>
#include <ctime>
#include "Box2D\Box2D.h"

class Grass {
public:

	////
	void step();
	void grow(int);
	int bitten(int);

	b2Body* stalk;

	b2World *myWorld;
	bool beingEaten;

	Grass(b2World *m_world, float32 x,float32 y);
	void die();
	~Grass(void);

private:
	void prep();
	void seed(b2World*);

	//Grass class function variables
	int second;//To track time

	//Genes + States
	float32 maxEn;		//Gene
	int en;			   //State
	float32 growthPoint;//Gene
	float32 maxHp;		//Gene
	int hp;				   //State

	float32 xp;
	float32 yp;

	//Physics stuff
	b2BodyDef m_stalkBody;
    b2PolygonShape stalkShape;
    b2FixtureDef stalkFixture;
	b2Fixture* stalkFix;

};

#endif