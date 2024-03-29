

#include "muduo/net/http/HttpServer.h"

#include "muduo/base/Logging.h"
#include "muduo/net/http/HttpContext.h"
#include "muduo/net/http/HttpRequest.h"
#include "muduo/net/http/HttpResponse.h"
#include <fstream>

using namespace muduo;
using namespace muduo::net;

namespace muduo {
    namespace net {
        namespace detail {

            void defaultHttpCallback(const HttpRequest &, HttpResponse *resp) {
                resp->setStatusCode(HttpResponse::k404NotFound);
                resp->setStatusMessage("Not Found");
                resp->setCloseConnection(true);
            }

        }  // namespace detail
    }  // namespace net
}  // namespace muduo

HttpServer::HttpServer(EventLoop *loop,
                       const InetAddress &listenAddr,
                       const string &name,
                       TcpServer::Option option)
        : server_(loop, listenAddr, name, option),
          httpCallback_(detail::defaultHttpCallback) {
    server_.setConnectionCallback(
            std::bind(&HttpServer::onConnection, this, _1));
    server_.setMessageCallback(
            std::bind(&HttpServer::onMessage, this, _1, _2, _3));
}

void HttpServer::start() {
    LOG_WARN << "HttpServer[" << server_.name()
             << "] starts listenning on " << server_.ipPort();
    server_.start();
}

void HttpServer::onConnection(const TcpConnectionPtr &conn) {
    if (conn->connected()) {
        conn->setContext(HttpContext());
    }
}

inline bool contains(const string &src, string &&sub) {
    string::size_type idx;
    idx = src.find(sub);//在a中查找b.
    if (idx == string::npos)return false;
    else {
        return true;
    }
}

void HttpServer::onMessage(const TcpConnectionPtr &conn,
                           Buffer *buf,
                           Timestamp receiveTime) {
    LOG_INFO << "On message!!!!!!!!!!!!" << buf->peek();
    string s = buf->peek();
    if (contains(s, "shidu")) {
        std::ofstream OpenFile("/home/wushuohan/webRoot/tianqi.txt");
        if (OpenFile.fail()) {
            exit(0);
            return;
        }
        OpenFile << buf->peek();
        OpenFile.close();
        return;
    }


    HttpContext *context = boost::any_cast<HttpContext>(conn->getMutableContext());

    if (!context->parseRequest(buf, receiveTime)) {
        conn->send("HTTP/1.1 400 Bad Request\r\n\r\n");
        conn->shutdown();
    }

    if (context->gotAll()) {
        onRequest(conn, context->request());
        context->reset();
    }
}

void HttpServer::onRequest(const TcpConnectionPtr &conn, const HttpRequest &req) {
    const string &connection = req.getHeader("Connection");
    bool close = connection == "close" ||
                 (req.getVersion() == HttpRequest::kHttp10 && connection != "Keep-Alive");
    HttpResponse response(close);
    httpCallback_(req, &response);
    Buffer buf;
    response.appendToBuffer(&buf);
    conn->send(&buf);
    if (response.closeConnection()) {
        conn->shutdown();
    }
}
