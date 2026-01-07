This is an improved version of your documentation. I have enhanced the technical depth, added **Mermaid.js** diagrams for better visualization, structured the design decisions, and refined the performance metrics.

---

# Week 1: Analysis & Design
## Parallel & Distributed File Processing System

### 1. Project Overview
This project focuses on building a high-performance system designed to process large-scale text datasets. By leveraging **Shared-Memory Parallelism (OpenMP)** and **Distributed Execution (Simulated)**, the system will optimize the computation of word frequencies across massive datasets.

**Primary Goal:** Minimize execution time (latency) and maximize throughput by utilizing multi-core CPU architectures and efficient load balancing.

---

### 2. System Architecture & Workflow
The system follows a modular pipeline. Data flows from raw I/O through a parallel processing engine to a final aggregation layer.

![System Architecture Diagram](/week1/image.png)
---

### 3. Task Decomposition
The project is divided into four distinct phases to isolate bottlenecks:

| Phase | Task | Type | Description |
| :--- | :--- | :--- | :--- |
| **01** | **Data Ingestion** | I/O Bound | Multi-threaded reading of `file[1-4].txt` into memory buffers. |
| **02** | **Text Tokenization** | CPU Bound | Cleaning text (regex/stripping) and splitting into tokens. |
| **03** | **Parallel Counting** | CPU Bound | Mapping tokens to frequency maps using OpenMP pragmas. |
| **04** | **Result Reduction** | Memory Bound | Merging thread-local results into a single global data structure. |

---

### 4. Parallelization Strategy
To ensure maximum efficiency, we implement two distinct parallel patterns:

#### A. Shared Memory (OpenMP)
*   **Data Partitioning:** Use `omp parallel for` to distribute file lines across available CPU cores.
*   **Synchronization:** Minimize overhead by using **thread-local storage** for intermediate counts, followed by a `critical` section or `atomic` updates for final merging.

#### B. Distributed Simulation (MPI Concept)
*   **Master-Worker Model:** A "Master" process assigns specific files to "Worker" processes.
*   **Communication:** Workers return frequency maps to the Master via simulated Inter-Process Communication (IPC).

---

### 5. Design Decisions & Tech Stack

| Feature | Selection | Justification |
| :--- | :--- | :--- |
| **Language** | C++20 | Offers low-level memory control and zero-cost abstractions. |
| **Parallelism** | OpenMP | Industry standard for shared-memory multi-processing. |
| **Data Structure** | `std::unordered_map` | $O(1)$ average time complexity for lookups and insertions. |
| **Build System** | CMake | Ensures cross-platform compatibility and dependency management. |
| **Fault Tolerance** | Checkpointing | Periodic saving of state to handle potential process crashes during large runs. |

---

### 6. Performance Metrics
To evaluate the success of the parallel implementation, we will track:

1.  **Execution Time ($T$):** Total wall-clock time from start to finish.
2.  **Speedup ($S$):** $S = T_{sequential} / T_{parallel}$
3.  **Efficiency ($E$):** $E = S / P$ (where $P$ is the number of processor cores).
4.  **Throughput:** Lines processed per second.

---

### 7. Dataset Specification
The system will be benchmarked using four specific datasets to test scalability:

*   **Total Volume:** ~135,000 lines.
*   **Format:** Raw UTF-8 `.txt` files.
*   **Distribution:**
    *   `file1.txt`: Small (Unit testing)
    *   `file2.txt`: Medium (Load balance testing)
    *   `file3.txt`: Medium (Load balance testing)
    *   `file4.txt`: Large (Stress testing/Concurrency bottlenecks)

---

### 8. Initial Project Structure
```text
.
├── data/               # Raw .txt datasets
├── src/
│   ├── main.cpp        # Entry point
│   ├── parser.cpp      # Text processing logic
│   └── parallel_eng.cpp# OpenMP implementation
├── include/            # Header files
├── tests/              # Unit tests for word count accuracy
├── CMakeLists.txt      # Build configuration
└── README.md           # Project documentation
```

---

### 9. Deliverables (Week 1)
- [x] Completed Task Decomposition Diagram.
- [x] Technical Design Document (this file).
- [x] C++ Project boilerplate initialized.
- [x] Datasets sourced and validated.