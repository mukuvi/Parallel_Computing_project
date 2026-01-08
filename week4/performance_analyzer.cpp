#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <fstream>

using namespace std;

// Simple performance data output for external plotting
void output_performance_data(const vector<pair<string, double>>& results, const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return;
    }

    file << "# Method Time(s) Speedup Efficiency" << endl;
    double baseline = results[0].second;
     // Sequential time

    for (size_t i = 0; i < results.size(); ++i) {
        const auto& result = results[i];
        double speedup = baseline / result.second;
        double efficiency = (i == 0) ? 1.0 : speedup / 4.0; // Assuming 4 threads/workers

        file << result.first << " " << fixed << setprecision(4)
             << result.second << " " << speedup << " " << efficiency << endl;
    }

    cout << "Performance data saved to " << filename << endl;
}

// Simple ASCII bar chart
void print_performance_chart(const vector<pair<string, double>>& results) {
    cout << "\nPerformance Comparison Chart:" << endl;
    cout << string(50, '=') << endl;

    double max_time = 0;
    for (const auto& result : results) {
        max_time = max(max_time, result.second);
    }

    for (const auto& result : results) {
        int bar_length = static_cast<int>((result.second / max_time) * 30);
        cout << setw(15) << left << result.first << " | ";
        for (int i = 0; i < bar_length; ++i) cout << "█";
        cout << " " << fixed << setprecision(4) << result.second << "s" << endl;
    }

    cout << string(50, '=') << endl;
}