Terrain_Generator
=================

Automatically generate terrain using openGL

![Sample Image 1](https://raw.github.com/rgruener/Terrain_Generator/master/sample_images/image1.png)

## Description ##

The goal of this project is to generate realistic looking terrain 
which could be used in computer generated films, games, or simulations.
Algorithm is based on http://www.gameprogrammer.com/fractal.html

This project was created for ECE462 Interactive Engineering Graphics at
Cooper Union taught by Professor Carl Sable in the Spring 2013 Term

## Dependencies ##

* OpenGL 3.0 or Higher
* GLUT
* GLEW

## Compilation ##

Compilation instructions are for linux machines.  For other operating
systems such as Windows or Mac OS compilation flags within the makefile
must be changed in addition to other changes.  Refer to 
http://www.cs.unm.edu/~angel/BOOK/INTERACTIVE_COMPUTER_GRAPHICS/SIXTH_EDITION/
for instructions on compiling on other environments.

To compile the make utility must be installed, simply type make in the root folder.
If all dependencies are installed it should successfully compile.

## Usage ##

### General Syntax ###
./terrain.exe [-l] [-t] [-o] [terrain order] [-h] [roughness factor]
[-r] [randomness range factor] [-i] [initial height]

### Options ###

-l Generate terrain using lines as oppoes to triangles, creating a wire mesh.

-t Generate terrain using shaded triangles. Default option.

-o Specify terrain order (size of terrain).  For example terrain order 8 will 
   create an 8 by 8 grid of terrain.  Default value is 8.  NOTE: Depending on 
   amount of RAM, terrain order above 10 or 11 is not recommended.
   
-o Specify roughness factor, a number between 0.0 and 1.0 which denotes how rough 
   the surface of the terrain will be.  Values near 0.0 create extremely rough surfaces
   while values near 1.0 create smoother surfaces.  Default value is 0.95.
   
-r Specify the randomness range factor.  Must be greater or equal to 0.  The larger 
   the randomness range factor the more jagged the terrain.  However it should be 
   noted it creates a different type of jaggedness than the roughness factor.  Feel
   free to play around with both.  Default value is 0.5.

-i Specify initial height of terrain.  As heights below 0 are rendered as water, controls
   how much water is in the terrain.  Default value is -0.2.
   
### Viewing Controls (Flyover Controls) ###

wasd - Translate
Arrow Keys - Rotate
Mouse Drag (Either right or left click) - Alternate Rotate
Mouse Wheel - Zoom (Only works within certain environments)

Controls are similar to those of games.

![Sample Image 2](https://raw.github.com/rgruener/Terrain_Generator/master/sample_images/image2.png)
