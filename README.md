Dependencies
------------

* OpenGL
* SDL
* SDL_image
* SDL_mixer
* SDL_ttf

Installation
------------

Just run `make`, then `sudo make install`. Make sure you have 
the necessary devel packages. Installation directory can be 
customized in `Makefile`.


Controls
--------

Use arrow keys and `Enter` to navigate through menu and map. 
To move through the room use arrow keys:

* Left, right - turn left or right
* Up - move forward

Camera:
* Home - zoom in
* End - zoom out
* Pg Up - rotate the camera clockwise (when option `camera` is set to `free`)
* Pg Down - rotate the camera counterclockwise (when option `camera` is set to `free`)
* Insert - lift the camera up
* Delete - lower the camera

About the game
--------------

`Charley the Ghost` is a 3D puzzle game. The main character of the game
is the ghost called Charley who goes for a walk in deserted castle. 
In each room of the castle you shall have to solve the puzzle and to lead 
Charley to the next one.

The game was written using OpenGL and SDL. It currently supports only linux
platforms.

How to play
-----------

A game level represent a room with one or more doors. To complete 
a level you should pass through a room to one of doors. Each 
of these doors opens a new level.

To know, which of the levels were completed, you can look at the map. 
If the level is completed on its thumbnail will be pictured keys. 
The color of the keys corresponds to the nameplate color above the exit.

Sometimes the door are closed. In this case need to take the key 
of corresponding color.

In the room you can walk on the slabs. Some of slabs are broken 
and can be used only once.

Brown slabs are movable. They can transport you over abyss. To use such slab, 
you need to stand on it and choose a direction. It will move until 
it meets with an obstacle.

Yet another way to overcome far distance are teleporters. Once you come 
into a teleport, you will appear at opposite side of other one with 
the marker of same color.

Also you can meet an elevator. It can be lifted up or lowered 
with help of the button on the floor. The button activates 
all elevators of the same color. Please note, that elevator will not be lowered, 
if movable slab is dispose below it.
