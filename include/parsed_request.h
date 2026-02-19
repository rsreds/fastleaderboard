#ifndef PARSED_REQUEST_H
#define PARSED_REQUEST_H

#include <string>

namespace http = beast::http;
using Request = http::request<http::string_body>;

struct ParsedRequest
{
    std::string method;
    std::string path;
    std::unordered_map<std::string, std::string> query_params;
    std::string body;
};

ParsedRequest parse_request(const Request& raw_request){
    ParsedRequest parsed_request;
    parsed_request.method = raw_request.method_string();
    std::string target = raw_request.target();
    auto pos = target.find('?');
    if (pos != std::string::npos) {
        parsed_request.path = target.substr(0, pos);
        std::string query_string = target.substr(pos + 1);
        std::istringstream query_stream(query_string);
        std::string pair;
        while (std::getline(query_stream, pair, '&')) {
            auto eq_pos = pair.find('=');
            if (eq_pos != std::string::npos) {
                std::string key = pair.substr(0, eq_pos);
                std::string value = pair.substr(eq_pos + 1);
                parsed_request.query_params[key] = value;
            }
        }
    } else {
        parsed_request.path = target;
    }
    parsed_request.body = raw_request.body();
    return parsed_request;
}


#endif //PARSED_REQUEST_H