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
void read_num(ifstream& infile, T& val) {
    infile.read(reinterpret_cast<char*>(&val), sizeof(val));
}

void read_word(ifstream& infile, string& word) {
    size_t word_length;
    read_num(infile, word_length);
    word.resize(word_length);
    infile.read(word.data(), word_length);
}

template<typename T>
void read_vec(ifstream& infile, vector<T>& vec) {
    size_t vec_size;
    read_num(infile, vec_size);
    vec.resize(vec_size);
    infile.read(reinterpret_cast<char*>(vec.data()), sizeof(T) * vec_size);
}

// ----------------------------------------------------------------------------

void load_inverted_index(unordered_map<string, unordered_map<Zone, vector<doc_id>>>& inverted_index, const string& file_path) {
    ifstream infile(file_path, ios::binary);
    size_t inverted_index_size;
    read_num(infile, inverted_index_size);
    for (size_t i = 0; i < inverted_index_size; ++i) {
        string word;
        read_word(infile, word);
        size_t zone_map_size;
        read_num(infile, zone_map_size);
        for (size_t j = 0; j < zone_map_size; ++j) {
            Zone zone;
            read_num(infile, zone);
            vector<doc_id> doc_ids;
            read_vec(infile, doc_ids);
            inverted_index[word][zone] = doc_ids;
        }
    }
    infile.close();
}

void load_docs_dict(unordered_map<doc_id, string>& docs_dict, const string& file_path) {
    ifstream infile(file_path, ios::binary);
    size_t docs_dict_size;
    read_num(infile, docs_dict_size);
    for (size_t i = 0; i < docs_dict_size; ++i) {
        doc_id id;
        read_num(infile, id);
        string doc_name;
        read_word(infile, doc_name);
        docs_dict[id] = doc_name;
    }
    infile.close();
}

void load_words_dict(unordered_map<string, word_id>& words_dict, const string& file_path) {
    ifstream infile(file_path, ios::binary);
    size_t words_dict_size;
    read_num(infile, words_dict_size);
    for (size_t i = 0; i < words_dict_size; ++i) {
        string word;
        read_word(infile, word);
        word_id id;
        read_num(infile, id);
        words_dict[word] = id;
    }
    infile.close();
}

void load_forward_index(unordered_map<doc_id, unordered_map<Zone, vector<word_id>>>& forward_index, const string& file_path) {
    ifstream infile(file_path, ios::binary);
    size_t forward_index_size;
    read_num(infile, forward_index_size);
    for (size_t i = 0; i < forward_index_size; ++i) {
        doc_id id;
        read_num(infile, id);
        size_t zone_map_size;
        read_num(infile, zone_map_size);
        for (size_t j = 0; j < zone_map_size; ++j) {
            Zone zone;
            read_num(infile, zone);
            vector<word_id> word_ids;
            read_vec(infile, word_ids);
            forward_index[id][zone] = word_ids;
        }
    }
    infile.close();
}

void load_tf_idf_index(unordered_map<doc_id, unordered_map<Zone, unordered_map<word_id, double>>>& tf_idf_index, const string& file_path) {
    ifstream infile(file_path, ios::binary);
    size_t tf_idf_index_size;
    read_num(infile, tf_idf_index_size);
    for (size_t i = 0; i < tf_idf_index_size; ++i) {
        doc_id id;
        read_num(infile, id);
        size_t zone_map_size;
        read_num(infile, zone_map_size);
        for (size_t j = 0; j < zone_map_size; ++j) {
            Zone zone;
            read_num(infile, zone);
            size_t word_map_size;
            read_num(infile, word_map_size);
            for (size_t k = 0; k < word_map_size; ++k) {
                word_id word_id;
                read_num(infile, word_id);
                double tf_idf_value;
                read_num(infile, tf_idf_value);
                tf_idf_index[id][zone][word_id] = tf_idf_value;
            }
        }
    }
    infile.close();
}


void SearchEngine::load_dicts(){
    load_inverted_index(inverted_index, INVERTED_INDEX_FILE);
    load_docs_dict(docs_dict, DOCS_DICT_FILE);
    load_forward_index(forward_index, FORWARD_INDEX_FILE);
    load_words_dict(words_dict, WORDS_DICT_FILE);
    load_tf_idf_index(tf_idf_index, TF_IDF_INDEX_FILE);
}