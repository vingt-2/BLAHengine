# BLAengine
Game Prototyping Engine.
![Image of Yaktocat](BLAEditor.gif)

Installation (Windows only):

BLAengine uses CMake to generate Visual Studio projects for itself and each dependencies.

    Make sure you have both recent releases of CMake and Git.
    Clone this git repo in a folder of your choice
    Run Cmake either using the GUI or with the command line:

    With CMake-Gui, Select the cloned repo as source path, and select whatever build path outside of the source path. Click Configure, verify that all paths (and dependencies are set), then Generate to genereate the VS Solution.
    In the command line, run cmake path/to/cloned/repo -B desired/path/to/VS/Solution

    In either case, if you just cloned the repo, CMake should try to retrieve the dependencies' submodules from each of their respective github repositories.
    Open the VS solution, select Release, and build all(in Debug, glew will output glewd.lib, but BLAengine only tries to link with glew.lib by default. You'll need to change the linker input to link against the debug library).
    You can run BLAeditor.exe
