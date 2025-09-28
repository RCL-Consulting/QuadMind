![License](https://img.shields.io/github/license/RCL-Consulting/QuadMind)
![GitHub code size in bytes](https://img.shields.io/github/languages/code-size/RCL-Consulting/QuadMind)
![GitHub last commit](https://img.shields.io/github/last-commit/RCL-Consulting/QuadMind)

# QuadMind – AI-Assisted Mesh Generation

QuadMind is an experimental project exploring the use of **machine learning** to assist in generating **unstructured finite element meshes**.  

As a starting point, [Karl Levik’s Q-Morph algorithm](https://github.com/KarlLevik/qmorph) has been ported from Java to modern **C++20**, with ongoing work on debugging, extending functionality, and preparing the mesh data for AI/ML pipelines.

---

## ✨ Features (current & planned)
- ✅ Port of Q-Morph core algorithm from Java → C++20  
- 🔄 Unit tests and validation against the original Java implementation  
- 📊 Designed to generate datasets for AI/ML mesh quality analysis  
- 🚀 Planned: support for surface meshing (NURBS, CAD) and 3D extension  

---

## 🔧 Build Instructions
This project uses **CMake** (with optional [vcpkg](https://github.com/microsoft/vcpkg) integration).  

```bash
git clone https://github.com/RCL-Consulting/QuadMind.git
cd QuadMind
cmake -B build
cmake --build build
