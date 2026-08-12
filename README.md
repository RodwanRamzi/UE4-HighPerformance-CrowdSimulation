# High-Performance Crowd Simulation Manager for Unreal Engine 4.27

A lightweight, data-driven crowd simulation framework engineered natively in C++ for Unreal Engine 4.27. This project demonstrates how to bypass the heavy CPU overhead of standard engine Actor ticking by centralizing transformation logic, matrix operations, and raycast channels into a single high-performance manager class.

## 🚀 Key Technical Features

### 1. Centralized Component Arrays (O(N) Traversal)
Instead of spawning hundreds of separate expensive `ACharacter` or `AActor` blueprints—which flood the engine with individual tick registration passes—this framework utilizes a single native Manager class. It spawns and tracks raw `USkeletalMeshComponent` objects inside a contiguous memory array (`TArray`). This design maximizes CPU cache efficiency and ensures deterministic updates across all simulation agents.

### 2. High-Utility Low-Level GPU Optimizations
To preserve high rendering framerates (60+ FPS) when handling large agent counts, the source code bypasses heavy sub-routines directly on the hardware level:
* **`VisibilityBasedAnimTickOption = OnlyTickPoseWhenRendered`**: Completely stops bone transform calculations for agents outside the camera view frustum.
* **Master Pose Linking**: Synchronizes spawned components to a hidden master evaluation node to dramatically conserve processing threads.
* **Component-Fixed Skeletal Bounds**: Disables dynamic bounding-box threading, forcing the GPU to read a fixed structural size.
* **Shadow Map Suppression**: Strips shadow-casting draw calls from background background agents, freeing up significant GPU cycle times.

### 3. Dynamic Vector Raycast Floor-Snapping
To prevent agents from flying or clipping into uneven 3D map geometry (such as stairs, ramps, or landscapes), the manager executes a localized physics raycast (`LineTraceSingleByChannel`) down to the ground plane on every agent's position vector *every single frame*. The agent's Z-axis coordinates are updated procedurally to match the terrain impact point without executing heavy rigid-body engine collision scripts.

### 4. Orientation Matrix & Axis Correction
Addresses Mixamo skeletal mesh asset orientation disparities natively at the source level. The tick loop implements a custom rotation translation matrix locked strictly to the Z-axis (Yaw Only). This completely locks Pitch and Roll to `0.0f` while applying a precise `-90.0f` degree angle offset correction to ensure all tracking agents face their target objective point face-forward throughout their movement trajectory.

### 5. Spatial Neighbor Proximity Separation
Implements a lightweight, high-performance crowd padding calculation. Instead of running a costly double-nested loop checking every agent against all other agents ($O(N^2)$ lookup matrix—which bottlenecks the CPU), the tick algorithm evaluates current position states relative to adjacent cache index neighbors. If a proximity threshold violation is detected, a separation displacement force is applied via safe normalization vectors to keep the crowd structure naturally spaced.

---

## 📁 Repository File Structure

* **`CrowdSimulationManager.h`**: Modular architectural layout, function definitions matching parent `AActor` signatures, and clean property exposures via `UPROPERTY` macro specifiers.
* **`CrowdSimulationManager.cpp`**: Core implementation logic handling low-level hardware flag configuration, vector physics translations, and raycast tracking loops.

---

## 🛠️ How to Integrate This Framework

1. Clone or copy these two source files into your local Unreal Engine project `Source/` folder directory.
2. Regenerate your Visual Studio project files (via command line or `.uproject` shell integration).
3. Open your project in Visual Studio and build your game module target.
4. Inside the Unreal Editor, create a Blueprint class inheriting natively from `CrowdSimulationManager`.
5. Assign your desired Skeletal Mesh and looping Animation Track asset into the `MasterAnimComponent` slot inside your Blueprint details panel.
6. Set your maximum target agent count and hit **Play**!
