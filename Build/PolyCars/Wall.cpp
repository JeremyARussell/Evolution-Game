#include "Wall.h"
#include "Build\PolyCars\collision_enums.h"

Wall::Wall(b2World *m_world, b2Vec2 pa, b2Vec2 pb)
{

	myWorld = m_world;

	posA = pa;
	posB = pb;

	//void buildWallPiece(b2Vec2 pa, b2Vec2 pb, b2Body* world) 

	b2Vec2 _bwpTva[4];
	float32 _bwpTslope = (pb.y - pa.y) / (pb.x - pa.x);

	if(_bwpTslope >= 1 | _bwpTslope < -1.0) {//Mostly vertical
		//A is lower than B
		if (pa.y < pb.y)
		{
			_bwpTva[0].Set(pb.x - 2.5f, pb.y);
			_bwpTva[1].Set(pa.x - 2.5f, pa.y);
			_bwpTva[2].Set(pa.x + 2.5f, pa.y); 
			_bwpTva[3].Set(pb.x + 2.5f, pb.y);   
		} else {
			_bwpTva[0].Set(pa.x - 2.5f, pa.y);
			_bwpTva[1].Set(pb.x - 2.5f, pb.y);
			_bwpTva[2].Set(pb.x + 2.5f, pb.y); 
			_bwpTva[3].Set(pa.x + 2.5f, pa.y);   
		}

	} else {//Mostly horizontal
		//A is to the left of B
		if (pa.x < pb.x)
		{
			_bwpTva[0].Set(pa.x, pa.y + 2.5f);
			_bwpTva[1].Set(pa.x, pa.y - 2.5f);
			_bwpTva[2].Set(pb.x, pb.y - 2.5f);
			_bwpTva[3].Set(pb.x, pb.y + 2.5f);
		} else {//B is to the left of A
			_bwpTva[0].Set(pb.x, pb.y + 2.5f);
			_bwpTva[1].Set(pb.x, pb.y - 2.5f);
			_bwpTva[2].Set(pa.x, pa.y - 2.5f);
			_bwpTva[3].Set(pa.x, pa.y + 2.5f);
		}
	}

	wallShape.Set(_bwpTva, 4);

	b2FixtureDef wallFixDef;
	wallFixDef.shape = &wallShape;
	wallFixDef.density = 0.0f;
	wallFixDef.filter.categoryBits = WALL;

	wall = myWorld->CreateBody(&wallBodyDef);
	wallFix = wall->CreateFixture(&wallFixDef);
	wall->SetUserData( this );

}

void Wall::destroy() {
	myWorld->DestroyBody(wall);
}

Wall::~Wall(void)
{
}
