BLAHengine - A little bit of everything
===================================

![alt tag](https://raw.github.com/vingt-2/BLAengine/master/illustration.bmp)

===================================

BLAHengine is real-time game engine inspired from Unity3D, and is a bit of a test bed for 
for my experimentations with real-time graphics and animation. The point being to write everything
from the ground's up and use as little libraries as possible.It is periodically updated with
various features, the most prevalent being as of now:
* Asset importation, with internal data structures stored in binary formats for fast load time.
* An Object and Object Components architecture, with full scene graph support (in the vein of Unity3d). 
* A deffered Renderer structure based on OpenGL 3.3, with real-time shadows and normal mapping (in rework)
* Full Object Physics routines to enable real-time interaction of 3D objects. 
* Fast Collision routines for transformed Meshes, primitives, and Ray intersections
* A real-time Linear Complementary based constraint solver, with which Mesh to Mesh rigid body collisions are handled.
* The ground work is laid to add further types of constraints on object.

* For the kicks, I also implemented a parallelized Physically Based Offline renderer as an Object Component !
* It implements a parallized Volumetric Photon Mapping renderer, and supports severals bounces on solid and participating media:

![alt tag](https://raw.github.com/vingt-2/BLAengine/master/PBR-VolumetricPhotonMapping.png)

===================================
The demo application "MainDemo" is a quick pot-pourris of the stuff in the engine. 
It populates a scene with objects and let you interact with it. It's not very pretty
and changes to help test features quickly.

* W,A,S,D to move the camera around.
* Right-Click + mouse to rotate the camera.
* Left-Click on object: Push a force on a rigidbody
* Space: Enable/Disable Rigidbodies simulation
* Backspace: Enable/Disable gravity
* Left/Right: Move Sun left and right
* CTRL: Move sun auto

===================================
Installation:

To compile and run this code sample, make sure you provide your programming environmment with:
* GLEW to enable OpenGL 3.0 and extensions
* GLFW (a GL toolkit library).
* GLM, a C++ mathematics library for graphics programming that mimics GLSL maths definitions.

Make sure you get all the STD/*.h includes resolved and linked and you should be good to go ! 
