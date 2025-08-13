#include "EVCalculator.h"

#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "BlackjackGame.h"
#include "BlackjackUtils.h"

// A private helper function to print help specific to this command
static void print_ev_help() {
  std::cout
      << "Mandatory: Input player card ranks and dealer upcard rank.\n"
         "Optional: Input rules or leave blank to use defaults.\n"
      << "Usage: ./bin/BlackjackLab.exe ev-calc [options]\n"
      << "\nOptions:\n"
      << "  --player-cards <cards>    Your starting hand (e.g., '10,10').\n"
      << "  --dealer-upcard <card>    The dealer's visible card (e.g., "
         "'A', '7').\n"
      << "  --dealer-checked <bool>   Has the dealer checked for blackjack? "
         "('true' or 'false', default: true).\n"
      << "  --decks <num>             Number of decks in play (default: "
         "6).\n"
      << "  --s17 <bool>              Does dealer hit on soft 17? ('true' "
         "or 'false', default: true).\n"
      << "  --das <bool>              Can double after split? ('true' or "
         "'false', default: true).\n"
      << "  --surrender <type>        Surrender type ('none', 'late', or "
         "'early', default: late).\n"
      << "  --blackjack-payout <num>  Payout for blackjack (default: "
         "1.5).\n"
      << "  --insurance-payout <num>  Payout for insurance (default: "
         "2.0).\n"
      << "  --can-split-aces <bool>   Can split aces? ('true' or 'false', "
         "default: true).\n"
      << "  --max-splits <num>        Maximum number of splits allowed "
         "(default: 3).\n";
}

int EVCalculator::run(int argc, char* argv[]) {
  if (argc > 2 && argv[2] == std::string("--help")) {
    print_ev_help();
    return 0;
  }

  std::map<std::string, std::string> args;

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

  // Check for required arguments
  if (args.find("player-cards") == args.end() ||
      args.find("dealer-upcard") == args.end()) {
    std::cerr << "Error: Required flags '--player-cards' and '--dealer-upcard' "
                 "are missing."
              << std::endl;
    return 1;
  }

  bool dealerChecked = true;
  if (args.find("dealer-checked") != args.end() &&
      args["dealer-checked"] == "false") {
    dealerChecked = false;
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

  double blackjackPayout = 1.5;
  if (args.find("blackjack-payout") != args.end()) {
    try {
      blackjackPayout = std::stod(args["blackjack-payout"]);
      if (blackjackPayout < 1.0) {
        throw std::out_of_range(
            "Invalid blackjack payout. Must be at least 1.0.");
      }
    } catch (const std::exception& e) {
      std::cerr
          << "Error: Invalid value for '--blackjack-payout'. Must be a number."
          << std::endl;
      return 1;
    }
  }

  double insurancePayout = 2.0;
  if (args.find("insurance-payout") != args.end()) {
    try {
      insurancePayout = std::stod(args["insurance-payout"]);
      if (insurancePayout < 1.0) {
        throw std::out_of_range(
            "Invalid insurance payout. Must be at least 1.0.");
      }
    } catch (const std::exception& e) {
      std::cerr
          << "Error: Invalid value for '--insurance-payout'. Must be a number."
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

  std::string playerCardsStr = args["player-cards"];
  std::string dealerUpcardStr = args["dealer-upcard"];
  std::vector<Card::Rank> playerRanks;
  Card::Rank dealerUpcardRank;

  // Parse player cards
  std::istringstream playerStream(playerCardsStr);
  std::string playerCard;
  while (std::getline(playerStream, playerCard, ',')) {
    playerRanks.push_back(BlackjackUtils::stringToRank(playerCard));
  }

  // Parse dealer upcard
  dealerUpcardRank = BlackjackUtils::stringToRank(dealerUpcardStr);

  BlackjackGame::GameState state = BlackjackGame::getGameStateForCalculation(
      playerRanks, dealerUpcardRank, numDecks, dealerChecked);
  BlackjackGame::GameRules rules{.numDecks = numDecks,
                                 .dealerHitsSoft17 = dealerHitsSoft17,
                                 .canDoubleAfterSplit = canDoubleAfterSplit,
                                 .surrenderType = surrenderType,
                                 .blackjackPayout = blackjackPayout,
                                 .insurancePayout = insurancePayout,
                                 .canSplitAces = canSplitAces,
                                 .maxSplits = maxSplits};
  BlackjackGame game(rules);
  std::cout << "Calculating EV for optimal strategy..." << std::endl;
  BlackjackGame::EVResult result = game.calculateEVForOptimalStrategy(state);

  // Output the results
  std::cout << "Hit EV: " << result.hitEV << std::endl;
  std::cout << "Stand EV: " << result.standEV << std::endl;
  std::cout << "Split EV: " << result.splitEV << std::endl;
  std::cout << "Double EV: " << result.doubleEV << std::endl;
  std::cout << "Surrender EV: " << result.surrenderEV << std::endl;
  std::cout << "\nOptimal Action: "
            << BlackjackUtils::playerActionToString(result.optimalAction)
            << std::endl;
  std::cout << "Optimal EV: " << result.optimalEV << std::endl;

  return 0;
}
