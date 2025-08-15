#pragma once
#include <string>
#include <vector>

#include "BlackjackGame.h"

// Stores the result of a strategy calculation
struct StrategyResult {
  std::string playerHand;
  std::string dealerUpcard;
  BlackjackGame::PlayerAction optimalAction;
  double expectedValue;
};

class StrategyGenerator {
 public:
  // Entry point for the strategy generator
  static int run(int argc, char* argv[]);

 private:
  // Generates a strategy based on the given game rules
  static int generate_strategy(const BlackjackGame::GameRules& rules,
                               const std::string& outputFileName);

  // Writes the strategy results to a CSV file
  static int writeToCSV(const std::string& filename,
                        const std::vector<StrategyResult>& results,
                        const BlackjackGame::GameRules& rules);
};