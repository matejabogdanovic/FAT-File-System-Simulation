#include "../h/Console.h"
#include <limits>

const std::map<std::string, Console::FuncPtr> Console::commands_map = std::map<std::string, Console::FuncPtr>{
        {"help",   Console::cmdHelp},

        {"cd",     Console::cmdCd},
        {"ls",     Console::cmdLs},
        {"tree",   Console::cmdTree},

        {"open",   Console::cmdOpen},
        {"write",  Console::cmdWrite},
        {"read",   Console::cmdRead},
        {"eof",    Console::cmdEof},
        {"cursor", Console::cmdCursor},
        {"seek",   Console::cmdSeek},
        {"close",  Console::cmdClose},
        {"rename", Console::cmdRename},
        {"remove", Console::cmdRemove},


        {"oft",    Console::cmdOft},
        {"fat",    Console::cmdFat},
};

int Console::open() {

    int ret = 0;
    std::string cmd;
    char *command = nullptr;
    while(true) {

        std::cout << FileSystem::get().getWorkingDirectoryPath() << ">";
        std::getline(std::cin, cmd);

        command = strtok(cmd.data(), " ");

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
            std::cout << getErrorMessage((ReturnStatus) ret) << std::endl;
        }
    }

    return OK;
}

int Console::cmdHelp(char *args1, char *args2, char *args3) {
    if(!args1 || strcmp(args1, "pls") != 0 || args2 || args3)return -1;
    std::cout << "=================================================================\n"
              << "Help arrived! Check out these sick commands!\n"
              << "$arg_name$ - required arguments\n"
              << "\t- $path$ can be relative or absolute\n"
              << "[arg_name] - optional arguments\n"
              << "=================================================================\n"
              << "'exit' - quit console\n"
              << "-----------------------DIRECTORY TRAVERSAL-----------------------\n"
              << "'cd $path$' - change directory\n"
              << "'ls' - list current directory files\n"
              << "'tree root' - print tree starting from root\n"
              << "-----------------FILE and DIRECTORY MANIPULATION-----------------\n"
              << "'open $path$' - create a file or directory if doesn't exist\n"
              << "\t- if the file exists, it will take an OFT entry\n"
              << "\t- note that if the directory exists and 'open' is called, \n"
              << "\tit will return an error\n"
              << "'write $path$' - write to a file\n"
              << "'read $path$ [num_of_characters]' - read from a file\n"
              << "\t- optionally specify `num_of_characters` to read\n"
              << "\t- if not specified, it will read until the end of the file\n"
              << "\tit behaves the same as 'read $path$ -1'\n"
              << "'eof $path$' - returns a cursor to the end of file\n"
              << "'cursor $path$' - returns a cursor\n"
              << "'seek $path$ $cursor$' - set a cursor\n"
              << "\t- should be used in combination with 'cursor' or 'eof'\n"
              << "'close $path$' - close a file\n"
              << "\t- releases the taken OFT entry\n"
              << "'rename $path$ $name$' - rename file or directory to $name$\n"
              << "'remove $path$' - delete a file or directory\n"
              << "----------------------------PRINTINGS----------------------------\n"
              << "'oft [last_entry_to_print]' - list oft\n"
              << "'fat [last_entry_to_print]' - list fat\n"
              << "=================================================================\n";
    return OK;
}

/* ==================== directory traversal ==================== */

int Console::cmdCd(char *args1, char *args2, char *args3) {
    if(!args1 || args2 || args3)return ERROR_INVALID_ARGS;
    return FileSystem::get().setWorkingDirectory(args1);
}

int Console::cmdLs(char *args1, char *args2, char *args3) {
    if(args1 || args2 || args3)return ERROR_INVALID_ARGS;
    FileSystem::get().listWorkingDirectory();
    return OK;
}

int Console::cmdTree(char *args1, char *args2, char *args3) {
    if(!args1 || strcmp(args1, "root") != 0 || args2 || args3) return ERROR_INVALID_ARGS;
    FileSystem::get().printTree();
    return OK;
}

/* ==================== directory changing ==================== */

int Console::cmdOpen(char *args1, char *args2, char *args3) {
    if(!args1 || args2 || args3)return ERROR_INVALID_ARGS;
    FHANDLE handle;
    int ret = FileSystem::get().open(args1, 1, &handle);
    FileSystem::get().printTree();
    return ret;
}

int Console::cmdWrite(char *args1, char *args2, char *args3) {
    if(!args1 || args2 || args3)return ERROR_INVALID_ARGS;
    FHANDLE handle = FileSystem::get().getFileHandle(args1);
    if(handle < 0) {
        std::cout << getErrorMessage(ERROR_FILE_NOT_OPENED) << std::endl;
        return OK;
    }

    std::cout << "Text to write: ";
    std::string data;
    std::getline(std::cin, data);

    auto ret = FileSystem::get().fwrite(handle, data.length(), data.data());
    std::cout << "Written: " << std::dec << ret << std::endl;

    return ret;
}

int Console::cmdRead(char *args1, char *args2, char *args3) {
    if(!args1 || args3)return ERROR_INVALID_ARGS;
    // if(atoi(args2) <= 0)return -2;
    int count = args2 ? atoi(args2) : -1; // if not given number of characters, read all
    FHANDLE handle = FileSystem::get().getFileHandle(args1);
    if(handle < 0) {
        std::cout << getErrorMessage(ERROR_FILE_NOT_OPENED) << std::endl;
        return OK;
    }
    if(count < 0) {
        uint16_t eof = 0;
        FileSystem::get().feof(handle, &eof);
        count = eof;
    }
    char *data = new char[count + 1]{0};
    auto ret = FileSystem::get().fread(handle, count, data);
    if(ret >= 0) {
        // data[ret] = 0;
        std::cout << "Reading: ";
        std::cout << data << std::endl;
        std::cout << "Read: " << std::dec << ret << std::endl;
    }
    delete[] data;
    return ret;
}

int Console::cmdEof(char *args1, char *args2, char *args3) {
    if(!args1 || args2 || args3)return ERROR_INVALID_ARGS;
    FHANDLE handle = FileSystem::get().getFileHandle(args1);
    if(handle < 0) {
        std::cout << getErrorMessage(ERROR_FILE_NOT_OPENED) << std::endl;
        return OK;
    }
    uint16_t eof = 0;
    auto ret = FileSystem::get().feof(handle, &eof);
    std::cout << "End of file is at: " << std::dec << eof << std::endl;

    return ret;
}

int Console::cmdCursor(char *args1, char *args2, char *args3) {
    if(!args1 || args2 || args3)return ERROR_INVALID_ARGS;
    FHANDLE handle = FileSystem::get().getFileHandle(args1);
    if(handle < 0) {
        std::cout << getErrorMessage(ERROR_FILE_NOT_OPENED) << std::endl;
        return OK;
    }
    uint16_t cursor = 0;
    auto ret = FileSystem::get().fcursor(handle, &cursor);
    std::cout << "Cursor is at: " << std::dec << cursor << std::endl;


    return ret;
}

int Console::cmdSeek(char *args1, char *args2, char *args3) {
    if(!args1 || !args2 || args3)return ERROR_INVALID_ARGS;
    FHANDLE handle = FileSystem::get().getFileHandle(args1);
    if(handle < 0) {
        std::cout << getErrorMessage(ERROR_FILE_NOT_OPENED) << std::endl;
        return OK;
    }

    return FileSystem::get().fseek(handle, (uint16_t) atoi(args2));;
}

int Console::cmdClose(char *args1, char *args2, char *args3) {
    if(!args1 || args2 || args3)return ERROR_INVALID_ARGS;

    return FileSystem::get().close(args1);
}

int Console::cmdRename(char *args1, char *args2, char *args3) {
    if(!args1 || !args2 || args3)return ERROR_INVALID_ARGS;

    return FileSystem::get().rename(args1, args2);
}

int Console::cmdRemove(char *args1, char *args2, char *args3) {
    if(!args1 || args2 || args3)return ERROR_INVALID_ARGS;
    return FileSystem::get().remove(args1);
}

/* ==================== other printing ==================== */

int Console::cmdOft(char *args1, char *args2, char *args3) {
    if(args2 || args3)return ERROR_INVALID_ARGS;
    FileSystem::get().oft.printOFT(args1 ? atoi(args1) : 255);
    return OK;
}

int Console::cmdFat(char *args1, char *args2, char *args3) {
    if(args2 || args3)return ERROR_INVALID_ARGS;
    FAT::printFAT(args1 ? atoi(args1) : 255);
    return OK;
}














