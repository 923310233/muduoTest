//
// Created by wushuohan on 19-5-21.
//

#ifndef MUDUO_CONFIGINIT_H
#define MUDUO_CONFIGINIT_H

#include <cstdio>
#include <muduo/base/Logging.h>
#include <muduo/base/FileReadStream.h>
#include "../../rapidjson/document.h"

namespace ConfigServer {
    //服务器的默认配置
    static int MAX_FD = 65535;
    static int MAX_EPOLL_EVENTS = 10000;
    static int MAX_THREADS = 8;
    static int READ_BUFFER = 2048;
    static int WRITE_BUFFER = 1024;
    static int MAX_FILE_LEN = 200;
    static uint16_t port = 8888;
    static std::string html_root = "/home/wushuohan/webRoot/html";
    static std::string pic_root = "/home/wushuohan/webRoot/image";
    static std::string media_root = "/home/wushuohan/webRoot/media";
    static std::string src_root = "/home/wushuohan/webRoot/src";
    static std::string error404_path = "/home/wushuohan/webRoot/html/404.html";

    class BaseFp {
    public:
        BaseFp() {}

        BaseFp(const BaseFp &) = delete;

        BaseFp &operator=(const BaseFp &) = delete;

        explicit BaseFp(const char *path, const char *mode = "r") {
            http_fp = fopen(path, mode);
            if (!http_fp) {
                LOG_ERROR << "配置文件打开失败" << path;
            }

        }

        ~BaseFp() {
            fclose(http_fp);
        }

        inline FILE *GetFp() {
            return http_fp;
        }

    protected:
        FILE *http_fp;
    };


    class ConfigInit {
    public:
        ConfigInit(const ConfigInit &);

        ConfigInit &operator=(const ConfigInit &);

        ~ConfigInit() {};

        ConfigInit() :
                server_fp("/home/wushuohan/test/muduo/examples/staticHttpWebServer/server_init.json") {
            initServer();
        }

    private:
        void initServer() {
            muduo::FileUtil::FileReadStream stream(server_fp.GetFp(), read_buffer, sizeof(read_buffer));
            rapidjson::Document bank;
            bank.ParseStream(stream);
            assert(bank.IsObject());

            ConfigServer::MAX_FD = bank["server-config"]["max-fd"].GetInt();
            ConfigServer::MAX_EPOLL_EVENTS = bank["server-config"]["max-epoll-events"].GetInt();
            ConfigServer::MAX_THREADS = bank["server-config"]["max-threads"].GetInt();
            ConfigServer::READ_BUFFER = bank["server-config"]["read-buffer"].GetInt();
            ConfigServer::WRITE_BUFFER = bank["server-config"]["write-buffer"].GetInt();
            ConfigServer::MAX_FILE_LEN = bank["server-config"]["filename-len"].GetInt();
            ConfigServer::html_root = bank["default-config"]["www-root"].GetString();
            ConfigServer::pic_root = bank["default-config"]["pic-root"].GetString();
            ConfigServer::media_root = bank["default-config"]["media-root"].GetString();
            ConfigServer::src_root = bank["default-config"]["src-root"].GetString();
            ConfigServer::error404_path = html_root + "/404.html";
            ConfigServer::port = 8888;
            LOG_INFO << "服务器的配置文件读取成功 webroot:" << html_root;
        }

        char read_buffer[65535];
        BaseFp server_fp;
    };

}


#endif //MUDUO_CONFIGINIT_H
