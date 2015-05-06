#ifndef COLLISION_ENUMS_H
#define COLLISION_ENUMS_H
#pragma once
/* NOTES

Step 1 - Create this file and test that everything still works...
Step 2 - go through and replace all the 0.x00nn with the real enum wording
Step 3 - Rename wall to ground and non_interactor with Wall
Step 4 - Map out what category is using what throughout the creature classes, etc.
Step 5 - Reorganize the order of the enums, environment -> Things -> Creatures.




*/


enum _entityCategory {
    SEED		     =  1,
    GRASS            =  2,
    WHEELER		     =  4,
    CARNIVORE	     =  8,
    NON_INTERACTOR   =  16,
    WALL             =  32,
    GRASS_SENSOR	 =  64,
//    GRASS_SENSOR	 =  128,

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