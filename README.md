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
