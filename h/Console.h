#pragma once
#include "FileSystem.h"
#include <iostream>
#include <cstring>
#include <map>

class Console {
public:
    static int open();

private:
    typedef int (*FuncPtr)(char *, char *, char *);

    static const std::map<std::string, FuncPtr> commands_map;

    static int cmdHelp(char *args1, char *args2, char *args3);

    /* directory traversal */

    static int cmdCd(char *args1, char *args2, char *args3);

    static int cmdLs(char *args1, char *args2, char *args3);

    static int cmdTree(char *args1, char *args2, char *args3);

    /* directory changing */

    static int cmdOpen(char *args1, char *args2, char *args3);

    static int cmdClose(char *args1, char *args2, char *args3);

    static int cmdRename(char *args1, char *args2, char *args3);

    static int cmdRemove(char *args1, char *args2, char *args3);

    /* other printing */

    static int cmdOft(char *args1, char *args2, char *args3);

    static int cmdFat(char *args1, char *args2, char *args3);
};


