# clothsim - Cloth Simulation in C

A simple mass–spring based cloth simulation written in **C** using **SDL2** for visualization.
This project explores basic physics modeling, numerical integration, and constraint solving to simulate a deformable fabric in real time.

---

## Overview

This simulation models the cloth as a 2D grid of particles connected by structural, shear, and bend springs.
Each frame computes spring forces, gravity, and damping, then integrates the positions using a semi-implicit Euler method.
Basic mouse interaction allows tearing or dragging particles.

---

## Features

* Mass–spring system with configurable grid size and stiffness
* Gravity, damping, and tearing behavior
* Real-time visualization using SDL2
* Simple mouse-based interaction
* Clear separation between physics update and rendering code

---

## Concepts Explored

* Vector math and time-step integration
* Constraint satisfaction and spring force computation
* Numerical stability trade-offs in explicit integration
* Low-level memory and performance considerations in C

---

## Dependencies

* **C compiler** (tested on GCC/Clang)
* **SDL2** (for rendering and input)

---

## Build Instructions

```bash
gcc cloth_sim.c -lSDL2 -lm -o cloth_sim
./cloth_sim
```

---

## 📘 Notes

This project is part of my exploration into **low-level physics and graphics simulation**, aimed at understanding how real-time systems model natural phenomena from scratch.

---
