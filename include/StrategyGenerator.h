#pragma once
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <tuple>
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
  static int generateStrategy(const BlackjackGame::GameRules& rules,
                              const std::string& outputFileName,
                              int threadCount);

  // Calculates the optimal strategy for a chunk of hands
  static void calculateChunk(
      const BlackjackGame::GameRules& rules,
      std::queue<std::tuple<std::string, std::string, int>>& workQueue,
      std::vector<StrategyResult>& results, std::mutex& workQueueMutex,
      std::atomic<int>& tasksCompleted);

  // Writes the strategy results to a CSV file
  static int writeToCSV(const std::string& filename,
                        const std::vector<StrategyResult>& results,
                        const BlackjackGame::GameRules& rules);
};