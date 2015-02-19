EngineOfLies
============

a 2D game engine with 3D graphics.  Written in C, compiled to a library

To be compiled as a library to be linked against for individual games.

Documentation can be found at:
    http://engineeroflies.github.io/EngineOfLies/

Library uses:
http://www.libsdl.org/
  SDL for graphics mode, image loading, audio and font rendering

http://icculus.org/physfs/
  PhysFS for loading from compression

http://www.opengl.org/
  OpenGL for rendering 3D graphics

http://chipmunk-physics.net/
  Chipmunk for 2D collision handling

For fmemopen compatibility on the MAC you may need:
  http://www.dragonflybsd.org/download/
  * though there are plans to remove this dependancy.
  
The library is being written in C with plans for bindings in other languages.
The library is geared towards platformers, shooters, role playing games and
adventure games.

On Ubuntu, you'll need the following packages
sudo apt-get install physfs-dev libyaml-dev libxmu-dev libxi-dev
