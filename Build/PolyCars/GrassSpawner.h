#pragma once
#include "grass.h"
class GrassSpawner : public Grass {
public:
	GrassSpawner(b2World *m_world, float32 x,float32 y);
	~GrassSpawner(void);

	void destroy();
	float32 spx; float32 spy;

	//void step();

private:
	b2Body* spawnerS1;
	//void prep();

};

