# Depth-based Toon Point Rendering

**Author:** Sean Brown

## Description
This is a point cloud rendering system designed to be extensible and efficient. Its purpose is to render points and implement a variety of point-related features. The included features are as follows:

- texture mapping
- size variation based on depth
- texture variation based on depth
- size controlled by user
- color change by user

## Build & Run

> While in the folder containing `main`:

### Unix
```bash
make
./main
```

### Windows
Open the project folder in **Visual Studio** and build the solution.

## Controls

| Key / Input | Action                         |
|-------------|--------------------------------|
| `1`         | White background               |
| `2`         | Black background               |
| `a`         | Decrease division of distance  |
| `s`         | Increase division of distance  |
| `x`         | Decrease size coefficient      |
| `z`         | Increase size coefficient      |
| `e`         | Decrease radius cutoff         |
| `r`         | Increase radius cutoff         |
| `q`         | Increase color divide distance |
| `w`         | Decrease color divide distance |
| **RMB**     | Open menu                      |

## References
- This program uses **gltb** by David Yu and David Blythe.
- The classes **PixelMap** and **Pixel** are written by Mario Costa Sousa.
- Shader loading code adapted from: <http://www.opengl-tutorial.org/beginners-tutorials/tutorial-2-the-first-triangle/>

