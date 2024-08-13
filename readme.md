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
