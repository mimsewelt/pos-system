#ifndef HTTP_SERVER_HPP
#define HTTP_SERVER_HPP

#include <string> // Fügen Sie diese Zeile hinzu
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class HttpServer {
public:
    HttpServer(int port);
    void run();

private:
    int port_;
    std::string extractToken(const std::string& request);
    void handleLogin(const std::string& body, std::string& response);
    void handleStatus(std::string& response);
    void handleProducts(const std::string& token, std::string& response);
    void handleUsers(const std::string& token, std::string& response);
    void handleMe(const std::string& token, std::string& response);
    void handleNotFound(std::string& response);
    bool isTokenValid(const std::string& token);
    std::string getRoleFromToken(const std::string& token, const json& users);
};

#endif // HTTP_SERVER_HPP