# 👨‍💻 High-Performance Crowd Simulation Framework for Unreal Engine 4.27

[![UE4](https://shields.io)](https://unrealengine.com)
[![C++](https://shields.io)](https://isocpp.org)
[![License: MIT](https://shields.io)](https://opensource.org)

A **lightweight, data-oriented crowd simulation framework** engineered natively in C++ for Unreal Engine 4.27. This framework completely bypasses the heavy CPU overhead of standard engine `AActor` ticking by centralizing transformation logic, matrix operations, and physics raycast channels into a single high-performance manager class.

> *"400+ fully rigged skeletal agents running at 60 FPS on low-spec legacy hardware — without individual Actor ticks."*

---

## ⚡ Low-Spec Hardware Benchmark & Stress Test

This framework was intentionally benchmarked on older, budget-constrained hardware to stress-test raw CPU cache-line efficiency, memory layout limits, and hardware-level thread optimizations.

### 🎥 Live Performance Demonstration
<video src="https://githubusercontent.com" width="100%" controls autoplay loop muted></video>

[Click here to watch or download the raw 401 Agent Performance Run](https://github.com/RodwanRamzi/UE4-HighPerformance-CrowdSimulation/blob/main/High_PerformanceCrowdSimulation_StressTest_1.mp4)


### 🖥️ Target Test Environment (Low-Spec Validation)
* **CPU:** Intel Core i7 (4th Generation)
* **RAM:** 8GB DDR3 (High-latency, bandwidth-constrained configuration)
* **GPU:** AMD Radeon RX 580
* **Simulation Load:** 401 True Dynamic Skeletal Meshes (Full bone structures, active proximity tracking logic enabled)

### 📈 Results
* **Frame Rate:** Stable **50 - 60 FPS** under full tracking saturation.
* **Memory Architecture:** Flat, contiguous `TArray` storage structures ensure high cache hit ratios during real-time updates, completely preventing the severe DDR3 memory access bottlenecks caused by standard engine pointer-chasing.
* **Algorithmic Traversal:** Centralized manager executes strict O(N) linear traversal, maximizing L3 cache efficiency on legacy architecture.

---

## 🚀 Key Technical Features

### 1. Centralized Component Arrays (Data-Oriented Design)
Instead of spawning hundreds of separate, expensive `ACharacter` or `AActor` Blueprints — which flood the engine with individual tick registration passes and fragment memory — this framework utilizes a single native Manager class. It spawns and tracks raw `USkeletalMeshComponent` objects inside a tightly packed contiguous memory array (`TArray`). This design maximizes CPU cache efficiency and ensures deterministic updates across all simulation agents.

### 2. High-Utility Low-Level GPU Optimizations
To preserve high rendering framerates when handling dense agent counts, the source code bypasses heavy sub-routines directly on the hardware level:

| Optimization | Impact |
| :--- | :--- |
| **VisibilityBasedAnimTickOption** | Stops bone transform calculations completely for agents outside the camera view frustum. |
| **Master Pose Linking** | Synchronizes components to a hidden master evaluation node to conserve processing threads. |
| **Component-Fixed Skeletal Bounds** | Disables dynamic bounding-box calculation threading, forcing the GPU to read a fixed structural size. |
| **Shadow Map Suppression** | Strips shadow-casting draw calls from background agents, freeing up significant GPU cycle times. |

### 3. Dynamic Vector Raycast Floor-Snapping
To prevent agents from flying or clipping into uneven 3D map geometry (such as stairs, ramps, or landscapes), the manager executes a localized physics raycast (`LineTraceSingleByChannel`) down to the ground plane on every agent's position vector every single frame. The agent's Z-axis coordinates are updated procedurally to match the terrain impact point without executing heavy rigid-body engine collision scripts.

### 4. Orientation Matrix & Axis Correction
Addresses Mixamo skeletal mesh asset orientation disparities natively at the source level. The tick loop implements a custom rotation translation matrix locked strictly to the Z-axis (Yaw Only). This completely locks Pitch and Roll to `0.0f` while applying a precise `-90.0f` degree angle offset correction to ensure all tracking agents face their target objective point face-forward throughout their movement trajectory.

### 5. Spatial Neighbor Proximity Separation
Implements a lightweight, high-performance crowd padding calculation. Instead of running a costly double-nested loop checking every agent against all other agents (O(N²) lookup matrix — which bottlenecks the CPU), the tick algorithm evaluates current position states relative to adjacent cache index neighbors. If a proximity threshold violation is detected, a separation displacement force is applied via safe normalization vectors to keep the crowd structure naturally spaced.

---

## 🏗️ Architecture Overview

```text
CrowdSimulationManager (AActor)
│
├── TArray<USkeletalMeshComponent> Agents
│   └── Each agent: True Skeletal Mesh + Active Animation Track
│
├── Tick Function (Every Frame)
│   ├── Floor-Snapping Raycast (LineTraceSingleByChannel)
│   ├── Orientation Matrix (Yaw-Only + -90° Correction)
│   ├── Proximity Separation (Neighbor Padding Algorithm)
│   └── GPU Optimizations (LOD, Shadow Suppression, Master Pose)
│
└── Blueprint Exposed Properties
    ├── MaxAgents (int)
    ├── MasterAnimComponent (UAnimationAsset)
    └── TargetLocation (FVector)
```

---

## 💡 Architectural Solutions Breakdown

| Challenge | Solution |
| :--- | :--- |
| **CPU Thread Overload** | Centralized manager — zero individual Actor ticks, strict O(N) lookup loop. |
| **GPU Bottleneck** | Visibility frustum culling, shadow map suppression, and fixed bounds evaluation. |
| **Terrain Clipping** | Dynamic, low-cost floor-snapping via single-channel vector raycasts. |
| **Asset Orientation Glitches** | Custom hardware-level rotation matrix with strict Yaw-only axis locking. |
| **Crowd Clumping** | Scalable, cache-friendly neighbor proximity padding algorithm. |

---

## 📁 Repository File Structure

| File | Description |
| :--- | :--- |
| `CrowdSimulationManager.h` | Modular architectural layout, function definitions matching parent `AActor` signatures, and clean property exposures via `UPROPERTY` macro specifiers. |
| `CrowdSimulationManager.cpp` | Core implementation logic handling low-level hardware flag configurations, vector physics translations, and cache-optimized tracking loops. |

---

## 🛠️ How to Integrate This Framework

1. Clone or copy these two source files into your local Unreal Engine project `Source/` folder directory.
2. Regenerate your Visual Studio project files (via command line or `.uproject` right-click context option).
3. Open your project in Visual Studio and build your game module target.
4. Inside the Unreal Editor, create a Blueprint class inheriting natively from `CrowdSimulationManager`.
5. Assign your desired Skeletal Mesh and looping Animation Track asset into the `MasterAnimComponent` slot inside your Blueprint details panel.
6. Set your maximum target agent count and hit Play!

---

## 🔗 Related Systems Engineering Projects

- [**MyGraphicEngineOpenGL**](https://github.com) – Custom OpenGL 4.6 Deferred PBR Renderer built from scratch in C++20.
- [**Snake AI – Reinforcement Learning**](https://github.com) – Deep Q-Network agent built using PyTorch and Pygame.
- [**Weather Regression Pipeline**](https://github.com) – End-to-end ML telemetry data pipeline utilizing Scikit-Learn.

---

## 📫 Connect With Me

- **LinkedIn**: [://linkedin.com](https://://linkedin.com)
- **GitHub**: [://github.com](https://://github.com)

---

## 📜 License

This project is licensed under the **MIT License** – feel free to use, modify, and distribute it in commercial pipelines.

---

**Built with 💻 in Libya 🇱🇾 by Rodwan Ramzi**
