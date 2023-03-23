#pragma once

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <map>
#include <algorithm>

using namespace std;

typedef unsigned int doc_id;
typedef unsigned int word_id; 

enum Zone { TITLE, FIRST_PARAGRAPH, REST };

const string INPUT_DIR = "../texts/en/";
const string BINS_FOLDER = "../search/bins"; 
const string FORWARD_INDEX_FILE = "../search/bins/forward_index.bin"; 
const string INVERTED_INDEX_FILE = "../search/bins/inverted_index.bin"; 
const string WORDS_DICT_FILE = "../search/bins/words_dict.bin";
const string DOCS_DICT_FILE = "../search/bins/docs_dict.bin";
const string TF_IDF_INDEX_FILE = "../search/bins/tf_idf_index.bin";

class SearchEngine {
private:
    unordered_map<string, unordered_map<Zone, vector<doc_id>>> inverted_index;
    unordered_map<doc_id, unordered_map<Zone, vector<word_id>>> forward_index;
    unordered_map<doc_id, string> docs_dict;
    unordered_map<string, word_id> words_dict;
    unordered_map<doc_id, unordered_map<Zone, unordered_map<word_id, double>>> tf_idf_index;
    unordered_map<doc_id, unordered_map<Zone, int>> doc_zone_word_count;

    // Получение ID слова
    word_id get_word_id(const string& word);

    // Вычисление TF
    double tf(int term_count, int zone_word_count);
    // Вычисление IDF
    double idf(int doc_count, int total_docs);
    // Подсчет вхождений слова
    int term_count(doc_id doc, Zone zone, const string& word);
    // Вычисление TF-IDF
    void compute_tf_idf();

    // Индексация зоны
    void index_zone(istringstream& iss, doc_id doc_counter, Zone zone);
    // Индексация файла
    void index_file(const string& file_path, int doc_counter);
    // Индексация папки
    void index_folder();

    // Сохранение словарей
    void save_dicts();
    // Загрузка словарей
    void load_dicts();

    // Проверка вхождения всех слов в статье
    bool all_words_exists(doc_id doc, Zone zone, const vector<word_id>& query_word_ids);
    // Получение лучших k результатов
    vector<string> get_top_k(const unordered_map<doc_id, double>& doc_scores, int k);

public:
    SearchEngine(bool force_create = false);

    vector<string> search(const string& query, Zone search_zone, int len = 50);
};