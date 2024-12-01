#include <Gliese/WebClient/WebClient.hpp>

template <typename DataType>
cppcoro::task<Json::Value> Gliese::WebClient::sendPostRequest(const std::string &url, DataType &data)
{
    HashMap urlParts = co_await getUrlParts(url);
    // Json::Value outputData;
    // urlParts["scheme"] = "http" ? http;
    boost::asio::io_context ioc;

    boost::asio::co_spawn(ioc, urlParts["scheme"] = "http" ? http_client->sendPostRequest<DataType>(urlParts, data) : https_client->sendPostRequest<DataType>(urlParts, data),
                          [&](std::exception_ptr e, Json::Value response)
                          {
                              if (e)
                              {
                                  try
                                  {
                                      std::rethrow_exception(e);
                                  }
                                  catch (const std::exception &ex)
                                  {
                                      std::cerr << "Error: " << ex.what() << std::endl;
                                  }
                              }
                              co_return response;
                          });

    ioc.run();
}

cppcoro::task<Gliese::HashMap> Gliese::WebClient::getUrlParts(const std::string &url)
{
    try
    {
        Gliese::HashMap urlParts;
        std::regex url_regex(R"((http|https)://([^:/]+):?(\d*)(/.*)?)");
        std::smatch url_match_result;
        std::string scheme;
        std::string port;
        if (std::regex_match(url, url_match_result, url_regex))
        {
            scheme = url_match_result[1];
            port = url_match_result[3];
            urlParts.insert(std::make_pair("scheme", scheme));
            if (port.empty())
            {
                port = (scheme == "https") ? "443" : "80";
            }
            urlParts.insert(std::make_pair("port", port));
            urlParts.insert(std::make_pair("host", url_match_result[2]));
            urlParts.insert(std::make_pair("path", url_match_result[4]));
        }
        co_return urlParts;
    }
    catch (const std::exception &error)
    {
        throw std::runtime_error(fmt::format("WebClient.getUrlParts.{}", error.what()));
    }
}