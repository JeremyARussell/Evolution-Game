# The Game of Evolution

## Quick Disclaimer
This is my first time open sourcing any semi big piece of my own work. Expect a bit
of.. roughness to my documentation and this very readme.. Please do me the favor of 
nitpicking at anything that might be confusing or reads weird.

## Compiling

Download and install Visual Studio C++ if you don't have it already, we're 
currently using 2015, then open up the project's solution file at Build/vs2010/Evolution.sln

If you don't know how to use Visual Studio I suggest searching up a simple starter
guide, basically you pick if you want to compile a release version (which is optimized
to run faster) or a debug version (which you need to, you know, debug..) then hit a 
play button and you're off.

## The Code

Until this get's a big refactor you're going to see code in a few different places 
that may not make any sense. Here's a quick overview of the code files and their 
locations. What they do, etc.

### Libraries
The Box2D, SOIL, freeglut and glui folders all hold various library files needed
to compile the game. Please checkout each one at their respective websites for 
documentation, etc. You can find them here [Box2D](box2d.org), 
[SOIL](http://www.lonesock.net/soil.html), [freeglut](http://freeglut.sourceforge.net/) 
and [glui](http://glui.sourceforge.net/).

### "Engine" and Universes.. (only 2 so far)

The Evolution/Framework folder contains a mix of mostly some modified Box2D testbed
code, an assets/images folder for the UI textures and a couple files used for individual 
"Universes" (see [Game Terms](https://github.com/JeremyARussell/Evolution-Game/wiki/Game-Terms)). 
Strictly speaking, I never setup my own game engine, I more or less kept butchering at 
the sample code that Erin Catto provides to let people test out his Box2D library. If
you're familiar with his library and the testbed you'll see what I'm talking about. And
yes. Eventually I will like to have an actual engine setup with actual game oriented 
code design, as it is be warned this code is hacky.

The following is a list of all the "Framework" or "Engine" code files. I originally 
start off writing this readme with an idea of giving fleshed out descriptions 
documenting functions, etc. But, as it so happens the code is too messy for me to 
easily go through and accomplish that in the here and now. So I'll just have to make 
a point to actually document everything at a later date and give the best I can for 
now. Each of these files can be found in Visual Studio under the Framework filter.
- GameWorlds.cpp - Has a list of the different worlds or universes that can be used in
the game. Currently contains only two universes.
- Main.cpp - If you're familiar with C++ you know this as the programs entry point. 
This file is in charge of telling the program how and where to start.
- Render.cpp - Is pretty much untouched original testbed code provided by the 
aforementioned Erin Catto, it gives you a way to render the box2D objects so that you 
can actually see them, this is not the way to do this, generally you would have a render
function in your object class so that you can use whatever graphics engine you want.. 
This code is technically supposed to be for debugging, but I ended up just using it 
since it so far has done the job nicely.
- Render.h - The header file for Render.cpp
- World.cpp - This file has a few general things that apply to each universe (it should
probably be called Universe.cpp really), like mouse handling..
- World.h - The header file for World.cpp

The following are the "Universe" files used to generate the levels. Currently there are 
only two of them, with the SandboxWorld being the main fleshed out world, and the 
Abiogenesis one being a more recent round of me playing around with ideas. Both 
Universes are located under the Game/Worlds filter in Visual Studio.
- Abiogenesis_Universe.h - A Universe I made to experiment with how to make life-like 
macro molecules using simulated "water" and "phospholipids", it's incomplete now but 
I hope to make some progress on it soon.
- SandboxWorld.h - The main Universe I worked on as I made this game. AKA the Wheeler's 
Universe. You can lay down pieces of "Ground" and "Wall" to make an environment, set out
"Grass Spawners" and spread "Seeds" to produce food for the Wheelers. Then randomly 
generate some Wheelers to live out their lives. Destroy Walls, Ground, Wheelers and 
Grass Spawners you aren't satisfied with. And if you feel like some extra divine 
intervention, "Grab" a Wheeler and toss it around to either help or hinder it.

Quick note on the Universe files.. Right now there is no in-game method of switching
which Universe you're playing in. Mostly this is due to not having more than one world
completed enough to really show off, but partly it's due to the fact that I haven't 
bothered setting things up with a real engine designed with the idea of multiple 
levels (or Universes). So until things are redesigned and refactored you can either
change the worldIndex variable's value to 1 instead of 0 @ line 387 in the Main.cpp
file (most recommended) OR swap out lines 36 and 37 in the GameWorlds.cpp file (which
would basically change the Universes in those lines to have different index values..

### The SandboxWorld's OR Wheeler's Universe, code and objects..

First off we have the Evolution/Framework/assets/images folder. Which contains the
texture files I use for the Wheeler Universe's power selection UI (the HUD)

And second off, located in the Build/Evolution folder are various files used for
the Wheeler's Universe. In the folder they're all located bunched together, but in
Visual Studio they are located in specific filters to keep them organized. I'll 
mention which filter they're in as we go, I'll list them alphabetically as they appear
in the file system though. Any header (.h) file that has a counter .cpp file will 
go to said .cpp file, and be located in the same filter.
- collision_enums.h - Located under the Game/Creatures filter. Holds an enum that 
Box2D uses to do collision filtering.
- Grass.cpp - Located under the Game/Creatures filter. Holds the code that controls 
how Grass acts.
- GrassSpawner.cpp - Located under the Game/Items filter. Holds the code used to 
control a Grass seed spawner, which keeps around a permanent piece of grass to keep
making more seeds (and therefore more grass).
- Ground.cpp - Located under the Game/Items filter. Holds the code that generates 
ground. Ground let's seeds plant and turn into grass.
- MainMenu.cpp - Located under the Game/GameStates filter. Contains the code for drawing
and highlighting on mouse hover for the main "Sandbox Mode" game menu you see when you
first start the game.
- PowerHUD.cpp - Located under the Game/Interface filter. All the code for that fancy
power selection bar you see is here, everything from drawing and texture handling to 
mouse events needed to do the actual selecting of a power.
- Seed.cpp - Located under the Game/Creatures filter. The code for creating a seed,
which upon hitting any piece of ground will turn into a budding blade of grass.
- Wall.cpp - Located under the Game/Items filter. Holds the code used to generate 
a piece of wall. Which seeds will bounce off of.
- Wheeler.cpp - Located under the Game/Creatures filter. Probably the biggest code 
file when you think about how much different stuff is in here and how many lines
of code there are, but basically this file handles all the stuff that controls are
main characters here, the Wheelers. How their bodies are built, how their genes are
created, how they mutate, how they move, etc, etc.

A note: Do not assume that all the code needed to run a Wheeler will be in Wheeler.cpp
or Wheeler.h, much of it will also be in the Universe code file "SandboxWorld.h", I 
wanted to figure out the best way to decouple the Universe from it's parts, but as I
haven't come across a situation where I actually would use one Universe's pieces in
another Universe, I haven't actually bothered doing that and I don't know if I will
unless it becomes necessary. The same rule applies to other Wheeler Universe objects 
(and future universes, for the time being anyways).

---

## Things I need to do...

- Add all the issues I know it has...
- Restructure and rename the code and folders.. 
- Update the wiki with some instructions on how to play..
- Add in the whole big picture stuff to the wiki and readme.. To add to that, 
I should also add a roadmap of some sort, what I pictured the versions being.. 
etc.
- Add the MIT license to all of "my" code.
- Put some more specific readme's in folders that need further explanation.

---

## License

The MIT License (MIT)

Copyright (c) 2016 Jeremy Russell

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

## Other Licenses
The libraries used with this game are subject to their own licenses outlined 
within the individual library's code.
