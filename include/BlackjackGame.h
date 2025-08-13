// BlackjackGame.h
#pragma once

#include <array>
#include <map>
#include <numeric>
#include <random>
#include <string>
#include <vector>

#include "Card.h"
#include "Deck.h"
#include "Hand.h"

class BlackjackGame {
 public:
  enum class PlayerAction { Hit, Stand, Split, Double, Surrender, None };
  enum class SurrenderType { Early, Late, None };

  // Stores the state of the game
  struct GameState {
    Hand playerHand;
    Card dealerUpcard;
    Hand dealerHand;

    std::map<Card::Rank, int> remainingCardCounts;
    int totalCardsRemaining;
    int originalNumDecks;
    bool dealerChecked = true;
    bool wasSplit = false;
    int numPlayerHands = 1;
  };

  // Stores the rules of the game
  struct GameRules {
    int numDecks = 6;
    bool dealerHitsSoft17 = true;
    bool canDoubleAfterSplit = true;
    SurrenderType surrenderType = SurrenderType::Late;
    double blackjackPayout = 1.5;
    double insurancePayout = 2.0;
    bool canSplitAces = true;
    int maxSplits = 3;
  };

  // Stores the probabilities of dealer drawing to specific totals
  struct DealerOutcomeProbabilities {
    double prob_17 = 0.0;
    double prob_18 = 0.0;
    double prob_19 = 0.0;
    double prob_20 = 0.0;
    double prob_21 = 0.0;
    double prob_blackjack = 0.0;
    double prob_bust = 0.0;
  };

  // Stores the expected value results for each player action
  struct EVResult {
    double hitEV = 0.0;
    double standEV = 0.0;
    double splitEV = 0.0;
    double doubleEV = 0.0;
    double surrenderEV = 0.0;
    PlayerAction optimalAction;
    double optimalEV = 0.0;
  };

  // Constructor for a BlackjackGame instance with custom rules (defaults to
  // standard rules)
  BlackjackGame(const GameRules& rules);

  // Clears the memoization caches
  void clearMemos() const;

  // Gets the a GameState object representing the current game state
  static GameState getGameStateForCalculation(
      const std::vector<Card::Rank>& player_ranks,
      const Card::Rank& dealer_upcard, const int num_decks,
      const bool dealerCheckedForBJ);

  // Calculates the expected value for hitting
  double calculateEVForHit(const GameState& state) const;
  // Calculates the expected value for standing
  double calculateEVForStand(const GameState& state) const;
  // Calculates the expected value for splitting
  double calculateEVForSplit(const GameState& state) const;
  // Calculates the expected value for doubling down
  double calculateEVForDouble(const GameState& state) const;
  // Calculates the expected value for surrendering
  double calculateEVForSurrender(const GameState& state) const;
  // Calculates the expected value for insurance
  double calculateEVForInsurance(const GameState& state) const;
  // Calculates the expected value for all player actions and returns an
  // EVResult struct containing the optimal action and its EV
  EVResult calculateEVForOptimalStrategy(const GameState& state) const;

  // Calculates the probabilities of dealer outcomes based on the current game
  DealerOutcomeProbabilities calcDealerOutcomeProbs(
      const GameState& state) const;

  // Returns a string representation of dealer outcomes
  std::string getDealerOutcomesAsString(const GameState& state);

 private:
  int numDecks;
  bool dealerHitsSoft17;
  double blackjackPayout;
  double insurancePayout;
  bool canDoubleAfterSplit;
  SurrenderType surrenderType;
  bool canSplitAces;
  int maxSplits;

  // remainingCardsCounts as array
  using DeckCounts = std::array<int, 10>;

  // Dealer hand score, isSoft, remaining card counts
  using DealerMemoKey = std::tuple<int, bool, DeckCounts>;
  using DealerMemo = std::map<DealerMemoKey, DealerOutcomeProbabilities>;

  // Player hand value, isSoft, canSplit, dealer upcard value, wasSplit,
  // dealerChecked, numPlayerHands, remaining card counts
  using PlayerMemoKey =
      std::tuple<int, bool, bool, int, bool, bool, int, DeckCounts>;
  using PlayerMemo = std::map<PlayerMemoKey, EVResult>;

  mutable DealerMemo DealerMemo_;
  mutable PlayerMemo PlayerMemo_;

  // Helper function to calculate the payout based on player and dealer scores
  double calculatePayout(int playerHandScore, int dealerHandScore,
                         bool isPlayerBlackjack, bool isDealerBlackjack,
                         bool isDoubledDown = false) const;

  // Helper function to convert a map of card counts to DeckCounts array
  DeckCounts convertMapToDeckCount(
      const std::map<Card::Rank, int>& remainingCardCounts) const;

  // Helper function to get a new GameState with a card dealt to the dealer
  GameState getGameStateMinusCardToDealer(const GameState& oldState,
                                          Card::Rank rankToDealer) const;

  // Helper function to get a new GameState with a card dealt to the player
  GameState getGameStateMinusCardToPlayer(const GameState& oldState,
                                          Card::Rank rankToPlayer) const;

  // Helper function to get a new GameState after a split
  GameState getGameStateAfterSplit(const GameState& oldState,
                                   Card cardToKeep) const;

  // Helper function to get the possibility of drawing a specific card rank
  double getCardDrawProbability(const GameState& state, Card::Rank cardRank,
                                bool cardForDealer = false) const;
};