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
    PERM_GROUND			=  1,
    GROUND				=  2,
    PERM_WALL			=  4,
    WALL				=  8,
    GRASS_SPAWNER_BASE	=  16,
    SEED				=  32,
    GRASS				=  64,
    ROOT				=  128,
    WHEELER_BODY		=  256,
    WHEELER				=  512	 
//    xxxx	 =  1024,
//    xxxx	 =  2048,
//    xxxx	 =  4096,
//    xxxx	 =  8192,
//    xxxx	 =  16384,
//    xxxx	 =  32768,

};

#endif