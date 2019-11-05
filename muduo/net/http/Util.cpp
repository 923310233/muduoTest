//
// Created by wushuohan on 19-5-18.
//

#include "Util.h"
#include "ConfigInit.h"

namespace Util {
    //获取请求的绝对路径
    string ConstructPath(string path) {
        string webHome = ConfigServer::html_root;
        if (path == "/") {
            return webHome + "/index.html";
        } else if (contains(path, "jpg") ||
                   contains(path, "ico") ||
                   contains(path, "png")) {
            string temp = path.substr(path.find_last_of("/"), path.size());
            return ConfigServer::pic_root + temp;
        } else if (contains(path, "mp4") || contains(path, "mp3")) {
            string temp = path.substr(path.find_last_of("/"), path.size());
            return ConfigServer::media_root + temp;
        } else if (contains(path, "html")) {
            return webHome + path;
        } else {
            return webHome + "/404";
        }
    }

    //获取文件扩展名
    string GetExtent(const string &path) {
        unsigned long i;
        for (i = path.size() - 1;; --i) {
            if (path[i] == '.')
                break;
        }
        return string(path.begin() + i + 1, path.end());
    }

    //获取content-Type
    string GetContentType(const string &path) {
        if (contains(path, "png")) {
            return "image/png";
        } else if (contains(path, "jpg")) {
            return "image/jpg";
        } else if (contains(path, "ico")) {
            return "image/x-icon";
        } else if (contains(path, "mp4")) {
            return "video/mpeg4";
        } else if (contains(path, "mp3")) {
            return "video/mpeg";
        } else if (contains(path, "html")) {
            return "text/html";
        } else {
            return "cgi";
        }


    }

    //获取Content
    string GetContent(const string &fileName) {
        if (contains(fileName, "weather")) {
            //能到这一步肯定能打开
            std::ifstream OpenFile("/home/wushuohan/webRoot/tianqi.txt");
            char str[20];
            OpenFile.getline(str, 20);
            OpenFile.close();
            return str;
        }


        fopen(fileName.c_str(), "r");
        std::ifstream fin(fileName, std::ios::in | std::ios::binary);
        if (fin.fail()) {
            return string("");
        }
        std::ostringstream oss;
        oss << fin.rdbuf();
        return std::string(oss.str());


//        FILE *pFile = fopen(fileName.c_str(), "r+");
//        //它从文件流中读数据，最多读取count个项，每个项size个字节
//        char buffer[10000];
//        fread(buffer,1, sizeof(buffer),pFile);
//        return buffer;
    }
}