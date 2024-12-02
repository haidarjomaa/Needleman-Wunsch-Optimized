#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <string>
#include <algorithm>
#include <functional>
#include <stack>

using namespace std;

struct ReadPair
{// I used structure to organize the work 
    string forward_seq;
    string reverse_seq;
    string forward_quality;
    string reverse_quality;
};

//  reverse the complement of reads_2.fastq
string reverse_complement(const string& seq) {
    string rev_comp;
    for (auto it = seq.rbegin(); it != seq.rend(); ++it) {
        switch (*it) {
            case 'A': rev_comp += 'T'; break;
            case 'T': rev_comp += 'A'; break;
            case 'C': rev_comp += 'G'; break;
            case 'G': rev_comp += 'C'; break;
            default: rev_comp += *it; break; // I used this for  'N' and other  unexpected characters
        }
    }
    return rev_comp;
}

// Read paired-end FASTQ files and put them in a vector of ReadPair
vector<ReadPair> read_file_pairs(const string &forward_file, const string &reverse_file) {
    ifstream forward(forward_file);
    ifstream reverse(reverse_file);
    
    vector<ReadPair> reads;

    if (!forward.is_open() || !reverse.is_open()) {
        cerr << "Error opening files: " << forward_file << " or " << reverse_file << endl;
        return reads;
    }

    string line1, line2, line3, line4;
    string line5, line6, line7, line8;

    cout << "1: Reading paired-end reads" << endl;
    while (getline(forward, line1) && getline(forward, line2) && getline(forward, line3) && getline(forward, line4) &&
          getline(reverse, line5) && getline(reverse, line6) && getline(reverse, line7) && getline(reverse, line8)) {
          string rev_complemented_reverse = reverse_complement(line6);
          reads.push_back({line2, rev_complemented_reverse, line4, line8});
    }

    forward.close();
    reverse.close();
    cout << "2: Finished reading paired-end reads. Total pairs: " << reads.size() << endl;
    return reads;
}

// average quality score for a read (Phred+33)
double calculate_average_quality(const string& quality_scores)
{
    int sum = 0;
    for (char score : quality_scores) {
        sum += (score - 33);  
    }
    return sum / static_cast<double>(quality_scores.size());
}

// ere I filtered the reads based on quality score and percentage of 'N'
void filter_reads_in_place(vector<ReadPair>& reads, double quality_threshold, double n_threshold) 
{
    cout << "Starting filtering..." << endl;
    auto is_valid_read = [quality_threshold, n_threshold](const ReadPair& read) {
        double forward_quality_avg = calculate_average_quality(read.forward_quality);
        double reverse_quality_avg = calculate_average_quality(read.reverse_quality);
        if (forward_quality_avg < quality_threshold || reverse_quality_avg < quality_threshold) 
        {
            return false;
        }
        int forward_n_count = count(read.forward_seq.begin(), read.forward_seq.end(), 'N');
        int reverse_n_count = count(read.reverse_seq.begin(), read.reverse_seq.end(), 'N');
        double forward_n_percentage = static_cast<double>(forward_n_count) / read.forward_seq.size();
        double reverse_n_percentage = static_cast<double>(reverse_n_count) / read.reverse_seq.size();

        return forward_n_percentage <= n_threshold && reverse_n_percentage <= n_threshold;
    };

    // Use std::partition to move valid reads to the front
    auto new_end = std::partition(reads.begin(), reads.end(), is_valid_read);
    reads.erase(new_end, reads.end());

    cout << "Filtering : done . Total reads after filtering: " << reads.size() << endl;
}

// I used this function to filter out short reads (it means here if they have a length less than k)
void filter_short_reads(vector<ReadPair>& reads, int k) 
{
    reads.erase(std::remove_if(reads.begin(), reads.end(), [k](const ReadPair& read) 
    {
        return read.forward_seq.size() < k || read.reverse_seq.size() < k;
    }), reads.end());

    cout << "Filtered out short reads. Remaining read pairs: " << reads.size() << endl;
}
void build_de_bruijn_graph(const vector<ReadPair>& reads, int k, unordered_map<string, vector<string>>& graph) {
    cout << "Building De Bruijn graph with k-mers of length " << k << "..." << endl;

    for (const auto& read : reads) {
        vector<string> sequences = {read.forward_seq, read.reverse_seq};
        for (const string& seq : sequences) {
            for (size_t i = 0; i <= seq.size() - k; ++i) {
                string kmer = seq.substr(i, k);
                string prefix = kmer.substr(0, k - 1);
                string suffix = kmer.substr(1, k - 1);
                graph[prefix].push_back(suffix);
            }
        }
    }
    cout << "De Bruijn graph construction: done . the total nodes :  " << graph.size() << endl;
}

// assemble contigs using an Eulerian path strategy
vector<string> assemble_sequences(unordered_map<string, vector<string>>& graph, int k) {
    cout << "Assembling sequences using Eulerian path strategy..." << endl;

    unordered_map<string, int> in_degree, out_degree;
    for (const auto& node : graph) {
        const string& from = node.first;
        for (const string& to : node.second) {
            out_degree[from]++;
            in_degree[to]++;
        }
    }

    //  I searched for a node with out-degree > in-degree
    string start_node;
    for (const auto& node : graph) {
        const string& key = node.first;
        if (out_degree[key] > in_degree[key]) {
            start_node = key;
            break;
        }
    }

    // Eulerian path 
    vector<string> contigs;
    stack<string> path_stack;
    unordered_map<string, vector<string>> graph_copy = graph; // to don't modify the old graph 
    vector<string> temp_contig;

    path_stack.push(start_node);

    while (!path_stack.empty()) {
        string current = path_stack.top();

        if (!graph_copy[current].empty()) {
            string next = graph_copy[current].back();
            graph_copy[current].pop_back();  
            path_stack.push(next);
        } else {
            temp_contig.push_back(current);
            path_stack.pop();
        }
        while (temp_contig.size() >= k) {
            string contig = temp_contig[0];
            for (size_t i = 1; i < k; ++i) {
                contig += temp_contig[i].back();
            }
            contigs.push_back(contig);
            temp_contig.erase(temp_contig.begin(), temp_contig.begin() + k); 
        }
    }

    // use leftover nodes to form the last contig
    if (!temp_contig.empty()) {
        string contig = temp_contig[0];
        for (size_t i = 1; i < temp_contig.size(); ++i) {
            contig += temp_contig[i].back();
        }
        contigs.push_back(contig);
    }

    cout << "Assembled " << contigs.size() << " contigs successfully." << endl;
    return contigs;
}

// Here I saved the contigs in FASTA format
void save_contigs(const vector<string>& contigs, const string& output_file) {
    cout << "Saving assembled contigs to " << output_file << "..." << endl;
    ofstream out(output_file);

    if (!out.is_open()) {
        cerr << "Error: Unable to open file " << output_file << endl;
        return;
    }

    for (size_t i = 0; i < contigs.size(); ++i) {
        out << ">contig_" << i + 1 << "\n" << contigs[i] << "\n";
    }

    out.close();
    cout << "Contigs saved to " << output_file << endl;
}

int main() {
    
    int k = 31; 
    double quality_threshold = 30.0; 
    double n_threshold = 0.1;  

    string forward_file = "reads_1.fastq";
    string reverse_file = "reads_2.fastq";
    string output_file = "assembled_contigs.fasta";
    vector<ReadPair> reads = read_file_pairs(forward_file, reverse_file);
    filter_reads_in_place(reads, quality_threshold, n_threshold);
    filter_short_reads(reads, k);
    unordered_map<string, vector<string>> graph;
    build_de_bruijn_graph(reads, k, graph);
    // Assemble the sequences
    vector<string> contigs = assemble_sequences(graph, k);
    save_contigs(contigs, output_file);

    return 0;
}
