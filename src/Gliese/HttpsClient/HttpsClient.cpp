#include <Gliese/HttpsClient/HttpsClient.hpp>

template <typename DataType>
boost::asio::awaitable<Json::Value> Gliese::HttpsClient::sendPostRequest(DataType &urlParts, DataType &data)
{
    try
    {
        Json::Value jsonData;
        Json::Value jsonResponse;
        Json::Reader reader;
        Json::FastWriter fastWriter;
        std::string strResponse;
        std::string headers;
        std::string jsonString;
        std::stringstream request;
        size_t content_length = 0;
        bool headers_read = false;
        boost::system::error_code ec;
        
        auto executor = co_await boost::asio::this_coro::executor;

        // Create SSL context and socket
        boost::asio::ssl::context ctx(boost::asio::ssl::context::sslv23_client);
        boost::asio::ssl::stream<boost::asio::ip::tcp::socket> ssock(executor, ctx);

        // Resolve the server address and port
        boost::asio::ip::tcp::resolver resolver(executor);
        auto endpoints = co_await resolver.async_resolve(urlParts["host"], urlParts["port"], boost::asio::use_awaitable);
        co_await boost::asio::async_connect(ssock.lowest_layer(), endpoints, boost::asio::use_awaitable);

        // Perform SSL handshake
        co_await ssock.async_handshake(boost::asio::ssl::stream_base::client, boost::asio::use_awaitable);

        if (!connectionData.empty())
        {
            for (const auto &element : connectionData)
            {
                jsonData[element.first] = element.second;
            }
        }

        for (const auto &element : data)
        {
            jsonData[element.first] = element.second;
        }
        
        jsonString = fastWriter.write(jsonData);//Json::writeString(writer, jsonData);

        // Create HTTP POST request
        
        request << fmt::format("POST {} HTTP/1.1\r\n", urlParts["path"]);
        request << fmt::format("Host: {}:{}\r\n", urlParts["host"], urlParts["port"]); // Update as needed
        request << "Content-Type: application/json\r\n";
        request << fmt::format("Content-Length: {}\r\n",std::to_string(jsonString.size()));
        request << "\r\n";
        request << jsonString;

        // Send the request
        co_await boost::asio::async_write(ssock, boost::asio::buffer(std::move(request.str())), boost::asio::use_awaitable);

        // Read the strResponse headers
        for (;;)
        {
            char buf[1024];
            std::size_t n = co_await ssock.async_read_some(boost::asio::buffer(buf), boost::asio::use_awaitable);
            strResponse.append(buf, buf + n);

            if (!headers_read)
            {
                auto pos = strResponse.find("\r\n\r\n");
                if (pos != std::string::npos)
                {
                    headers = strResponse.substr(0, pos);
                    strResponse = strResponse.substr(pos + 4);
                    headers_read = true;

                    // Parse headers to find Content-Length
                    std::istringstream stream(headers);
                    std::string line;
                    while (std::getline(stream, line) && line != "\r")
                    {
                        if (line.find("Content-Length:") == 0)
                        {
                            content_length = std::stoi(line.substr(16));
                        }
                    }

                    if (content_length == 0) break;
                }
            }

            if (headers_read && strResponse.size() >= content_length) break;
        }
        
        reader.parse(strResponse,jsonResponse);
        co_return jsonResponse;
    }
    catch(const std::exception &error)
    {
        throw std::runtime_error(fmt::format("sendPostRequest. {}",error.what()));
    }
}

Gliese::HttpsClient& Gliese::HttpsClient::setConnectionData(Gliese::HashMap &connection_data)
{
    connectionData = std::move(connection_data);
    return *this;
}

Gliese::HttpsClient& Gliese::HttpsClient::setUrl(const char *connection_url)
{
    connectionUrl = std::move(connection_url);
    return *this;
}

Gliese::HttpsClient& Gliese::HttpsClient::setJsonQueryStr(const char *json_query_str)
{
    jsonQueryStr = std::move(json_query_str);
    return *this;
}

Gliese::HttpsClient& Gliese::HttpsClient::setJsonDataStr(const char *json_data_str)
{
    jsonDataStr = std::move(json_data_str);
    return *this;
}

template boost::asio::awaitable<Json::Value> Gliese::HttpsClient::sendPostRequest<Gliese::HashMap>(Gliese::HashMap &, Gliese::HashMap &);
template boost::asio::awaitable<Json::Value> Gliese::HttpsClient::sendPostRequest<Gliese::Map>(Gliese::Map &, Gliese::Map &);