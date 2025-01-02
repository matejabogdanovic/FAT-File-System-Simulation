#include "../h/Console.h"


const std::map<std::string, Console::FuncPtr> Console::commands_map = std::map<std::string, Console::FuncPtr>{
        {"help",   Console::cmdHelp},

        {"cd",     Console::cmdCd},
        {"ls",     Console::cmdLs},
        {"tree",   Console::cmdTree},

        {"open",   Console::cmdOpen},
        {"close",  Console::cmdClose},
        {"rename", Console::cmdRename},
        {"remove", Console::cmdRemove},


        {"oft",    Console::cmdOft},
        {"fat",    Console::cmdFat},
};

int Console::open() {

    int ret = 0;
    char cmd[PATHNAME_SZ + 10];
    char *command = nullptr;
    while(true) {

        std::cout << FileSystem::get().getWorkingDirectoryPath() << ">";
        std::cin.getline(cmd, PATHNAME_SZ + 10);

        command = strtok(cmd, " ");

        if(!command) {
            FileSystem::get().printTree();
            continue;
        }
        char *args1 = strtok(NULL, " ");
        char *args2 = strtok(NULL, " ");
        char *args3 = strtok(NULL, " ");
        if(strtok(NULL, " ")) {
            std::cout << "Invalid argument number.\n";
            continue;
        }
        // std::cout << "command " << command;
        //std::cout << " args " << (args ? args : "no") << std::endl;
        if(!strcmp(command, "exit"))
            break;

        auto pair = commands_map.find(std::string(command));

        if(pair != commands_map.end()) {
            ret = pair->second(args1, args2, args3);
        } else {
            std::cout << "Unknown command. Please, type 'help pls' for help.\n";
            continue;
        }
        if(ret < 0) {
            if(ret == -1) std::cout << "Invalid arguments." << std::endl;
            else std::cout << "Error: " << std::dec << ret << std::endl;
        }
    }

    return 0;
}

int Console::cmdHelp(char *args1, char *args2, char *args3) {
    if(!args1 || strcmp(args1, "pls") != 0)return -1;
    std::cout << "=================================================================\n"
              << "Help arrived! Check out these sick commands!\n"
              << "$arg_name$ - required arguments\n"
              << "[arg_name] - optional arguments\n"
              << "=================================================================\n"
              << "'exit' - quit console\n"
              << "'cd $path$' - change directory; $path$ can be relative or absolute\n"
              << "'ls' - list current directory files\n"
              << "'tree root' - print tree starting from root\n"
              << "'open $path$' - open file/directory; $path$ can be relative or absolute\n"
              << "'close $path$' - close file/directory; $path$ can be relative or absolute\n"
              << "'rename $path$ $name$' - rename file/directory to $name$; $path$ can be relative or absolute\n"
              << "'remove $path$' - quit console; $path$ can be relative or absolute\n"
              << "'oft [last_entry_to_print]' - list oft\n"
              << "'fat [last_entry_to_print]' - list fat\n"
              << "=================================================================\n";
    return 0;
}

/* ==================== directory traversal ==================== */

int Console::cmdCd(char *args1, char *args2, char *args3) {
    if(!args1 || args2 || args3)return -1;
    //  if(!strcmp(args, ".."))
    // FileSystem::get().setWDtoParent();
    //  else if(!strcmp(args, ".")) {
    //   continue;
    // } else {
    int ret = FileSystem::get().setWorkingDirectory(args1);
    if(ret == -10)std::cout << "Invalid directory name." << std::endl;
    return ret;
}

int Console::cmdLs(char *args1, char *args2, char *args3) {
    if(args1 || args2 || args3)return -1;
    FileSystem::get().listWorkingDirectory();
    return 0;
}

int Console::cmdTree(char *args1, char *args2, char *args3) {
    if(!args1 || strcmp(args1, "root") != 0 || args2 || args3) return -1;
    FileSystem::get().printTree();
    return 0;
}

/* ==================== directory changing ==================== */

int Console::cmdOpen(char *args1, char *args2, char *args3) {
    if(!args1 || args2 || args3)return -1;
    int ret = FileSystem::get().open(args1, 1);
    FileSystem::get().printTree();
    return ret;
}

int Console::cmdClose(char *args1, char *args2, char *args3) {
    if(!args1 || args2 || args3)return -1;

    return FileSystem::get().close(args1);
}

int Console::cmdRename(char *args1, char *args2, char *args3) {
    if(!args1 || !args2 || args3)return -1;

    return FileSystem::get().rename(args1, args2);
}

int Console::cmdRemove(char *args1, char *args2, char *args3) {
    if(!args1 || args2 || args3)return -1;
    return FileSystem::get().remove(args1);
}

/* ==================== other printing ==================== */

int Console::cmdOft(char *args1, char *args2, char *args3) {
    if(args2 || args3)return -1;
    FileSystem::get().oft.printOFT(args1 ? atoi(args1) : 255);
    return 0;
}

int Console::cmdFat(char *args1, char *args2, char *args3) {
    if(args2 || args3)return -1;
    FAT::printFAT(args1 ? atoi(args1) : 255);
    return 0;
}




