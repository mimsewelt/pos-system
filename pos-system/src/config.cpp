#include "config.hpp"

std::string Config::getUsersFilePath() {
return "../data/users.json";
}

std::string Config::getProductsFilePath() {
return "../data/products.json";
}

std::string Config::getLogFilePath() {
return "../data/log.txt";
}

int Config::getServerPort() {
return 8080;
}