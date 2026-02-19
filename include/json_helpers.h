#ifndef JSON_HELPERS_H
#define JSON_HELPERS_H

#include <boost/json.hpp>
#include <boost/beast/http.hpp>
#include <string>
#include <vector>

namespace http = boost::beast::http;
using Response = http::response<http::string_body>;

namespace json_helpers {

// Create a JSON success response
inline Response make_json_response(
    unsigned version, 
    bool keep_alive, 
    const boost::json::object& json_body,
    http::status status = http::status::ok
) {
    Response res{status, version};
    res.keep_alive(keep_alive);
    res.set(http::field::content_type, "application/json");
    res.set(http::field::access_control_allow_origin, "*");
    res.body() = boost::json::serialize(json_body);
    res.prepare_payload();
    return res;
}

// Create a text error response
inline Response make_error_response(
    unsigned version, 
    bool keep_alive, 
    http::status status,
    const std::string& message
) {
    Response res{status, version};
    res.keep_alive(keep_alive);
    res.set(http::field::access_control_allow_origin, "*");
    res.body() = message;
    res.prepare_payload();
    return res;
}

// Convert a vector of JSON-serializable objects to a JSON array
template<typename T>
boost::json::array to_json_array(const std::vector<T>& items) {
    boost::json::array arr;
    for (const auto& item : items) {
        arr.push_back(item.to_json());
    }
    return arr;
}

// Wrap a JSON array in an object with the given key
inline boost::json::object wrap_array(const std::string& key, const boost::json::array& arr) {
    boost::json::object obj;
    obj[key] = arr;
    return obj;
}

} // namespace json_helpers

#endif // JSON_HELPERS_H
