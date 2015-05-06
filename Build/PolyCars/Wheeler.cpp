#include "Wheeler.h"
#include "Build\PolyCars\collision_enums.h"

#include <stdio.h>
using namespace std;
 
#define DEGTORAD 0.0174532925199432957f
#define RADTODEG 57.295779513082320876f

//Rendering for the Wheeler
void Wheeler::render() {

    b2Vec2 pos = cart->GetPosition();
  
    glPushMatrix();
    glTranslatef( pos.x, pos.y, 0 );
 
	glColor3f(1,1,1);//white
  
	glRasterPos2i(2, 2);

	char buf[256];
	sprintf_s(buf, "%d", health);

	glutBitmapString(GLUT_BITMAP_HELVETICA_10, (const unsigned char *)buf);

    glPopMatrix();
}
	
//Constructor for a fresh randomly generated Wheeler
Wheeler::Wheeler(b2World *m_world, float32 x,float32 y) {
	myWorld = m_world;

    totalAngleValue = 0;

	generateFreshChromosome();

	buildWheeler(x, y);
}

//Constructor for a baby Wheeler
Wheeler::Wheeler(Wheeler parent, b2World *m_world, float32 x,float32 y) {
	myWorld = m_world;

    totalAngleValue = 0;

	generateChildChromosome(parent);

	buildWheeler(x, y);
}

//Constructor for a loaded Wheeler
Wheeler::Wheeler(b2World *m_world, vector<float32> genes) {
	myWorld = m_world;

    totalAngleValue = 0;

	loadWheeler(genes);
}

//Code to generate some random genes
void Wheeler::generateFreshChromosome() {
	spinDirection = -6 * b2_pi;//Probably should be called spinSpeed

	MIN_WHEEL = 0.1f;//Wheel radius or diameter
	MAX_WHEEL = 1.5f;
	MIN_CART= 0.1f;
	MAX_CART = 3.0f;
    m_wheelProb = 0.45f;//p_wheelProb;

	health = 15;

    int spoke_index = 0;
    while (spoke_index < 8) {
		spokeAngles[spoke_index] = randomNumber(0.05f, 1.0f);
        totalAngleValue = totalAngleValue + spokeAngles[spoke_index];
		spokeLengths[spoke_index] = randomNumber(MIN_CART, MAX_CART);
		hasWheel[spoke_index] = randomNumber(0.01f, 1.0f) < m_wheelProb ? (randomNumberInt(0, (VECTOR_NUM - 1))) : (-1);
		axleAngle[spoke_index] = randomNumber(0, 5 * b2_pi);
		wheelRadius[spoke_index] = randomNumber(MIN_WHEEL, MAX_WHEEL);

        spoke_index++;
    }
}

//Code to generate genes based off of parent, with mutation
void Wheeler::generateChildChromosome(Wheeler parent) {
	health = 50;

	spinDirection = parent.spinDirection;//Probably should be called spinSpeed

    int spoke_index = 0;
    while (spoke_index < 8) {
		
		spokeAngles[spoke_index] = parent.spokeAngles[spoke_index];    //randomNumber(0.05f, 1.0f);
        totalAngleValue = totalAngleValue + spokeAngles[spoke_index];
		
		spokeLengths[spoke_index] = parent.spokeLengths[spoke_index] + randomNumber(-0.3f, 0.3f);
		if (spokeLengths[spoke_index] < 0) {spokeLengths[spoke_index] = 0.3f;}
		hasWheel[spoke_index] = parent.hasWheel[spoke_index];
		axleAngle[spoke_index] = parent.axleAngle[spoke_index] + randomNumber(-1.0f, 1.0f * b2_pi);
		wheelRadius[spoke_index] = parent.wheelRadius[spoke_index] + randomNumber(-0.2f, 0.2f);

        spoke_index++;
    }
}

//Code to piece together a Wheeler
void Wheeler::buildWheeler(float32 x,float32 y) {
	needsToReproduce = false;
	reproductionCounter = 0;
	minutesToLive = 28800;//8 minutes of life in a 1/60 timestep

	//Initializing vars
	oldShortPos = b2Vec2(0.0f, 0.0f);
	newShortPos = b2Vec2(0.0f, 0.0f);
	oldLongPos  = b2Vec2(0.0f, 0.0f);
	newLongPos  = b2Vec2(0.0f, 0.0f);
	hasGoodShortDistance = false;
	hasGoodLongDistance = false;
	longCounter = 0;

    int spoke_index;
	b2BodyDef m_cart_body;
    b2PolygonShape cartWorkingPiece;
    b2CircleShape wheel_shape;
    b2RevoluteJointDef wheel_motor_joint;
    b2PrismaticJointDef wheel_shock;
    float32 _loc_20;
    float32 _loc_21;
    float32 _loc_22;
    float32 _loc_23;
    float32 _loc_24;
    float32 cart_piece_edge_1_x;
    float32 cart_piece_edge_1_y;
    int wheelIndex;
    float32 wshock_angle;
    b2FixtureDef wheel_fixture;
    b2BodyDef wheel_bodyDef;
    b2FixtureDef cart_piece_fixture;
    m_cart_body.position.Set(x, y);
    m_cart_body.angle = 0;
    m_cart_body.type = b2_dynamicBody;
    cart = myWorld->CreateBody(&m_cart_body);

	maxWheels = 8;//TODO - Investigate why this is 3 and if that prevents 3 wheels if they
				  //were only on later spokes --- UPDATE: It needs to run at 8 to prevent an
				  //uninitialized pointer problem.

	cart_piece_fixture.filter.categoryBits = NON_INTERACTOR;
    wheel_fixture.filter.categoryBits = NON_INTERACTOR;//TODO - change to something Wheeler specific, like Wheeler_Body or something.

	healthDownCounter = 120;

    spoke_index = 0;
    while (spoke_index < 8) {//Going through all the spokes, in order. -- This whole block wasn't needed earlier. hmm again
        if (spokeAngles[spoke_index] >= totalAngleValue / 2) {//If the current spoke angle is greater than or equal to half the total
            _loc_20 = spokeAngles[spoke_index] - totalAngleValue / 2 - 0.08f;//Through multiple trials this never gets triggered. hmm.
            spokeAngles[spoke_index] = totalAngleValue / 2 - 0.08f;
            totalAngleValue = totalAngleValue - _loc_20;
        }
        spoke_index++;
    }
    float32 runningAngleTotal = 0;
    float32 opposingAngles[8];//I think these are the opposing angles, maybe.
    spoke_index = 0;
    while (spoke_index < 8) {//adds up all the individual angles until it reaches nearly one
        opposingAngles[spoke_index] = runningAngleTotal + spokeAngles[spoke_index] / totalAngleValue;
        runningAngleTotal = opposingAngles[spoke_index];
        spoke_index++;
    }

    wheel_shock.lowerTranslation = -0.1f;
    wheel_shock.upperTranslation = 0.25f;
    wheel_shock.enableLimit = true;
    wheel_shock.enableMotor = true;
    b2Vec2 cart_piece_verts[3];
    int cartPieceIndex = 0;
    while (cartPieceIndex < 3) {
        cart_piece_verts[cartPieceIndex] = b2Vec2();
        cartPieceIndex = cartPieceIndex + 1;
    }

    spoke_index = 0;
    while (spoke_index < 8) {//Through the spokes again.
        _loc_21 = opposingAngles[spoke_index] * b2_pi * 2;
        _loc_22 = spokeLengths[spoke_index];
        _loc_23 = opposingAngles[(spoke_index + 1) % 8] * b2_pi * 2;
        _loc_24 = spokeLengths[(spoke_index + 1) % 8];
        cart_piece_edge_1_x = _loc_22 * cos(_loc_21);
        cart_piece_edge_1_y = _loc_22 * sin(_loc_21);
        cart_piece_verts[0].Set(0, 0);
        cart_piece_verts[1].Set(cart_piece_edge_1_x, cart_piece_edge_1_y);
        cart_piece_verts[2].Set(_loc_24 * cos(_loc_23), _loc_24 * sin(_loc_23));
        cartWorkingPiece.Set(cart_piece_verts, 3);
        cart_piece_fixture.shape = &cartWorkingPiece;
        cart_piece_fixture.density = 2.0f;
        cart_piece_fixture.friction = 10.0f;
        cart_piece_fixture.restitution = 0.05f;
        cart_piece_fixture.filter.groupIndex = -1;

        cartFixture[spoke_index] = cart->CreateFixture(&cart_piece_fixture);
        axleFixture[spoke_index] = NULL;
        wheelOn[spoke_index] = -1;
        spoke_index++;
    }

    spoke_index = 0;
	while (spoke_index < maxWheels) {
        if (hasWheel[spoke_index] >= 0) {//A wheel was generated for the gene
            wheelIndex = hasWheel[spoke_index];
            _loc_21 = opposingAngles[wheelIndex] * b2_pi * 2;
            _loc_22 = spokeLengths[wheelIndex];
            cart_piece_edge_1_x = _loc_22 * cos(_loc_21);
            cart_piece_edge_1_y = _loc_22 * sin(_loc_21);
			wshock_angle = spokeAngles[spoke_index];
            cartWorkingPiece.SetAsBox(0.2f, 0.1f, b2Vec2(cart_piece_edge_1_x, cart_piece_edge_1_y), wshock_angle);
            cart_piece_fixture.shape = &cartWorkingPiece;
            cart_piece_fixture.density = 2;
            cart_piece_fixture.friction = 10;
            cart_piece_fixture.restitution = 0.05f;
            cart_piece_fixture.filter.groupIndex = -1;
            axleFixture[spoke_index] = cart->CreateFixture(&cart_piece_fixture);
            wheelOn[spoke_index] = wheelIndex;
            axle[spoke_index] = myWorld->CreateBody(&m_cart_body);
			axle[spoke_index]->SetUserData( this );
            cartWorkingPiece.SetAsBox(0.2f, 0.05f, b2Vec2(cart_piece_edge_1_x - 0.3f * cos(wshock_angle), cart_piece_edge_1_y - 0.3f * sin(wshock_angle)), wshock_angle);
            cart_piece_fixture.density = 20;
            axle[spoke_index]->CreateFixture(&cart_piece_fixture);
            wheel_shock.Initialize(cart, axle[spoke_index], axle[spoke_index]->GetWorldCenter(), b2Vec2(cos(wshock_angle), sin(wshock_angle)));
            spring[spoke_index] = (b2PrismaticJoint*)myWorld->CreateJoint(&wheel_shock);
			wheel_shape.m_radius = wheelRadius[spoke_index];

            wheel_fixture.shape = &wheel_shape;
            wheel_fixture.density = 0.5f;
            wheel_fixture.friction = 10;//GENE - Slippery slope helping? Grab of the tires.
            wheel_fixture.restitution = 0.1f;
            wheel_fixture.filter.groupIndex = -1;

            wheel_bodyDef.position.Set(cart->GetPosition().x + cart_piece_edge_1_x - 0.5f * cos(wshock_angle), cart->GetPosition().y + cart_piece_edge_1_y - 0.5f * sin(wshock_angle));
            wheel_bodyDef.allowSleep = false;
            wheel_bodyDef.type = b2_dynamicBody;
            wheel[spoke_index] = myWorld->CreateBody(&wheel_bodyDef);
			wheel[spoke_index]->SetUserData( this );
            wheel[spoke_index]->CreateFixture(&wheel_fixture);
            wheel_motor_joint.enableMotor = true;
            wheel_motor_joint.Initialize(axle[spoke_index], wheel[spoke_index], wheel[spoke_index]->GetWorldCenter());
            motor[spoke_index] = (b2RevoluteJoint*)myWorld->CreateJoint(&wheel_motor_joint);
		} else {
            axle[spoke_index] = NULL;
            wheel[spoke_index] = NULL;
			motor[spoke_index] = NULL;
		}
        spoke_index++;
    }
    float32 totalMass = cart->GetMass();
    wheelsOn = 0;
    spoke_index = 0;
    while (spoke_index < maxWheels) {
		if (hasWheel[spoke_index] >= 0) {
            int _loc_32 = wheelsOn + 1;
            wheelsOn = _loc_32;
        }
        spoke_index++;
    }
	torque = totalMass * 1.5f * /*myWorld->GetGravity()*/ 30.0f / wheelsOn;
    spoke_index = 0;
    while (spoke_index < maxWheels) {
		if (hasWheel[spoke_index] >= 0) {
            motor[spoke_index]->SetMotorSpeed(spinDirection);
            motor[spoke_index]->SetMaxMotorTorque(torque);
        }
        spoke_index++;
    }
    baseSpringForce = totalMass * 7.5f;

	b2CircleShape sensorCircle;
	sensorCircle.m_radius = 5.0f;

	b2FixtureDef fd;
	fd.shape = &sensorCircle;
	fd.density = 0.0f;
	fd.isSensor = true;
	fd.filter.categoryBits = WHEELER;//Wheeler_Sensor, or something like that...
	fd.filter.maskBits = GRASS;
			
	cart->CreateFixture(&fd);

	cart->SetUserData( this );

	Wheeler *testWheeler = (Wheeler *)cart->GetUserData();
	if (dynamic_cast<Wheeler*>(testWheeler) == NULL) {
		int i = 0;
	}
}

//Code for when we're loading Wheeler genes from a file
//Note this code both loads the genes in the chromosome and builds the Wheeler
void Wheeler::loadWheeler(vector<float32> genes) {
	health = genes[4];
	spinDirection = genes[3];//Probably should be called spinSpeed

    int spoke_index = 0;
    while (spoke_index < 8) {

		spokeAngles[spoke_index] = genes[spoke_index + 6];
        totalAngleValue = totalAngleValue + spokeAngles[spoke_index];
		
		spokeLengths[spoke_index] = genes[spoke_index + 14];
		if (spokeLengths[spoke_index] < 0) {spokeLengths[spoke_index] = 0.3f;}
		hasWheel[spoke_index] = genes[spoke_index + 22];
		axleAngle[spoke_index] = genes[spoke_index + 30];
		wheelRadius[spoke_index] = genes[spoke_index + 38];

        spoke_index++;
    }

	needsToReproduce = false;
	reproductionCounter = 0;
	minutesToLive = genes[5];//8 minutes of life in a 1/60 timestep

    spoke_index;
	b2BodyDef m_cart_body;
    b2PolygonShape cartWorkingPiece;
    b2CircleShape wheel_shape;
    b2RevoluteJointDef wheel_motor_joint;
    b2PrismaticJointDef wheel_shock;
    float32 _loc_20;
    float32 _loc_21;
    float32 _loc_22;
    float32 _loc_23;
    float32 _loc_24;
    float32 cart_piece_edge_1_x;
    float32 cart_piece_edge_1_y;
    int wheelIndex;
    float32 wshock_angle;
    b2FixtureDef wheel_fixture;
    b2BodyDef wheel_bodyDef;
    b2FixtureDef cart_piece_fixture;
    m_cart_body.position.Set(genes[0], genes[1] + 7);
    m_cart_body.angle = 0;//genes[2];
    m_cart_body.type = b2_dynamicBody;
    cart = myWorld->CreateBody(&m_cart_body);

	maxWheels = 8;//TODO - Investigate why this is 3 and if that prevents 3 wheels if they
				  //were only on later spokes --- UPDATE: It needs to run at 8 to prevent an
				  //uninitialized pointer problem.
	
	cart->SetUserData( this );

	cart_piece_fixture.filter.categoryBits = NON_INTERACTOR;
    wheel_fixture.filter.categoryBits = NON_INTERACTOR;

	healthDownCounter = 120;

    spoke_index = 0;
    while (spoke_index < 8) {//Going through all the spokes, in order. -- This whole block wasn't needed earlier. hmm again
        if (spokeAngles[spoke_index] >= totalAngleValue / 2) {//If the current spoke angle is greater than or equal to half the total
            _loc_20 = spokeAngles[spoke_index] - totalAngleValue / 2 - 0.08f;//Through multiple trials this never gets triggered. hmm.
            spokeAngles[spoke_index] = totalAngleValue / 2 - 0.08f;
            totalAngleValue = totalAngleValue - _loc_20;
        }
        spoke_index++;
    }
    float32 runningAngleTotal = 0;
    float32 opposingAngles[8];//I think these are the opposing angles, maybe.
    spoke_index = 0;
    while (spoke_index < 8) {//adds up all the individual angles until it reaches nearly one
        opposingAngles[spoke_index] = runningAngleTotal + spokeAngles[spoke_index] / totalAngleValue;
        runningAngleTotal = opposingAngles[spoke_index];
        spoke_index++;
    }

    wheel_shock.lowerTranslation = -0.1f;
    wheel_shock.upperTranslation = 0.25f;
    wheel_shock.enableLimit = true;
    wheel_shock.enableMotor = true;
    b2Vec2 cart_piece_verts[3];
    int cartPieceIndex = 0;
    while (cartPieceIndex < 3) {
        cart_piece_verts[cartPieceIndex] = b2Vec2();
        cartPieceIndex = cartPieceIndex + 1;
    }

    spoke_index = 0;
    while (spoke_index < 8) {//Through the spokes again.
        _loc_21 = opposingAngles[spoke_index] * b2_pi * 2;
        _loc_22 = spokeLengths[spoke_index];
        _loc_23 = opposingAngles[(spoke_index + 1) % 8] * b2_pi * 2;
        _loc_24 = spokeLengths[(spoke_index + 1) % 8];
        cart_piece_edge_1_x = _loc_22 * cos(_loc_21);
        cart_piece_edge_1_y = _loc_22 * sin(_loc_21);
        cart_piece_verts[0].Set(0, 0);
        cart_piece_verts[1].Set(cart_piece_edge_1_x, cart_piece_edge_1_y);
        cart_piece_verts[2].Set(_loc_24 * cos(_loc_23), _loc_24 * sin(_loc_23));
        cartWorkingPiece.Set(cart_piece_verts, 3);
        cart_piece_fixture.shape = &cartWorkingPiece;
        cart_piece_fixture.density = 2.0f;
        cart_piece_fixture.friction = 10.0f;
        cart_piece_fixture.restitution = 0.05f;
        cart_piece_fixture.filter.groupIndex = -1;

        cartFixture[spoke_index] = cart->CreateFixture(&cart_piece_fixture);
        axleFixture[spoke_index] = NULL;
        wheelOn[spoke_index] = -1;
        spoke_index++;
    }

    spoke_index = 0;
	while (spoke_index < maxWheels) {
        if (hasWheel[spoke_index] >= 0) {//A wheel was generated for the gene
            wheelIndex = hasWheel[spoke_index];
            _loc_21 = opposingAngles[wheelIndex] * b2_pi * 2;
            _loc_22 = spokeLengths[wheelIndex];
            cart_piece_edge_1_x = _loc_22 * cos(_loc_21);
            cart_piece_edge_1_y = _loc_22 * sin(_loc_21);
			wshock_angle = spokeAngles[spoke_index];
            cartWorkingPiece.SetAsBox(0.2f, 0.1f, b2Vec2(cart_piece_edge_1_x, cart_piece_edge_1_y), wshock_angle);
            cart_piece_fixture.shape = &cartWorkingPiece;
            cart_piece_fixture.density = 2;
            cart_piece_fixture.friction = 10;
            cart_piece_fixture.restitution = 0.05f;
            cart_piece_fixture.filter.groupIndex = -1;
            axleFixture[spoke_index] = cart->CreateFixture(&cart_piece_fixture);
            wheelOn[spoke_index] = wheelIndex;
            axle[spoke_index] = myWorld->CreateBody(&m_cart_body);
			axle[spoke_index]->SetUserData( this );
            cartWorkingPiece.SetAsBox(0.2f, 0.05f, b2Vec2(cart_piece_edge_1_x - 0.3f * cos(wshock_angle), cart_piece_edge_1_y - 0.3f * sin(wshock_angle)), wshock_angle);
            cart_piece_fixture.density = 20;
            axle[spoke_index]->CreateFixture(&cart_piece_fixture);
            wheel_shock.Initialize(cart, axle[spoke_index], axle[spoke_index]->GetWorldCenter(), b2Vec2(cos(wshock_angle), sin(wshock_angle)));
            spring[spoke_index] = (b2PrismaticJoint*)myWorld->CreateJoint(&wheel_shock);
			wheel_shape.m_radius = wheelRadius[spoke_index];

            wheel_fixture.shape = &wheel_shape;
            wheel_fixture.density = 0.5f;
            wheel_fixture.friction = 10;//GENE - Slippery slope helping? Grab of the tires.
            wheel_fixture.restitution = 0.1f;
            wheel_fixture.filter.groupIndex = -1;

            wheel_bodyDef.position.Set(cart->GetPosition().x + cart_piece_edge_1_x - 0.5f * cos(wshock_angle), cart->GetPosition().y + cart_piece_edge_1_y - 0.5f * sin(wshock_angle));
            wheel_bodyDef.allowSleep = false;
            wheel_bodyDef.type = b2_dynamicBody;
            wheel[spoke_index] = myWorld->CreateBody(&wheel_bodyDef);
			wheel[spoke_index]->SetUserData( this );
            wheel[spoke_index]->CreateFixture(&wheel_fixture);
            wheel_motor_joint.enableMotor = true;
            wheel_motor_joint.Initialize(axle[spoke_index], wheel[spoke_index], wheel[spoke_index]->GetWorldCenter());
            motor[spoke_index] = (b2RevoluteJoint*)myWorld->CreateJoint(&wheel_motor_joint);
		} else {
            axle[spoke_index] = NULL;
            wheel[spoke_index] = NULL;
			motor[spoke_index] = NULL;
		}
        spoke_index++;
    }
    float32 totalMass = cart->GetMass();
    wheelsOn = 0;
    spoke_index = 0;
    while (spoke_index < maxWheels) {
		if (hasWheel[spoke_index] >= 0) {
            int _loc_32 = wheelsOn + 1;
            wheelsOn = _loc_32;
        }
        spoke_index++;
    }
	torque = totalMass * 1.5f * /*myWorld->GetGravity()*/ 30.0f / wheelsOn;
    spoke_index = 0;
    while (spoke_index < maxWheels) {
		if (hasWheel[spoke_index] >= 0) {
            motor[spoke_index]->SetMotorSpeed(spinDirection);
            motor[spoke_index]->SetMaxMotorTorque(torque);
        }
        spoke_index++;
    }
    baseSpringForce = totalMass * 7.5f;

	b2BodyDef wheelerSensor;

	b2CircleShape sensorCircle;
	sensorCircle.m_radius = 5.0f;

	b2FixtureDef fd;
	fd.shape = &sensorCircle;
	fd.density = 0.0f;
	fd.isSensor = true;
	fd.filter.categoryBits = WHEELER;
	fd.filter.maskBits = GRASS;
			
	cart->CreateFixture(&fd);

	cart->SetTransform(cart->GetPosition(), genes[2]);
}

//Function used to turn the Wheeler's wheel's rotation around
void Wheeler::turnAround() {
	int32 spoke_index = 0;
	spinDirection = -spinDirection;
    while (spoke_index < 8) {
		if(motor[spoke_index] != NULL) {
			motor[spoke_index]->SetMotorSpeed(spinDirection);
			motor[spoke_index]->SetMaxMotorTorque(torque);
		}
        spoke_index++;
    }
}

Wheeler::~Wheeler(void) {

}

void Wheeler::die() {
	myWorld->DestroyBody(cart);
	for (int i = 0; i < 8 ;i++) {
		if (axle[i] != NULL) {
			myWorld->DestroyBody(axle[i]);	
		}	
		if (wheel[i] != NULL) {
			myWorld->DestroyBody(wheel[i]);	
		}
	}
}