#include "../h/Console.h"
#include <iostream>

// todo optimize with map?
int Console::open() {

    std::string command;
    while(true) {
        std::cout << FileSystem::get().getWorkingDirectoryName() << ">";

        std::getline(std::cin, command);
        if(command == "exit")
            break;
        else if(command == "cd ..") {
            FileSystem::get().setWDtoParent();
        } else if(command == "ls") {
            FileSystem::get().listWDFiles();
        } else if(command == "tree -root") {
            FileSystem::get().printTree();
        } else if(command == "tree") {
            //FileSystem::get().printTree(true);
        } else if(command == "help pls") {
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
