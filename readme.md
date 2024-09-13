How to build with cmake

```
// clone and cd into the repo
git submodule update --init --recursive
mkdir build
cd build
// install dependencies: cmake, ninja, opengl
// detailed instructions are in .github/workflows/release.yaml
cmake -GNinja ..
ninja
```

to run:
```
# from within the build directory
./OpenGL/SpaceBoom
```

to package:
1. You need one folder called `res` with the contents of `OpenGL/res/*` and the built binary to sit next to one another.
