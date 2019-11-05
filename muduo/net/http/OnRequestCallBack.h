//
// Created by wushuohan on 19-5-21.
//

#ifndef MUDUO_ONREQUESTCALLBACK_H
#define MUDUO_ONREQUESTCALLBACK_H

#include <muduo/base/Logging.h>
#include "muduo/net/TcpServer.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "ConfigInit.h"
#include <sys/stat.h>
#include <wait.h>
#include "Util.h"
#include "fastcgi/fcgi.h"

namespace muduo {
    namespace net {
        //用于重定向的几个测试网页
        std::map<string, string> redirections;

        inline bool contains(const string &src, string &&sub) {
            string::size_type idx;
            idx = src.find(sub);//在a中查找b.
            if (idx == string::npos)return false;
            else {
                return true;
            }
        }

        void redirect_init() {
            redirections.clear();
            redirections.insert(pair<string, string>("/1", "www.baidu.com"));
            redirections.insert(pair<string, string>("/2", "www.sina.com"));
            redirections.insert(pair<string, string>("/3", "www.sogou.com"));
        }


        void http_server_response_get(const HttpRequest &req, HttpResponse *resp) {
            LOG_INFO << "请求头：" << req.path();

            //如果是一个重定向的请求——>跳转
            std::map<string, string>::const_iterator it = redirections.find(req.path());
            if (it != redirections.end()) {
                resp->setStatusCode(HttpResponse::k301MovedPermanently);
                resp->setStatusMessage("Moved Permanently");
                resp->addHeader("Location", it->second);
                resp->setCloseConnection(true);
                return;
            }
            string path = Util::ConstructPath(req.path());
            LOG_INFO << "资源的路径为" << path;

            if (req.path() == "/") {  //如果没有任何的有效信息  则跳转到主页
                resp->setStatusCode(HttpResponse::k200Ok);
                resp->setStatusMessage("OK");
                resp->setContentType("text/html");
                string content = Util::GetContent(path);
                if (content.empty()) {
                    resp->setStatusCode(HttpResponse::k404NotFound);
                    resp->setStatusMessage("Not Found");
                    resp->setContentType("text/html");
                    resp->setBody(Util::GetContent(ConfigServer::error404_path));
                    return;
                } else {
                    resp->setBody(content);
                    resp->setStatusCode(HttpResponse::k200Ok);
                    resp->setStatusMessage("OK");
                    resp->setContentType("text/html");
                    return;
                }
            }
            const string &type = Util::GetContentType(req.path());
            LOG_INFO << "请求文件的类型是" << type;
            if (type != "cgi") {

                struct stat filestat;
                int ret = stat(path.c_str(), &filestat);  // 执行成功则返回0，失败返回-1，错误代码存于errno

                if (contains(req.path(), "weather")) {
                    std::ifstream OpenFile1("/home/wushuohan/webRoot/tianqi.txt");
                    if (!OpenFile1.fail()) ret = 1;
                }

                if (ret < 0) // 处理文件不存在的情况
                {
                    LOG_INFO << "文件不存在";
                    resp->setStatusCode(HttpResponse::k404NotFound);
                    resp->setStatusMessage("Not Found");
                    resp->setCloseConnection(true);
                    resp->setBody(Util::GetContent(ConfigServer::error404_path));
                    return;
                }
                resp->setStatusCode(HttpResponse::k200Ok);
                resp->setStatusMessage("OK");
                resp->setContentType(type);
                resp->setBody(Util::GetContent(path));
                return;
            } else {
                int cgi_output[2];
                int cgi_input[2];
                pid_t pid;
                int status;
                char c;

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


//            Path = "/home/wushuohan/test/muduo/examples/shorturl/" + Path;
                    string query(req.query().begin() + 1, req.query().end());

                    sprintf(meth_env, "REQUEST_METHOD=%s", req.methodString());
                    sprintf(query_env, "QUERY_STRING=%s", query.c_str());

                    LOG_INFO << "查询信息为" << query;
                    path = ConfigServer::src_root + req.path();
                    LOG_INFO << "资源路径为" << path;

//            （文件描述符0、1、2），（stdin、stdout、stderr）
                    dup2(cgi_output[1], 1);//输出用标准输出
                    dup2(cgi_input[0], 0);//输入用标准输入
                    close(cgi_output[0]);
                    close(cgi_input[1]);
                    putenv(meth_env);
                    putenv(query_env);
                    execl(path.c_str(), query.c_str(), NULL);
//                    execl("/home/wushuohan/test/hello/test", "1", NULL);
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

            }
        }


    }
}

#endif //MUDUO_ONREQUESTCALLBACK_H
