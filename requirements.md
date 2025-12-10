
3. Build arena
4. Test arena



The Game

The game is simple. It consists of an Arena that loads a list of robots (which are just C++ programs) and the robots move around and shoot each other. The game is turn based (so you don't need to create and manage threads). It is really just a 'simulation' program that will handle the interactions between the robots and then display the outcome of each turn. The Arena is an X by Y sized array of 'rows' and 'columns' (something which you should be painfully familiar with by now...)

 

The Robots

The robots inherit from a base class called RobotBase, which is supplied in the starter code for the assignment repo. You will implement the pure virtual functions and add whatever code you want to make the robots do what you want them to do. At runtime, your Arena will compile the code for the robots and load them as 'shared libraries' or .so's. Here is some information about the Robots:

 A robot has:

Health
Move speed 
Armor level
A weapon type: railgun, flamethrower, hammer, grenade launcher. 
Robot attributes are specified at compile time and determine the strategy that the robot will use to try to outwit its opponents.

 

Weapons

Flamethrower - shoots 3 cells wide and 4 cells long. Anything in the box gets toasted.
Railgun - shoots one cell wide, all the way across the arena, and through obstacles and even other robots. All robots in the path take a hit.
Hammer - 'shoots' just one neighboring cell to the robot. If there's a robot there, it gets POUNDED.
Grenade Launcher - can target any cell in the arena. The grenade does damage in a 3x3 square at the location it lands. The grenade launcher only contains 10 grenades, and once they are gone, the Robot can no longer shoot. 
Armor reduces the amount of damage taken. When a robot gets hit, no matter the weapon, it loses one armor. When it's health goes to 0 (or below) the robot is 'dead' and gets no more turns. 

 

The Arena

The Arena is a 2 dimensional board with rows and columns. It keeps track of the robots and obstacles and handles the interaction between the robots. It provides the 'radar' information to the robots so they can 'see.'  It orchestrates the movement of the robots, following the directions given to it by the robots themselves. It determines the outcome of 'shots' and calculates damage. The arena is where all your code will reside. Here are the things the arena must do:

It has to load the robots. It will look in the directory and find any file named Robot_<robot-name>.cpp and compile it into a shared library, then LOAD the shared library into the current process space. This is TOTALLY COOL and I'll provide  some code to help you do it. 

It has to manage a turn-based simulation, allowing each robot to do what it wants on its turn. It does this by calling a specific set of functions implemented by the robot (defined in RobotBase.cpp)
It has to handle the radar capability, looking in the arena, and passing the state to the robot so it can make decisions. The robot has no ability other than through radar to 'look' at the arena directly. 
It handles movement of the robots, checking to see if they run into each other or other obstacles.
It handles the shots specified by the robots, determines if they hit anything and then calls functions on the 'hit' robots to cause them to take damage.
It prints out a description of what is happening. 
It determines the winner (last robot alive) 
 

Obstacles:

The board has a random number of obstacles. They are: 

Flamethrower - does damage just as if the robot got shot by a flamethrower. 
Pit - If a robot falls into the pit it can no longer move for the rest of the game. (move = 0). It can still shoot. 
Mound - robots can't move through the mound - they have to go around it. Mounds do not protect from shots. 
You can design and code your board however you like, but I suggest using a two-dimensional array of 'char' and using the char to keep track of what's actually in each 'cell.' This way the arena array keeps the state AND can easily display the state.

 

Here is an example round 0  from my solution:

jeffcompas ~/UtahTech/CS3005/FinalExam $ ./RobotWarz

Loading Robots...
Compiling Robot_Flame_e_o.cpp to libFlame_e_o.so...
boundaries: 20, 20
Loaded robot: Flame_e_o at (3, 12)
Compiling Robot_HammerTime.cpp to libHammerTime.so...
boundaries: 20, 20
Loaded robot: HammerTime at (17, 13)
Compiling Robot_Ratboy.cpp to libRatboy.so...
boundaries: 20, 20
Loaded robot: Ratboy at (17, 14)

=========== starting round 0 ===========
     0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19

 0   .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .

 1   .  .  .  .  .  .  .  .  .  F  .  .  .  .  .  .  .  .  .  .

 2   .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .

 3   .  .  .  .  .  .  .  .  .  .  .  . R!  .  .  .  .  .  .  .

 4   .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .

 5   .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .

 6   .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  F  .  .  .

 7   .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .

 8   .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .

 9   .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .

10   P  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .

11   .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .

12   .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  M  .  .

13   .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  M  .  .

14   .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  M  .  .

15   .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .

16   .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .

17   .  .  .  .  .  .  .  .  .  .  .  F  . R@ R#  .  .  .  .  .

18   .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .

19   .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .


Flame_e_o ! begins turn.
Flame_e_o:
  Health: 100
  Weapon: Flamethrower
  Armor: 5
  Move Speed: 2
  Location: (3,12)
  checking radar ...  found nothing. 
Moving: Flame_e_o moves to (4,11).

HammerTime @ begins turn.
HammerTime:
  Health: 100
  Weapon: Hammer
  Armor: 5
  Move Speed: 2
  Location: (17,13)
  checking radar ...  found 'R' at (17,14)
Moving: HammerTime moves to (17,12).

Ratboy # begins turn.
Ratboy:
  Health: 100
  Weapon: Railgun
  Armor: 4
  Move Speed: 3
  Location: (17,14)
  checking radar ...  found 'R' at (17,12)
Shooting: railgun HammerTime takes 12 damage. Health: 88
 

***IMPORTANT***

Once again, you have a lot of freedom in the way the arena works. But the arena MUST use the RobotBase as the base class for robots, and it MUST call the functions on the robot according to the flow chart below. You must ALSO use RadarObj objects to report the findings of the radar scan to the robots. Do not change RobotBase or RadarObj. These are base classes that underpin the solution and will make it so that robots will be interchangeable across arenas.




You will receive a grade of 0 points if your game does not build or run. We've done this enough that you can't turn in busted code. To grade your code, I'll clone your repo and run make. I'll load your robots and also some of my own, so you'll need to stick strictly to the specifications. 
10 points for basic structure. Did you read the instructions and create the general solution? Did you use RobotBase correctly and call the functions according to the spec?
10 points for a working 'simulation' - your game runs, loads the robots and at least shows some interaction.
10 points for handling movement - robots move correctly and legally on their turns, respect obstacles and boundaries
20 points for handling shooting - robots shoot and take damage appropriately.
10 points Good UI - I can see your board, I can see what happens each round.
10 points for game resolution - someone actually wins. 
minus points for bugs, seg-faults, endless loops or nutty robot behavior (like, a robot shoots the flamethrower all the way across the arena or hops over an obstacle because its move speed is 5.)
 

 Here are the rules for the robots:

 

They must follow the specifications in the RobotWarz spec without any additional features or hacks. 

They must inherit from RobotBase and implement the required functions.

They have to load. 

 

You don't have to do anything else. 