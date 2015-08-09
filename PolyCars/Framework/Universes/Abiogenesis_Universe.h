#ifndef ABIOGENESIS_H
#define ABIOGENESIS_H
#pragma once

class aWheelerContactListener : public b2ContactListener {
	void BeginContact(b2Contact* contact) {

		b2Fixture* fixtureA = contact->GetFixtureA();
		b2Fixture* fixtureB = contact->GetFixtureB();

		//When a root is touching another root. --- USE this to start later...
		if (fixtureA->GetFilterData().categoryBits == ROOT && 
			fixtureB->GetFilterData().categoryBits == ROOT) {
				Grass* seedTest = (Grass *)fixtureB->GetBody()->GetUserData(); 
				Grass* seedTest2 = (Grass *)fixtureA->GetBody()->GetUserData(); 
				seedTest->crowded = true;
				seedTest2->crowded = true;
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

		#pragma region Boundaries
		{

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
			_bgpTfd.filter.categoryBits = PERM_WALL;

			world->CreateFixture(&_bgpTfd);

			//Top Bar
			_bgpTva[0].Set(-50, 50);
			_bgpTva[1].Set(-50, 48);
			_bgpTva[2].Set( 50, 48); 
			_bgpTva[3].Set( 50, 50);   

			_bgpTps.Set(_bgpTva, 4);

			_bgpTfd.shape = &_bgpTps;
			_bgpTfd.density = 0.0f;
			_bgpTfd.filter.categoryBits = PERM_WALL;

			world->CreateFixture(&_bgpTfd);

			//Left Bar
			_bgpTva[0].Set(-50,  50);
			_bgpTva[1].Set(-50, -50);
			_bgpTva[2].Set(-48, -50); 
			_bgpTva[3].Set(-48,  50);   

			_bgpTps.Set(_bgpTva, 4);

			_bgpTfd.shape = &_bgpTps;
			_bgpTfd.density = 0.0f;
			_bgpTfd.filter.categoryBits = PERM_WALL;

			world->CreateFixture(&_bgpTfd);

			//Right Bar
			_bgpTva[0].Set( 48,  50);
			_bgpTva[1].Set( 48, -50);
			_bgpTva[2].Set( 50, -50); 
			_bgpTva[3].Set( 50,  50);   

			_bgpTps.Set(_bgpTva, 4);

			_bgpTfd.shape = &_bgpTps;
			_bgpTfd.density = 0.0f;
			_bgpTfd.filter.categoryBits = PERM_WALL;

			world->CreateFixture(&_bgpTfd);

			//Water

			b2Vec2 waterPos;

			//for 100
				//for 50+

			waterPos.Set(0, 0);

			spawnWater(waterPos);


		}
		#pragma endregion
	}
	
	void spawnWater(b2Vec2 pos) {

		b2BodyDef myBodyDef;
		myBodyDef.type = b2_dynamicBody; //this will be a dynamic body
		myBodyDef.position.Set(pos.x, pos.y); //set the starting position
		myBodyDef.angle = 0; //set the starting angle

		b2Body* dynamicBody = m_world->CreateBody(&myBodyDef);

		b2CircleShape circleShape;
		circleShape.m_p.Set(0, 0); //position, relative to body position
		circleShape.m_radius = 1; //radius

		b2FixtureDef myFixtureDef;
		myFixtureDef.shape = &circleShape; //this is a pointer to the shape above
		dynamicBody->CreateFixture(&myFixtureDef); //add a fixture to the body

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