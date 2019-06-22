BLAHengine - A little bit of everything
===================================

![demo](BLAengine.gif)

What is this ?
===================================

BLAHengine is real-time game engine inspired from Unity3D, and is a bit of a test bed for 
for my experimentations with real-time graphics and animation. The point being to write everything
from the ground's up and use as little libraries as possible.It is periodically updated with
various features, the most prevalent being as of now:
* Asset importation, with internal data structures stored in binary formats for fast load time.
* An Object and Object Components architecture, with full scene graph support (in the vein of Unity3d). 
* A deffered Renderer structure based on OpenGL 3.3, with point/directional lights, real-time shadows and normal maps.
* Full Object Physics routines to enable real-time interaction of 3D objects. 
* Fast Collision routines for transformed Meshes, primitives, and Ray intersections
* A real-time Linear Complementary based constraint solver, with which Mesh to Mesh rigid body collisions are handled.
* The ground work is laid to add further types of constraints on object.

* For kicks, I also implemented a parallelized Physically Based Offline renderer as an Object Component! It implements a parallized (CPU) Volumetric Photon Mapping renderer, and supports severals bounces on solid and participating media:
* BLAH, Blah, blah.
![alt tag](https://raw.github.com/vingt-2/BLAengine/master/PBR-VolumetricPhotonMapping.png)
 

Installation (Windows only):
===================================
BLAengine uses CMake to generate Visual Studio projects for itself and each dependencies.
1. Make sure you have both recent releases of CMake and Git.
2. Clone this git repo in a folder of your choice
3. Run Cmake either using the GUI or with the command line:  
  * With CMake-Gui, Select the cloned repo as source path, and select whatever build path **outside** of the source path. Click *Configure*, verify that all paths (and dependencies are set), then *Generate* to genereate the VS Solution.
  * In the command line, run *cmake path/to/cloned/repo -B desired/path/to/VS/Solution*  
4. In either case, if you just cloned the repo, CMake should try to retrieve the dependencies' submodules from each of their respective github repositories.
5. Open the VS solution, **select Release**, and build all(in Debug, glew will output glewd.lib, but BLAengine only tries to link with glew.lib by default. You'll need to change the linker input to link against the debug library).
6. You can run BLAengine.exe
