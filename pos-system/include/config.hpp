#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>

class Config {
public:
static std::string getUsersFilePath();
static std::string getProductsFilePath();
static std::string getLogFilePath();
static int getServerPort();
};

#endif // CONFIG_HPP