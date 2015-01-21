#ifndef MTN_RIDING_H
#define MTN_RIDING_H
#pragma once

#include <fstream>
#include <string>
#include "Build\PolyCars\Wheeler.h"
#include "Build\PolyCars\Grass.h"

enum _entityCategory {///HACKY - Just use the bitwise values inside the wheeler class, etc.
    //EMPTY		     =  0x0001,
    GRASS            =  0x0002,
    WHEELER		     =  0x0004,
    //CARNIVORE	     =  0x0008,
    NON_INTERACTOR   =  0x0010,
    //EMPTY          =  0x0020,
    //EMPTY		     =  0x0040,
  };

//Sensing
class WheelerContactListener : public b2ContactListener { 
	void BeginContact(b2Contact* contact) {
		b2Fixture* fixtureA = contact->GetFixtureA();
		b2Fixture* fixtureB = contact->GetFixtureB();

		//Wheeler colliding with grass
		if (fixtureA->GetFilterData().categoryBits == 0x0004 && 
			fixtureB->GetFilterData().categoryBits == 0x0002) {
			Grass* activeGrass = (Grass *)fixtureB->GetBody()->GetUserData();
			Wheeler* activeWheeler = (Wheeler *)fixtureA->GetBody()->GetUserData();
			if (activeWheeler->health < 30) {
				activeGrass->beingEaten = true;
				activeWheeler->needsToReproduce = true;
				activeWheeler->health = 50;
			}
		}
		if (fixtureB->GetFilterData().categoryBits == 0x0004 && 
			fixtureA->GetFilterData().categoryBits == 0x0002) {
			Grass* activeGrass = (Grass *)fixtureA->GetBody()->GetUserData();
			Wheeler* activeWheeler = (Wheeler *)fixtureB->GetBody()->GetUserData();
			if (activeWheeler->health < 30) {
				activeGrass->beingEaten = true;
				activeWheeler->needsToReproduce = true;
				activeWheeler->health = 50;
			}
		}
	}
  
    void EndContact(b2Contact* contact) {

    }
};

WheelerContactListener thisWheelerContactListener;

class MountainRiding : public World {
public:

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

		m_world->SetContactListener(&thisWheelerContactListener);

		{
			//The box that is the ground, ceiling and walls which keep everyone inside.
			b2BodyDef bd;
			b2Body* world = m_world->CreateBody(&bd);
			
			//Setup an array to hold the vertices of the world.
			b2Vec2 vs[12];
			vs[0].Set(-225.0f, 70.0f);
			vs[1].Set(-225.0f, 10.0f);
			vs[2].Set(-175.0f, 10.0f);
			vs[3].Set(-125.0f, 0.0f);
			vs[4].Set(-75.0f, 0.0f);
			vs[5].Set(-25.0f, 0.0f);
			vs[6].Set(25.0f, 0.0f);
			vs[7].Set(75.0f, 0.0f);
			vs[8].Set(125.0f, 0.0f);
			vs[9].Set(175.0f, 0.0f);
			vs[10].Set(225.0f, 10.0f);
			vs[11].Set(225.0f, 70.0f);

			//Shape and fixture for the world's boundaries.
			b2ChainShape loop;
			loop.CreateLoop(vs, 12);
			b2FixtureDef fd;
			fd.shape = &loop;
			fd.density = 0.0f;
			fd.filter.categoryBits = NON_INTERACTOR;

			//Add the boundaries to our world
			world->CreateFixture(&fd);
		}

		{
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
			fd.filter.categoryBits = NON_INTERACTOR;

			hex->CreateFixture(&fd);
		}		
	}
	
	void Keyboard(unsigned char key) {
		switch (key) {
		case 'a':
			if (wheelers.size() < 30) {
				cW = 0;
				xpos = wheelers.size() * 5 - 50.0f;
				wheelers.push_back(new Wheeler(m_world,  xpos, 5.0f));
			}			
			break;
		case 'w':
			grasses.push_back(new Grass(m_world, randomNumber(-125.0f, 125.0f),  0.5f));
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
		m_debugDraw.DrawString(10, m_textLine, "Hit 'W' to generate some grass, hit 'A' to randomly generate Wheelers");
		m_textLine += 15;

		//Check if grass is being eaten this step
		for (int i = 0; i < grasses.size(); i ++) {
			if (grasses[i]->beingEaten) {
				Grass *dying = grasses[i];
				grassToDelete.push_back(*dying);
				grasses.erase( std::find(grasses.begin(), grasses.end(), dying ) );
			}
		}

		//Timer and code for the spawning of grass
		if (!settings->pause) { grassSpawnCounter++; }
		if (grassSpawnCounter > settings->grassSpawnRate * 60) {
			grassSpawnCounter = 0;
			grasses.push_back(new Grass(m_world, randomNumber(-125.0f, 125.0f),  0.5f));
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
				wheelers.push_back(new Wheeler(*wheelers[i], m_world,  wheelers[i]->cart->GetPosition().x,  wheelers[i]->cart->GetPosition().y + 5.0f));
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
		wheelersToDelete.clear();
		grassToDelete.clear();
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
	//Dying creatures
	vector<Wheeler> wheelersToDelete;
	vector<Grass> grassToDelete;

	//Used to place random generated Wheelers
	float32 xpos;

	//Used for tracking Wheelers
	int cW;
	bool following;

	//Grass spawn counter - Kind of self explinatory
	int grassSpawnCounter;
};
#endif