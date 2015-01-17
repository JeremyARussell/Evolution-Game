#pragma once

using namespace std;

#include <vector>
#include <ctime>
#include <Box2D\Box2D.h>
#include "freeglut/freeglut.h"

class Wheeler
{
public:

	//GA relates variables
	float32 spokeAngles[8];
	float32 spokeLengths[8];
	float32 hasWheel[8];
	float32 axleAngle[8];
	float32 wheelRadius[8];

    float32 MIN_WHEEL;
    float32 MAX_WHEEL;
    float32 MIN_CART;
    float32 MAX_CART;
	float32 baseSpringForce;

	float32 spinDirection;
	b2RevoluteJoint* motor[8];

	/////
	b2Body* cart;//Actual Wheeler body

	//Below in Mountain
	b2Body* wheel[8];
	b2Body* axle[8];

	//For sensing
	bool nearWall;//TODO - Give a public handler for a private variable
	int nearWallCounter;
	
	//Life and Death
	int health;
	int healthDownCounter;
	int minutesToLive;

	//Cart user data used to

	//REPRODUCTION
	bool needsToReproduce;
	int reproductionCounter;
	//int timesToEat;



	b2World *myWorld;

	void render();

	Wheeler(b2World *m_world, float32 x,float32 y);
	Wheeler(Wheeler parent, b2World *m_world, float32 x,float32 y);
	Wheeler(b2World *m_world, vector<float32> genes);
	~Wheeler(void);
	
    float32 randomNumber(float32 low = 0, float32 high = 1) {
		float retNum;
		retNum = low + (float)rand() / ((float)RAND_MAX / (high-low));
        return retNum;
    }

    int32 randomNumberInt(float32 param1 = 0, float32 param2 = 1) {
        return (int32)floor(randomNumber(param1, (param2 + 1)));
	}

	void turnAround ();
	void die();

private:


	void generateFreshChromosome();
	void generateChildChromosome(Wheeler parent);
	void buildWheeler(float32 x,float32 y);
	void loadWheeler(vector<float32> genes);


	//Above in Mountain
	int32 wheelOn[8];//For storing if we need a wheel during build time
	b2PrismaticJoint* spring[8];
	b2Fixture* axleFixture[8];
	b2Fixture* cartFixture[8];

		
	float32 torque;
	int32 wheelsOn;
	int32 maxWheels;


	//From scratch variables
	float32 m_wheelProb;
	const static int32 VECTOR_NUM = 8;

	//REFACTOR
	float32 totalAngleValue;


};

