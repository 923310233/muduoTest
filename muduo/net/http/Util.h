//
// Created by wushuohan on 19-5-18.
//

#ifndef MUDUO_UTIL_H
#define MUDUO_UTIL_H

#include <string>
#include <algorithm>
#include <fstream>
#include <vector>
#include <sstream>
#include "muduo/net/Buffer.h"


using namespace std;
namespace Util {
    inline bool contains(const string &src, string &&sub) {
        string::size_type idx;
        idx = src.find(sub);//在a中查找b.
        if (idx == string::npos)return false;
        else {
            return true;
        }
    }


    string ConstructPath(string path);

    string GetExtent(const string &path);

    string GetContentType(const string &path);

    string GetContent(const string &);

    string GetImage(const string &);
}


#endif //MUDUO_UTIL_H
