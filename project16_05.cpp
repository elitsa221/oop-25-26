// project16_05.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include "FileSerializer.h"
#include "CommandDispatcher.h"
#include "Session.h"

static const std::string DATA_FILE = "goodreads_data.txt";
int main()
{
    std::cout << "=== Goodreads Console ===\n";
    std::cout << "Type 'help' for a list of commands.\n\n";
    
    try {
        FileSerializer::load(DATA_FILE);
        std::cout << "[INFO] Data loaded from " << DATA_FILE << "\n";
    }
    catch (const std::exception& e) {
        std::cout << "[INFO] Starting fresh (" << e.what() << ")\n";
    }

    CommandDispatcher dispatcher;
    std::string line;

    while (true) {
        std::cout << "> ";
        if (!std::getline(std::cin, line)) break;
        if (!dispatcher.dispatch(line, DATA_FILE)) break;
    }

    return 0;
}


