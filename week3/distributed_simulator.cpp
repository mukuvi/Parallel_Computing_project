#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <chrono>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <random>

using namespace std;

// Function to read a file and return its content
string read_file(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return "";
    }
    string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    return content;
}

// Function to process text and count words
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

// Message structure for communication
struct Message {
    int worker_id;
    string filename;
    unordered_map<string, int> result;
    bool is_failure;
};

// Thread-safe queue for communication
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

// Master function for distributed simulation
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
        Message request{i % num_workers, filenames[i], {}, false};
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
            cout << "Retrying failed task for " << response.worker_id << endl;
            string content = read_file(filenames[response.worker_id]);
            auto word_count = count_words(content);
            for (const auto& pair : word_count) {
                total_count[pair.first] += pair.second;
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

int main() {
    vector<string> filenames = {
        "data/file1.txt",
        "data/file2.txt",
        "data/file3.txt",
        "data/file4.txt"
    };

    cout << "Processing " << filenames.size() << " files" << endl;

    // Distributed simulation
    cout << "\n--- Distributed Simulation ---" << endl;
    int num_workers = min(4, (int)filenames.size());
    auto [dist_counts, dist_time] = distributed_word_count(filenames, num_workers);
    cout << "Distributed simulation completed in " << dist_time << " seconds using " << num_workers << " workers." << endl;

    // Display top 10 words
    cout << "\n--- Top 10 Words ---" << endl;
    vector<pair<string, int>> sorted_words(dist_counts.begin(), dist_counts.end());
    sort(sorted_words.begin(), sorted_words.end(),
         [](const pair<string, int>& a, const pair<string, int>& b) {
             return a.second > b.second;
         });

    size_t top_n = min(10, (int)sorted_words.size());
    for (size_t i = 0; i < top_n; ++i) {
        cout << sorted_words[i].first << ": " << sorted_words[i].second << endl;
    }

    return 0;
}