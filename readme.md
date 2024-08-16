How to build with cmake

```
// clone and cd into the repo
mkdir build
cd build
// install dependencies: cmake, ninja, opengl, glew, glfw3, openal-soft
cmake -GNinja .
ninja
```

to run:
```
cd ../OpenGL
../build/OpenGL/OpenGL
```

to package:
1. You need one folder with the contents of OpenGL/res/* and the built binary to sit next to one another.
