#pragma once
#include "FileSystem.h"
#include <iostream>
#include <cstring>
#include <map>

class Console {
public:
    static int open();

private:
    typedef int (*FunctionPointer)(char *);

    static const std::map<std::string, FunctionPointer> map;

    static int cmdHelp(char *args);

    /* directory traversal */

    static int cmdCd(char *args);

    static int cmdLs(char *args);

    static int cmdTree(char *args);

    /* directory changing */

    static int cmdOpen(char *args);

    static int cmdClose(char *args);

    static int cmdRemove(char *args);

    /* other printing */

    static int cmdOft(char *args);

    static int cmdFat(char *args);
};


