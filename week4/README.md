# Week 4: Integration & Acceleration

## Overview
This final week integrates all components from previous weeks into a comprehensive parallel and distributed file processing system. It provides performance comparisons, visualization and a complete analysis of the parallel computing implementation.

## Implementation Details
- Integrates sequential, OpenMP, and distributed simulation components
- Comprehensive performance analysis with speedup and efficiency metrics
- Performance visualization with ASCII charts and data export
- Final system evaluation and comprehensive report

## Key Components
- **System Integration**: Unified interface for all processing methods
- **Performance Analysis**: Detailed metrics calculation and comparison
- **Visualization**: ASCII performance charts and data export for external plotting
- **Final Report**: Comprehensive analysis and conclusions

## Performance Results
- **Sequential**: ~0.37 seconds (baseline)
- **OpenMP Parallel**: ~0.30 seconds (1.23x speedup, 30.8% efficiency)
- **Distributed**: ~0.33 seconds (1.10x speedup, 27.4% efficiency)

## Files
- `integrated_system.cpp`: Complete integrated system with all methods
- `Makefile`: Build script
- `data/`: Symbolic link to sample text files
- `report.md`: Final comprehensive project report
- `visualize_performance.py`: Python script for generating graphical plots
- `performance_analysis.png`: Generated detailed performance analysis plot
- `performance_comparison.png`: Generated time vs speedup comparison plot
- `performance_data.txt`: Exported performance data for analysis

## How to Build and Run
```bash
make
./integrated_system
```

## Features Demonstrated
- All three processing methods integrated in one system
- Performance comparison and visualization
- Comprehensive metrics and analysis
- Data export for external plotting tools
- Final project report and conclusions
## Visualization Features
- **ASCII Charts**: Built-in terminal-based performance charts
- **Data Export**: Performance data saved to `performance_data.txt`
- **Graphical Plots**: Python script generates PNG visualizations:
  - `performance_analysis.png`: Detailed 4-panel analysis (time, speedup, efficiency, scatter plot)
  - `performance_comparison.png`: Dual-axis time vs speedup comparison
