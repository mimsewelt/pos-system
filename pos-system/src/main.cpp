
#include "http_server.hpp"

int main() {
    HttpServer server(8080);
    server.run();
    return 0;
}
