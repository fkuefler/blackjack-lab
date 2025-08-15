#include "StrategyGenerator.h"

#include <fstream>
#include <iostream>
#include <map>
#include <numeric>
#include <sstream>

#include "BlackjackUtils.h"

// Helper function to print strategy usage information
static void print_strategy_help() {
  std::cout
      << "Usage: ./bin/BlackjackLab.exe strategy [options]\n"
      << "Provide output file name or leave blank to use defaults.\n"
      << "Include specific game rules or leave blank to use defaults.\n"
      << "\nOptions:\n"
      << "  --output <filename>       Output CSV file name (default: "
         "strategy.csv).\n"
      << "  --decks <num>             Number of decks in play (default: "
         "6).\n"
      << "  --s17 <bool>              Does dealer hit on soft 17? ('true' "
         "or 'false', default: true).\n"
      << "  --das <bool>              Can double after split? ('true' or "
         "'false', default: true).\n"
      << "  --surrender <type>        Surrender type ('none', 'late', or "
         "'early', default: late).\n"
      << "  --can-split-aces <bool>   Can split aces? ('true' or 'false', "
         "default: true).\n"
      << "  --max-splits <num>        Maximum number of splits allowed "
         "(default: 3; use 0 for no splitting allowed).\n";
}

int StrategyGenerator::run(int argc, char* argv[]) {
  // Print help message if requested
  if (argc > 2 && argv[2] == std::string("--help")) {
    print_strategy_help();
    return 0;
  }

  std::map<std::string, std::string> args;

  // Parse command-line arguments
  for (int i = 2; i < argc; ++i) {
    std::string arg = argv[i];
    if (arg.rfind("--", 0) == 0) {
      if (i + 1 < argc) {
        std::string key = arg.substr(2);
        std::string value = argv[++i];
        args[key] = value;
      } else {
        std::cerr << "Error: Missing value for argument " << arg << "\n";
        return 1;
      }
    }
  }

  int numDecks = 6;
  if (args.find("decks") != args.end()) {
    try {
      numDecks = std::stoi(args["decks"]);
      if (numDecks < 1 || numDecks > 8) {
        throw std::out_of_range("Invalid deck count. Must be between 1 and 8.");
      }
    } catch (const std::exception& e) {
      std::cerr
          << "Error: Invalid value for '--decks'. Must be an integer (1-8)."
          << std::endl;
      return 1;
    }
  }

  bool dealerHitsSoft17 = true;
  if (args.find("s17") != args.end() && args["s17"] == "false") {
    dealerHitsSoft17 = false;
  }

  bool canDoubleAfterSplit = true;
  if (args.find("das") != args.end() && args["das"] == "false") {
    canDoubleAfterSplit = false;
  }

  BlackjackGame::SurrenderType surrenderType =
      BlackjackGame::SurrenderType::Late;
  if (args.find("surrender") != args.end()) {
    const std::string& type = args["surrender"];
    if (type == "none") {
      surrenderType = BlackjackGame::SurrenderType::None;
    } else if (type == "late") {
      surrenderType = BlackjackGame::SurrenderType::Late;
    } else if (type == "early") {
      surrenderType = BlackjackGame::SurrenderType::Early;
    } else {
      std::cerr << "Error: Invalid value for '--surrender'. Must be "
                   "'none', 'late', or 'early'."
                << std::endl;
      return 1;
    }
  }

  bool canSplitAces = true;
  if (args.find("can-split-aces") != args.end() &&
      args["can-split-aces"] == "false") {
    canSplitAces = false;
  }

  int maxSplits = 3;
  if (args.find("max-splits") != args.end()) {
    try {
      maxSplits = std::stoi(args["max-splits"]);
      if (maxSplits < 0 || maxSplits > 3) {
        throw std::out_of_range(
            "Invalid max splits. Must be between 0 (splitting not allowed) and "
            "3.");
      }
    } catch (const std::exception& e) {
      std::cerr << "Error: Invalid value for '--max-splits'. Must be an "
                   "integer (0-3)."
                << std::endl;
      return 1;
    }
  }

  BlackjackGame::GameRules rules{.numDecks = numDecks,
                                 .dealerHitsSoft17 = dealerHitsSoft17,
                                 .canDoubleAfterSplit = canDoubleAfterSplit,
                                 .surrenderType = surrenderType,
                                 .canSplitAces = canSplitAces,
                                 .maxSplits = maxSplits};

  // Get output file name or use default
  std::string outputFileName = "strategy.csv";
  if (args.find("output") != args.end()) {
    outputFileName = args["output"];
  }

  return generate_strategy(rules, outputFileName);
}

int StrategyGenerator::generate_strategy(const BlackjackGame::GameRules& rules,
                                         const std::string& outputFileName) {
  std::cout << "Generating strategy chart...\n";

  std::vector<StrategyResult> allResults;

  // Generate all possible player hands (hard totals, soft totals, pairs)
  std::vector<std::string> playerHands;
  // Hard totals 5-12
  for (int i = 3; i <= 10; ++i) {
    playerHands.push_back(std::to_string(i) + ",2");
  }

  // Hard totals 13-19
  for (int i = 3; i <= 9; ++i) {
    playerHands.push_back(std::to_string(i) + ",10");
  }

  // Soft totals
  for (int i = 2; i <= 9; ++i) {
    playerHands.push_back("A," + std::to_string(i));
  }

  // Pairs
  for (int i = 2; i <= 10; ++i) {
    playerHands.push_back(std::to_string(i) + "," + std::to_string(i));
  }
  playerHands.push_back("A,A");

  std::vector<std::string> dealerUpcards = {"2", "3", "4", "5",  "6",
                                            "7", "8", "9", "10", "A"};

  BlackjackGame game(rules);

  // Iterate through all player hand vs dealer upcard matchups
  for (const auto& playerHand : playerHands) {
    for (const auto& dealerUpcard : dealerUpcards) {
      // Get the player hand cards
      std::stringstream ss(playerHand);
      std::string firstCardStr, secondCardStr;
      std::getline(ss, firstCardStr, ',');
      std::getline(ss, secondCardStr);

      std::vector<Card::Rank> playerRanks = {
          BlackjackUtils::stringToRank(firstCardStr),
          BlackjackUtils::stringToRank(secondCardStr)};
      Card::Rank dealerUpcardRank = BlackjackUtils::stringToRank(dealerUpcard);
      BlackjackGame::GameState state =
          BlackjackGame::getGameStateForCalculation(
              playerRanks, dealerUpcardRank, rules.numDecks, true);
      BlackjackGame::EVResult evResult =
          game.calculateEVForOptimalStrategy(state);
      // Convert hard totals to single number if necessary
      std::string playerHandTotalString;
      if (firstCardStr != secondCardStr && firstCardStr != "A" &&
          secondCardStr != "A") {
        playerHandTotalString =
            std::to_string(BlackjackUtils::stringToValue(firstCardStr) +
                           BlackjackUtils::stringToValue(secondCardStr));
      } else {
        playerHandTotalString = playerHand;
      }
      // Create a StrategyResult for this matchup
      StrategyResult result = {.playerHand = playerHandTotalString,
                               .dealerUpcard = dealerUpcard,
                               .optimalAction = evResult.optimalAction,
                               .expectedValue = evResult.optimalEV};
      allResults.push_back(result);
    }
    std::cout << "Finished " << playerHand << std::endl;
  }
  // Write the results to a CSV file
  return writeToCSV(outputFileName, allResults, rules);
}

int StrategyGenerator::writeToCSV(const std::string& filename,
                                  const std::vector<StrategyResult>& results,
                                  const BlackjackGame::GameRules& rules) {
  std::ofstream file(filename);
  if (!file.is_open()) {
    std::cerr << "Error: Could not open file " << filename << " for writing."
              << std::endl;
    return 1;
  }

  // Add rules used for generation to top of file
  file << "#Rules Used for Generation:\n";
  file << "#Number of Decks: " << rules.numDecks << "\n";
  file << "#Dealer Hits Soft 17: " << (rules.dealerHitsSoft17 ? "Yes" : "No")
       << "\n";
  file << "#Can Double After Split: "
       << (rules.canDoubleAfterSplit ? "Yes" : "No") << "\n";
  file << "#Surrender Type: "
       << BlackjackUtils::surrenderTypeToString(rules.surrenderType) << "\n";
  file << "#Can Split Aces: " << (rules.canSplitAces ? "Yes" : "No") << "\n";
  file << "#Max Splits: " << rules.maxSplits << "\n";

  file << "Player Hand,Dealer Upcard,Optimal Action,Expected Value\n";
  for (const auto& result : results) {
    file << "\"" << result.playerHand << "\"" << "," << result.dealerUpcard
         << "," << BlackjackUtils::playerActionToString(result.optimalAction)
         << "," << result.expectedValue << "\n";
  }
  file.close();
  std::cout << "Strategy chart written to " << filename << std::endl;
  return 0;
}
