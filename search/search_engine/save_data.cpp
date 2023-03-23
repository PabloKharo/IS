#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <filesystem>

#include "search_engine.hpp"

using namespace std;

template<typename T>
void write_num(ofstream& outfile, T val){
    outfile.write(reinterpret_cast<const char*>(&val), sizeof(val));
}

void write_word(ofstream& outfile, const string& word){
    write_num(outfile, word.length());
    outfile.write(word.c_str(), word.length());
}

template<typename T>
void write_vec(ofstream& outfile, const vector<T>& vec){
    write_num(outfile, vec.size());
    outfile.write(reinterpret_cast<const char*>(vec.data()), sizeof(T) * vec.size());
}


// ----------------------------------------------------------------------------

void save_inverted_index(const unordered_map<string, unordered_map<Zone, vector<doc_id>>>& inverted_index, const string& file_path) {
    ofstream outfile(file_path, ios::binary);
    write_num(outfile, inverted_index.size());
    for (auto& [word, zone_map] : inverted_index) {
        write_word(outfile, word);
        write_num(outfile, zone_map.size());
        for (auto& [zone, doc_ids] : zone_map) {
            write_num(outfile, zone);
            write_vec(outfile, doc_ids);
        }
    }
    outfile.close();
}

void save_docs_dict(const unordered_map<doc_id, string>& docs_dict, const string& file_path) {
    ofstream outfile(file_path, ios::binary);
    write_num(outfile, docs_dict.size());
    for (auto& [doc_id, doc_name] : docs_dict) {
        write_num(outfile, doc_id);
        write_word(outfile, doc_name);
    }
    outfile.close();
}

void save_words_dict(const unordered_map<string, word_id>& words_dict, const string& file_path) {
    ofstream outfile(file_path, ios::binary);
    write_num(outfile, words_dict.size());
    for (auto& [word, id] : words_dict) {
        write_word(outfile, word);
        write_num(outfile, id);
    }
    outfile.close();
}

void save_forward_index(const unordered_map<doc_id, unordered_map<Zone, vector<word_id>>>& forward_index, const string& file_path) {
    ofstream outfile(file_path, ios::binary);
    write_num(outfile, forward_index.size());
    for (auto& [doc_id, zone_map] : forward_index) {
        write_num(outfile, doc_id);
        write_num(outfile, zone_map.size());
        for (auto [zone, word_ids] : zone_map) {
            write_num(outfile, zone);
            write_vec(outfile, word_ids);
        }
    }
    outfile.close();
}

void save_tf_idf_index(const unordered_map<doc_id, unordered_map<Zone, unordered_map<word_id, double>>>& tf_idf_index, const string& file_path) {
    ofstream outfile(file_path, ios::binary);
    write_num(outfile, tf_idf_index.size());
    for (auto& [doc_id, zone_map] : tf_idf_index) {
        write_num(outfile, doc_id);
        write_num(outfile, zone_map.size());
        for (auto& [zone, word_map] : zone_map) {
            write_num(outfile, zone);
            write_num(outfile, word_map.size());
            for (auto& [word_id, tf_idf_value] : word_map) {
                write_num(outfile, word_id);
                write_num(outfile, tf_idf_value);
            }
        }
    }
    outfile.close();
}


void SearchEngine::save_dicts(){
    save_inverted_index(inverted_index, INVERTED_INDEX_FILE);
    save_docs_dict(docs_dict, DOCS_DICT_FILE);
    save_forward_index(forward_index, FORWARD_INDEX_FILE);
    save_words_dict(words_dict, WORDS_DICT_FILE);
    save_tf_idf_index(tf_idf_index, TF_IDF_INDEX_FILE);
}