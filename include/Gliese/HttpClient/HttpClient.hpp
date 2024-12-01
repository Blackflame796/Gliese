#ifndef HTTP_CLIENT_HPP
#define HTTP_CLIENT_HPP

#include <Gliese/General.hpp>
#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <json/json.h>

namespace http = boost::beast::http;

namespace Gliese
{
    class HttpClient
    {
    public:
        template <typename DataType>
        boost::asio::awaitable<Json::Value> sendPostRequest(DataType &urlParts,DataType &data);

        // template <typename DataType>
        HttpClient &setConnectionData(HashMap &connection_data);
        HttpClient &setUrl(const char *connection_url);
        HttpClient &setJsonQueryStr(const char *json_query_str);
        HttpClient &setJsonDataStr(const char *json_data_str);

    protected:

    private:
        std::string connectionUrl;
        std::string jsonQueryStr;
        std::string jsonDataStr;
        HashMap connectionData;
    };
}
#endif