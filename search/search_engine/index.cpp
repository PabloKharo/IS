#include <cmath>

#include "search_engine.hpp"

// ------------ TF IDF -------------------------------

double SearchEngine::tf(int term_count, int zone_word_count) {
    return (double)term_count / zone_word_count;
}

double SearchEngine::idf(int doc_count, int total_docs) {
    return log((double)total_docs / (1 + doc_count));
}

int SearchEngine::term_count(doc_id doc, Zone zone, const string& word) {
    return count(forward_index[doc][zone].begin(), forward_index[doc][zone].end(), get_word_id(word));
}

void SearchEngine::compute_tf_idf() {
    int total_docs = docs_dict.size();
    for (const auto& word_entry : inverted_index) {
        const string& word = word_entry.first;
        for (const auto& zone_entry : word_entry.second) {
            Zone zone = zone_entry.first;
            const vector<doc_id>& doc_ids = zone_entry.second;
            int doc_count = doc_ids.size();
            for (doc_id doc : doc_ids) {
                int term_cnt = term_count(doc, zone, word);
                int zone_word_count = doc_zone_word_count[doc][zone];
                double tf_value = tf(term_cnt, zone_word_count);
                double idf_value = idf(doc_count, total_docs);
                tf_idf_index[doc][zone][get_word_id(word)] = abs(tf_value * idf_value);
            }
        }
    }
}


// ------------ INDEXING -------------------------------

void SearchEngine::index_zone(istringstream& iss, doc_id doc_counter, Zone zone) {
    string word;
    while (iss >> word) {
        word_id id = get_word_id(word);
        inverted_index[word][zone].push_back(doc_counter);
        forward_index[doc_counter][zone].push_back(id);
        doc_zone_word_count[doc_counter][zone]++;
    }
}

void SearchEngine::index_file(const string& file_path, int doc_counter) {
    ifstream infile(file_path);
    string title, first_paragraph, rest;

    getline(infile, title);
    getline(infile, first_paragraph);
    getline(infile, rest);

    infile.close();

    istringstream iss_title(title), iss_first(first_paragraph), iss_rest(rest);
    index_zone(iss_title, doc_counter, TITLE);
    index_zone(iss_first, doc_counter, FIRST_PARAGRAPH);
    index_zone(iss_rest, doc_counter, REST);
}

void SearchEngine::index_folder() {
    doc_id doc_counter = 0;
    for (const auto& entry : filesystem::directory_iterator(INPUT_DIR)) {
        if (entry.is_regular_file()) {
            string file_path = entry.path().string();
            docs_dict[doc_counter] = file_path;
            index_file(file_path, doc_counter);
            doc_counter++;
        }
    }
    compute_tf_idf();
}