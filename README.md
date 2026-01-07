# Parallel and Distributed File Processing System (C++ Version)

## Overview
This is a C++ implementation of the parallel and distributed file processing system, providing better performance than the Python version through native compilation and optimized parallel execution.

## Project Structure
```
cpp_parallel_project/
├── README.md              # Main project documentation
├── week1/
│   └── README.md          # Analysis & Design deliverables
├── week2/
│   ├── README.md          # Shared-Memory Implementation
│   ├── word_count.cpp     # OpenMP parallel word counting
│   ├── Makefile           # Build script
│   └── data/              # Symbolic link to data files
├── week3/
│   ├── README.md          # Distributed Simulation & Communication
│   ├── distributed_simulator.cpp # Thread-based distributed simulation
│   ├── Makefile           # Build script
│   └── data/              # Symbolic link to data files
├── week4/
│   ├── README.md          # Integration & Acceleration
│   ├── integrated_system.cpp # Complete integrated system
│   ├── report.md          # Final project report
│   ├── Makefile           # Build script
│   └── data/              # Symbolic link to data files
└── (complete)
```

## Weekly Breakdown

### Week 1: Analysis & Design
- Task decomposition and parallelization analysis
- Dataset selection and preparation
- Design of parallel algorithms
- Project planning and architecture

### Week 2: Shared-Memory Implementation
- OpenMP-based parallel word counting
- Performance measurement and optimization
- Thread-safe result aggregation
- Speedup and efficiency calculations

### Week 3: Distributed Simulation & Communication
- Multi-threaded distributed node simulation
- Thread-safe inter-thread communication
- Fault tolerance with failure simulation and recovery
- Result aggregation from simulated distributed nodes

### Week 4: Integration & Acceleration
- Complete system integration of all components
- Advanced visualization with ASCII charts and graphical plots
- Comprehensive performance analysis and comparison
- ASCII visualization and data export for plotting
- Final project report with conclusions and lessons learned

## Technologies Used
- **C++17**: Core programming language
- **OpenMP**: Shared-memory parallelism
- **Standard Library**: Containers, algorithms, regex
- **GNU Make**: Build system

## Building and Running
Each week's implementation includes its own build instructions in the respective README.md files.

## Performance Comparison
The C++ implementation shows better parallel performance compared to Python:
- More predictable speedup due to native threading
- Lower overhead in parallel execution
- Better memory management

## Requirements
- GCC with OpenMP support
- GNU Make
- C++17 compatible compiler