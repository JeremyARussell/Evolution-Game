#ifndef ABIOGENESIS_H
#define ABIOGENESIS_H
#pragma once


enum _AbioCategories {
    AWALL			=  1,
    WATER_CORE				=  2,
    WATER_SHELL			=  4,
    PHOBIC				=  8,
    PHILLIC	=  16,
//    SEED				=  32,
//    GRASS				=  64,
//    ROOT				=  128,
//    WHEELER_BODY		=  256,
//    WHEELER				=  512	 
//    xxxx	 =  1024,
//    xxxx	 =  2048,
//    xxxx	 =  4096,
//    xxxx	 =  8192,
//    xxxx	 =  16384,
//    xxxx	 =  32768,

};


class aWheelerContactListener : public b2ContactListener {
	void BeginContact(b2Contact* contact) {

		b2Fixture* fixtureA = contact->GetFixtureA();
		b2Fixture* fixtureB = contact->GetFixtureB();

		if (fixtureA->GetFilterData().categoryBits == WATER_SHELL && 
			fixtureB->GetFilterData().categoryBits == WATER_SHELL) {

				b2Vec2 testPointA = fixtureA->GetBody()->GetPosition();
				b2Vec2 testPointB = fixtureB->GetBody()->GetPosition();

				fixtureA->GetBody()->ApplyLinearImpulse( 
					b2Vec2((testPointA.x - testPointB.x) * 2, (testPointA.y - testPointB.y) * 2), 
					fixtureA->GetBody()->GetWorldCenter() );

				fixtureB->GetBody()->ApplyLinearImpulse( 
					b2Vec2((testPointB.x - testPointA.x) * 2, (testPointB.y - testPointA.y) * 2), 
					fixtureB->GetBody()->GetWorldCenter() );
		}

		if (fixtureA->GetFilterData().categoryBits == PHOBIC && 
			 fixtureB->GetFilterData().categoryBits == WATER_SHELL) {

				b2Vec2 testPointA = fixtureA->GetBody()->GetPosition();
				b2Vec2 testPointB = fixtureB->GetBody()->GetPosition();

				fixtureA->GetBody()->ApplyLinearImpulse( 
					b2Vec2((testPointA.x - testPointB.x), (testPointA.y - testPointB.y)), 
					fixtureA->GetBody()->GetWorldCenter() );

				fixtureB->GetBody()->ApplyLinearImpulse( 
					b2Vec2((testPointB.x - testPointA.x), (testPointB.y - testPointA.y)), 
					fixtureB->GetBody()->GetWorldCenter() );
		}

		if (fixtureA->GetFilterData().categoryBits == WATER_SHELL && 
			fixtureB->GetFilterData().categoryBits == PHILLIC) {

				b2Vec2 testPointA = fixtureA->GetBody()->GetPosition();
				b2Vec2 testPointB = fixtureB->GetBody()->GetPosition();

				fixtureB->GetBody()->ApplyLinearImpulse( 
					b2Vec2((testPointA.x - testPointB.x), (testPointA.y - testPointB.y)), 
					fixtureB->GetBody()->GetWorldCenter() );

				fixtureA->GetBody()->ApplyLinearImpulse( 
					b2Vec2((testPointB.x - testPointA.x), (testPointB.y - testPointA.y)), 
					fixtureA->GetBody()->GetWorldCenter() );
		}



	}
  
    void EndContact(b2Contact* contact) {
		
    }
};

aWheelerContactListener athisWheelerContactListener;

class aQueryCallback : public b2QueryCallback {//This may or may not be needed, it exists in the World class.
public:
	aQueryCallback(const b2Vec2& point) {
		m_point = point;
		m_fixture = NULL;
	}

	bool ReportFixture(b2Fixture* fixture) {
		b2Body* body = fixture->GetBody();
		if (body->GetType() == b2_dynamicBody) {
			bool inside = fixture->TestPoint(m_point);
			if (inside) {
				m_fixture = fixture;
				return false;
			}
		}
		return true;
	}

	b2Vec2 m_point;
	b2Fixture* m_fixture;
};

class Abiogenesis : public World {
public:

	//These functions are tied to the GLUI GUI
	void nextWheeler() { }
	void previusWheeler() { }
	void destroyCreature() { }

	void saveWorld() { }
	void loadWorld() { }

	void exportCreature() { }
	void importCreature() { }

	b2Body* world;

	//Level building, and default creature placement go here.
    Abiogenesis() {

		activePower = GRAB;
		ma.x = 0.0f; ma.y = 0.0f;
		m_world->SetContactListener(&athisWheelerContactListener);

		top = 48;
		left = -48;
		bottom = -48;
		right = 48;

		#pragma region Boundaries
		{

			m_world->SetGravity(b2Vec2(0, 0));

			b2BodyDef bd;
			world = m_world->CreateBody(&bd);

			b2Vec2 _bgpTva[4];
			b2PolygonShape _bgpTps;
			b2FixtureDef _bgpTfd;

			//Bottom Bar
			_bgpTva[0].Set(-50, -48);
			_bgpTva[1].Set(-50, -50);
			_bgpTva[2].Set( 50, -50); 
			_bgpTva[3].Set( 50, -48);   

			_bgpTps.Set(_bgpTva, 4);

			_bgpTfd.shape = &_bgpTps;
			_bgpTfd.density = 0.0f;
			_bgpTfd.filter.categoryBits = AWALL;
			_bgpTfd.filter.maskBits = WATER_CORE | PHOBIC | PHILLIC;

			world->CreateFixture(&_bgpTfd);

			//Top Bar
			_bgpTva[0].Set(-50, 50);
			_bgpTva[1].Set(-50, 48);
			_bgpTva[2].Set( 50, 48); 
			_bgpTva[3].Set( 50, 50);   

			_bgpTps.Set(_bgpTva, 4);

			_bgpTfd.shape = &_bgpTps;
			_bgpTfd.density = 0.0f;
			_bgpTfd.filter.categoryBits = AWALL;
			_bgpTfd.filter.maskBits = WATER_CORE | PHOBIC | PHILLIC;

			world->CreateFixture(&_bgpTfd);

			//Left Bar
			_bgpTva[0].Set(-50,  50);
			_bgpTva[1].Set(-50, -50);
			_bgpTva[2].Set(-48, -50); 
			_bgpTva[3].Set(-48,  50);   

			_bgpTps.Set(_bgpTva, 4);

			_bgpTfd.shape = &_bgpTps;
			_bgpTfd.density = 0.0f;
			_bgpTfd.filter.categoryBits = AWALL;
			_bgpTfd.filter.maskBits = WATER_CORE | PHOBIC | PHILLIC;

			world->CreateFixture(&_bgpTfd);

			//Right Bar
			_bgpTva[0].Set( 48,  50);
			_bgpTva[1].Set( 48, -50);
			_bgpTva[2].Set( 50, -50); 
			_bgpTva[3].Set( 50,  50);   

			_bgpTps.Set(_bgpTva, 4);

			_bgpTfd.shape = &_bgpTps;
			_bgpTfd.density = 0.0f;
			_bgpTfd.filter.categoryBits = AWALL;
			_bgpTfd.filter.maskBits = WATER_CORE | PHOBIC | PHILLIC;

			world->CreateFixture(&_bgpTfd);
		}
		#pragma endregion

		#pragma region Water

		//Water

		b2Vec2 waterPos;

		//for 100
			//for 50+

		for (int ix = -47; ix < 48; ix++) {
			for (int iy = -47; iy < 48; iy++) {
				waterPos.Set(ix, iy);
				spawnWater(waterPos);
			}
		}

		#pragma endregion

		for (int ix = - 15; ix < 15; ix++) {
			for (int iy = -8; iy < 8; iy = iy + 3) {
				spawnPhLipid(b2Vec2( ix, iy));
			}
		}

	}
	
	void spawnWater(b2Vec2 pos) {

		b2BodyDef waterBodDef;
		waterBodDef.type = b2_dynamicBody;
		waterBodDef.position.Set(pos.x, pos.y);
		waterBodDef.angle = 0;

		b2Body* waterBod = m_world->CreateBody(&waterBodDef);

		b2CircleShape circleShape;
		b2FixtureDef waterFixtureDef;

		//Water Core
		circleShape.m_p.Set(0, 0);
		circleShape.m_radius = 0.25;
		waterFixtureDef.shape = &circleShape;
		waterFixtureDef.filter.categoryBits = WATER_CORE;
		waterFixtureDef.filter.maskBits = WATER_CORE | AWALL | PHOBIC | PHILLIC;

		waterBod->CreateFixture(&waterFixtureDef);

		//Water Shell
		circleShape.m_p.Set(0, 0);
		circleShape.m_radius = 0.4995;
		waterFixtureDef.shape = &circleShape;
		waterFixtureDef.isSensor = true;
		waterFixtureDef.filter.categoryBits = WATER_SHELL;
		waterFixtureDef.filter.maskBits = WATER_SHELL | WATER_CORE | AWALL | PHOBIC | PHILLIC;

		waterBod->CreateFixture(&waterFixtureDef);
	}

	void spawnPhLipid(b2Vec2 pos) {

		b2BodyDef waterBodDef;
		waterBodDef.type = b2_dynamicBody;
		waterBodDef.position.Set(pos.x, pos.y);
		waterBodDef.angle = 0;

		b2Body* waterBod = m_world->CreateBody(&waterBodDef);

		b2CircleShape phobicShape;
		b2PolygonShape phillicShape;
		b2FixtureDef lipidFixDef;

		lipidFixDef.density = 0.5;

		//Phillic
		phobicShape.m_p.Set(0, 0.5);
		phobicShape.m_radius = 0.38;
		lipidFixDef.shape = &phobicShape;
		lipidFixDef.filter.categoryBits = PHILLIC;
		lipidFixDef.filter.maskBits = WATER_SHELL | WATER_CORE | AWALL | PHOBIC | PHILLIC;

		waterBod->CreateFixture(&lipidFixDef);

		//Phobic
		phillicShape.SetAsBox(0.25, 0.5, b2Vec2(0, -0.5), 0);
		lipidFixDef.shape = &phillicShape;

		lipidFixDef.filter.categoryBits = PHOBIC;
		lipidFixDef.filter.maskBits = WATER_SHELL | WATER_CORE | AWALL | PHOBIC | PHILLIC;

		waterBod->CreateFixture(&lipidFixDef);

	}

	float32 clickZoomLevel;
	b2Vec2  clickViewCenter;

	b2Vec2 ma, mb;//Used to handle mouse position

	void Abiogenesis::MouseDown(const b2Vec2& p) {
										
		m_mouseWorld = p;
	
		if (m_mouseJoint != NULL) {
			return;
		}

		// Make a small box.
		b2AABB aabb;
		b2Vec2 d;
		d.Set(0.001f, 0.001f);
		aabb.lowerBound = p - d;
		aabb.upperBound = p + d;

		// Query the world for overlapping shapes.
		QueryCallback callback(p);
		m_world->QueryAABB(&callback, aabb);

		if (callback.m_fixture) {
			if (activePower == GRAB) {
				b2Body* body = callback.m_fixture->GetBody();
				b2MouseJointDef md;
				md.bodyA = m_groundBody;
				md.bodyB = body;
				md.target = p;
				md.maxForce = 1000.0f * body->GetMass();
				m_mouseJoint = (b2MouseJoint*)m_world->CreateJoint(&md);
				body->SetAwake(true);
			}
		}
	}

	void Abiogenesis::MouseUp(const b2Vec2& p) {
		World::MouseUp(p);
	}

	void Keyboard(unsigned char key) {
		switch (key) {
		case '1':
			break;
		}
	}

	void Step(Settings* settings) {

		if (clickZoomLevel != settings->zoomLevel) clickZoomLevel = settings->zoomLevel;
		if (clickViewCenter.x != settings->viewCenter.x &&
			clickViewCenter.y != settings->viewCenter.y) clickViewCenter = settings->viewCenter;
		//Used with the power HUD - Not sure if I'll need it though....//if (powerHUD.getActivePower() != activePower) activePower = powerHUD.getActivePower();

		//For tracking/following the active Wheeler
		if (activeWheeler != NULL && activeWheeler->health > -1 && settings->followCreature == 1) {
			settings->viewCenter.Set(activeWheeler->cart->GetPosition().x, activeWheeler->cart->GetPosition().y);
		}

		//Game text
		m_debugDraw.DrawString(10, m_textLine, "Welcome to the Abiogenesis Universe - See how nonliving matter can start to form proto-life.");
		m_textLine += 15;

		if (settings->pause) {
			//Used to show world stats, not sure what it'll show, if anything, now.
		}

		World::Step(settings);
	}

	static World* Create() {
		return new Abiogenesis;
	}

private:

	//Random number generation - I tried putting this stuff in it's own file, I failed :(
    float32 randomNumber(float32 low = 0, float32 high = 1) {
		float retNum;
		retNum = low + (float)rand() / ((float)RAND_MAX / (high-low));
        return retNum;
    }

	//Put vectors of all the "bio" pieces
		//Water list [ / ]
		//Lipid List [ ]
			//--> Amino Acids

};
#endif