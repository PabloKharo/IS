#include <iostream>
#include <string>
#include <vector>

#include "search_engine/search_engine.hpp"

#include <cpprest/http_listener.h>
#include <cpprest/json.h>
using namespace web;
using namespace web::http;
using namespace web::http::experimental::listener;

void handle_request(SearchEngine& search_engine, http_request request) {
    json::value request_body = request.extract_json().get();

    string query = request_body["query"].as_string();
    string option = request_body["option"].as_string();

    Zone zone;
    if (option == "title")
        zone = TITLE;
    else if (option == "title-and-first")
        zone = FIRST_PARAGRAPH;
    else if (option == "full-text")
        zone = REST;

    clock_t start = clock();
    vector<string> result = search_engine.search(query, zone);
    clock_t end = clock();
    cout << "Time: " << double(end - start) / CLOCKS_PER_SEC << "s Query: " << query << " Zone: " << zone << endl;

    json::value response;
    for (size_t i = 0; i < result.size(); i++) {
        response[i] = json::value::string(result[i]);
    }

    request.reply(status_codes::OK, response);
}

void run_server(SearchEngine& search_engine){
    http_listener listener("http://localhost:8081");
    listener.support(methods::POST, bind(handle_request, ref(search_engine), placeholders::_1));
    try {
        listener
            .open()
            .then([&listener]() { cout << "Listening on " << listener.uri().to_string() << endl; })
            .wait();

        cin.get();
    }
    catch (exception const &e) {
        cout << "Error: " << e.what() << endl;
    }
}

int main() {
    SearchEngine search_engine;
    run_server(search_engine);

    return 0;
}