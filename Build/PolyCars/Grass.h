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
	b2Body* stalkSensor;

	b2World *myWorld;
	bool beingEaten;

	bool seeding;
	bool crowded;
	bool fresh;

	Grass(b2World *m_world, float32 x,float32 y);
	void die();
	~Grass(void);

protected:
    float32 randomNumber(float32 low = 0, float32 high = 1) {
		float retNum;
		retNum = low + (float)rand() / ((float)RAND_MAX / (high-low));
        return retNum;
    }

	void prep();
	void seed();

	//Grass class function variables
	int second;//To track time

	//Genes + States
	float32 maxEn;		//Gene
	int en;					//State
	float32 growthPoint;//Gene
	float32 maxHp;		//Gene
	int hp;					//State

	int crowdTimer;
	float32 xp;
	float32 yp;

	//Physics stuff
	b2BodyDef m_stalkBody;
    b2PolygonShape stalkShape;
    b2FixtureDef stalkFixture;
	b2Fixture* stalkFix;

	b2BodyDef m_stalkSensorBody;
    b2PolygonShape stalkSensorShape;
    b2FixtureDef stalkSensorFixture;
	b2Fixture* stalkSensorFix;

	b2WeldJointDef testweld;

};

#endif