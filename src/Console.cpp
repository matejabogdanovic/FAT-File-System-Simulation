#include "../h/Console.h"


const std::map<std::string, Console::FunctionPointer> Console::map = std::map<std::string, Console::FunctionPointer>{
        {"help",   Console::cmdHelp},

        {"cd",     Console::cmdCd},
        {"ls",     Console::cmdLs},
        {"tree",   Console::cmdTree},

        {"open",   Console::cmdOpen},
        {"close",  Console::cmdClose},
        {"remove", Console::cmdRemove},


        {"oft",    Console::cmdOft},
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
        char *args = strtok(NULL, " ");
        if(strtok(NULL, " ")) {
            std::cout << "INVALID COMMAND\n";
            continue;
        }
        std::cout << "command " << command;
        std::cout << " args " << (args ? args : "no") << std::endl;
        if(!strcmp(command, "exit"))
            break;

        auto pair = map.find(std::string(command));

        if(pair != map.end()) {
            ret = pair->second(args);
        } else {
            std::cout << "Unknown command. Please, type 'help pls' for help.\n";
        }
        if(ret < 0) {
            if(ret == -1) std::cout << "Invalid arguments." << std::endl;
            else std::cout << "Error: " << std::dec << ret << std::endl;
        }
    }

    return 0;
}

int Console::cmdHelp(char *args) {
    if(!args || strcmp(args, "pls") != 0)return -1;
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
              << "'remove $path$' - quit console; $path$ can be relative or absolute\n"
              << "'oft [number_of_lines]' - list oft\n"
              << "=================================================================\n";
    return 0;
}

/* ==================== directory traversal ==================== */

int Console::cmdCd(char *args) {
    if(!args)return -1;
    //  if(!strcmp(args, ".."))
    // FileSystem::get().setWDtoParent();
    //  else if(!strcmp(args, ".")) {
    //   continue;
    // } else {
    int ret = FileSystem::get().setWorkingDirectory(args);
    if(ret == -10)std::cout << "Invalid directory name." << std::endl;
    return ret;
}

int Console::cmdLs(char *args) {
    if(args)return -1;
    FileSystem::get().listWorkingDirectory();
    return 0;
}

int Console::cmdTree(char *args) {
    if(!args || strcmp(args, "root") != 0) return -1;
    FileSystem::get().printTree();
    return 0;
}

/* ==================== directory changing ==================== */

int Console::cmdOpen(char *args) {
    if(!args)return -1;
    int ret = FileSystem::get().open(args, FILE_EXT::DIR, 1);
    FileSystem::get().printTree();
    return ret;
}

int Console::cmdClose(char *args) {
    if(!args)return -1;

    return FileSystem::get().close(args, FILE_EXT::DIR);
}

int Console::cmdRemove(char *args) {
    if(!args)return -1;
    return FileSystem::get().remove(args, FILE_EXT::DIR);;
}

/* ==================== other printing ==================== */

int Console::cmdOft(char *args) {
    FileSystem::get().oft.printOFT(args ? atoi(args) : 255);
    return 0;
}



