#ifndef COLLISION_ENUMS_H
#define COLLISION_ENUMS_H
#pragma once
/* NOTES

Step 1 - Create this file and test that everything still works...
Step 2 - go through and replace all the 0.x00nn with the real enum wording
Step 3a - Rename wall to ground and non_interactor with Wall - and wheeler_body - gs to root
Step 3b - Split Non interactor ----------------------------------------A
Step 4 - Map out what category is using what throughout the creature classes, etc. -- kind of did this during the whole thing anyways.
Step 5 - Reorganize the order of the enums, environment -> Things -> Creatures.




*/


enum _entityCategory {
    SEED		     =  1, //0x0001
    GRASS            =  2, //0x0002
    WHEELER		     =  4, //0x0004
    WALL			 =  8, //0x0008
    WHEELER_BODY	 =  16,//0x0010
    GROUND           =  32,//0x0020
    ROOT			 =  64,//0x0040
    GRASS_SPAWNER_BASE		 =  128

//    GRASS_SENSOR	 =  256,
//    GRASS_SENSOR	 =  512,
//    GRASS_SENSOR	 =  1024,
//    GRASS_SENSOR	 =  2048,
//    GRASS_SENSOR	 =  4096,
//    GRASS_SENSOR	 =  8192,
//    GRASS_SENSOR	 =  16384,
//    GRASS_SENSOR	 =  32768,

};

#endif