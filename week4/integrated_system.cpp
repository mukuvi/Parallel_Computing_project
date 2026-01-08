#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <chrono>
#include <omp.h>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <random>
#include <iomanip>

using namespace std;

// using namespace std;

// Forward declarations
string read_file(const string& filename);
unordered_map<string, int> count_words(const string& text);
pair<unordered_map<string, int>, double> sequential_word_count(const vector<string>& filenames);
pair<unordered_map<string, int>, double> parallel_word_count(const vector<string>& filenames, int num_threads);
pair<unordered_map<string, int>, double> distributed_word_count(const vector<string>& filenames, int num_workers);

// Implementation of helper functions
string read_file(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return "";
    }
    string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    return content;
}

unordered_map<string, int> count_words(const string& text) {
    unordered_map<string, int> word_count;
    size_t pos = 0;
    string word;
    string lower_text = text;
    transform(lower_text.begin(), lower_text.end(), lower_text.begin(), ::tolower);

    while ((pos = lower_text.find_first_not_of(" \t\n\r\f\v", pos)) != string::npos) {
        size_t end = lower_text.find_first_of(" \t\n\r\f\v", pos);
        if (end == string::npos) end = lower_text.length();
        word = lower_text.substr(pos, end - pos);

        // Remove punctuation
        word.erase(remove_if(word.begin(), word.end(),
                    [](char c) { return !isalnum(c); }), word.end());

        if (!word.empty()) {
            word_count[word]++;
        }
        pos = end;
    }

    return word_count;
}

// Sequential word count
pair<unordered_map<string, int>, double> sequential_word_count(const vector<string>& filenames) {
    auto start = chrono::high_resolution_clock::now();

    unordered_map<string, int> total_count;
    for (const auto& filename : filenames) {
        string content = read_file(filename);
        auto file_count = count_words(content);
        for (const auto& pair : file_count) {
            total_count[pair.first] += pair.second;
        }
    }

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;

    return {total_count, duration.count()};
}

// Parallel word count using OpenMP
pair<unordered_map<string, int>, double> parallel_word_count(const vector<string>& filenames, int num_threads) {
    auto start = chrono::high_resolution_clock::now();

    unordered_map<string, int> total_count;

    #pragma omp parallel num_threads(num_threads)
    {
        unordered_map<string, int> local_count;

        #pragma omp for
        for (size_t i = 0; i < filenames.size(); ++i) {
            string content = read_file(filenames[i]);
            auto file_count = count_words(content);
            for (const auto& pair : file_count) {
                local_count[pair.first] += pair.second;
            }
        }

        #pragma omp critical
        {
            for (const auto& pair : local_count) {
                total_count[pair.first] += pair.second;
            }
        }
    }

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;

    return {total_count, duration.count()};
}

// Message structure for distributed simulation
struct Message {
    int worker_id;
    string filename;
    unordered_map<string, int> result;
    bool is_failure;
};

// Thread-safe queue
class SafeQueue {
private:
    queue<Message> q;
    mutex mtx;
    condition_variable cv;
public:
    void push(const Message& msg) {
        unique_lock<mutex> lock(mtx);
        q.push(msg);
        cv.notify_one();
    }

    Message pop() {
        unique_lock<mutex> lock(mtx);
        cv.wait(lock, [this]() { return !q.empty(); });
        Message msg = q.front();
        q.pop();
        return msg;
    }
};

// Worker thread function
void worker_thread(SafeQueue& request_queue, SafeQueue& response_queue, int worker_id) {
    while (true) {
        Message request = request_queue.pop();
        if (request.filename.empty()) break; // Exit signal

        // Simulate potential failure (10% chance)
        random_device rd;
        mt19937 gen(rd());
        uniform_real_distribution<> dis(0, 1);
        if (dis(gen) < 0.1) {
            cout << "Worker " << worker_id << " failed (simulated)" << endl;
            Message response{worker_id, "", {}, true};
            response_queue.push(response);
            continue;
        }

        // Process file
        string content = read_file(request.filename);
        auto word_count = count_words(content);

        Message response{worker_id, request.filename, word_count, false};
        response_queue.push(response);
        cout << "Worker " << worker_id << " completed processing " << request.filename << endl;
    }
}

// Distributed word count
pair<unordered_map<string, int>, double> distributed_word_count(const vector<string>& filenames, int num_workers) {
    auto start = chrono::high_resolution_clock::now();

    SafeQueue request_queue;
    SafeQueue response_queue;

    // Start worker threads
    vector<thread> workers;
    for (int i = 0; i < num_workers; ++i) {
        workers.emplace_back(worker_thread, ref(request_queue), ref(response_queue), i);
    }

    // Send work requests
    for (size_t i = 0; i < filenames.size(); ++i) {
        Message request{static_cast<int>(i % num_workers), filenames[i], {}, false};
        request_queue.push(request);
    }

    // Send exit signals
    for (int i = 0; i < num_workers; ++i) {
        Message exit_msg{-1, "", {}, false};
        request_queue.push(exit_msg);
    }

    // Collect results
    unordered_map<string, int> total_count;
    int responses_received = 0;
    int expected_responses = filenames.size();

    while (responses_received < expected_responses) {
        Message response = response_queue.pop();

        if (response.is_failure) {
            // Retry failed task sequentially
            cout << "Retrying failed task for worker " << response.worker_id << endl;
            // Find the filename for this worker
            if (response.worker_id < (int)filenames.size()) {
                string content = read_file(filenames[response.worker_id]);
                auto word_count = count_words(content);
                for (const auto& pair : word_count) {
                    total_count[pair.first] += pair.second;
                }
            }
        } else {
            // Aggregate successful results
            for (const auto& pair : response.result) {
                total_count[pair.first] += pair.second;
            }
        }
        responses_received++;
    }

    // Wait for workers to finish
    for (auto& worker : workers) {
        worker.join();
    }

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;

    return {total_count, duration.count()};
}

// Performance analyzer functions
void output_performance_data(const vector<pair<string, double>>& results, const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return;
    }

    file << "# Method Time(s) Speedup Efficiency" << endl;
    double baseline = results[0].second; // Sequential time

    for (size_t i = 0; i < results.size(); ++i) {
        const auto& result = results[i];
        double speedup = baseline / result.second;
        double efficiency = (i == 0) ? 1.0 : speedup / 4.0; // Assuming 4 threads/workers

        file << result.first << " " << fixed << setprecision(4)
             << result.second << " " << speedup << " " << efficiency << endl;
    }

    cout << "Performance data saved to " << filename << endl;
}

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

int main() {
    vector<string> filenames = {
        "data/file1.txt",
        "data/file2.txt",
        "data/file3.txt",
        "data/file4.txt"
    };

    cout << "=== Parallel and Distributed File Processing System ===" << endl;
    cout << "Processing " << filenames.size() << " files" << endl << endl;

    vector<pair<string, double>> results;
    unordered_map<string, int> final_counts;

    // Sequential processing
    cout << "--- Sequential Processing ---" << endl;
    auto [seq_counts, seq_time] = sequential_word_count(filenames);
    final_counts = seq_counts;
    results.emplace_back("Sequential", seq_time);
    cout << "Completed in " << fixed << setprecision(4) << seq_time << " seconds." << endl << endl;

    // Parallel processing (OpenMP)
    cout << "--- Parallel Processing (Shared Memory) ---" << endl;
    int num_threads = min(4, (int)filenames.size());
    auto [par_counts, par_time] = parallel_word_count(filenames, num_threads);
    results.emplace_back("Parallel", par_time);
    cout << "Completed in " << fixed << setprecision(4) << par_time << " seconds using " << num_threads << " threads." << endl;

    // Verify consistency
    if (seq_counts == par_counts) {
        cout << "✓ Results are consistent with sequential version." << endl;
    } else {
        cout << "✗ Results differ from sequential version!" << endl;
    }
    cout << endl;

    // Distributed simulation
    cout << "--- Distributed Simulation ---" << endl;
    int num_workers = min(4, (int)filenames.size());
    auto [dist_counts, dist_time] = distributed_word_count(filenames, num_workers);
    results.emplace_back("Distributed", dist_time);
    cout << "Completed in " << fixed << setprecision(4) << dist_time << " seconds using " << num_workers << " workers." << endl;

    // Verify consistency
    if (seq_counts == dist_counts) {
        cout << "✓ Results are consistent with sequential version." << endl;
    } else {
        cout << "✗ Results differ from sequential version!" << endl;
    }
    cout << endl;

    // Performance Analysis
    cout << "--- Performance Analysis ---" << endl;
    cout << fixed << setprecision(4);
    cout << setw(15) << "Method" << setw(12) << "Time(s)" << setw(10) << "Speedup" << setw(12) << "Efficiency" << endl;
    cout << string(50, '-') << endl;

    double baseline_time = seq_time;
    for (const auto& result : results) {
        double speedup = baseline_time / result.second;
        double efficiency = (result.first == "Sequential") ? 1.0 : speedup / num_threads;
        cout << setw(15) << result.first << setw(12) << result.second
             << setw(10) << speedup << setw(12) << efficiency << endl;
    }
    cout << endl;

    // Output performance data for external plotting
    output_performance_data(results, "performance_data.txt");

    // Print ASCII performance chart
    print_performance_chart(results);

    // Top 10 words
    cout << "--- Top 10 Words ---" << endl;
    vector<pair<string, int>> sorted_words(final_counts.begin(), final_counts.end());
    sort(sorted_words.begin(), sorted_words.end(),
         [](const pair<string, int>& a, const pair<string, int>& b) {
             return a.second > b.second;
         });

    size_t top_n = min(10, (int)sorted_words.size());
    for (size_t i = 0; i < top_n; ++i) {
        cout << sorted_words[i].first << ": " << sorted_words[i].second << endl;
    }

    cout << endl << "=== System Integration Complete ===" << endl;

    return 0;
}