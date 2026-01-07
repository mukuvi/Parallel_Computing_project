# Week 3: Distributed Simulation & Communication

## Overview
This week implements distributed simulation using multiple threads to simulate distributed computing nodes. The system demonstrates inter-thread communication, data aggregation, and basic fault tolerance.

## Implementation Details
- Uses multiple threads to simulate distributed nodes
- Implements thread-safe communication via message queues
- Handles collective communication for result aggregation
- Includes fault tolerance with process failure simulation and recovery

## Key Components
- **Thread Management**: std::thread-based worker simulation
- **Communication**: Thread-safe queue for message passing
- **Aggregation**: Master-worker pattern for result collection
- **Fault Tolerance**: Random failure simulation with retry mechanisms

## Performance Results
- Distributed simulation: ~0.36 seconds (4 workers)
- Fault tolerance: Automatic retry on simulated failures
- Communication: Efficient thread-safe message passing

## Files
- `distributed_simulator.cpp`: Main distributed simulation implementation
- `Makefile`: Build script
- `data/`: Symbolic link to sample text files

## How to Build and Run
```bash
make
./distributed_simulator
```

## Features Demonstrated
- Multi-threaded distributed simulation
- Thread-safe communication
- Fault tolerance with recovery
- Result aggregation from simulated nodes