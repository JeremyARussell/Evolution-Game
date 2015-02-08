#ifndef MTN_RIDING_H
#define MTN_RIDING_H
#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include "Build\PolyCars\Wheeler.h"
#include "Build\PolyCars\Seed.h"
#include "Build\PolyCars\Grass.h"
#include "Build\PolyCars\GrassSpawner.h"

#include "SOIL\SOIL.h";


enum _entityCategory {///HACKY - Just use the bitwise values inside the wheeler class, etc.
    SEED		     =  0x0001,
    GRASS            =  0x0002,
    WHEELER		     =  0x0004,
    //CARNIVORE	     =  0x0008,
    NON_INTERACTOR   =  0x0010,
    WALL             =  0x0020,
    GRASS_SENSOR	 =  0x0040,
};

enum _power {
    GRAB,
    SELECT,
	DESTROY,
	SPAWN_SEED,
	SPAWN_WHEELER//,
	//FEED
};

//Sensing
class WheelerContactListener : public b2ContactListener { 
	void BeginContact(b2Contact* contact) {
		b2Fixture* fixtureA = contact->GetFixtureA();
		b2Fixture* fixtureB = contact->GetFixtureB();

		//Wheeler colliding with grass
//		if (fixtureA->GetFilterData().categoryBits == WHEELER && 
//			fixtureB->GetFilterData().categoryBits == GRASS) {
//			Grass* activeGrass = (Grass *)fixtureB->GetBody()->GetUserData();
//			Wheeler* activeWheeler = (Wheeler *)fixtureA->GetBody()->GetUserData();
//			if (activeWheeler->health < 30) {
//				activeGrass->beingEaten = true;
//				activeWheeler->needsToReproduce = true;
//				activeWheeler->health = 50;
//			}
//		}
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
		if (fixtureA->GetFilterData().categoryBits == WALL && 
			fixtureB->GetFilterData().categoryBits == SEED) {
				Seed* seedTest = (Seed *)fixtureB->GetBody()->GetUserData(); 
				//fixtureB->GetBody()->GetPosition();
				seedTest->timetoSeed = true;
				//Make grass here
				//grasses.push_back(new Grass(m_world, randomNumber(-125.0f, 125.0f),  0.0f));
		}
		if (fixtureA->GetFilterData().categoryBits == GRASS_SENSOR && 
			fixtureB->GetFilterData().categoryBits == GRASS_SENSOR) {
				Grass* seedTest = (Grass *)fixtureB->GetBody()->GetUserData(); 
				Grass* seedTest2 = (Grass *)fixtureA->GetBody()->GetUserData(); 
				seedTest->crowded = true;
				seedTest2->crowded = true;
		}
//		if (fixtureB->GetFilterData().categoryBits == NON_INTERACTOR && 
//			fixtureA->GetFilterData().categoryBits == SEED) {
//		}
	}
  
    void EndContact(b2Contact* contact) {

    }
};

WheelerContactListener thisWheelerContactListener;

class QueryCallback : public b2QueryCallback {
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

class MountainRiding : public World {
public:

	_power activePower;

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
		ofstream saveFile("MountainRidingWorld.sav");

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
		ifstream loadFile("MountainRidingWorld.sav");

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

	//Level building, and default creature placement go here.
    MountainRiding() {
		cW = 0;
		activeWheeler = NULL;
		grassSpawnCounter = 0;
		following = false;
		activePower = GRAB;

		x = 0.0f;
		y = 0.0f;
		rsize = 8;
		i[0] = 0;
		i[1] = 0;
		i[2] = 0;
		i[3] = 0;
		i[4] = 0;
		i[5] = 0;
		i[6] = 0;
		i[7] = 0;

		m_world->SetContactListener(&thisWheelerContactListener);
		#pragma region World 
		{
			//The World's boundaries
			b2BodyDef bd;
			b2Body* world = m_world->CreateBody(&bd);
			
			//Ground
			b2Vec2 vsGround1[4];
			vsGround1[0].Set(-225.0f, 1.0f);
			vsGround1[1].Set(-225.0f, -1.0f);
			vsGround1[2].Set(225.0f, -1.0f);
			vsGround1[3].Set(225.0f, 1.0f);

			b2PolygonShape polyGround1;
			polyGround1.Set(vsGround1, 4);

			b2FixtureDef fdGround1;
			fdGround1.shape = &polyGround1;
			fdGround1.density = 0.0f;
			fdGround1.filter.categoryBits = WALL;

			world->CreateFixture(&fdGround1);
			//Ground #2
			//b2Vec2 vsGround2[4];
			//vsGround2[0].Set(-225.0f, 1.0f);
			//vsGround2[1].Set(-200.0f, -1.0f);
			//vsGround2[2].Set(225.0f, -1.0f);
			//vsGround2[3].Set(225.0f, 1.0f);

			//b2PolygonShape polyGround2;
			//polyGround2.Set(vsGround2, 4);

			//b2FixtureDef fdGround2;
			//fdGround2.shape = &polyGround2;
			//fdGround2.density = 0.0f;
			//fdGround2.filter.categoryBits = WALL;

			//world->CreateFixture(&fdGround2);
			//Left Wall
			b2Vec2 vsLeftWall[4];
			vsLeftWall[0].Set(-225.0f, 68.0f);
			vsLeftWall[1].Set(-225.0f, 1.0f);
			vsLeftWall[2].Set(-223.0f, 1.0f);
			vsLeftWall[3].Set(-223.0f, 68.0f);

			b2PolygonShape polyLeftWall;
			polyLeftWall.Set(vsLeftWall, 4);

			b2FixtureDef fdLeftWall;
			fdLeftWall.shape = &polyLeftWall;
			fdLeftWall.density = 0.0f;
			fdLeftWall.filter.categoryBits = NON_INTERACTOR;

			world->CreateFixture(&fdLeftWall);
			//Ceiling
			b2Vec2 vsCeiling[4];
			vsCeiling[0].Set(-225.0f, 70.0f);
			vsCeiling[1].Set(-225.0f, 68.0f);
			vsCeiling[2].Set(225.0f, 68.0f);
			vsCeiling[3].Set(225.0f, 70.0f);

			b2PolygonShape polyCeiling;
			polyCeiling.Set(vsCeiling, 4);

			b2FixtureDef fdCeiling;
			fdCeiling.shape = &polyCeiling;
			fdCeiling.density = 0.0f;
			fdCeiling.filter.categoryBits = NON_INTERACTOR;

			world->CreateFixture(&fdCeiling);	
			//Right Wall
			b2Vec2 vsRightWall[4];
			vsRightWall[0].Set(223.0f, 68.0f);
			vsRightWall[1].Set(223.0f, 1.0f);
			vsRightWall[2].Set(225.0f, 1.0f);
			vsRightWall[3].Set(225.0f, 68.0f);

			b2PolygonShape polyRightWall;
			polyRightWall.Set(vsRightWall, 4);

			b2FixtureDef fdRightWall;
			fdRightWall.shape = &polyRightWall;
			fdRightWall.density = 0.0f;
			fdRightWall.filter.categoryBits = NON_INTERACTOR;

			world->CreateFixture(&fdRightWall);

		}
		#pragma endregion Ground and Walls

		/*{
			//Building the Hexagon for throwing away unwanted Wheelers
			b2BodyDef bd;
			b2Body* hex = m_world->CreateBody(&bd);
			
			b2Vec2 vs[12];

			float32 wallLength = 15.0f;
			b2Vec2 S;
			S.Set(-wallLength / 2, 40.0f);

			float32 A = wallLength;//Wall length, and Center to vertice
			float32 B = A / 2;
			float32 C = A * 0.866;//The 0.866 is sin(60)
			
			//Outside Hex
			vs[0].Set( -B + S.x		  ,  C + S.y);
			vs[1].Set( -A + S.x		  ,      S.y);
			vs[2].Set( -B + S.x		  , -C + S.y);
			vs[3].Set(  B + S.x		  , -C + S.y);
			vs[4].Set(  A + S.x		  ,      S.y);
			vs[5].Set(  B + S.x		  ,  C + S.y);
			//Inside hex - to create a hole in the top - Zoom in to see what I mean
			vs[6].Set(  B + S.x - 0.2 ,  C + S.y);
			vs[7].Set(  A + S.x - 0.2 ,      S.y);
			vs[8].Set(  B + S.x - 0.2 , -C + S.y + 0.2);
			vs[9].Set( -B + S.x + 0.2 , -C + S.y + 0.2);
			vs[10].Set(-A + S.x + 0.2 ,      S.y);
			vs[11].Set(-B + S.x + 0.2 ,  C + S.y);

			b2ChainShape loop;
			loop.CreateLoop(vs, 12);
			b2FixtureDef fd;
			fd.shape = &loop;
			fd.density = 0.0f;
			fd.filter.categoryBits = WALL;

			hex->CreateFixture(&fd);
		}*/


		#pragma region Spawners 
		{
			float32 spx; float32 spy;
			spx = -50.0f; spy = 40.0f;
			grassSpawners.push_back(new GrassSpawner(m_world, spx, spy));

			spx = -40.0f; spy = 45.0f;
			grassSpawners.push_back(new GrassSpawner(m_world, spx, spy));
			
			spx = -30.0f; spy = 40.0f;
			grassSpawners.push_back(new GrassSpawner(m_world, spx, spy));
			
			spx = 15.0f; spy = 43.0f;
			grassSpawners.push_back(new GrassSpawner(m_world, spx, spy));
			
			spx = 20; spy = 40.0f;
			grassSpawners.push_back(new GrassSpawner(m_world, spx, spy));
		}
		#pragma endregion Units for spawning
	}
	
	GLuint tex_2d[8];

	int i[8];//8 Texture for now, one for the background, 3 for powers, probably more.
	float32 phX[8];
	float32 phY[8];
	float32 phWidth[8];
	float32 phHeight[8];

	GLfloat x;	  //Not using atm
	GLfloat y;	  //Not using atm
	GLfloat rsize;//Not using atm

	void RenderUI(Settings* settings) {

		glEnable(GL_TEXTURE_2D);
		glColor3f(1.0, 1.0, 1.0);
		
		if (i[0] == 0) {
			glGenTextures(1, &tex_2d[0]);

			tex_2d[0] = SOIL_load_OGL_texture("D:\\Private\\The Real 2D\\Code\\PolyCars\\Framework\\assets\\images\\Simple_PowersHUD.bmp",
				SOIL_LOAD_RGBA, SOIL_CREATE_NEW_ID, SOIL_FLAG_NTSC_SAFE_RGB );
			i[0] = 1;
		}

		glBindTexture(GL_TEXTURE_2D, tex_2d[0]);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		phX[0] = -9.25f;
		phY[0] = 20.75f;
		phHeight[0] = 3.5f;
		phWidth[0] = 19.0f;

		//The main HUD background
		glBegin(GL_POLYGON);				   // Size + position							|--Aplying zoom			|--Adding the new center
											   // from center + pad adjust					v--to retain size		v--to maintain position
		glTexCoord2f(0.0, 1.0); glVertex2f((   (( phX[0]				) * settings->zoomLevel)  + settings->viewCenter.x ),    
											   (( phY[0] - phHeight[0] ) * settings->zoomLevel)  + settings->viewCenter.y );		
												//Left Bottom 

		glTexCoord2f(1.0, 1.0); glVertex2f((   (( phX[0] + phWidth[0]	) * settings->zoomLevel)  + settings->viewCenter.x ),	
											   (( phY[0] - phHeight[0] ) * settings->zoomLevel)  + settings->viewCenter.y );		
												//Right Bottom 

		glTexCoord2f(1.0, 0.0); glVertex2f((   (( phX[0] + phWidth[0]	) * settings->zoomLevel)  + settings->viewCenter.x ),
											   (( phY[0]				) * settings->zoomLevel)  + settings->viewCenter.y );
												//Right Top

		glTexCoord2f(0.0, 0.0); glVertex2f((   (( phX[0]				) * settings->zoomLevel)  + settings->viewCenter.x ),	
											   (( phY[0]				) * settings->zoomLevel)  + settings->viewCenter.y );
		glEnd();								//Left Top
		/////

		//Hilighter
		if (i[7] == 0) {
			glGenTextures(1, &tex_2d[7]);
			tex_2d[7] = SOIL_load_OGL_texture ( "D:\\Private\\The Real 2D\\Code\\PolyCars\\Framework\\assets\\images\\Simple_Powers_Hilighter.bmp", 
				SOIL_LOAD_RGBA, SOIL_CREATE_NEW_ID, SOIL_FLAG_NTSC_SAFE_RGB );
			i[7] = 1;
		}

		glBindTexture(GL_TEXTURE_2D, tex_2d[7]);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		switch (activePower) {
		case GRAB:
			phX[7] = -9.0f;
			phY[7] = 20.5f;
			break;
		case SELECT:
			phX[7] = -6.0f;
			phY[7] = 20.5f;
			break;
		case DESTROY:
			phX[7] = -3.0f;
			phY[7] = 20.5f;
			break;
		case SPAWN_SEED:
			phX[7] = 0.0f;
			phY[7] = 20.5f;
			break;
		case SPAWN_WHEELER:
			phX[7] = 3.0f;
			phY[7] = 20.5f;
			break;
		}

		//phX[7] = -9.0f;
		//phY[7] = 20.5f;
		phHeight[7] = 3.0f;
		phWidth[7] = 3.0f;

		glBegin(GL_POLYGON);				   // Size + position							|--Aplying zoom			|--Adding the new center
											   // from center + pad adjust					v--to retain size		v--to maintain position
		glTexCoord2f(0.0, 1.0); glVertex2f((   (( phX[7]				) * settings->zoomLevel)  + settings->viewCenter.x ),    
											   (( phY[7] - phHeight[7] ) * settings->zoomLevel)  + settings->viewCenter.y );		
												//Left Bottom 

		glTexCoord2f(1.0, 1.0); glVertex2f((   (( phX[7] + phWidth[7]	) * settings->zoomLevel)  + settings->viewCenter.x ),	
											   (( phY[7] - phHeight[7] ) * settings->zoomLevel)  + settings->viewCenter.y );		
												//Right Bottom 

		glTexCoord2f(1.0, 0.0); glVertex2f((   (( phX[7] + phWidth[7]	) * settings->zoomLevel)  + settings->viewCenter.x ),
											   (( phY[7]				) * settings->zoomLevel)  + settings->viewCenter.y );
												//Right Top

		glTexCoord2f(0.0, 0.0); glVertex2f((   (( phX[7]				) * settings->zoomLevel)  + settings->viewCenter.x ),	
											   (( phY[7]				) * settings->zoomLevel)  + settings->viewCenter.y );
		glEnd();								//Left Top 
		///
		/////

		///Grab
		if (i[1] == 0) {
			glGenTextures(1, &tex_2d[1]);
			tex_2d[1] = SOIL_load_OGL_texture ( "D:\\Private\\The Real 2D\\Code\\PolyCars\\Framework\\assets\\images\\Simple_Powers_Grab.bmp", 
				SOIL_LOAD_RGBA, SOIL_CREATE_NEW_ID, SOIL_FLAG_NTSC_SAFE_RGB );
			i[1] = 1;
		}

		glBindTexture(GL_TEXTURE_2D, tex_2d[1]);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		phX[1] = -8.75f;
		phY[1] = 20.25f;
		phHeight[1] = 2.5f;
		phWidth[1] = 2.5f;

		glBegin(GL_POLYGON);				   // Size + position							|--Aplying zoom			|--Adding the new center
											   // from center + pad adjust					v--to retain size		v--to maintain position
		glTexCoord2f(0.0, 1.0); glVertex2f((   (( phX[1]				 ) * settings->zoomLevel)  + settings->viewCenter.x ),    
											   (( phY[1] - phHeight[1] ) * settings->zoomLevel)  + settings->viewCenter.y );		
												//Left Bottom 

		glTexCoord2f(1.0, 1.0); glVertex2f((   (( phX[1] + phWidth[1]	 ) * settings->zoomLevel)  + settings->viewCenter.x ),	
											   (( phY[1] - phHeight[1] ) * settings->zoomLevel)  + settings->viewCenter.y );		
												//Right Bottom 

		glTexCoord2f(1.0, 0.0); glVertex2f((   (( phX[1] + phWidth[1]	) * settings->zoomLevel)  + settings->viewCenter.x ),
											   (( phY[1]				) * settings->zoomLevel)  + settings->viewCenter.y );
												//Right Top

		glTexCoord2f(0.0, 0.0); glVertex2f((   (( phX[1]				) * settings->zoomLevel)  + settings->viewCenter.x ),	
											   (( phY[1]				) * settings->zoomLevel)  + settings->viewCenter.y );
		glEnd();								//Left Top 

		///Select
		if (i[2] == 0) {
			glGenTextures(1, &tex_2d[2]);
			tex_2d[2] = SOIL_load_OGL_texture ( "D:\\Private\\The Real 2D\\Code\\PolyCars\\Framework\\assets\\images\\Simple_Powers_Select.bmp", 
				SOIL_LOAD_RGBA, SOIL_CREATE_NEW_ID, SOIL_FLAG_NTSC_SAFE_RGB );
			i[2] = 1;
		}

		glBindTexture(GL_TEXTURE_2D, tex_2d[2]);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		phX[2] = -5.75f;
		phY[2] = 20.25f;
		phHeight[2] = 2.5f;
		phWidth[2] = 2.5f;

		glBegin(GL_POLYGON);				   // Size + position							|--Aplying zoom			|--Adding the new center
											   // from center + pad adjust					v--to retain size		v--to maintain position
		glTexCoord2f(0.0, 1.0); glVertex2f((   (( phX[2]				 ) * settings->zoomLevel)  + settings->viewCenter.x ),    
											   (( phY[2] - phHeight[2] ) * settings->zoomLevel)  + settings->viewCenter.y );		
												//Left Bottom 

		glTexCoord2f(1.0, 1.0); glVertex2f((   (( phX[2] + phWidth[2]	 ) * settings->zoomLevel)  + settings->viewCenter.x ),	
											   (( phY[2] - phHeight[2] ) * settings->zoomLevel)  + settings->viewCenter.y );		
												//Right Bottom 

		glTexCoord2f(1.0, 0.0); glVertex2f((   (( phX[2] + phWidth[2]	) * settings->zoomLevel)  + settings->viewCenter.x ),
											   (( phY[2]				) * settings->zoomLevel)  + settings->viewCenter.y );
												//Right Top

		glTexCoord2f(0.0, 0.0); glVertex2f((   (( phX[2]				) * settings->zoomLevel)  + settings->viewCenter.x ),	
											   (( phY[2]				) * settings->zoomLevel)  + settings->viewCenter.y );
		glEnd();								//Left Top 

		//Destroy
		if (i[3] == 0) {
			glGenTextures(1, &tex_2d[3]);
			tex_2d[3] = SOIL_load_OGL_texture ( "D:\\Private\\The Real 2D\\Code\\PolyCars\\Framework\\assets\\images\\Simple_Powers_Destroy.bmp", 
				SOIL_LOAD_RGBA, SOIL_CREATE_NEW_ID, SOIL_FLAG_NTSC_SAFE_RGB );
			i[3] = 1;
		}

		glBindTexture(GL_TEXTURE_2D, tex_2d[3]);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		phX[3] = -2.75f;
		phY[3] = 20.25f;
		phHeight[3] = 2.5f;
		phWidth[3] = 2.5f;

		glBegin(GL_POLYGON);				   // Size + position							|--Aplying zoom			|--Adding the new center
											   // from center + pad adjust					v--to retain size		v--to maintain position
		glTexCoord2f(0.0, 1.0); glVertex2f((   (( phX[3]				) * settings->zoomLevel)  + settings->viewCenter.x ),    
											   (( phY[3] - phHeight[3] ) * settings->zoomLevel)  + settings->viewCenter.y );		
												//Left Bottom 

		glTexCoord2f(1.0, 1.0); glVertex2f((   (( phX[3] + phWidth[3]	) * settings->zoomLevel)  + settings->viewCenter.x ),	
											   (( phY[3] - phHeight[3] ) * settings->zoomLevel)  + settings->viewCenter.y );		
												//Right Bottom 

		glTexCoord2f(1.0, 0.0); glVertex2f((   (( phX[3] + phWidth[3]	) * settings->zoomLevel)  + settings->viewCenter.x ),
											   (( phY[3]				) * settings->zoomLevel)  + settings->viewCenter.y );
												//Right Top

		glTexCoord2f(0.0, 0.0); glVertex2f((   (( phX[3]				) * settings->zoomLevel)  + settings->viewCenter.x ),	
											   (( phY[3]				) * settings->zoomLevel)  + settings->viewCenter.y );
		glEnd();								//Left Top 

		//Seeds
		if (i[4] == 0) {
			glGenTextures(1, &tex_2d[4]);
			tex_2d[4] = SOIL_load_OGL_texture ( "D:\\Private\\The Real 2D\\Code\\PolyCars\\Framework\\assets\\images\\Simple_Powers_Seed.bmp", 
				SOIL_LOAD_RGBA, SOIL_CREATE_NEW_ID, SOIL_FLAG_NTSC_SAFE_RGB );
			i[4] = 1;
		}

		glBindTexture(GL_TEXTURE_2D, tex_2d[4]);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		phX[4] = 0.25f;
		phY[4] = 20.25f;
		phHeight[4] = 2.5f;
		phWidth[4] = 2.5f;

		glBegin(GL_POLYGON);				   // Size + position							|--Aplying zoom			|--Adding the new center
											   // from center + pad adjust					v--to retain size		v--to maintain position
		glTexCoord2f(0.0, 1.0); glVertex2f((   (( phX[4]				) * settings->zoomLevel)  + settings->viewCenter.x ),    
											   (( phY[4] - phHeight[4] ) * settings->zoomLevel)  + settings->viewCenter.y );		
												//Left Bottom 

		glTexCoord2f(1.0, 1.0); glVertex2f((   (( phX[4] + phWidth[4]	) * settings->zoomLevel)  + settings->viewCenter.x ),	
											   (( phY[4] - phHeight[4] ) * settings->zoomLevel)  + settings->viewCenter.y );		
												//Right Bottom 

		glTexCoord2f(1.0, 0.0); glVertex2f((   (( phX[4] + phWidth[4]	) * settings->zoomLevel)  + settings->viewCenter.x ),
											   (( phY[4]				) * settings->zoomLevel)  + settings->viewCenter.y );
												//Right Top

		glTexCoord2f(0.0, 0.0); glVertex2f((   (( phX[4]				) * settings->zoomLevel)  + settings->viewCenter.x ),	
											   (( phY[4]				) * settings->zoomLevel)  + settings->viewCenter.y );
		glEnd();								//Left Top 

		//Wheelers
		if (i[5] == 0) {
			glGenTextures(1, &tex_2d[5]);
			tex_2d[5] = SOIL_load_OGL_texture ( "D:\\Private\\The Real 2D\\Code\\PolyCars\\Framework\\assets\\images\\Simple_Powers_Wheeler.bmp", 
				SOIL_LOAD_RGBA, SOIL_CREATE_NEW_ID, SOIL_FLAG_NTSC_SAFE_RGB );
			i[5] = 1;
		}

		glBindTexture(GL_TEXTURE_2D, tex_2d[5]);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		//float32 padWheeler = 0;
		//if (activePower == SPAWN_WHEELER) {
		//	padWheeler = 0.25f;
		//} else {
		//	padWheeler = 0;
		//}

		phX[5] = 3.25f;
		phY[5] = 20.25f;
		phHeight[5] = 2.5f;
		phWidth[5] = 2.5f;

		glBegin(GL_POLYGON);				   // Size + position							|--Aplying zoom			|--Adding the new center
											   // from center + pad adjust					v--to retain size		v--to maintain position
		glTexCoord2f(0.0, 1.0); glVertex2f((   (( phX[5]				) * settings->zoomLevel)  + settings->viewCenter.x ),    
											   (( phY[5] - phHeight[5] ) * settings->zoomLevel)  + settings->viewCenter.y );		
												//Left Bottom 

		glTexCoord2f(1.0, 1.0); glVertex2f((   (( phX[5] + phWidth[5]	) * settings->zoomLevel)  + settings->viewCenter.x ),	
											   (( phY[5] - phHeight[5] ) * settings->zoomLevel)  + settings->viewCenter.y );		
												//Right Bottom 

		glTexCoord2f(1.0, 0.0); glVertex2f((   (( phX[5] + phWidth[5]	) * settings->zoomLevel)  + settings->viewCenter.x ),
											   (( phY[5]				) * settings->zoomLevel)  + settings->viewCenter.y );
												//Right Top

		glTexCoord2f(0.0, 0.0); glVertex2f((   (( phX[5]				) * settings->zoomLevel)  + settings->viewCenter.x ),	
											   (( phY[5]				) * settings->zoomLevel)  + settings->viewCenter.y );
		glEnd();								//Left Top 
		

		glDisable(GL_TEXTURE_2D);
	}

	void MountainRiding::MouseDown(const b2Vec2& p) {
										
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



		if (activePower == SPAWN_SEED) {
			float32 xt = randomNumber(-3.0f, 3.0f);
			float32 yt = randomNumber(1.0f, 3.0f);
			
			//new Seed(m_world, b2Vec2(xt, yt)
			seeds.push_back(new Seed(m_world, callback.m_point, b2Vec2(xt, yt)));
			
		}

		if (activePower == SPAWN_WHEELER) {
			if (wheelers.size() < 100) {
				cW = 0;
				wheelers.push_back(new Wheeler(m_world,  callback.m_point.x, callback.m_point.y));
			}			
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
					//callback.m_fixture->GetFilterData().categoryBits == GRASS |
					callback.m_fixture->GetFilterData().categoryBits == GRASS_SENSOR) return;

				Wheeler *testWheeler = (Wheeler *)body->GetUserData();
				activeWheeler = testWheeler;
			}
			if (activePower == DESTROY) {
				b2Body* body = callback.m_fixture->GetBody();

				if (callback.m_fixture->GetFilterData().categoryBits == SEED |
					//callback.m_fixture->GetFilterData().categoryBits == GRASS | - Grass doesn't trigger a callback thingy anyways
					callback.m_fixture->GetFilterData().categoryBits == GRASS_SENSOR) return;
				
				Wheeler *testWheeler = (Wheeler *)body->GetUserData();
				wheelersToDelete.push_back(*testWheeler);
				wheelers.erase( std::find(wheelers.begin(), wheelers.end(), testWheeler ) );
				activeWheeler = NULL;
				testWheeler = NULL;
			}
		}
	}

	void Keyboard(unsigned char key) {
		switch (key) {
		case '1':
			activePower = GRAB;
			break;
		case '2':
			activePower = SELECT;
			break;
		case '3':
			activePower = DESTROY;
			break;
		case '4':
			activePower = SPAWN_SEED;
			break;
		case '5':
			activePower = SPAWN_WHEELER;
			break;
		}
	}

	void Step(Settings* settings) {
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
		m_debugDraw.DrawString(10, m_textLine, "Hit 'W' to create some grass seeds, hit 'A' to randomly generate Wheelers");
		m_textLine += 15;

		//RenderUI(settings);


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

		//Timer and code for the spawning of grass
		//if (!settings->pause) { grassSpawnCounter++; }
		//if (grassSpawnCounter > settings->grassSpawnRate * 60) {
		//	grassSpawnCounter = 0;
		//	grasses.push_back(new Grass(m_world, randomNumber(-125.0f, 125.0f),  0.0f));
		//}

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
		return new MountainRiding;
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