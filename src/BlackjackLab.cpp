// Blackjacklab.cpp : This file contains the 'main' function. Program
// execution begins and ends there.

#include <iostream>
#include <string>

#include "EVCalculator.h"
#include "StrategyGenerator.h"

void print_main_help() {
  std::cout
      << "Usage: ./bin/BlackjackLab.exe [command] [options]\n"
      << "\nCommands:\n"
      << "  --ev-calc         Calculates the expected value of each action for "
         "a specific hand.\n"
      << "  --strategy        Generates a basic or customized strategy chart.\n"
      << "  --help            Displays this help message.\n"
      << "  Type a command followed by --help for details on how to use that "
         "command.\n";
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    print_main_help();
    return 0;
  }

  std::string command = argv[1];

  if (command == "--help") {
    print_main_help();
    return 0;
  } else if (command == "--ev-calc") {
    int result = EVCalculator::run(argc, argv);
    return result;
  } else {
    std::cerr << "Unknown command: " << command << "\n";
    print_main_help();
    return 1;
  }
}
