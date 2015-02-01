#pragma once
#include "grass.h"
class GrassSpawner : public Grass {
public:
	GrassSpawner(b2World *m_world, float32 x,float32 y);
	~GrassSpawner(void);

	//void step();

private:

	//void prep();

};

