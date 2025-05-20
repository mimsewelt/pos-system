#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <cassert>
#include <nlohmann/json.hpp>
#include "../include/config.hpp"
#include "../include/sha256.hpp"
#include <curl/curl.h>

using json = nlohmann::json;

static std::string token = "";

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* out) {
    size_t totalSize = size * nmemb;
    out->append((char*)contents, totalSize);
    return totalSize;
}

std::string performRequest(const std::string& url, const std::string& method,
                           const std::string& data = "", const std::string& auth_token = "") {
    CURL* curl = curl_easy_init();
    std::string readBuffer;

    if (curl) {
        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        if (!auth_token.empty()) {
            headers = curl_slist_append(headers, ("Authorization: Bearer " + auth_token).c_str());
        }

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        if (method == "POST") {
            curl_easy_setopt(curl, CURLOPT_POST, 1L);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
        }

        curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }

    return readBuffer;
}

void test_login_success() {
    std::string data = R"({"benutzer":"admin","passwort":"geheim123"})";
    std::string response = performRequest("http://localhost:8080/login", "POST", data);
    std::cout << "[LOGIN SUCCESS] Response: " << response << std::endl;
    auto json_response = json::parse(response);
    assert(json_response["status"] == "ok");
    token = json_response["token"];
    assert(!token.empty());
}

void test_get_status() {
    std::string response = performRequest("http://localhost:8080/status", "GET");
    std::cout << "[STATUS] Response: " << response << std::endl;
    auto json_response = json::parse(response);
    assert(json_response["status"] == "ok");
}

void test_get_me() {
    std::string response = performRequest("http://localhost:8080/me", "GET", "", token);
    std::cout << "[ME] Response: " << response << std::endl;
    auto json_response = json::parse(response);
    assert(json_response["benutzer"] == "admin");
    assert(json_response["rolle"] == "admin");
}

void test_get_users() {
    std::string response = performRequest("http://localhost:8080/users", "GET", "", token);
    std::cout << "[USERS] Response: " << response << std::endl;
    auto json_response = json::parse(response);
    assert(json_response.contains("users"));
    assert(json_response["users"].is_array());
}

void test_get_products() {
    std::string response = performRequest("http://localhost:8080/products", "GET", "", token);
    std::cout << "[PRODUCTS] Response: " << response << std::endl;
    auto json_response = json::parse(response);
    assert(json_response.contains("produkte"));
    assert(json_response["produkte"].is_array());
}

int main() {
    test_login_success();
    test_get_status();
    test_get_me();
    test_get_users();
    test_get_products();
    std::cout << "✅ Alle Tests erfolgreich bestanden." << std::endl;
    return 0;
}