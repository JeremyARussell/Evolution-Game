#ifndef SANDBOX_H
#define SANDBOX_H
#pragma once
//Includes for file stream and standard classes.
#include <fstream>
#include <iostream>
#include <string>
//Includes needed for game specific classes
#include "Build\PolyCars\Wheeler.h"
#include "Build\PolyCars\Seed.h"
#include "Build\PolyCars\Grass.h"
#include "Build\PolyCars\GrassSpawner.h"
//Includes for third party engine stuff (not including the box2d engine stuff)
#include "SOIL\SOIL.h";

//An enum I need to fix and move to it's own header...
enum _entityCategory {///HACKY - Just use the bitwise values inside the wheeler class, etc.
    SEED		     =  0x0001,
    GRASS            =  0x0002,
    WHEELER		     =  0x0004,
    //CARNIVORE	     =  0x0008,
    NON_INTERACTOR   =  0x0010,
    WALL             =  0x0020,
    GRASS_SENSOR	 =  0x0040,
};

//Sensing
class WheelerContactListener : public b2ContactListener {
	void BeginContact(b2Contact* contact) {

		b2Fixture* fixtureA = contact->GetFixtureA();
		b2Fixture* fixtureB = contact->GetFixtureB();

		//When a wheeler's sensor collides with a piece of grass.
		if (fixtureB->GetFilterData().categoryBits == WHEELER && 
			fixtureA->GetFilterData().categoryBits == GRASS) {
			Grass* activeGrass = (Grass *)fixtureA->GetBody()->GetUserData();
			Wheeler* activeWheeler = (Wheeler *)fixtureB->GetBody()->GetUserData();

			if (activeWheeler->health < 30) {
				if (activeGrass->fresh)	return;
				activeGrass->beingEaten = true;
				activeWheeler->needsToReproduce = true;
				activeWheeler->health = 50;
			}
		}		
		//When a seed touches a pieces of WALL(Really Ground, I need to change that)
		if (fixtureA->GetFilterData().categoryBits == WALL && 
			fixtureB->GetFilterData().categoryBits == SEED) {
				Seed* seedTest = (Seed *)fixtureB->GetBody()->GetUserData(); 
				seedTest->timetoSeed = true;
		}
		//When a root is touching another root.
		if (fixtureA->GetFilterData().categoryBits == GRASS_SENSOR && 
			fixtureB->GetFilterData().categoryBits == GRASS_SENSOR) {
				Grass* seedTest = (Grass *)fixtureB->GetBody()->GetUserData(); 
				Grass* seedTest2 = (Grass *)fixtureA->GetBody()->GetUserData(); 
				seedTest->crowded = true;
				seedTest2->crowded = true;
		}
	}
  
    void EndContact(b2Contact* contact) {
		//Nothing happens when contacting ends... Not sure how it would be useful right now yet.
    }
};

WheelerContactListener thisWheelerContactListener;

class QueryCallback : public b2QueryCallback {//This may or may not be needed, it exists in the World class.
public:
	QueryCallback(const b2Vec2& point) {
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

class SandboxWorld : public World {
public:

	//These functions are tied to the GLUI GUI
	void nextWheeler() {
		if (wheelers.size() == 0) return;
		cW++;
		if (cW == wheelers.size()) {
			cW = 0;
		}
		activeWheeler = wheelers[cW];
	}
	void previusWheeler() {
		if (wheelers.size() == 0) return;
		cW--;
		if (cW == 0) {
			cW = wheelers.size();
		}
		activeWheeler = wheelers[cW];
	}

	void destroyCreature() {
		if ( activeWheeler != NULL && following) {
			if (wheelers.size() == 0) return;
			wheelersToDelete.push_back(*activeWheeler);
			wheelers.erase( std::find(wheelers.begin(), wheelers.end(), activeWheeler ) );
			activeWheeler = NULL;
		}
	}

	void saveWorld() {
		ofstream saveFile("SandboxWorld.sav");

		//For each grass - Save the grass position
		for (int i = 0; i < grasses.size(); i ++) {
			b2Vec2 grassPosition = grasses[i]->stalk->GetPosition();
			saveFile << "grassPos" << ' ' << grassPosition.x << ' ' << grassPosition.y << endl;
		}

		//For each Wheeler ----- Save the wheeler stuff
		for (int i = 0; i < wheelers.size(); i ++) {
			b2Vec2 wheelerPos = wheelers[i]->cart->GetPosition();

			saveFile << "wPos" << ' ' << wheelerPos.x << ' ' << wheelerPos.y << endl;
			saveFile << "wAngleAndDirection" << ' ' << wheelers[i]->cart->GetAngle() << ' ' << wheelers[i]->spinDirection << endl;
			saveFile << "wLife" << ' ' << wheelers[i]->health << ' ' << wheelers[i]->minutesToLive << endl;

			saveFile << "spokeAngles1" << ' ' << wheelers[i]->spokeAngles[0] << ' ' << wheelers[i]->spokeAngles[1] << endl;
			saveFile << "spokeAngles2" << ' ' << wheelers[i]->spokeAngles[2] << ' ' << wheelers[i]->spokeAngles[3] << endl;
			saveFile << "spokeAngles3" << ' ' << wheelers[i]->spokeAngles[4] << ' ' << wheelers[i]->spokeAngles[5] << endl;
			saveFile << "spokeAngles4" << ' ' << wheelers[i]->spokeAngles[6] << ' ' << wheelers[i]->spokeAngles[7] << endl;

			saveFile << "spokeLengths1" << ' ' << wheelers[i]->spokeLengths[0] << ' ' << wheelers[i]->spokeLengths[1] << endl;
			saveFile << "spokeLengths2" << ' ' << wheelers[i]->spokeLengths[2] << ' ' << wheelers[i]->spokeLengths[3] << endl;
			saveFile << "spokeLengths3" << ' ' << wheelers[i]->spokeLengths[4] << ' ' << wheelers[i]->spokeLengths[5] << endl;
			saveFile << "spokeLengths4" << ' ' << wheelers[i]->spokeLengths[6] << ' ' << wheelers[i]->spokeLengths[7] << endl;

			saveFile << "hasWheel1" << ' ' << wheelers[i]->hasWheel[0] << ' ' << wheelers[i]->hasWheel[1] << endl;
			saveFile << "hasWheel2" << ' ' << wheelers[i]->hasWheel[2] << ' ' << wheelers[i]->hasWheel[3] << endl;
			saveFile << "hasWheel3" << ' ' << wheelers[i]->hasWheel[4] << ' ' << wheelers[i]->hasWheel[5] << endl;
			saveFile << "hasWheel4" << ' ' << wheelers[i]->hasWheel[6] << ' ' << wheelers[i]->hasWheel[7] << endl;

			saveFile << "axleAngle1" << ' ' << wheelers[i]->axleAngle[0] << ' ' << wheelers[i]->axleAngle[1] << endl;
			saveFile << "axleAngle2" << ' ' << wheelers[i]->axleAngle[2] << ' ' << wheelers[i]->axleAngle[3] << endl;
			saveFile << "axleAngle3" << ' ' << wheelers[i]->axleAngle[4] << ' ' << wheelers[i]->axleAngle[5] << endl;
			saveFile << "axleAngle4" << ' ' << wheelers[i]->axleAngle[6] << ' ' << wheelers[i]->axleAngle[7] << endl;

			saveFile << "wheelRadius1" << ' ' << wheelers[i]->wheelRadius[0] << ' ' << wheelers[i]->wheelRadius[1] << endl;
			saveFile << "wheelRadius2" << ' ' << wheelers[i]->wheelRadius[2] << ' ' << wheelers[i]->wheelRadius[3] << endl;
			saveFile << "wheelRadius3" << ' ' << wheelers[i]->wheelRadius[4] << ' ' << wheelers[i]->wheelRadius[5] << endl;
			saveFile << "wheelRadius4" << ' ' << wheelers[i]->wheelRadius[6] << ' ' << wheelers[i]->wheelRadius[7] << endl;
		}
		saveFile.close();
	}
	void loadWorld() {
		ifstream loadFile("SandboxWorld.sav");

		string loadType = "";
		float32 var1;
		float32 var2;

		vector<float32> genes;
		bool readyToWheel = false;

		while (loadFile >> loadType >> var1 >> var2 ) {
			//Load the grass
			if (loadType == "grassPos") {
				grasses.push_back(new Grass(m_world, var1, var2 ));
			}

			//Load the Wheelers
			if (loadType == "wPos") { genes.push_back(var1); genes.push_back(var2); }
			if (loadType == "wAngleAndDirection") { genes.push_back(var1); genes.push_back(var2); }
			if (loadType == "wLife") { genes.push_back(var1); genes.push_back(var2); }
			
			if (loadType == "spokeAngles1") { genes.push_back(var1); genes.push_back(var2); }
			if (loadType == "spokeAngles2") { genes.push_back(var1); genes.push_back(var2); }
			if (loadType == "spokeAngles3") { genes.push_back(var1); genes.push_back(var2); }
			if (loadType == "spokeAngles4") { genes.push_back(var1); genes.push_back(var2); }

			if (loadType == "spokeLengths1") { genes.push_back(var1); genes.push_back(var2); }
			if (loadType == "spokeLengths2") { genes.push_back(var1); genes.push_back(var2); }
			if (loadType == "spokeLengths3") { genes.push_back(var1); genes.push_back(var2); }
			if (loadType == "spokeLengths4") { genes.push_back(var1); genes.push_back(var2); }

			if (loadType == "hasWheel1") { genes.push_back(var1); genes.push_back(var2); }
			if (loadType == "hasWheel2") { genes.push_back(var1); genes.push_back(var2); }
			if (loadType == "hasWheel3") { genes.push_back(var1); genes.push_back(var2); }
			if (loadType == "hasWheel4") { genes.push_back(var1); genes.push_back(var2); }

			if (loadType == "axleAngle1") { genes.push_back(var1); genes.push_back(var2); }
			if (loadType == "axleAngle2") { genes.push_back(var1); genes.push_back(var2); }
			if (loadType == "axleAngle3") { genes.push_back(var1); genes.push_back(var2); }
			if (loadType == "axleAngle4") { genes.push_back(var1); genes.push_back(var2); }

			if (loadType == "wheelRadius1") { genes.push_back(var1); genes.push_back(var2); }
			if (loadType == "wheelRadius2") { genes.push_back(var1); genes.push_back(var2); }
			if (loadType == "wheelRadius3") { genes.push_back(var1); genes.push_back(var2); }
			if (loadType == "wheelRadius4") { genes.push_back(var1); genes.push_back(var2); readyToWheel = true;}

			if (readyToWheel) {
				wheelers.push_back(new Wheeler(m_world, genes));
				genes.clear();
				readyToWheel = false;
			}
		}
		loadFile.close();
	}

	void exportCreature() {
		if ( activeWheeler != NULL && following) {
			if (wheelers.size() == 0) return;
			ofstream saveFile("WheelerExport.sav");

			saveFile << "wPos" << ' ' << -200.0f << ' ' << 15.0f << endl;
			float32 directionAndSpeed;
			if (activeWheeler->spinDirection < 0) {
				directionAndSpeed = -activeWheeler->spinDirection;
			} else {
				directionAndSpeed = activeWheeler->spinDirection;
			}
			saveFile << "wAngleAndDirection" << ' ' << 0.0f << ' ' << directionAndSpeed << endl;//Setting Angle probably doesn't matter, it's set to zero on Wheeler creation anyways regardless.
			saveFile << "wLife" << ' ' << 50/*TODO - MAX HEALTH LATER ON*/ << ' ' << 28800 << endl;

			saveFile << "spokeAngles1" << ' ' << activeWheeler->spokeAngles[0] << ' ' << activeWheeler->spokeAngles[1] << endl;
			saveFile << "spokeAngles2" << ' ' << activeWheeler->spokeAngles[2] << ' ' << activeWheeler->spokeAngles[3] << endl;
			saveFile << "spokeAngles3" << ' ' << activeWheeler->spokeAngles[4] << ' ' << activeWheeler->spokeAngles[5] << endl;
			saveFile << "spokeAngles4" << ' ' << activeWheeler->spokeAngles[6] << ' ' << activeWheeler->spokeAngles[7] << endl;

			saveFile << "spokeLengths1" << ' ' << activeWheeler->spokeLengths[0] << ' ' << activeWheeler->spokeLengths[1] << endl;
			saveFile << "spokeLengths2" << ' ' << activeWheeler->spokeLengths[2] << ' ' << activeWheeler->spokeLengths[3] << endl;
			saveFile << "spokeLengths3" << ' ' << activeWheeler->spokeLengths[4] << ' ' << activeWheeler->spokeLengths[5] << endl;
			saveFile << "spokeLengths4" << ' ' << activeWheeler->spokeLengths[6] << ' ' << activeWheeler->spokeLengths[7] << endl;

			saveFile << "hasWheel1" << ' ' << activeWheeler->hasWheel[0] << ' ' << activeWheeler->hasWheel[1] << endl;
			saveFile << "hasWheel2" << ' ' << activeWheeler->hasWheel[2] << ' ' << activeWheeler->hasWheel[3] << endl;
			saveFile << "hasWheel3" << ' ' << activeWheeler->hasWheel[4] << ' ' << activeWheeler->hasWheel[5] << endl;
			saveFile << "hasWheel4" << ' ' << activeWheeler->hasWheel[6] << ' ' << activeWheeler->hasWheel[7] << endl;

			saveFile << "axleAngle1" << ' ' << activeWheeler->axleAngle[0] << ' ' << activeWheeler->axleAngle[1] << endl;
			saveFile << "axleAngle2" << ' ' << activeWheeler->axleAngle[2] << ' ' << activeWheeler->axleAngle[3] << endl;
			saveFile << "axleAngle3" << ' ' << activeWheeler->axleAngle[4] << ' ' << activeWheeler->axleAngle[5] << endl;
			saveFile << "axleAngle4" << ' ' << activeWheeler->axleAngle[6] << ' ' << activeWheeler->axleAngle[7] << endl;

			saveFile << "wheelRadius1" << ' ' << activeWheeler->wheelRadius[0] << ' ' << activeWheeler->wheelRadius[1] << endl;
			saveFile << "wheelRadius2" << ' ' << activeWheeler->wheelRadius[2] << ' ' << activeWheeler->wheelRadius[3] << endl;
			saveFile << "wheelRadius3" << ' ' << activeWheeler->wheelRadius[4] << ' ' << activeWheeler->wheelRadius[5] << endl;
			saveFile << "wheelRadius4" << ' ' << activeWheeler->wheelRadius[6] << ' ' << activeWheeler->wheelRadius[7] << endl;
		
			saveFile.close();
		}
	}
	void importCreature() {
		ifstream loadFile("WheelerExport.sav");

		string loadType = "";
		float32 var1;
		float32 var2;

		vector<float32> genes;
		bool readyToWheel = false;

		while (loadFile >> loadType >> var1 >> var2 ) {

			if (loadType == "wPos") { genes.push_back(var1); genes.push_back(var2); }
			if (loadType == "wAngleAndDirection") { genes.push_back(var1); genes.push_back(var2); }
			if (loadType == "wLife") { genes.push_back(var1); genes.push_back(var2); }
			
			if (loadType == "spokeAngles1") { genes.push_back(var1); genes.push_back(var2); }
			if (loadType == "spokeAngles2") { genes.push_back(var1); genes.push_back(var2); }
			if (loadType == "spokeAngles3") { genes.push_back(var1); genes.push_back(var2); }
			if (loadType == "spokeAngles4") { genes.push_back(var1); genes.push_back(var2); }

			if (loadType == "spokeLengths1") { genes.push_back(var1); genes.push_back(var2); }
			if (loadType == "spokeLengths2") { genes.push_back(var1); genes.push_back(var2); }
			if (loadType == "spokeLengths3") { genes.push_back(var1); genes.push_back(var2); }
			if (loadType == "spokeLengths4") { genes.push_back(var1); genes.push_back(var2); }

			if (loadType == "hasWheel1") { genes.push_back(var1); genes.push_back(var2); }
			if (loadType == "hasWheel2") { genes.push_back(var1); genes.push_back(var2); }
			if (loadType == "hasWheel3") { genes.push_back(var1); genes.push_back(var2); }
			if (loadType == "hasWheel4") { genes.push_back(var1); genes.push_back(var2); }

			if (loadType == "axleAngle1") { genes.push_back(var1); genes.push_back(var2); }
			if (loadType == "axleAngle2") { genes.push_back(var1); genes.push_back(var2); }
			if (loadType == "axleAngle3") { genes.push_back(var1); genes.push_back(var2); }
			if (loadType == "axleAngle4") { genes.push_back(var1); genes.push_back(var2); }

			if (loadType == "wheelRadius1") { genes.push_back(var1); genes.push_back(var2); }
			if (loadType == "wheelRadius2") { genes.push_back(var1); genes.push_back(var2); }
			if (loadType == "wheelRadius3") { genes.push_back(var1); genes.push_back(var2); }
			if (loadType == "wheelRadius4") { genes.push_back(var1); genes.push_back(var2); readyToWheel = true;}

			if (readyToWheel) {
				wheelers.push_back(new Wheeler(m_world, genes));
				genes.clear();
				readyToWheel = false;
			}
		}
		loadFile.close();
	}

	void buildGroundPiece(b2Vec2 pa, b2Vec2 pb, b2Body* world) {

		b2Vec2 _bgpTva[4];
		float32 _bgpTslope = (pb.y - pa.y) / (pb.x - pa.x);

		if(_bgpTslope >= 1 | _bgpTslope < -1.0) {//Mostly vertical
			//A is lower than B
			if (pa.y < pb.y)
			{
				_bgpTva[0].Set(pb.x - 2.5f, pb.y);
				_bgpTva[1].Set(pa.x - 2.5f, pa.y);
				_bgpTva[2].Set(pa.x + 2.5f, pa.y); 
				_bgpTva[3].Set(pb.x + 2.5f, pb.y);   
			} else {
				_bgpTva[0].Set(pa.x - 2.5f, pa.y);
				_bgpTva[1].Set(pb.x - 2.5f, pb.y);
				_bgpTva[2].Set(pb.x + 2.5f, pb.y); 
				_bgpTva[3].Set(pa.x + 2.5f, pa.y);   
			}

		} else {//Mostly horizontal
			//A is to the left of B
			if (pa.x < pb.x)
			{
				_bgpTva[0].Set(pa.x, pa.y + 2.5f); /// top left
				_bgpTva[1].Set(pa.x, pa.y - 2.5f);/// bottom left
				_bgpTva[2].Set(pb.x, pb.y - 2.5f); /// bottom right
				_bgpTva[3].Set(pb.x, pb.y + 2.5f);  /// top right 
			} else {//B is to the left of A
				_bgpTva[0].Set(pb.x, pb.y + 2.5f); /// top left
				_bgpTva[1].Set(pb.x, pb.y - 2.5f);/// bottom left
				_bgpTva[2].Set(pa.x, pa.y - 2.5f); /// bottom right
				_bgpTva[3].Set(pa.x, pa.y + 2.5f);  /// top right 
			}
		}

		b2PolygonShape _bgpTps;
		_bgpTps.Set(_bgpTva, 4);

		b2FixtureDef _bgpTfd;
		_bgpTfd.shape = &_bgpTps;
		_bgpTfd.density = 0.0f;
		_bgpTfd.filter.categoryBits = WALL;//TODO - WALL here should really be GROUND

		world->CreateFixture(&_bgpTfd);

	}

	void buildWallPiece(b2Vec2 pa, b2Vec2 pb, b2Body* world) {

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

		b2PolygonShape _bwpTps;
		_bwpTps.Set(_bwpTva, 4);

		b2FixtureDef _bwpTfd;
		_bwpTfd.shape = &_bwpTps;
		_bwpTfd.density = 0.0f;
		_bwpTfd.filter.categoryBits = NON_INTERACTOR;//TODO - NON_INTERACTOR here should really be WALL

		world->CreateFixture(&_bwpTfd);

	}

	b2Body* world;

	//Level building, and default creature placement go here.
    SandboxWorld() {
		cW = 0;
		activeWheeler = NULL;
		grassSpawnCounter = 0;
		following = false;
		activePower = GRAB;

		ma.x = 0.0f; ma.y = 0.0f;

		top = 68;
		left = -223;
		bottom = 0;
		right = 223;


		hudX = 250, hudY = 50;
		_power worldPowers = (_power)(GRAB | SELECT | DESTROY | SPAWN_SEED | SPAWN_WHEELER 
									| CREATE_GROUND | CREATE_WALL | GRASS_SPAWNER);
		powerHUD = PowerHUD(hudX, hudY, worldPowers, activePower);

		m_world->SetContactListener(&thisWheelerContactListener);

		#pragma region World 
		{
			//The World's boundaries
			b2BodyDef bd;
			world = m_world->CreateBody(&bd);
			
			//Ground
			buildGroundPiece(b2Vec2( 225.0f, -1.5f), b2Vec2(-225.0f, -1.5f), world);

			//Wall
			buildWallPiece(b2Vec2( -222.5f, 0.0f ), b2Vec2( -222.5f, 70.0f ), world);//Left Wall
			buildWallPiece(b2Vec2( -222.5f, 70.0f), b2Vec2( 222.5f, 70.0f), world);//Ceiling
			buildWallPiece(b2Vec2( 222.5f, 0.0f ), b2Vec2( 222.5f, 70.0f ), world);//Right Wall

		}
		#pragma endregion Ground and Walls
	}
	
	float32 clickZoomLevel;
	b2Vec2  clickViewCenter;

	b2Vec2 ma, mb;//used for the new ground and wall powers.

	void SandboxWorld::MouseDown(const b2Vec2& p) {
										
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

		//Power implementation code
		if (activePower == SPAWN_SEED) {
			float32 xt = randomNumber(-3.0f, 3.0f);
			float32 yt = randomNumber(1.0f, 3.0f);//<-- Maybe do an option to have them randomly fly off or not?
			seeds.push_back(new Seed(m_world, callback.m_point, b2Vec2(xt, yt)));
			
		}

		if (activePower == SPAWN_WHEELER) {
			if (wheelers.size() < 100) {
				cW = 0;
				wheelers.push_back(new Wheeler(m_world,  callback.m_point.x, callback.m_point.y));
			}			
		}

		if (activePower == GRASS_SPAWNER) {
			grassSpawners.push_back(new GrassSpawner(m_world, p.x, p.y));
		}

		if (activePower == CREATE_GROUND | activePower == CREATE_WALL) {
			ma = b2Vec2(p.x, p.y);
		}

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

			if (activePower == SELECT) {
				b2Body* body = callback.m_fixture->GetBody();

				if (callback.m_fixture->GetFilterData().categoryBits == SEED |
					callback.m_fixture->GetFilterData().categoryBits == GRASS_SENSOR) return;

				Wheeler *testWheeler = (Wheeler *)body->GetUserData();
				activeWheeler = testWheeler;
			}

			if (activePower == DESTROY) {
				b2Body* body = callback.m_fixture->GetBody();

				if (callback.m_fixture->GetFilterData().categoryBits == SEED |
					callback.m_fixture->GetFilterData().categoryBits == GRASS_SENSOR) return;
				
				Wheeler *testWheeler = (Wheeler *)body->GetUserData();
				wheelersToDelete.push_back(*testWheeler);
				wheelers.erase( std::find(wheelers.begin(), wheelers.end(), testWheeler ) );
				activeWheeler = NULL;
				testWheeler = NULL;
			}
		}
	}

	void SandboxWorld::MouseUp(const b2Vec2& p) {
		if (activePower == CREATE_GROUND | activePower == CREATE_WALL) {
			if (ma.x != 0.0f && ma.y != 0.0f) {
				mb = b2Vec2(p.x, p.y);

				float32 dist = sqrt((mb.x - ma.x) * (mb.x - ma.x) + (mb.y - ma.y) * (mb.y - ma.y));

				if (dist > 1.0f)
				{
					if(activePower == CREATE_GROUND) {
						buildGroundPiece(ma, mb, world);
					} else {
						buildWallPiece(ma, mb, world);
					} 
				}

				ma.x = 0.0f;
				ma.y = 0.0f;
			}
		}

		World::MouseUp(p);
	}

	void Keyboard(unsigned char key) {
		switch (key) {
		case '1':
			activePower = GRAB;
			powerHUD.setActivePower(activePower);
			break;
		case '2':
			activePower = SELECT;
			powerHUD.setActivePower(activePower);
			break;
		case '3':
			activePower = DESTROY;
			powerHUD.setActivePower(activePower);
			break;
		case '4':
			activePower = SPAWN_SEED;
			powerHUD.setActivePower(activePower);
			break;
		case '5':
			activePower = SPAWN_WHEELER;
			powerHUD.setActivePower(activePower);
			break;//TODO - Add the rest of the powers later
		}
	}

	void Step(Settings* settings) {

		if (clickZoomLevel != settings->zoomLevel) clickZoomLevel = settings->zoomLevel;
		if (clickViewCenter.x != settings->viewCenter.x &&
			clickViewCenter.y != settings->viewCenter.y) clickViewCenter = settings->viewCenter;
		if (powerHUD.getActivePower() != activePower) activePower = powerHUD.getActivePower();

		//For knowing if our settings file has the following flag triggered
		if (settings->followCreature == true) {
			following = true;
		} else {
			following = false;
		}

		//For tracking/following the active Wheeler
		if (activeWheeler != NULL && activeWheeler->health > -1 && settings->followCreature == 1) {
			settings->viewCenter.Set(activeWheeler->cart->GetPosition().x, activeWheeler->cart->GetPosition().y);
		}

		//Game text
		m_debugDraw.DrawString(10, m_textLine, "Welcome to Evolution, a game/project to create worlds and creatures which compete and evolve");
		m_textLine += 15;

		if (settings->pause) {//TODO - Make a checkbox for this guy to not need to be paused.

			m_debugDraw.DrawString(240, 85, "======= World Stats =======");

			int32 wheelerCount = wheelers.size();
			m_debugDraw.DrawString(240, 100, "Wheelers = %d", wheelerCount);

			int32 grassCount = grasses.size();
			m_debugDraw.DrawString(240, 115, "Blades of Grass = %d", grassCount);
		}

		int trackwsp = 85;

		if (activeWheeler != NULL && settings->drawGenes) {
			m_debugDraw.DrawString(15, trackwsp, "======= Wheeler Stats ======");
			trackwsp += 15;
			m_debugDraw.DrawString(15, trackwsp, "= Spoke - Length  / Angle  =");
			trackwsp += 15;
			m_debugDraw.DrawString(15, trackwsp, "= #1    - %06.3f  / %06.3f =", activeWheeler->spokeLengths[0], activeWheeler->spokeAngles[0]);
			trackwsp += 15;
			m_debugDraw.DrawString(15, trackwsp, "= #2    - %06.3f  / %06.3f =", activeWheeler->spokeLengths[1], activeWheeler->spokeAngles[1]);
			trackwsp += 15;
			m_debugDraw.DrawString(15, trackwsp, "= #3    - %06.3f  / %06.3f =", activeWheeler->spokeLengths[2], activeWheeler->spokeAngles[2]);
			trackwsp += 15;
			m_debugDraw.DrawString(15, trackwsp, "= #4    - %06.3f  / %06.3f =", activeWheeler->spokeLengths[3], activeWheeler->spokeAngles[3]);
			trackwsp += 15;
			m_debugDraw.DrawString(15, trackwsp, "= #5    - %06.3f  / %06.3f =", activeWheeler->spokeLengths[4], activeWheeler->spokeAngles[4]);
			trackwsp += 15;
			m_debugDraw.DrawString(15, trackwsp, "= #6    - %06.3f  / %06.3f =", activeWheeler->spokeLengths[5], activeWheeler->spokeAngles[5]);
			trackwsp += 15;
			m_debugDraw.DrawString(15, trackwsp, "= #7    - %06.3f  / %06.3f =", activeWheeler->spokeLengths[6], activeWheeler->spokeAngles[6]);
			trackwsp += 15;
			m_debugDraw.DrawString(15, trackwsp, "= #8    - %06.3f  / %06.3f =", activeWheeler->spokeLengths[7], activeWheeler->spokeAngles[7]);
			trackwsp += 15;

			m_debugDraw.DrawString(15, trackwsp, "========== Wheels ==========");
			trackwsp += 15;
			m_debugDraw.DrawString(15, trackwsp, "= Spoke - Length  / Angle  =");
			trackwsp += 15;
			m_debugDraw.DrawString(15, trackwsp, "= #1    - %06.3f  / %06.3f =", activeWheeler->hasWheel[0], activeWheeler->wheelRadius[0]);
			trackwsp += 15;
			m_debugDraw.DrawString(15, trackwsp, "= #2    - %06.3f  / %06.3f =", activeWheeler->hasWheel[1], activeWheeler->wheelRadius[1]);
			trackwsp += 15;
			m_debugDraw.DrawString(15, trackwsp, "= #3    - %06.3f  / %06.3f =", activeWheeler->hasWheel[2], activeWheeler->wheelRadius[2]);
			trackwsp += 15;
			m_debugDraw.DrawString(15, trackwsp, "= #4    - %06.3f  / %06.3f =", activeWheeler->hasWheel[3], activeWheeler->wheelRadius[3]);
			trackwsp += 15;
			m_debugDraw.DrawString(15, trackwsp, "= #5    - %06.3f  / %06.3f =", activeWheeler->hasWheel[4], activeWheeler->wheelRadius[4]);
			trackwsp += 15;
			m_debugDraw.DrawString(15, trackwsp, "= #6    - %06.3f  / %06.3f =", activeWheeler->hasWheel[5], activeWheeler->wheelRadius[5]);
			trackwsp += 15;
			m_debugDraw.DrawString(15, trackwsp, "= #7    - %06.3f  / %06.3f =", activeWheeler->hasWheel[6], activeWheeler->wheelRadius[6]);
			trackwsp += 15;
			m_debugDraw.DrawString(15, trackwsp, "= #8    - %06.3f  / %06.3f =", activeWheeler->hasWheel[7], activeWheeler->wheelRadius[7]);
			trackwsp += 15;

		}

		for (int i = 0; i < grassSpawners.size(); i ++) {
			if (!settings->pause) { grassSpawners[i]->step(); }

			if (grassSpawners[i]->seeding && !(grassSpawners[i]->crowded)) {

				float32 xt = randomNumber(-3.0f, 3.0f);
				float32 yt = randomNumber(1.0f, 3.0f);
				float32 xt2 = grassSpawners[i]->stalk->GetPosition().x;
				float32 yt2 = grassSpawners[i]->stalk->GetPosition().y;

				seeds.push_back(new Seed(m_world, b2Vec2(xt2, yt2), b2Vec2(xt, yt)));
				//But with grass as parent once genes for them are setup.
				
				grassSpawners[i]->seeding = false;
			}
		}

		for (int i = 0; i < seeds.size(); i ++) {
			if (!settings->pause) { 
				if (seeds[i]->timetoSeed) {
					Seed *dying = seeds[i];
					float32 xt2 = dying->seed->GetPosition().x;
					float32 yt2 = dying->seed->GetPosition().y;
					seedsToDelete.push_back(*dying);
					seeds.erase( std::find(seeds.begin(), seeds.end(), dying ) );
					grasses.push_back(new Grass(m_world, xt2,  yt2));
				}
			}
		}

		//Check if grass is being eaten this step
		for (int i = 0; i < grasses.size(); i ++) {

			if (!settings->pause) { grasses[i]->step(); }
			
			if (grasses[i]->seeding && !(grasses[i]->crowded)) {

				float32 xt = randomNumber(-3.0f, 3.0f);
				float32 yt = randomNumber(1.0f, 3.0f);
				float32 xt2 = grasses[i]->stalk->GetPosition().x;
				float32 yt2 = grasses[i]->stalk->GetPosition().y;


				seeds.push_back(new Seed(m_world, b2Vec2(xt2, yt2), b2Vec2(xt, yt)));
				//But with grass as parent once genes for them are setup.
				
				grasses[i]->seeding = false;
			}

			if (grasses[i]->beingEaten) {
				Grass *dying = grasses[i];
				grassToDelete.push_back(*dying);
				grasses.erase( std::find(grasses.begin(), grasses.end(), dying ) );
			}
		}

		//Wheeler Step code
		for (int i = 0; i < wheelers.size(); i ++) {
			//Call to render
			wheelers[i]->render();
			
			//REPRODUCING STUFF
			if (!settings->pause) { wheelers[i]->reproductionCounter++; }
			if (wheelers[i]->needsToReproduce) {
				wheelers[i]->reproductionCounter = 0;
				wheelers[i]->needsToReproduce = false;
				wheelers.push_back(new Wheeler(*wheelers[i], m_world,  wheelers[i]->cart->GetPosition().x,  wheelers[i]->cart->GetPosition().y + 1.0f));
			}

			//Dying stuff is done at the end of the for loop, this ways they don't cause
			//subscript exceptions, the top dying method causes a break to prevent
			//the buttom dying code from breaking things. there's probably a better
			//way to do this.
			//Dying from old age
			if (wheelers[i]->minutesToLive > 0) {
				if (!settings->pause) { wheelers[i]->minutesToLive--; }
			} else {
				Wheeler *dying = wheelers[i];
				wheelersToDelete.push_back(*dying);
				wheelers.erase( std::find(wheelers.begin(), wheelers.end(), dying ) );
				break;
			}
			//Dying
			if (wheelers[i]->health < 0) {
				Wheeler *dying = wheelers[i];
				wheelersToDelete.push_back(*dying);
				wheelers.erase( std::find(wheelers.begin(), wheelers.end(), dying ) );
			} else {
				if (wheelers[i]->healthDownCounter > 0) {
					if (!settings->pause) { wheelers[i]->healthDownCounter--; }
				} else { 
					//Movement sensing
					wheelers[i]->setNewShortPos(wheelers[i]->cart->GetPosition());
					if (wheelers[i]->hasGoodShortDistance && wheelers[i]->longDistance() < 1.0f) { 
						wheelers[i]->turnAround();
					}
					if (wheelers[i]->longCounter < 4) {
						wheelers[i]->longCounter++;
					} else {
						wheelers[i]->setNewLongPos(wheelers[i]->cart->GetPosition());
						if (wheelers[i]->hasGoodLongDistance && wheelers[i]->longDistance() < 5.0f) { 
							wheelers[i]->turnAround();
						}
						wheelers[i]->longCounter = 0;
					}
					//Health stuff again
					wheelers[i]->healthDownCounter = 60;
					wheelers[i]->health--;
				}
			}
		}

		World::Step(settings);

		//Taking care of all the dying.
		for (int i = 0; i < wheelersToDelete.size(); i ++) {
			if (cW >= wheelers.size()) {
				cW = 0;
			}
			wheelersToDelete[i].die();
			if (wheelers.size() == 0) {
				activeWheeler = NULL;
			}
		}
		for (int i = 0; i < grassToDelete.size(); i ++) {
			grassToDelete[i].die();
		}
		for (int i = 0; i < seedsToDelete.size(); i ++) {
			seedsToDelete[i].destroy();
		}
		wheelersToDelete.clear();
		grassToDelete.clear();
		seedsToDelete.clear();
	}

	static World* Create() {
		return new SandboxWorld;
	}

private:

	//Random number generation - I tried putting this stuff in it's own file, I failed :(
    float32 randomNumber(float32 low = 0, float32 high = 1) {
		float retNum;
		retNum = low + (float)rand() / ((float)RAND_MAX / (high-low));
        return retNum;
    }

	//Live creatures
	vector<Wheeler*> wheelers;
	vector<Grass*> grasses;
	vector<GrassSpawner*> grassSpawners;
	vector<Seed*> seeds;
	//Dying creatures
	vector<Wheeler> wheelersToDelete;
	vector<Grass> grassToDelete;
	vector<Seed> seedsToDelete;

	//Used to place random generated Wheelers
	float32 xpos;

	//Used for tracking Wheelers
	int cW;
	bool following;

	//Grass spawn counter - Kind of self explinatory
	int grassSpawnCounter;
};
#endif