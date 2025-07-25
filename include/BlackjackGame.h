// BlackjackGame.h
#pragma once

#include <map>
#include <numeric>
#include <random>
#include <string>
#include <vector>

#include "Card.h"
#include "Deck.h"
#include "Hand.h"

struct GameState {
  Hand playerHand;
  Card dealerUpcard;
  Hand dealerHand;

  std::map<Rank, int> remainingCardCounts;
  int totalCardsRemaining;
  int originalNumDecks;
  bool dealerChecked;
  bool wasSplit = false;
};

enum class PlayerAction { Hit, Stand, Split, Double, Surrender };

enum class SurrenderType { None, Late, Early };

struct DealerOutcomeProbabilities {
  double prob_17 = 0.0;
  double prob_18 = 0.0;
  double prob_19 = 0.0;
  double prob_20 = 0.0;
  double prob_21 = 0.0;
  double prob_blackjack = 0.0;
  double prob_bust = 0.0;
};

// remainingCardsCounts as array
using DeckCounts = std::array<int, 10>;

// Dealer hand score, isSoft, remaining card counts as array
using DealerMemoKey = std::tuple<int, bool, DeckCounts>;

struct EVResult {
  double hitEV = 0.0;
  double standEV = 0.0;
  double splitEV = 0.0;
  double doubleEV = 0.0;
  double surrenderEV = 0.0;
  PlayerAction optimalAction;
  double optimalEV;
};

class BlackjackGame {
 private:
  int numDecks;
  bool dealerHitsSoft17;
  double blackjackPayout;
  double insurancePayout;
  bool canDoubleAfterSplit;
  SurrenderType surrenderType;
  bool canSplitAces;

  double calculatePayout(int playerHandScore, int dealerHandScore,
                         bool isPlayerBlackjack, bool isDealerBlackjack,
                         bool isDoubledDown = false) const;

  DeckCounts convertMapToDeckCount(
      const std::map<Rank, int>& remainingCardCounts) const;

  GameState getGameStateMinusCardToDealer(const GameState& oldState,
                                          Rank rankToDealer) const;

  GameState getGameStateMinusCardToPlayer(const GameState& oldState,
                                          Rank rankToPlayer) const;

  GameState getGameStateAfterSplit(const GameState& oldState,
                                   Card cardToKeep) const;

  double getCardDrawProbability(const GameState& state, Rank cardRank) const;

 public:
  BlackjackGame(int decks = 6, bool h17 = true, double bjPayout = 1.5,
                bool das = true, SurrenderType surrender = SurrenderType::Late,
                bool splitAces = false);

  GameState getGameState(const Hand& playerHand, const Card& dealerUpcard,
                         const Deck& currentDeck,
                         const bool dealerCheckedForBJ) const;

  double calculateEVForHit(const GameState& state) const;
  double calculateEVForStand(const GameState& state) const;
  double calculateEVForSplit(const GameState& state) const;
  double calculateEVForDouble(const GameState& state) const;
  double calculateEVForSurrender(const GameState& state) const;
  double calculateEVForInsurance(const GameState& state) const;
  double calculateEVForOptimalStrategy(const GameState& state) const;

  DealerOutcomeProbabilities calcDealerOutcomeProbs(
      const GameState& state,
      std::map<DealerMemoKey, DealerOutcomeProbabilities>& memo) const;

  std::string getRuleDescription() const;

  std::string getDealerOutcomesAsString(const GameState& state);
};