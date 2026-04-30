#ifndef ABIOGENESIS_H
#define ABIOGENESIS_H
#pragma once

// ---------------------------------------------------------------------------
// Collision categories
// ---------------------------------------------------------------------------
enum _AbioCategories {
    AWALL        = 1,
    WATER_CORE   = 2,
    WATER_SHELL  = 4,   // sensor — drives hydrophobic/phillic forces
    PHOBIC       = 8,   // hydrophobic tail
    PHILLIC      = 16,  // hydrophilic head
};

// ---------------------------------------------------------------------------
// Local AABB query callback (used in MouseDown)
// ---------------------------------------------------------------------------
class aQueryCallback : public b2QueryCallback {
public:
    aQueryCallback(const b2Vec2& point) : m_point(point), m_fixture(NULL) {}

    bool ReportFixture(b2Fixture* fixture) {
        b2Body* body = fixture->GetBody();
        if (body->GetType() == b2_dynamicBody && fixture->TestPoint(m_point)) {
            m_fixture = fixture;
            return false;
        }
        return true;
    }

    b2Vec2    m_point;
    b2Fixture* m_fixture;
};

// ---------------------------------------------------------------------------
// Abiogenesis world
//
// A gravity-free water world. Phospholipid molecules (hydrophilic head +
// hydrophobic tail) self-organise into bilayer sheets and eventually
// pseudo-cell vesicles driven by the hydrophobic effect.
//
// Physics model
// -------------
// Every Step() the active contact list is walked. For each touching pair:
//   WATER_SHELL ↔ WATER_SHELL   : mutual repulsion   (water pressure)
//   PHOBIC      ↔ WATER_SHELL   : mutual repulsion   (hydrophobic effect)
//   PHILLIC     ↔ WATER_SHELL   : mutual attraction  (hydrophilic effect)
//
// Forces are applied to the fixture's world position, not the body centre.
// This generates a net torque that orients each lipid (head toward water,
// tail away), which is the key mechanism for bilayer formation.
// ---------------------------------------------------------------------------
class Abiogenesis : public World {
public:

    // ---- GUI stubs (required by World interface) ----
    void nextWheeler()    {}
    void previusWheeler() {}
    void destroyCreature(){}
    void saveWorld()      {}
    void loadWorld()      {}
    void exportCreature() {}
    void importCreature() {}

    // ---- Tunable force constants ----
    //   Increase F_PHOBIC_REPEL to make bilayers form faster/tighter.
    //   Increase damping values if molecules oscillate too much.
    static const float32 F_WATER_REPEL;      // water–water pressure
    static const float32 F_PHOBIC_REPEL;     // hydrophobic repulsion
    static const float32 F_PHILLIC_ATTRACT;  // hydrophilic attraction

    static const float32 WORLD_HALF;         // half-width of the bounded area

    b2Body* m_bounds;
    b2Vec2  ma;

    // ------------------------------------------------------------------ //
    Abiogenesis() {
        activePower = GRAB;
        ma.x = 0.0f; ma.y = 0.0f;

        m_world->SetGravity(b2Vec2(0.0f, -0.1f));

        top    = (int) WORLD_HALF;
        left   = -(int)WORLD_HALF;
        bottom = -(int)WORLD_HALF;
        right  = (int) WORLD_HALF;

        buildBoundaries();
        spawnWaterField();
        spawnLipidField();
    }

    // ------------------------------------------------------------------ //
    //  World construction helpers
    // ------------------------------------------------------------------ //

    void buildBoundaries() {
        b2BodyDef bd;
        m_bounds = m_world->CreateBody(&bd);

        float32 h = WORLD_HALF;
        float32 e = 2.0f;  // extra thickness outside the boundary

        // { inner-a, inner-b, outer-b, outer-a } for each wall
        addWallFixture(b2Vec2(-h-e, -h-e), b2Vec2( h+e, -h-e), b2Vec2( h+e, -h),  b2Vec2(-h-e, -h )); // bottom
        addWallFixture(b2Vec2(-h-e,  h  ), b2Vec2( h+e,  h  ), b2Vec2( h+e,  h+e),b2Vec2(-h-e,  h+e)); // top
        addWallFixture(b2Vec2(-h-e, -h-e), b2Vec2(-h,   -h-e), b2Vec2(-h,    h+e),b2Vec2(-h-e,  h+e)); // left
        addWallFixture(b2Vec2( h,   -h-e), b2Vec2( h+e, -h-e), b2Vec2( h+e,  h+e),b2Vec2( h,    h+e)); // right
    }

    void addWallFixture(b2Vec2 v0, b2Vec2 v1, b2Vec2 v2, b2Vec2 v3) {
        b2Vec2 verts[4] = { v0, v1, v2, v3 };
        b2PolygonShape ps;
        ps.Set(verts, 4);
        b2FixtureDef fd;
        fd.shape = &ps;
        fd.density = 0.0f;
        fd.filter.categoryBits = AWALL;
        fd.filter.maskBits     = WATER_CORE | PHOBIC | PHILLIC;
        m_bounds->CreateFixture(&fd);
    }

    // Spawn water on a uniform grid, leaving a circular gap for lipids.
    void spawnWaterField() {
        const float32 spacing      = 2.0f;
        const float32 extent       = WORLD_HALF - 1.5f;
        const float32 clearRadius2 = 7.0f * 7.0f;

        for (float32 x = -extent; x <= extent; x += spacing) {
            for (float32 y = -extent; y <= extent; y += spacing) {
                if (x*x + y*y < clearRadius2) continue;
                spawnWater(b2Vec2(x, y));
            }
        }
    }

    // Spawn a loose cluster of lipids in the centre.
    void spawnLipidField() {
        const int   HALF  = 4;
        const float32 GAP = 2.0f;

        for (int ix = -HALF; ix <= HALF; ix++) {
            for (int iy = -HALF; iy <= HALF; iy++) {
                float32 x     = ix * GAP + randomNumber(-0.4f, 0.4f);
                float32 y     = iy * GAP + randomNumber(-0.4f, 0.4f);
                float32 angle = randomNumber(0.0f, 2.0f * b2_pi);
                spawnPhospholipid(b2Vec2(x, y), angle);
            }
        }
    }

    // ------------------------------------------------------------------ //
    //  Spawn individual particles
    // ------------------------------------------------------------------ //

    void spawnWater(b2Vec2 pos) {
        b2BodyDef bd;
        bd.type           = b2_dynamicBody;
        bd.position       = pos;
        bd.linearDamping  = 2.0f;   // viscosity
        bd.angularDamping = 2.0f;
        b2Body* body = m_world->CreateBody(&bd);

        // Physical core — collides with other cores and walls
        b2CircleShape core;
        core.m_radius = 0.22f;
        b2FixtureDef coreDef;
        coreDef.shape    = &core;
        coreDef.density  = 1.0f;
        coreDef.filter.categoryBits = WATER_CORE;
        coreDef.filter.maskBits     = WATER_CORE | AWALL | PHOBIC | PHILLIC;
        body->CreateFixture(&coreDef);

        // Interaction shell — sensor that triggers the force contacts
        b2CircleShape shell;
        shell.m_radius = 0.7f;
        b2FixtureDef shellDef;
        shellDef.shape    = &shell;
        shellDef.isSensor = true;
        shellDef.filter.categoryBits = WATER_SHELL;
        shellDef.filter.maskBits     = WATER_SHELL | PHOBIC | PHILLIC;
        body->CreateFixture(&shellDef);
    }

    void spawnPhospholipid(b2Vec2 pos, float32 angle = 0.0f) {
        b2BodyDef bd;
        bd.type           = b2_dynamicBody;
        bd.position       = pos;
        bd.angle          = angle;
        bd.linearDamping  = 1.5f;
        bd.angularDamping = 4.0f;  // high — keeps orientation stable once formed
        b2Body* body = m_world->CreateBody(&bd);

        // Hydrophilic head (attracted to water) — positioned at the +y end
        b2CircleShape head;
        head.m_p      = b2Vec2(0.0f, 0.7f);
        head.m_radius = 0.38f;
        b2FixtureDef headDef;
        headDef.shape   = &head;
        headDef.density = 0.9f;
        headDef.filter.categoryBits = PHILLIC;
        headDef.filter.maskBits     = WATER_SHELL | AWALL | PHILLIC | PHOBIC;
        body->CreateFixture(&headDef);

        // Hydrophobic tail (repelled by water) — positioned at the -y end
        b2PolygonShape tail;
        tail.SetAsBox(0.22f, 0.55f, b2Vec2(0.0f, -0.5f), 0.0f);
        b2FixtureDef tailDef;
        tailDef.shape   = &tail;
        tailDef.density = 0.9f;
        tailDef.filter.categoryBits = PHOBIC;
        tailDef.filter.maskBits     = WATER_SHELL | AWALL | PHILLIC | PHOBIC;
        body->CreateFixture(&tailDef);
    }

    // ------------------------------------------------------------------ //
    //  Input
    // ------------------------------------------------------------------ //

    void MouseDown(const b2Vec2& p) {
        m_mouseWorld = p;
        if (m_mouseJoint != NULL) return;

        b2AABB aabb;
        b2Vec2 d(0.001f, 0.001f);
        aabb.lowerBound = p - d;
        aabb.upperBound = p + d;

        aQueryCallback callback(p);
        m_world->QueryAABB(&callback, aabb);

        if (callback.m_fixture && activePower == GRAB) {
            b2Body* body = callback.m_fixture->GetBody();
            b2MouseJointDef md;
            md.bodyA    = m_groundBody;
            md.bodyB    = body;
            md.target   = p;
            md.maxForce = 1000.0f * body->GetMass();
            m_mouseJoint = (b2MouseJoint*)m_world->CreateJoint(&md);
            body->SetAwake(true);
        }
    }

    void MouseUp(const b2Vec2& p) {
        World::MouseUp(p);
    }

    void Keyboard(unsigned char key) {
        switch (key) {
        case '1': break;
        }
    }

    // ------------------------------------------------------------------ //
    //  Step — applies continuous intermolecular forces each frame
    // ------------------------------------------------------------------ //
    void Step(Settings* settings) {

        // Walk the live contact list and apply forces between touching fixtures.
        //
        // Forces are applied to each fixture's AABB centre (world space), not
        // the body's centre of mass.  This means phobic and phillic fixtures on
        // the same lipid body receive different force vectors, generating the
        // net torque that orients each molecule correctly.
        for (b2Contact* c = m_world->GetContactList(); c; c = c->GetNext()) {
            if (!c->IsTouching()) continue;

            b2Fixture* fA = c->GetFixtureA();
            b2Fixture* fB = c->GetFixtureB();

            // Skip contacts between fixtures on the same body
            if (fA->GetBody() == fB->GetBody()) continue;

            uint16 catA = fA->GetFilterData().categoryBits;
            uint16 catB = fB->GetFilterData().categoryBits;

            // World-space centres of the two fixtures (for torque-generating force)
            b2Vec2 centreA = fA->GetBody()->GetWorldPoint(getCentroid(fA));
            b2Vec2 centreB = fB->GetBody()->GetWorldPoint(getCentroid(fB));
            b2Vec2 delta   = centreA - centreB;
            float32 dist   = delta.Length();
            if (dist < 0.01f) continue;
            b2Vec2 dir = (1.0f / dist) * delta;  // unit vector A←B

            // ---- Water shell ↔ Water shell : repulsion (pressure) ----
            if (catA == WATER_SHELL && catB == WATER_SHELL) {
                fA->GetBody()->ApplyForceToCenter( F_WATER_REPEL * dir);
                fB->GetBody()->ApplyForceToCenter(-F_WATER_REPEL * dir);
                continue;
            }

            // ---- Phobic ↔ Water shell : hydrophobic repulsion ----
            if (catA == PHOBIC && catB == WATER_SHELL) {
                fA->GetBody()->ApplyForce( F_PHOBIC_REPEL * dir,  centreA);
                fB->GetBody()->ApplyForce(-F_PHOBIC_REPEL * dir,  centreB);
                continue;
            }
            if (catA == WATER_SHELL && catB == PHOBIC) {
                fB->GetBody()->ApplyForce(-F_PHOBIC_REPEL * dir, centreB);
                fA->GetBody()->ApplyForce( F_PHOBIC_REPEL * dir, centreA);
                continue;
            }

            // ---- Phillic ↔ Water shell : hydrophilic attraction ----
            if (catA == PHILLIC && catB == WATER_SHELL) {
                // attraction: pull A toward B, pull B toward A
                fA->GetBody()->ApplyForce(-F_PHILLIC_ATTRACT * dir, centreA);
                fB->GetBody()->ApplyForce( F_PHILLIC_ATTRACT * dir, centreB);
                continue;
            }
            if (catA == WATER_SHELL && catB == PHILLIC) {
                fB->GetBody()->ApplyForce( F_PHILLIC_ATTRACT * dir, centreB);
                fA->GetBody()->ApplyForce(-F_PHILLIC_ATTRACT * dir, centreA);
                continue;
            }
        }

        // HUD text
        m_debugDraw.DrawString(10, m_textLine,
            "Abiogenesis Universe — phospholipid bilayer simulation");
        m_textLine += 15;
        m_debugDraw.DrawString(10, m_textLine,
            "Bodies: %d   Contacts: %d",
            m_world->GetBodyCount(), m_world->GetContactCount());
        m_textLine += 15;
        m_debugDraw.DrawString(10, m_textLine,
            "Tune: F_PHOBIC_REPEL=%.0f  F_PHILLIC_ATTRACT=%.0f  F_WATER_REPEL=%.0f",
            F_PHOBIC_REPEL, F_PHILLIC_ATTRACT, F_WATER_REPEL);
        m_textLine += 15;

        World::Step(settings);
    }

    static World* Create() {
        return new Abiogenesis;
    }

private:

    float32 randomNumber(float32 low = 0.0f, float32 high = 1.0f) {
        return low + (float32)rand() / ((float32)RAND_MAX / (high - low));
    }

    // Returns the local centroid of a fixture's shape (approximation).
    // For circles this is the centre offset; for polygons we use the
    // centroid of the local AABB — close enough for force application.
    static b2Vec2 getCentroid(b2Fixture* f) {
        switch (f->GetType()) {
        case b2Shape::e_circle:
            return static_cast<const b2CircleShape*>(f->GetShape())->m_p;
        case b2Shape::e_polygon: {
            const b2PolygonShape* ps =
                static_cast<const b2PolygonShape*>(f->GetShape());
            b2Vec2 c(0, 0);
            for (int32 i = 0; i < ps->GetVertexCount(); i++)
                c += ps->GetVertex(i);
            return (1.0f / ps->GetVertexCount()) * c;
        }
        default:
            return b2Vec2(0, 0);
        }
    }
};

// Out-of-class definitions for static const float members
const float32 Abiogenesis::F_WATER_REPEL     = 8.0f;
const float32 Abiogenesis::F_PHOBIC_REPEL    = 32.0f;
const float32 Abiogenesis::F_PHILLIC_ATTRACT = 22.0f;
const float32 Abiogenesis::WORLD_HALF        = 22.0f;

#endif



#if 0   // ---- old skeleton disabled — replaced by the implementation above ----
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