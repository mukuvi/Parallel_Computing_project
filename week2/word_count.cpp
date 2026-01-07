#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <chrono>
#include <omp.h>
#include <regex>

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
    regex word_regex("\\b\\w+\\b");
    auto words_begin = sregex_iterator(text.begin(), text.end(), word_regex);
    auto words_end = sregex_iterator();

    for (sregex_iterator i = words_begin; i != words_end; ++i) {
        string word = i->str();
        transform(word.begin(), word.end(), word.begin(), ::tolower);
        word_count[word]++;
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

int main() {
    vector<string> filenames = {
        "data/file1.txt",
        "data/file2.txt",
        "data/file3.txt",
        "data/file4.txt"
    };

    cout << "Processing " << filenames.size() << " files" << endl;

    // Sequential processing
    cout << "\n--- Sequential Processing ---" << endl;
    auto [seq_counts, seq_time] = sequential_word_count(filenames);
    cout << "Sequential processing completed in " << seq_time << " seconds." << endl;

    // Parallel processing
    cout << "\n--- Parallel Processing (Shared Memory) ---" << endl;
    int num_threads = min(4, (int)filenames.size());
    auto [par_counts, par_time] = parallel_word_count(filenames, num_threads);
    cout << "Parallel processing completed in " << par_time << " seconds using " << num_threads << " threads." << endl;

    // Verify results
    if (seq_counts == par_counts) {
        cout << "Results are consistent." << endl;
    } else {
        cout << "Warning: Results differ!" << endl;
    }

    // Calculate speedup
    double speedup = seq_time / par_time;
    double efficiency = speedup / num_threads;
    cout << "\nSpeedup: " << speedup << endl;
    cout << "Efficiency: " << efficiency << endl;

    // Display top 10 words
    cout << "\n--- Top 10 Words ---" << endl;
    vector<pair<string, int>> sorted_words(seq_counts.begin(), seq_counts.end());
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