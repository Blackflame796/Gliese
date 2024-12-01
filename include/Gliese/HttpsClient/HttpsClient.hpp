#ifndef HTTPS_CLIENT_HPP
#define HTTPS_CLIENT_HPP

#include <Gliese/General.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <json/json.h>
#include <sstream>

namespace Gliese
{
    class HttpsClient
    {
    public:
        template <typename DataType>
        boost::asio::awaitable<Json::Value> sendPostRequest(DataType &urlParts, DataType &data);

        // template <typename DataType>
        HttpsClient& setConnectionData(HashMap &connection_data);
        HttpsClient& setUrl(const char *connection_url);
        HttpsClient& setJsonQueryStr(const char *json_query_str);
        HttpsClient& setJsonDataStr(const char *json_data_str);

    protected:
    private:
        std::string connectionUrl;
        std::string jsonQueryStr;
        std::string jsonDataStr;
        HashMap connectionData;
    };
}

#endif