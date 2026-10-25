#include "api/httplib.h"

int main() {
    // HTTP
httplib::Server svr;

// HTTPS

svr.Get("/hi", [](const httplib::Request &, httplib::Response &res) {
  res.set_content("Hello World!", "text/plain");
});

svr.listen("0.0.0.0", 8080);
}