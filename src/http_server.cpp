#include "http_server.hpp"
#include "sha256.hpp"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <netinet/in.h>
#include <nlohmann/json.hpp>
#include "config.hpp"

using json = nlohmann::json;

HttpServer::HttpServer(int port) : port_(port) {}

void HttpServer::run() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port_);
    bind(server_fd, (struct sockaddr*)&address, sizeof(address));
    listen(server_fd, 3);
    std::cout << "Server läuft auf Port " << port_ << std::endl;

    while (true) {
        int client_socket = accept(server_fd, nullptr, nullptr);
        if (client_socket < 0) continue;

        char buffer[8192] = {0};
        read(client_socket, buffer, sizeof(buffer) - 1);
        std::string request(buffer);
        std::string response;

        size_t body_pos = request.find("\r\n\r\n");
        std::string body = (body_pos != std::string::npos) ? request.substr(body_pos + 4) : "";

        std::string token = extractToken(request);

        if (request.find("POST /login") != std::string::npos) {
            handleLogin(body, response);
        } else if (request.find("GET /status") != std::string::npos) {
            handleStatus(response);
        } else if (request.find("GET /products") != std::string::npos) {
            handleProducts(token, response);
        } else if (request.find("GET /users") != std::string::npos) {
            handleUsers(token, response);
        } else if (request.find("GET /me") != std::string::npos) {
            handleMe(token, response);
        }
        else {
            handleNotFound(response);
        }
        send(client_socket, response.c_str(), response.size(), 0);
        close(client_socket);
    }
}

std::string HttpServer::extractToken(const std::string& request) {
    std::istringstream req_stream(request);
    std::string line, token;
    while (std::getline(req_stream, line) && line != "\r") {
        if (line.find("Authorization: Bearer ") != std::string::npos) {
            token = line.substr(line.find("Bearer ") + 7);
            token.erase(token.find_last_not_of("\r\n") + 1);
        }
    }
    return token;
}

void HttpServer::handleLogin(const std::string& body, std::string& response) {
    if (body.empty()) {
        response = "HTTP/1.1 400 Bad Request\r\nContent-Type: application/json\r\nContent-Length: 52\r\n\r\n"
                   R"({"status":"fehler","message":"Leerer Request"})";
        return;
    }

    std::ifstream file(Config::getUsersFilePath());
    if (!file.is_open()) {
        response = "HTTP/1.1 500 Internal Server Error\r\nContent-Type: application/json\r\nContent-Length: 58\r\n\r\n"
                   R"({"status":"fehler","message":"Benutzerdaten konnten nicht geladen werden"})";
        return;
    }

    json users;
    file >> users;

    json input = json::parse(body);
    std::string benutzer = input["benutzer"];
    std::string passwort = input["passwort"];
    std::string hash = sha256(passwort);

    json antwort;
    bool gefunden = false;
    for (const auto& user : users["users"]) {
        if (user["benutzer"] == benutzer && user["passwort_hash"] == hash) {
            antwort["status"] = "ok";
            antwort["token"] = user["token"];
            gefunden = true;
            break;
        }
    }
    if (!gefunden) {
        antwort["status"] = "fehler";
        antwort["message"] = "Login fehlgeschlagen";
    }

    std::string antwort_str = antwort.dump();
    response = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: " +
               std::to_string(antwort_str.size()) + "\r\n\r\n" + antwort_str;
}

void HttpServer::handleStatus(std::string& response) {
    std::string antwort = R"({"status":"ok","nachricht":"Server erreichbar"})";
    response = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: " +
               std::to_string(antwort.size()) + "\r\n\r\n" + antwort;
}

void HttpServer::handleProducts(const std::string& token, std::string& response) {
    std::ifstream file(Config::getProductsFilePath());
    if (!file.is_open()) {
        response = "HTTP/1.1 500 Internal Server Error\r\nContent-Type: application/json\r\nContent-Length: 58\r\n\r\n"
                   R"({"status":"fehler","message":"Produktdaten konnten nicht geladen werden"})";
        return;
    }

    json produktdaten;
    file >> produktdaten;

    if (!isTokenValid(token)) {
        std::string antwort = R"({"status":"fehler","message":"Token ungültig"})";
        response = "HTTP/1.1 401 Unauthorized\r\nContent-Type: application/json\r\nContent-Length: " +
                   std::to_string(antwort.size()) + "\r\n\r\n" + antwort;
        return;
    }

    std::string antwort_str = produktdaten.dump();
    response = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: " +
               std::to_string(antwort_str.size()) + "\r\n\r\n" + antwort_str;
}

void HttpServer::handleUsers(const std::string& token, std::string& response) {
    std::ifstream file(Config::getUsersFilePath());
    if (!file.is_open()) {
        response = "HTTP/1.1 500 Internal Server Error\r\nContent-Type: application/json\r\nContent-Length: 58\r\n\r\n"
                   R"({"status":"fehler","message":"Benutzerdaten konnten nicht geladen werden"})";
        return;
    }

    json users;
    file >> users;

    if (!isTokenValid(token)) {
        std::string antwort = R"({"status":"fehler","message":"Token ungültig"})";
        response = "HTTP/1.1 401 Unauthorized\r\nContent-Type: application/json\r\nContent-Length: " +
                   std::to_string(antwort.size()) + "\r\n\r\n" + antwort;
        return;
    }

    std::string rolle = getRoleFromToken(token, users);
    if (rolle != "admin") {
        std::string antwort = R"({"status":"fehler","message":"Zugriff verweigert"})";
        response = "HTTP/1.1 403 Forbidden\r\nContent-Type: application/json\r\nContent-Length: " +
                   std::to_string(antwort.size()) + "\r\n\r\n" + antwort;
        return;
    }

    json result;
    for (const auto& user : users["users"]) {
        result["users"].push_back(user["benutzer"]);
    }

    std::string antwort_str = result.dump();
    response = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: " +
               std::to_string(antwort_str.size()) + "\r\n\r\n" + antwort_str;
}

void HttpServer::handleNotFound(std::string& response) {
    std::string notfound = "404 Not Found";
    response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\nContent-Length: " +
               std::to_string(notfound.size()) + "\r\n\r\n" + notfound;
}

bool HttpServer::isTokenValid(const std::string& token) {
    std::ifstream file(Config::getUsersFilePath());
    if (!file.is_open()) return false;

    json users;
    file >> users;

    for (const auto& user : users["users"]) {
        if (user["token"] == token) {
            return true;
        }
    }
    return false;
}

std::string HttpServer::getRoleFromToken(const std::string& token, const json& users) {
    for (const auto& user : users["users"]) {
        if (user["token"] == token) {
            return user["rolle"];
        }
    }
    return "";
}

void HttpServer::handleMe(const std::string& token, std::string& response) {
    std::ifstream file(Config::getUsersFilePath());
    if (!file.is_open()) {
        response = "HTTP/1.1 500 Internal Server Error\r\nContent-Type: application/json\r\nContent-Length: 64\r\n\r\n"
        R"({"status":"fehler","message":"Benutzerdaten konnten nicht geladen werden"})";
        return;
    }
    json users;
    file >> users;

    for (const auto& user : users["users"]) {
        if (user["token"] == token) {
            json antwort;
            antwort["benutzer"] = user["benutzer"];
            antwort["rolle"] = user["rolle"];
            std::string antwort_str = antwort.dump();
            response = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: " +
                       std::to_string(antwort_str.size()) + "\r\n\r\n" + antwort_str;
            return;
        }
    }

    std::string antwort = R"({"status":"fehler","message":"Token ungültig"})";
    response = "HTTP/1.1 401 Unauthorized\r\nContent-Type: application/json\r\nContent-Length: " +
               std::to_string(antwort.size()) + "\r\n\r\n" + antwort;
}