#include "../h/Console.h"
#include <iostream>
#include <cstring>

// todo optimize with map?
int Console::open() {


    while(true) {
        std::cout << FileSystem::get().getWorkingDirectoryName() << ">";

        char cmd[PATHNAME_SZ + 10];
        std::cin.getline(cmd, PATHNAME_SZ + 10);
        char *command = strtok(cmd, " ");
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
        else if(!strcmp(command, "oft")) {
            FileSystem::get().oft.printOFT(args ? atoi(args) : 255);
        } else if(!args && !strcmp(command, "reset")) {
            // FileSystem::reset();
        } else if(args && !strcmp(command, "remove")) {
            int ret = FileSystem::get().remove(args, FILE_EXT::DIR);
            if(ret < 0)std::cout << "Error: " << std::dec << ret << std::endl;
        } else if(args && !strcmp(command, "open")) {
            int ret = FileSystem::get().open(args, FILE_EXT::DIR, 1);
            FileSystem::get().printTree();
            if(ret < 0)std::cout << "Error: " << std::dec << ret << std::endl;
        } else if(args && !strcmp(command, "close")) {
            int ret = FileSystem::get().close(args, FILE_EXT::DIR);
            if(ret < 0)std::cout << "Error: " << std::dec << ret << std::endl;
        } else if(args && !strcmp(command, "cd")) {
            //  if(!strcmp(args, ".."))
            // FileSystem::get().setWDtoParent();
            //  else if(!strcmp(args, ".")) {
            //   continue;
            // } else {
            int ret = FileSystem::get().setWDto(args);
            if(ret == -10)std::cout << "Invalid directory name." << std::endl;
            else if(ret < 0)std::cout << "Error: " << std::dec << ret << std::endl;
            //}
        } else if(!args && !strcmp(command, "ls")) {
            FileSystem::get().listWDFiles();
        } else if(args && !strcmp(command, "tree") && !strcmp(args, "-root")) {
            FileSystem::get().printTree();
        } else if(!args && !strcmp(command, "tree")) {
            //FileSystem::get().printTree(true);
        } else if(args && !strcmp(command, "help") && !strcmp(args, "pls")) {
            printHelp();
        } else {
            std::cout << "Unknown command. Please, type 'help pls' for help.\n";
        }
    }

    return 0;
}

void Console::printHelp() {
    std::cout << "No problem. Check out these sick commands!\n"
              << "'exit' - quit console\n"
              << "'tree -root' - list tree\n";
}
