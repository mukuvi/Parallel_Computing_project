# Week 2: Shared-Memory Implementation

## Overview
This week implements the core parallel word counting using OpenMP for shared-memory parallelism.

## Implementation Details
- Uses OpenMP parallel for loops and critical sections
- Processes multiple files concurrently
- Measures execution time and calculates speedup and efficiency

## Performance Results
- Sequential: ~1.13 seconds
- Parallel (4 threads): ~1.02 seconds
- Speedup: 1.11x
- Efficiency: 27.7%

## Files
- `word_count.cpp`: Main implementation with OpenMP
- `Makefile`: Build script
- `data/`: Symbolic link to sample text files

## How to Build and Run
```bash
make
./word_count
```

## Key Features
- Thread-safe result aggregation using OpenMP critical sections
- Consistent results between sequential and parallel versions
- Performance metrics calculation