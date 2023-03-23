#include <unordered_set>

#include "search_engine.hpp"

// ------------ LOADING -------------------------------

bool dicts_exist(){
    vector<filesystem::path> files = {FORWARD_INDEX_FILE, INVERTED_INDEX_FILE, WORDS_DICT_FILE,
                                    DOCS_DICT_FILE, TF_IDF_INDEX_FILE};

    if (!filesystem::is_directory(BINS_FOLDER)){
        filesystem::create_directory(BINS_FOLDER);
        return false;
    }
    
    for (auto& file : files) {
        if (!filesystem::exists(file))
            return false;
    }

    return true;
}

SearchEngine::SearchEngine(bool force_create) {
    if(!dicts_exist || force_create){
        cout << "Starting indexing...\n";
        clock_t start = clock();
        index_folder();
        clock_t end = clock();
        cout << "Indexed time: " << double(end - start) / CLOCKS_PER_SEC << endl;

        save_dicts();
    }
    else{
        cout << "Loading dicts...\n";
        clock_t start = clock();
        load_dicts();
        clock_t end = clock();
        cout << "Load time: " << double(end - start) / CLOCKS_PER_SEC << endl;
    }
}

// ------------ SEARCHING -------------------------------

word_id SearchEngine::get_word_id(const string& word) {
    static word_id next_word_id = 0;
    auto it = words_dict.find(word);
    if (it == words_dict.end()) {
        words_dict[word] = next_word_id;
        return next_word_id++;
    }
    return it->second;
}

vector<string> SearchEngine::search(const string& query, Zone search_zone, int k) {
    istringstream iss_query(query);
    string word;
    unordered_map<doc_id, double> doc_scores;
    vector<word_id> query_word_ids;
    unordered_set<doc_id> candidate_docs;

    while (iss_query >> word) {
        word_id id = get_word_id(word);
        query_word_ids.push_back(id);

        if (inverted_index.count(word) == 0) {
            return vector<string>();
        }

        for (int zone = TITLE; zone <= search_zone; ++zone) {
            if (inverted_index[word].count((Zone)zone) == 0) {
                continue;
            }

            const vector<doc_id>& doc_ids = inverted_index[word][(Zone)zone];
            candidate_docs.insert(doc_ids.begin(), doc_ids.end());
        }
    }

    for (const auto& doc : candidate_docs) {
        if (!all_words_exists(doc, search_zone, query_word_ids)) {
            continue;
        }

        double total_tf_idf_value = 0.0;
        for (word_id id : query_word_ids) {
            for (int zone = TITLE; zone <= search_zone; ++zone) {
                total_tf_idf_value += tf_idf_index[doc][(Zone)zone][id];
            }
        }

        doc_scores[doc] = total_tf_idf_value;
    }

    vector<string> result = get_top_k(doc_scores, k);
    return result;
}

bool SearchEngine::all_words_exists(doc_id doc, Zone search_zone, const vector<word_id>& query_word_ids) {
    for (word_id word_id : query_word_ids) {
        bool word_present = false;
        for (int zone = TITLE; zone <= search_zone; ++zone) {
            if (find(forward_index[doc][(Zone)zone].begin(),
                     forward_index[doc][(Zone)zone].end(),
                     word_id) != forward_index[doc][(Zone)zone].end()) {
                word_present = true;
                break;
            }
        }
        if (!word_present) {
            return false;
        }
    }
    return true;
}

vector<string> SearchEngine::get_top_k(const unordered_map<doc_id, double>& doc_scores, int k) {
    vector<pair<doc_id, double>> ranked_docs(doc_scores.begin(), doc_scores.end());
    sort(ranked_docs.begin(), ranked_docs.end(),
         [](const auto& a, const auto& b) { return a.second > b.second; });

    vector<string> result;
    for (int i = 0; i < k && i < ranked_docs.size(); i++) {
        result.push_back(docs_dict[ranked_docs[i].first]);
    }

    return result;
}