//
// Created by wushuohan on 19-5-21.
//

#include "muduo/base/Logging.h"
#include "muduo/net/EventLoop.h"
#include "muduo/net/TcpServer.h"
#include <muduo/base/Logging.h>
#include <muduo/net/EventLoopThreadPool.h>
#include <muduo/net/http/HttpServer.h>
#include <muduo/net/http/ConfigInit.h>
#include <muduo/net/http/OnRequestCallBack.h>


using namespace std;
using namespace muduo;
using namespace muduo::net;

int main(int argc, char *argv[]) {
    if (argc <= 2) {
        LOG_ERROR << "usage : " << basename(argv[0]) << " 端口号  线程数   ";
        return 1;
    }

//    //TODO:
    uint16_t port = static_cast<uint16_t>(atoi(argv[1]));
//
    int numThreads = 1;
    if (argc == 3) {
        numThreads = atoi(argv[2]);
    }

    LOG_INFO << "端口号" << port << "  线程数" << numThreads;

#ifdef SO_REUSEPORT
    LOG_DEBUG << "SO_REUSEPORT";
    EventLoop loop;
    EventLoopThreadPool threadPool(&loop, "StaticWebServer");
    if (numThreads > 1) {
        threadPool.setThreadNum(numThreads);
    } else {
        numThreads = 1;
    }
    threadPool.start();

    ConfigServer::ConfigInit init;

    std::vector<std::unique_ptr<HttpServer>> servers;

    for (int i = 0; i < numThreads; ++i) {
        servers.emplace_back(new HttpServer(threadPool.getNextLoop(),
                                            InetAddress(port),
                                            "StaticWebServer",
                                            TcpServer::kReusePort));
        servers.back()->setHttpCallback(http_server_response_get);
        servers.back()->getLoop()->runInLoop(
                std::bind(&HttpServer::start, servers.back().get()));
    }
    loop.loop();
#endif

    return 0;
}
