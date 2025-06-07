# COL781: Computer Graphics Assignments

This repository contains a collection of computer graphics assignments completed for the COL781 course. Each project implements a fundamental concept in computer graphics, ranging from rendering and mesh processing to animation and collision detection.

---

## Assignments

### 🎨 A1: Software Rasterizer

This assignment implements a software rasterizer from the ground up. A rasterizer is a core component of the graphics pipeline that transforms 3D vector graphics into a 2D raster image (a grid of pixels) for display.

**Key concepts explored:**
* Triangle rasterization
* Barycentric coordinates
* Z-buffering for depth testing
* Shading models (e.g., Blinn-Phong , Phong)

***

### 🕸️ A2: Mesh Processing

This project delves into algorithms for processing and manipulating 3D geometric meshes. Mesh processing is a vital area in 3D modeling, computer-aided design, and geometry processing.

**Key concepts explored:**
* Data structures for representing meshes
* Mesh simplification and subdivision
* Mesh smoothing and fairing
* Geometric property calculations (e.g., curvature)

***

### ✨ A3: Path Tracing

This assignment implements a path tracer, a powerful physically-based rendering technique. By simulating the paths of light rays, it can generate highly realistic images that accurately model global illumination, soft shadows, and complex material interactions.

**Key concepts explored:**
* The rendering equation and Monte Carlo integration
* Unbiased and consistent rendering
* Recursive ray tracing
* Advanced material models (BRDFs)
* Global illumination
* Implementing Mesh Loading
* Reflections and Path tracing
* Direct and Indirect Illumination

***

### 🤸 A4: Keyframing & Collision Detection

This project explores the principles of 3D animation and physical simulation. It combines keyframe animation to define motion with collision detection algorithms to handle interactions between objects in a dynamic scene.

**Key concepts explored:**
* Spline-based interpolation for smooth keyframing
* Collision detection algorithms
* Simple physics-based responses to collisions
* Spring Mass systems

---

## Getting Started

To get a local copy up and running, follow these simple steps.

### Prerequisites

Ensure you have a C++ compiler and a build system like `make` installed. Then go through the Makefile of the corresponding assignment and install the mentioned libraries and set the paths inside makefile and run it to see the results as shown in the report.
