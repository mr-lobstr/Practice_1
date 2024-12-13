#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <iterator>
#include <nlohmann/json.hpp>
using namespace std;

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;

using tcp = boost::asio::ip::tcp;
using json = nlohmann::json;
using http::verb;

string get_body (http::response<http::dynamic_body> const& response) {
    string str;

    for (auto seq : response.body().data()) {
        auto* cbuf = net::buffer_cast<const char*> (seq);
        str.append(cbuf, net::buffer_size(seq));
    }

    return str;
}

auto send (beast::tcp_stream& stream, http::verb v, string const& target, string const& body = "", string const& key = "") {
    http::request<http::string_body> request (v, target, 11);
    request.set(http::field::content_type, "application/json");
    
    if (not key.empty()) {
        request.set ("X-USER-KEY",key);
    }

    if (not body.empty()) {
        request.body() = json::parse (body).dump();
        request.prepare_payload();
    }

    http::write (stream, request);

    http::response<http::dynamic_body> response;
    beast::flat_buffer buffer; 
    http::read (stream, buffer, response);

    if (response.result() != http::status::ok) {
        throw runtime_error (get_body (response));
    }
    
    return json::parse (get_body (response));
}

void requests (beast::tcp_stream& stream, string const& pathR) {
    ifstream file (pathR);
    auto req = json::parse (file);
    file.close();

    auto post_user1 = req["post_user1"].dump();
    auto post_user2 = req["post_user2"].dump();
    auto post_order1 = req["post_order1"].dump();
    auto post_order2 = req["post_order2"].dump();

    auto key1 = send (stream, verb::post, "/user", post_user1)["key"];
    auto key2 = send (stream, verb::post, "/user", post_user2)["key"];
    send (stream, verb::post, "/order", post_order1, key1);
    send (stream, verb::post, "/order", post_order2, key2);

    auto orders = send (stream, verb::get, "/order");
    auto balance1 = send (stream, verb::get, "/balance", "", key1);
    auto balance2 = send (stream, verb::get, "/balance", "", key2);

    cout << orders.dump(4) << endl;
    cout << balance1.dump(4) << endl;
    cout << balance2.dump(4) << endl;
}

int argsCount = 0;
char** argV = nullptr;

void register_args (int argc, char** argv) {
    argsCount = argc;
    argV = argv;
}

string get_arg_or (int n, string const& def) {
    return (n < argsCount) ? argV[n] : def;
}

int main (int argc, char** argv) {
    register_args (argc, argv);

    string pathR = get_arg_or (1, "./requests.json");
    string ip = get_arg_or (2, "0.0.0.0");
    string port = get_arg_or (3, "8081");

    net::io_context ioc;
    tcp::socket socket(ioc);

    auto res = tcp::resolver (ioc).resolve (ip, port);
    beast::tcp_stream stream (ioc);
    stream.connect (res);

    try {
        requests (stream, pathR);
    } catch (exception const& e) {
        cout << e.what() << endl;
    }

    beast::error_code ec;
    stream.socket().shutdown (tcp::socket::shutdown_both, ec);
}

