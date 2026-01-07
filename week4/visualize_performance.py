#!/usr/bin/env python3
"""
Performance Visualization Script

This script reads performance data from performance_data.txt
and generates a visualization plot.
"""

import matplotlib
matplotlib.use('Agg')  # Use non-interactive backend
import matplotlib.pyplot as plt
import numpy as np

def load_performance_data(filename):
    """Load performance data from file."""
    methods = []
    times = []
    speedups = []
    efficiencies = []

    try:
        with open(filename, 'r') as f:
            next(f)  # Skip header
            for line in f:
                parts = line.strip().split()
                if len(parts) >= 4:
                    methods.append(parts[0])
                    times.append(float(parts[1]))
                    speedups.append(float(parts[2]))
                    efficiencies.append(float(parts[3]))
    except FileNotFoundError:
        print(f"Error: {filename} not found. Run the integrated system first.")
        return None, None, None, None

    return methods, times, speedups, efficiencies

def create_performance_plot(methods, times, speedups, efficiencies):
    """Create performance visualization plots."""

    # Create subplots
    fig, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2, figsize=(12, 8))
    fig.suptitle('Parallel and Distributed File Processing - Performance Analysis', fontsize=14)

    # Colors for different methods
    colors = ['red', 'blue', 'green']

    # Execution Time Plot
    bars1 = ax1.bar(methods, times, color=colors, alpha=0.7)
    ax1.set_title('Execution Time Comparison')
    ax1.set_ylabel('Time (seconds)')
    ax1.set_xlabel('Processing Method')
    ax1.grid(True, alpha=0.3)

    # Add value labels on bars
    for bar, time in zip(bars1, times):
        ax1.text(bar.get_x() + bar.get_width()/2, bar.get_height() + 0.001,
                f'{time:.4f}s', ha='center', va='bottom', fontsize=9)

    # Speedup Plot
    bars2 = ax2.bar(methods, speedups, color=colors, alpha=0.7)
    ax2.set_title('Speedup Analysis')
    ax2.set_ylabel('Speedup Factor')
    ax2.set_xlabel('Processing Method')
    ax2.axhline(y=1.0, color='black', linestyle='--', alpha=0.5, label='Baseline')
    ax2.grid(True, alpha=0.3)
    ax2.legend()

    # Add value labels on bars
    for bar, speedup in zip(bars2, speedups):
        ax2.text(bar.get_x() + bar.get_width()/2, bar.get_height() + 0.01,
                f'{speedup:.2f}x', ha='center', va='bottom', fontsize=9)

    # Efficiency Plot
    bars3 = ax3.bar(methods, efficiencies, color=colors, alpha=0.7)
    ax3.set_title('Parallel Efficiency')
    ax3.set_ylabel('Efficiency')
    ax3.set_xlabel('Processing Method')
    ax3.axhline(y=1.0, color='red', linestyle='--', alpha=0.5, label='Perfect Efficiency')
    ax3.grid(True, alpha=0.3)
    ax3.legend()

    # Add value labels on bars
    for bar, efficiency in zip(bars3, efficiencies):
        ax3.text(bar.get_x() + bar.get_width()/2, bar.get_height() + 0.01,
                f'{efficiency:.2f}', ha='center', va='bottom', fontsize=9)

    # Time vs Speedup Scatter Plot
    ax4.scatter(times, speedups, s=100, c=colors, alpha=0.7)
    for i, method in enumerate(methods):
        ax4.annotate(method, (times[i], speedups[i]),
                    xytext=(5, 5), textcoords='offset points', fontsize=10)
    ax4.set_title('Time vs Speedup Relationship')
    ax4.set_xlabel('Execution Time (seconds)')
    ax4.set_ylabel('Speedup Factor')
    ax4.grid(True, alpha=0.3)

    plt.tight_layout()
    plt.savefig('performance_analysis.png', dpi=150, bbox_inches='tight')
    print("Performance visualization saved as 'performance_analysis.png'")

    # Also create a simple comparison chart
    plt.figure(figsize=(10, 6))
    x = np.arange(len(methods))
    width = 0.35

    fig, ax = plt.subplots(figsize=(10, 6))
    bars1 = ax.bar(x - width/2, times, width, label='Time (s)', alpha=0.7, color='skyblue')
    ax2 = ax.twinx()
    bars2 = ax2.bar(x + width/2, speedups, width, label='Speedup', alpha=0.7, color='lightcoral')

    ax.set_xlabel('Processing Method')
    ax.set_ylabel('Execution Time (seconds)', color='skyblue')
    ax2.set_ylabel('Speedup Factor', color='lightcoral')
    ax.set_title('Performance Comparison: Time vs Speedup')
    ax.set_xticks(x)
    ax.set_xticklabels(methods)

    # Add value labels
    for bar, time in zip(bars1, times):
        ax.text(bar.get_x() + bar.get_width()/2, bar.get_height() + 0.001,
               f'{time:.4f}s', ha='center', va='bottom', fontsize=8)

    for bar, speedup in zip(bars2, speedups):
        ax2.text(bar.get_x() + bar.get_width()/2, bar.get_height() + 0.01,
                f'{speedup:.2f}x', ha='center', va='bottom', fontsize=8)

    plt.tight_layout()
    plt.savefig('performance_comparison.png', dpi=150, bbox_inches='tight')
    print("Performance comparison saved as 'performance_comparison.png'")

if __name__ == "__main__":
    print("Generating performance visualizations...")

    methods, times, speedups, efficiencies = load_performance_data("performance_data.txt")

    if methods:
        create_performance_plot(methods, times, speedups, efficiencies)
        print("\nVisualization complete!")
        print("Generated files:")
        print("- performance_analysis.png (detailed analysis)")
        print("- performance_comparison.png (time vs speedup)")
    else:
        print("No performance data found. Run the integrated system first.")