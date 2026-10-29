# 👨 High-Performance Crowd Simulation Manager for Unreal Engine 4.27

[![UE4](https://img.shields.io/badge/Unreal%20Engine-4.27-blue.svg)](https://www.unrealengine.com/)
[![C++](https://img.shields.io/badge/C++-17-blue.svg)](https://isocpp.org/)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

A **lightweight, data-driven crowd simulation framework** engineered natively in C++ for Unreal Engine 4.27. This project demonstrates how to bypass the heavy CPU overhead of standard engine Actor ticking by centralizing transformation logic, matrix operations, and raycast channels into a single high-performance manager class.

> *"50+ animated skeletal agents running at 60+ FPS – without individual Actor ticks."*

---

## 🚀 Key Technical Features

### 1. Centralized Component Arrays (O(N) Traversal)
Instead of spawning hundreds of separate expensive `ACharacter` or `AActor` Blueprints—which flood the engine with individual tick registration passes—this framework utilizes a single native Manager class. It spawns and tracks raw `USkeletalMeshComponent` objects inside a contiguous memory array (`TArray`). This design maximizes CPU cache efficiency and ensures deterministic updates across all simulation agents.

### 2. High-Utility Low-Level GPU Optimizations
To preserve high rendering framerates (60+ FPS) when handling large agent counts, the source code bypasses heavy sub-routines directly on the hardware level:

| Optimization | Impact |
| :--- | :--- |
| **VisibilityBasedAnimTickOption** | Stops bone transform calculations for agents outside the camera view frustum. |
| **Master Pose Linking** | Synchronizes components to a hidden master evaluation node to conserve processing threads. |
| **Component-Fixed Skeletal Bounds** | Disables dynamic bounding-box threading, forcing the GPU to read a fixed structural size. |
| **Shadow Map Suppression** | Strips shadow-casting draw calls from background agents, freeing up significant GPU cycle times. |

### 3. Dynamic Vector Raycast Floor-Snapping
To prevent agents from flying or clipping into uneven 3D map geometry (such as stairs, ramps, or landscapes), the manager executes a localized physics raycast (`LineTraceSingleByChannel`) down to the ground plane on every agent's position vector every single frame. The agent's Z-axis coordinates are updated procedurally to match the terrain impact point without executing heavy rigid-body engine collision scripts.

### 4. Orientation Matrix & Axis Correction
Addresses Mixamo skeletal mesh asset orientation disparities natively at the source level. The tick loop implements a custom rotation translation matrix locked strictly to the Z-axis (Yaw Only). This completely locks Pitch and Roll to 0.0f while applying a precise -90.0f degree angle offset correction to ensure all tracking agents face their target objective point face-forward throughout their movement trajectory.

### 5. Spatial Neighbor Proximity Separation
Implements a lightweight, high-performance crowd padding calculation. Instead of running a costly double-nested loop checking every agent against all other agents (O(N²) lookup matrix—which bottlenecks the CPU), the tick algorithm evaluates current position states relative to adjacent cache index neighbors. If a proximity threshold violation is detected, a separation displacement force is applied via safe normalization vectors to keep the crowd structure naturally spaced.

---

## ⚡ 401 Skeletal Mesh Stress Test (60 FPS Benchmark)

This framework is built to maximize CPU cache-line efficiency and memory layout architecture under dense simulation loads.

### 🎥 Live Performance Demonstration
[Click here to watch the 401 Agent Performance Run](High_PerformanceCrowdSimulation_StressTest_1.mp4)

### 🖥️ Target Test Environment (Low-Spec Validation)
* **CPU:** Intel Core i7 (4th Generation)
* **RAM:** 8GB DDR3 
* **GPU:** AMD Radeon RX 580
* **Load:** 401 True Dynamic Skeletal Meshes (Full bone rigs, dynamic proximity tracking enabled)

### 📈 Results
* **Frame Rate:** Stable 50 - 60 FPS under full tracking saturation.
* **Architecture:** Flat, contiguous `TArray` storage structure bypasses standard `AActor::Tick` pointer-chasing, preventing severe DDR3 memory access bottlenecks.


## 🏗️ Architecture Overview
CrowdSimulationManager (AActor)
│
├── TArray<USkeletalMeshComponent> Agents
│ └── Each agent: Skeletal Mesh + Animation
│
├── Tick Function (Every Frame)
│ ├── Floor-Snapping Raycast (LineTraceSingleByChannel)
│ ├── Orientation Matrix (Yaw-Only + -90° Correction)
│ ├── Proximity Separation (Neighbor Padding Algorithm)
│ └── GPU Optimizations (LOD, Shadow Suppression, Master Pose)
│
└── Blueprint Exposed Properties
├── MaxAgents (int)
├── MasterAnimComponent (UAnimationAsset)
└── TargetLocation (FVector)

text

---

## 📁 Repository File Structure

| File | Description |
| :--- | :--- |
| `CrowdSimulationManager.h` | Modular architectural layout, function definitions matching parent `AActor` signatures, and clean property exposures via `UPROPERTY` macro specifiers. |
| `CrowdSimulationManager.cpp` | Core implementation logic handling low-level hardware flag configuration, vector physics translations, and raycast tracking loops. |

---

## 🛠️ How to Integrate This Framework

1. Clone or copy these two source files into your local Unreal Engine project `Source/` folder directory.
2. Regenerate your Visual Studio project files (via command line or `.uproject` shell integration).
3. Open your project in Visual Studio and build your game module target.
4. Inside the Unreal Editor, create a Blueprint class inheriting natively from `CrowdSimulationManager`.
5. Assign your desired Skeletal Mesh and looping Animation Track asset into the `MasterAnimComponent` slot inside your Blueprint details panel.
6. Set your maximum target agent count and hit Play!

---

## 💡 Why This Project Matters

| Challenge | Solution |
| :--- | :--- |
| **CPU Overload** | Centralized manager – no individual Actor ticks |
| **GPU Bottleneck** | Visibility culling, shadow suppression, fixed bounds |
| **Terrain Clipping** | Dynamic floor-snapping via raycast |
| **Orientation Issues** | Custom rotation matrix with Yaw-only lock |
| **Crowd Clumping** | Lightweight neighbor separation algorithm |

---


## 🔗 Related Projects

- [**MyGraphicEngineOpenGL**](https://github.com/RodwanRamzi/MyGraphicEngineOpenGL) – Custom OpenGL 4.6 Deferred PBR Renderer
- [**Snake AI – Reinforcement Learning**](https://github.com/RodwanRamzi/snake-ai-reinforcement-learning) – Deep Q-Network agent
- [**Weather Regression Pipeline**](https://github.com/RodwanRamzi/weather-regression-pipeline) – ML data pipeline

---

## 📫 Connect With Me

- **LinkedIn**: [linkedin.com/in/rodwan-ramzi](https://linkedin.com/in/rodwan-ramzi)
- **GitHub**: [github.com/RodwanRamzi](https://github.com/RodwanRamzi)

---

## 📜 License

This project is licensed under the **MIT License** – feel free to use, modify, and distribute.

---

**Made with ❤️ in Libya 🇱🇾** | **Unreal Engine 4.27** | **C++17**
