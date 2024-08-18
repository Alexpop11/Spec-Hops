How to build with cmake

```
// clone and cd into the repo
git submodule update --init --recursive
mkdir build
cd build
// install dependencies: cmake, ninja, opengl
// detailed instructions are in .github/workflows/release.yaml
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
