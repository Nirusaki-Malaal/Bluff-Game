# Bluff-Game

Bluff-Game is a native C++ implementation of the classic Bluff (also known as Cheat or I Doubt It) card game. The project is engineered to demonstrate core systems programming principles, multi-threaded concurrency, low-latency socket networking, process synchronization, and hardware-accelerated 2D graphics rendering using the SDL3 API.

## System Architecture

The project is structured into modular, decoupled libraries and binary targets to separate networking, game domain logic, artificial intelligence, and hardware presentation layers:

- `common`: An interface library defining application-layer network protocols, packet framing, binary serialization structures, and core card game primitives.
- `server`: A standalone systems library and network engine managing authoritative game state, client session multiplexing, turn validation, and synchronization barriers.
- `client`: A client-side subsystem handling local player state, input collection, network communication with the remote server, and event dispatch.
- `src/main.cpp`: The application entry point responsible for initializing runtime environments, configuring operational modes, and driving the SDL3 main rendering loop.

```
       +-------------------------------------------------------+
       |                  Bluff Application                    |
       |                      (bluff)                          |
       +---------------------------+---------------------------+
                                   |
         +-------------------------+-------------------------+
         |                                                   |
         v                                                   v
+------------------+                               +------------------+
|  Client Engine   |                               |  Server Daemon   |
|   (client_lib)   |                               |   (server_lib)   |
+--------+---------+                               +---------+--------+
         |                                                   |
         +-------------------------+-------------------------+
                                   |
                                   v
                   +-------------------------------+
                   |       Common Protocol         |
                   |       & State Domain          |
                   |          (common)             |
                   +---------------+---------------+
                                   |
                                   v
                   +-------------------------------+
                   |         SDL3 Renderer         |
                   |      & Hardware Backend       |
                   +-------------------------------+
```

## Operating Modes

Bluff-Game supports three primary operational configurations designed to exercise distinct synchronization and communication topologies:

### 1. Local Multiplayer Mode
In local multiplayer mode, multiple human players interact on a single host machine through shared input devices or alternating turn sessions.
- State Isolation: The engine manages private hand visibility by swapping viewport masks and active player contexts, preventing illicit observation of opponent cards.
- Synchronous In-Memory Dispatch: Actions are routed through an internal event bus, bypassing socket overhead while preserving identical deterministic state machine transitions used across network modes.

### 2. Online Multiplayer Mode
Online multiplayer operates on an authoritative client-server architecture designed to eliminate desynchronization and client-side state manipulation.
- Authoritative Simulation: The dedicated server maintains the master copy of the deck, player hands, discard pile, and turn order. Clients transmit intentions (play cards, challenge bluff); the server validates legality, resolves challenges, and broadcasts updated state snapshots.
- Asynchronous Network I/O: Socket operations are decoupled from rendering, ensuring that network jitter or blocking system calls do not freeze visual presentation.
- Binary Protocol Serialization: All data payloads are serialized into tightly packed binary packets with strict network byte order (Big Endian) conversions.

### 3. CPU Mode (Automated Adversaries)
CPU mode introduces autonomous artificial intelligence agents capable of participating in both local and online sessions.
- Heuristic and Probabilistic Modeling: CPU agents evaluate bluff probabilities using Bayesian updates based on observed discard counts, known cards held in hand, declared ranks, and historical claim consistency of opponents.
- Non-Blocking Background Evaluation: AI decision calculations execute on dedicated worker threads, ensuring that complex tree searches and statistical sampling do not degrade frame pacing.

## Hardware-Accelerated Rendering Pipeline (SDL3)

The graphical interface is built directly on the Simple DirectMedia Layer 3 (SDL3) API, taking advantage of modern GPU-accelerated 2D rendering backends:

- Hardware Abstraction: Utilizes `SDL_Renderer` and `SDL_Texture` abstractions backed by platform-native APIs (Vulkan, OpenGL, Direct3D, or Metal) to ensure consistent high frame rates and minimal CPU overhead.
- Decoupled Tick and Frame Architecture: The simulation executes at a fixed timestep (60 Hz) for deterministic state evaluation, while the rendering pipeline runs with variable display refresh rates. Visual card movement, pile drops, and UI highlights employ linear interpolation (lerp) across tick states to deliver fluid animations.
- Resource Encapsulation: Texture atlases, fonts, and window contexts are managed via strict RAII (Resource Acquisition Is Initialization) wrappers, eliminating resource leaks and dangling handle dereferences across window lifecycle events.

## Core Computer Science and Systems Concepts

### 1. Systems Programming
The codebase emphasizes direct hardware awareness, predictable memory layouts, and efficient interaction with OS-level facilities:
- Contiguous Memory Layouts: Deck instances, player card collections, and discard stacks are laid out in contiguous memory buffers (`std::vector` and `std::span`). This design avoids pointer indirection, preserves spatial locality, and maximizes CPU L1/L2 cache hit ratios during frequent turn evaluations.
- Memory Alignment and Packing: Network structures utilize explicit packing directives (`#pragma pack(push, 1)`) and fixed-width scalar types (`uint8_t`, `uint16_t`, `uint32_t`) to prevent compiler-injected padding bytes from altering binary layouts across differing hardware targets.
- Zero-Copy Parsing: Received socket streams are read directly into pre-allocated memory arenas. Packet headers and payloads are accessed via typed pointer overlays and memory views without intermediate string formatting or redundant heap allocations.
- Deterministic Lifetime Control: All native handles, including POSIX socket file descriptors, SDL3 window handles, and thread handles, are bound to C++ stack-allocated RAII guardians to ensure deterministic destruction under normal operation and exception unwinding.

### 2. Concurrency and Parallelism
The application implements multi-threaded concurrency to maintain high responsiveness and throughput:
- Thread Role Partitioning:
  1. Main / Render Thread: Manages OS window events, user inputs, and SDL3 GPU command generation.
  2. Network Worker Thread: Runs non-blocking socket loops, draining kernel receive buffers and enqueuing validated packets.
  3. Game Simulation Thread: Drives turn progression, validates card plays, and resolves bluff challenges.
  4. AI Worker Pool: Computes probabilistic bluff thresholds and optimal actions across parallel CPU threads.
- Atomic Memory Ordering: Critical flags (such as shutdown notifications and connection state changes) employ `std::atomic` variables configured with explicit memory ordering semantics (`std::memory_order_acquire`, `std::memory_order_release`, and `std::memory_order_relaxed`), avoiding unnecessary CPU memory bus serialization penalties.

### 3. Networking and Socket Engineering
The network layer is designed for robust distributed communication over Layer 4 protocols:
- Reliable Stream Transport: TCP sockets are used to guarantee in-order, lossless transmission of game actions, state transitions, and bluff resolutions.
- Binary Protocol Framing:
  * Packets begin with a fixed 3-byte header:
    - `PacketType` (1 byte): Identifies packet category (Handshake, PlayCard, CallBluff, StateUpdate, ErrorNotification).
    - `PayloadLength` (2 bytes, network byte order): Explicit byte count of the trailing payload.
  * Framing handles TCP stream fragmentation and aggregation: the network receiver reconstructs logical packets across partial socket reads before dispatching to the simulation queue.
- Socket Multiplexing: The server employs non-blocking sockets integrated with OS multiplexing primitives (such as `epoll` on Linux or POSIX `poll`), allowing a single network thread to monitor and service dozens of concurrent client connections without spawning a dedicated thread per connection.
- Defensive Packet Validation: Every received message undergoes bounds checking and sanity validation against the current game state before execution, guarding against malformed packets and invalid state transitions.

### 4. Process and Thread Synchronization
Concurrent subsystems coordinate state transitions through robust synchronization primitives:
- Mutual Exclusion: Shared resources, such as the authoritative game state and active client registries, are protected using `std::mutex` and `std::shared_mutex`. Reader threads (such as the renderer or monitoring threads) acquire shared locks, while writer threads (turn processing) acquire exclusive locks.
- Condition Variables: Inter-thread signaling between network receivers and the game simulation engine utilizes `std::condition_variable`. Threads remain suspended in kernel wait queues until work arrives, eliminating CPU-consuming busy-wait polling loops.
- Lock-Free Ring Buffers: Low-latency inter-thread handoffs between the network thread and the main simulation loop employ Single-Producer Single-Consumer (SPSC) lock-free circular queues, eliminating lock contention on high-frequency packet processing paths.
- Deadlock Prevention: Mutex acquisition follows a strict global hierarchical order. Compound lock acquisitions leverage `std::scoped_lock` with deadlock-avoidance algorithms to prevent circular wait conditions.

## Project Structure

```
Bluff-Game/
├── CMakeLists.txt        # Root build configuration and target definitions
├── Makefile              # Convenience build automation
├── LICENSE               # Project license (MIT)
├── README.md             # Systems documentation and technical specification
├── .gitignore            # Version control exclusions for C++ and CMake
├── assets/               # Visual assets and font definitions
│   └── .gitkeep
├── common/               # Protocol and shared domain types
│   ├── CMakeLists.txt
│   ├── include/
│   │   └── .gitkeep
│   └── src/
│       └── .gitkeep
├── server/               # Authoritative game server engine
│   ├── CMakeLists.txt
│   ├── include/
│   │   └── server.hpp
│   └── src/
│       └── server.cpp
├── client/               # Client-side networking and state cache
│   ├── CMakeLists.txt
│   ├── include/
│   │   └── client.hpp
│   └── src/
│       └── client.cpp
├── src/                  # Main entry point and SDL3 presentation loop
│   └── main.cpp
└── tests/                # Unit and integration test suites
    └── .gitkeep
```

## Build and Execution

### Prerequisites
- CMake 3.20 or newer
- C++20 compliant compiler (GCC 11+, Clang 13+, or MSVC 2019+)
- Ninja build system (or Make)
- SDL3 development libraries installed on host system

### Compilation
Configure and build using CMake and Ninja:

```bash
# Configure build tree
cmake -B build -G Ninja

# Compile all targets
cmake --build build
```

Alternatively, use the provided Makefile:

```bash
make build
```

### Execution
Run the compiled binary:

```bash
./build/bin/bluff
```

Or execute via Make:

```bash
make run
```
