#include "muduo/net/http/HttpServer.h"
#include "muduo/net/http/HttpRequest.h"
#include "muduo/net/http/HttpResponse.h"
#include "muduo/net/EventLoop.h"
#include "muduo/net/EventLoopThreadPool.h"
#include "muduo/base/Logging.h"

#include <map>

#include <sys/socket.h>  // SO_REUSEPORT
#include <muduo/net/http/Util.h>
#include <wait.h>

using namespace muduo;
using namespace muduo::net;

extern char favicon[555];
bool benchmark = false;

std::map<string, string> redirections;


inline bool contains(const string &src, string &&sub) {
    string::size_type idx;
    idx = src.find(sub);//在a中查找b.
    if (idx == string::npos)return false;
    else {
        return true;
    }
}


void onRequestAdvice(const HttpRequest &req, HttpResponse *resp) {
    int cgi = 0;//on - off
    if (req.method() != HttpRequest::kGet && req.method() != HttpRequest::kPost) {
        resp->setStatusCode(HttpResponse::k501);
        resp->setStatusMessage("Method Not Implemented");
        resp->setContentType("text/html");
        resp->setBody(
                "<HTML><HEAD><TITLE>Method Not Implemented</TITLE></HEAD><BODY><P>HTTP request method not supported.</BODY></HTML>");
        return;
    }
    if (req.method() == HttpRequest::kPost ||
        (req.method() == HttpRequest::kGet && !req.query().empty())) {
        cgi = 1;
    }
    if (!cgi) {
        string path = Util::ConstructPath(req.path());
        string extent = Util::GetExtent(path);
        string contentType = "";
//        Util::GetContentType(extent, contentType);
//
//        LOG_INFO << "contentType是" << contentType;

        string content = Util::GetContent(path);
        if (content.empty()) {
            resp->setStatusCode(HttpResponse::k404NotFound);
            resp->setStatusMessage("Not Found");
        } else {
            resp->setBody(content);
            resp->setStatusCode(HttpResponse::k200Ok);
            resp->setStatusMessage("OK");
            resp->setContentType("text/html");
        }
        return;
    }

    /*****************************************/

    //CGI SCRIPT
#if 1
    int cgi_output[2];
    int cgi_input[2];
    pid_t pid;
    int status;
    char c;
    //POST
    if (req.method() == HttpRequest::kPost) {
//	cout<<req.getBody()<<endl;
//	cout<<"xxxx"<<req.getHeader("Content-Length")<<endl;

        int contentLengthValue = std::stoi(req.getHeader("Content-Length"));
        string databody = "";
        resp->setStatusCode(HttpResponse::k200Ok);
        resp->setStatusMessage("OK");
        //PIPE需要提供两个文件描述符来操作管道。其中一个对管道进行写操作，另一个对管道进行读操作。
        //如果成功则返回0
        if (pipe(cgi_output) < 0) {
            printf("pipe error\r\n");
            return;
        }
        if (pipe(cgi_input) < 0) {
            printf("pipe error\r\n");
            return;
        }
        if ((pid = fork()) < 0) {
            printf("fork error\r\n");
            return;
        }
        if (pid == 0) {
            char meth_env[255];
            char length_env[255];

            string Path(req.path().begin() + 1, req.path().end());
            LOG_INFO << "req.methodString():" << req.methodString();
            LOG_INFO << "contentLengthValue:" << contentLengthValue;

            //主要功能是把格式化的数据写入某个字符串中
            sprintf(meth_env, "REQUEST_METHOD=%s", req.methodString());
            sprintf(length_env, "CONTENT_LENGTH=%d", contentLengthValue);

            //增加环境变量的内容
            putenv(meth_env);
            putenv(length_env);

            //复制文件描述符   dup2(int oldfd,int newfd);
            //可以用参数newfd指定新文件描述符的数值
            dup2(cgi_output[1], 1);
            dup2(cgi_input[0], 0);
            close(cgi_output[0]);
            close(cgi_input[1]);

//            execl()函数：执行文件函数
//            函数说明：execl()用来执行参数path 字符串所代表的文件路径,
//            接下来的参数代表执行该文件时传递过去的argv(0),argv[1], ..., 最后一个参数必须用空指针(NULL)作结束.
            LOG_INFO << "PATH:" << Path.c_str();
            execl(Path.c_str(), Path.c_str(), NULL);
            exit(0);
        } else {
            string cgibody;
            close(cgi_output[1]);
            close(cgi_input[0]);

            for (long unsigned int i = 0; i < databody.size(); ++i) {
                write(cgi_input[1], &databody[i], 1);
            }
//	    write(cgi_input[1],databody.c_str() ,databody.size() );
#if 1
            while (read(cgi_output[0], &c, 1) > 0) {
                cgibody += c;
            }
#endif
            close(cgi_output[0]);
            close(cgi_input[1]);
            resp->setBody(cgibody);
            waitpid(pid, &status, 0);
        }

    }//POST END
    else if (req.method() == HttpRequest::kGet)//GET
    {
        resp->setStatusCode(HttpResponse::k200Ok);
        resp->setStatusMessage("OK");
        if (pipe(cgi_output) < 0) {
            printf("pipe error\r\n");
            return;
        }
        if (pipe(cgi_input) < 0) {
            printf("pipe error\r\n");
            return;
        }
        if ((pid = fork()) < 0) {
            printf("fork error\r\n");
            return;
        }

        if (pid == 0) {
            char meth_env[255];

            char query_env[255];

            string Path(req.path().begin() + 1, req.path().end());
//            Path = "/home/wushuohan/test/muduo/examples/shorturl/" + Path;
            string Query(req.query().begin() + 1, req.query().end());
            Query = Query.substr(8, Query.size());//截去command =

            sprintf(meth_env, "REQUEST_METHOD=%s", req.methodString());
            sprintf(query_env, "QUERY_STRING=%s", Query.c_str());

//            LOG_INFO << "req.methodString():" << req.methodString();
//            LOG_INFO << "QUERY_STRING:" << Query.c_str();

            string temp[4];
            memset(temp, 0, sizeof(temp));
            uint16_t s = 0;
            for (unsigned long int i = 0; i < Query.size(); i++) { //将字符串分割成字符串数组
                if (Query[i] == '+') {                 //以空格作为分隔符
                    s++;
                    continue;
                }
                temp[s] += Query[i];                      //将分割好的字符串放到K数组里
            }
            string shell = "/bin/" + temp[0];
            LOG_INFO << "执行的脚本是" << shell;


//            （文件描述符0、1、2），（stdin、stdout、stderr）
            dup2(cgi_output[1], 1);//输出用标准输出
            dup2(cgi_input[0], 0);//输入用标准输入
            LOG_INFO<<" "<<cgi_output[1]<<" "<<cgi_input[0];
            close(cgi_output[0]);
            close(cgi_input[1]);
            putenv(meth_env);
            putenv(query_env);
            LOG_INFO << "script PATH:" << shell;
            LOG_INFO << "shell脚本开始执行<<<<<<<<<<";
            execl("/home/wushuohan/test/hello/test", "1", NULL);
//            if (s == 0) {
//                execl(shell.c_str(), temp[0].c_str(), NULL);
//            } else if (s == 1) {
//                LOG_INFO << "TEST" << temp[0] << " " << temp[1];
//                execl(shell.c_str(), temp[0].c_str(), temp[1].c_str(), NULL);
//            } else if (s == 2) {
//                execl(shell.c_str(), temp[0].c_str(), temp[1].c_str(), temp[2].c_str(), NULL);
//            }
//            execl(Path.c_str(), Path.c_str(), NULL);
            exit(0);
        } else {
            string cgibody;
            close(cgi_output[1]);
            close(cgi_input[0]);
#if 1
            while (read(cgi_output[0], &c, 1) > 0) {
                cgibody += c;
            }
#endif

            close(cgi_output[0]);
            close(cgi_input[1]);
            resp->setBody(cgibody);
            waitpid(pid, &status, 0);
        }

    }//GET END
    else {
        // Bad Request!
        //FIXME:
        return;
    }
#endif
    //cgi
}


/***********************************/

void onRequest(const HttpRequest &req, HttpResponse *resp) {
    LOG_INFO << "Headers " << req.methodString() << " " << req.path();
    LOG_INFO << "请求地址" << req.path();
    if (!benchmark) {
        const std::map<string, string> &headers = req.headers();
        for (std::map<string, string>::const_iterator it = headers.begin();
             it != headers.end();
             ++it) {
            LOG_DEBUG << it->first << ": " << it->second;
        }
    }

    // TODO: support PUT and DELETE to create new redirections on-the-fly.

    std::map<string, string>::const_iterator it = redirections.find(req.path());
    if (it != redirections.end()) {
        resp->setStatusCode(HttpResponse::k301MovedPermanently);
        resp->setStatusMessage("Moved Permanently");
        resp->addHeader("Location", it->second);
        // resp->setCloseConnection(true);
    } else if (req.path() == "/") {
        resp->setStatusCode(HttpResponse::k200Ok);
        resp->setStatusMessage("OK");
        resp->setContentType("text/html");
        string path = Util::ConstructPath(req.path());

        LOG_INFO << "path是" << path;

        string extent = Util::GetExtent(path);
        string content = Util::GetContent(path);
        if (content.empty()) {
            resp->setStatusCode(HttpResponse::k404NotFound);
            resp->setStatusMessage("Not Found");
        } else {
            resp->setBody(content);
            resp->setStatusCode(HttpResponse::k200Ok);
            resp->setStatusMessage("OK");
            resp->setContentType("text/html");
        }
        return;
    } else if (contains(req.path(), "png")) {
        const string &constructPath = Util::ConstructPath(req.path());

        LOG_INFO << "需要资源的位置是" << constructPath;

        resp->setStatusCode(HttpResponse::k200Ok);
        resp->setStatusMessage("OK");
        resp->setContentType("image/png");
        resp->setBody(Util::GetContent(constructPath));
    } else if (contains(req.path(), "jpg")) {
        const string &constructPath = Util::ConstructPath(req.path());

        LOG_INFO << "需要资源的位置是" << constructPath;

        resp->setStatusCode(HttpResponse::k200Ok);
        resp->setStatusMessage("OK");
        resp->setContentType("image/jpg");
        resp->setBody(Util::GetContent(constructPath));
    } else if (contains(req.path(), "ico")) {
        const string &constructPath = Util::ConstructPath(req.path());

        LOG_INFO << "需要资源的位置是" << constructPath;

        resp->setStatusCode(HttpResponse::k200Ok);
        resp->setStatusMessage("OK");
        resp->setContentType("image/x-icon");
        resp->setBody(Util::GetContent(constructPath));
    } else if (contains(req.path(), "mp4")) {
        const string &constructPath = Util::ConstructPath(req.path());

        LOG_INFO << "需要资源的位置是" << constructPath;

        resp->setStatusCode(HttpResponse::k200Ok);
        resp->setStatusMessage("OK");
        resp->setContentType("video/mpeg4");
        resp->setBody(Util::GetContent(constructPath));
    } else if (contains(req.path(), "mp3")) {
        const string &constructPath = Util::ConstructPath(req.path());

        LOG_INFO << "需要资源的位置是" << constructPath;

        resp->setStatusCode(HttpResponse::k200Ok);
        resp->setStatusMessage("OK");
        resp->setContentType("video/mpeg");
        resp->setBody(Util::GetContent(constructPath));
    } else {

    }
}

int main(int argc, char *argv[]) {
    redirections["/1"] = "www.baidu.com";
    redirections["/2"] = "www.sina.com";

    int numThreads = 1;
    benchmark = true;
//    if (argc > 1) {
//        benchmark = true;
//        Logger::setLogLevel(Logger::WARN);
//        numThreads = 4;
//    }

#ifdef SO_REUSEPORT
    LOG_WARN << "SO_REUSEPORT";
    EventLoop loop;
    EventLoopThreadPool threadPool(&loop, "shorturl");
    if (numThreads > 1) {
        threadPool.setThreadNum(numThreads);
    } else {
        numThreads = 1;
    }
    threadPool.start();

    std::vector<std::unique_ptr<HttpServer>> servers;
    for (int i = 0; i < numThreads; ++i) {
        servers.emplace_back(new HttpServer(threadPool.getNextLoop(),
                                            InetAddress(8888),
                                            "shorturl",
                                            TcpServer::kReusePort));
        servers.back()->setHttpCallback(onRequestAdvice);
//        servers.back()->setHttpCallback(onRequest);
        servers.back()->getLoop()->runInLoop(
                std::bind(&HttpServer::start, servers.back().get()));
    }
    loop.loop();
#else
    LOG_WARN << "Normal";
    EventLoop loop;
    HttpServer server(&loop, InetAddress(8888), "shorturl");
    server.setHttpCallback(onRequest);
    server.setThreadNum(numThreads);
    server.start();
    loop.loop();
#endif
}

char favicon[555] = {
        '\x89', 'P', 'N', 'G', '\xD', '\xA', '\x1A', '\xA',
        '\x0', '\x0', '\x0', '\xD', 'I', 'H', 'D', 'R',
        '\x0', '\x0', '\x0', '\x10', '\x0', '\x0', '\x0', '\x10',
        '\x8', '\x6', '\x0', '\x0', '\x0', '\x1F', '\xF3', '\xFF',
        'a', '\x0', '\x0', '\x0', '\x19', 't', 'E', 'X',
        't', 'S', 'o', 'f', 't', 'w', 'a', 'r',
        'e', '\x0', 'A', 'd', 'o', 'b', 'e', '\x20',
        'I', 'm', 'a', 'g', 'e', 'R', 'e', 'a',
        'd', 'y', 'q', '\xC9', 'e', '\x3C', '\x0', '\x0',
        '\x1', '\xCD', 'I', 'D', 'A', 'T', 'x', '\xDA',
        '\x94', '\x93', '9', 'H', '\x3', 'A', '\x14', '\x86',
        '\xFF', '\x5D', 'b', '\xA7', '\x4', 'R', '\xC4', 'm',
        '\x22', '\x1E', '\xA0', 'F', '\x24', '\x8', '\x16', '\x16',
        'v', '\xA', '6', '\xBA', 'J', '\x9A', '\x80', '\x8',
        'A', '\xB4', 'q', '\x85', 'X', '\x89', 'G', '\xB0',
        'I', '\xA9', 'Q', '\x24', '\xCD', '\xA6', '\x8', '\xA4',
        'H', 'c', '\x91', 'B', '\xB', '\xAF', 'V', '\xC1',
        'F', '\xB4', '\x15', '\xCF', '\x22', 'X', '\x98', '\xB',
        'T', 'H', '\x8A', 'd', '\x93', '\x8D', '\xFB', 'F',
        'g', '\xC9', '\x1A', '\x14', '\x7D', '\xF0', 'f', 'v',
        'f', '\xDF', '\x7C', '\xEF', '\xE7', 'g', 'F', '\xA8',
        '\xD5', 'j', 'H', '\x24', '\x12', '\x2A', '\x0', '\x5',
        '\xBF', 'G', '\xD4', '\xEF', '\xF7', '\x2F', '6', '\xEC',
        '\x12', '\x20', '\x1E', '\x8F', '\xD7', '\xAA', '\xD5', '\xEA',
        '\xAF', 'I', '5', 'F', '\xAA', 'T', '\x5F', '\x9F',
        '\x22', 'A', '\x2A', '\x95', '\xA', '\x83', '\xE5', 'r',
        '9', 'd', '\xB3', 'Y', '\x96', '\x99', 'L', '\x6',
        '\xE9', 't', '\x9A', '\x25', '\x85', '\x2C', '\xCB', 'T',
        '\xA7', '\xC4', 'b', '1', '\xB5', '\x5E', '\x0', '\x3',
        'h', '\x9A', '\xC6', '\x16', '\x82', '\x20', 'X', 'R',
        '\x14', 'E', '6', 'S', '\x94', '\xCB', 'e', 'x',
        '\xBD', '\x5E', '\xAA', 'U', 'T', '\x23', 'L', '\xC0',
        '\xE0', '\xE2', '\xC1', '\x8F', '\x0', '\x9E', '\xBC', '\x9',
        'A', '\x7C', '\x3E', '\x1F', '\x83', 'D', '\x22', '\x11',
        '\xD5', 'T', '\x40', '\x3F', '8', '\x80', 'w', '\xE5',
        '3', '\x7', '\xB8', '\x5C', '\x2E', 'H', '\x92', '\x4',
        '\x87', '\xC3', '\x81', '\x40', '\x20', '\x40', 'g', '\x98',
        '\xE9', '6', '\x1A', '\xA6', 'g', '\x15', '\x4', '\xE3',
        '\xD7', '\xC8', '\xBD', '\x15', '\xE1', 'i', '\xB7', 'C',
        '\xAB', '\xEA', 'x', '\x2F', 'j', 'X', '\x92', '\xBB',
        '\x18', '\x20', '\x9F', '\xCF', '3', '\xC3', '\xB8', '\xE9',
        'N', '\xA7', '\xD3', 'l', 'J', '\x0', 'i', '6',
        '\x7C', '\x8E', '\xE1', '\xFE', 'V', '\x84', '\xE7', '\x3C',
        '\x9F', 'r', '\x2B', '\x3A', 'B', '\x7B', '7', 'f',
        'w', '\xAE', '\x8E', '\xE', '\xF3', '\xBD', 'R', '\xA9',
        'd', '\x2', 'B', '\xAF', '\x85', '2', 'f', 'F',
        '\xBA', '\xC', '\xD9', '\x9F', '\x1D', '\x9A', 'l', '\x22',
        '\xE6', '\xC7', '\x3A', '\x2C', '\x80', '\xEF', '\xC1', '\x15',
        '\x90', '\x7', '\x93', '\xA2', '\x28', '\xA0', 'S', 'j',
        '\xB1', '\xB8', '\xDF', '\x29', '5', 'C', '\xE', '\x3F',
        'X', '\xFC', '\x98', '\xDA', 'y', 'j', 'P', '\x40',
        '\x0', '\x87', '\xAE', '\x1B', '\x17', 'B', '\xB4', '\x3A',
        '\x3F', '\xBE', 'y', '\xC7', '\xA', '\x26', '\xB6', '\xEE',
        '\xD9', '\x9A', '\x60', '\x14', '\x93', '\xDB', '\x8F', '\xD',
        '\xA', '\x2E', '\xE9', '\x23', '\x95', '\x29', 'X', '\x0',
        '\x27', '\xEB', 'n', 'V', 'p', '\xBC', '\xD6', '\xCB',
        '\xD6', 'G', '\xAB', '\x3D', 'l', '\x7D', '\xB8', '\xD2',
        '\xDD', '\xA0', '\x60', '\x83', '\xBA', '\xEF', '\x5F', '\xA4',
        '\xEA', '\xCC', '\x2', 'N', '\xAE', '\x5E', 'p', '\x1A',
        '\xEC', '\xB3', '\x40', '9', '\xAC', '\xFE', '\xF2', '\x91',
        '\x89', 'g', '\x91', '\x85', '\x21', '\xA8', '\x87', '\xB7',
        'X', '\x7E', '\x7E', '\x85', '\xBB', '\xCD', 'N', 'N',
        'b', 't', '\x40', '\xFA', '\x93', '\x89', '\xEC', '\x1E',
        '\xEC', '\x86', '\x2', 'H', '\x26', '\x93', '\xD0', 'u',
        '\x1D', '\x7F', '\x9', '2', '\x95', '\xBF', '\x1F', '\xDB',
        '\xD7', 'c', '\x8A', '\x1A', '\xF7', '\x5C', '\xC1', '\xFF',
        '\x22', 'J', '\xC3', '\x87', '\x0', '\x3', '\x0', 'K',
        '\xBB', '\xF8', '\xD6', '\x2A', 'v', '\x98', 'I', '\x0',
        '\x0', '\x0', '\x0', 'I', 'E', 'N', 'D', '\xAE',
        'B', '\x60', '\x82',
};
