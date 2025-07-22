// BlackjackGame.cpp
#include "BlackjackGame.h"

#include <algorithm>
#include <array>
#include <iostream>
#include <numeric>
#include <stdexcept>

#include "Card.h"
#include "Deck.h"
#include "Hand.h"

double BlackjackGame::calculatePayout(int playerHandScore, int dealerHandScore,
                                      bool isPlayerBlackjack,
                                      bool isDealerBlackjack,
                                      bool isDoubledDown) const {
  double basePayout = 0.0;  // Default

  if (playerHandScore > 21) {  // Player bust
    basePayout = -1.0;
  }

  else if (dealerHandScore > 21) {  // Dealer bust
    basePayout = 1.0;
  }

  else if (isPlayerBlackjack && isDealerBlackjack) {  // Both have blackjack
    basePayout = 0.0;
  }

  else if (isPlayerBlackjack) {  // Player blackjack
    basePayout = blackjackPayout;
  }

  else if (isDealerBlackjack) {  // Dealer blackjack
    basePayout = -1.0;
  }

  else if (playerHandScore < dealerHandScore) {  // Player loses
    basePayout = -1.0;
  }

  else if (playerHandScore > dealerHandScore) {  // Player wins
    basePayout = 1.0;
  }

  // Push scenario not needed since basePayout initialized to zero

  if (isDoubledDown) {  // If hand was doubled down
    return basePayout * 2.0;
  }
  return basePayout;
}

// Converts the remainingCardCounts map to an array
DeckCounts BlackjackGame::convertMapToDeckCount(
    const std::map<Rank, int>& remainingCardCounts) {
  DeckCounts arrayCounts = {};

  arrayCounts[0] = remainingCardCounts.at(Rank::Two);
  arrayCounts[1] = remainingCardCounts.at(Rank::Three);
  arrayCounts[2] = remainingCardCounts.at(Rank::Four);
  arrayCounts[3] = remainingCardCounts.at(Rank::Five);
  arrayCounts[4] = remainingCardCounts.at(Rank::Six);
  arrayCounts[5] = remainingCardCounts.at(Rank::Seven);
  arrayCounts[6] = remainingCardCounts.at(Rank::Eight);
  arrayCounts[7] = remainingCardCounts.at(Rank::Nine);
  arrayCounts[8] = remainingCardCounts.at(Rank::Ten);
  arrayCounts[9] = remainingCardCounts.at(Rank::Ace);
  return arrayCounts;
}

// Helper method to make a new GameState that has added a card to the dealer
// hand and removed it from the shoe Since this is for calculation purposes,
// only the rank of the card matters and the suit is defaulted to hearts
GameState BlackjackGame::getGameStateMinusCardToDealer(
    const GameState& oldState, Rank rankToDealer) const {
  GameState newState;
  newState.playerHand = oldState.playerHand;
  newState.dealerHand = oldState.dealerHand;
  newState.dealerHand.addCard(Card(rankToDealer, Suit::Hearts));
  newState.totalCardsRemaining = oldState.totalCardsRemaining - 1;
  newState.remainingCardCounts = oldState.remainingCardCounts;
  // Decrement the count for the rank of the card we just removed from the shoe
  newState.remainingCardCounts[rankToDealer]--;
  // This method only runs once the dealer has atleast two cards--they will
  // never check their next card
  newState.dealerChecked = false;
  return newState;
}

// Helper method to make a new GameState that has added a card to the player
// hand and removed it from the shoe Since this is for calculation purposes,
// only the rank of the card matters and the suit is defaulted to hearts
GameState BlackjackGame::getGameStateMinusCardToPlayer(
    const GameState& oldState, Rank rankToPlayer) const {
  GameState newState;
  newState.dealerHand = oldState.dealerHand;
  newState.playerHand = oldState.playerHand;
  newState.playerHand.addCard(Card(rankToPlayer, Suit::Hearts));
  newState.totalCardsRemaining = oldState.totalCardsRemaining - 1;
  newState.remainingCardCounts = oldState.remainingCardCounts;
  // Decrement the count for the rank of the card we just removed from the shoe
  newState.remainingCardCounts[rankToPlayer]--;
  newState.dealerChecked = oldState.dealerChecked;
  return newState;
}

double BlackjackGame::getCardDrawProbability(const GameState& state,
                                             Rank cardRank) const {
  if (state.totalCardsRemaining <= 0) {
    return 0.0;
  }

  if (state.remainingCardCounts.count(cardRank) == 0 ||
      state.remainingCardCounts.at(cardRank) == 0) {
    return 0.0;
  }

  double countOfRank = state.remainingCardCounts.at(cardRank);
  double totalCards = state.totalCardsRemaining;

  // If dealer checked for blackjack and doesn't have it, we can adjust
  // probabilities based on the hole card not completing a blackjack.
  if (state.dealerChecked) {
    // If dealer upcard is a 10-value card, the hole card cannot be an Ace.
    if (state.dealerUpcard.getValue() == 10) {
      if (cardRank == Rank::Ace) {
        return 0.0;
      }
      totalCards -= state.remainingCardCounts.at(Rank::Ace);
    }
    // If dealer upcard is an Ace, the hole card cannot be a 10-value card.
    else if (state.dealerUpcard.rank == Rank::Ace) {
      if (Card(cardRank, Suit::Hearts).getValue() == 10) {
        return 0.0;
      }
      totalCards -= (state.remainingCardCounts.at(Rank::Ten) +
                     state.remainingCardCounts.at(Rank::Jack) +
                     state.remainingCardCounts.at(Rank::Queen) +
                     state.remainingCardCounts.at(Rank::King));
    }
  }

  if (totalCards <= 0) return 0.0;

  return countOfRank / totalCards;
}

BlackjackGame::BlackjackGame(int decks, bool h17, double bjPayout, bool das,
                             bool surrender, bool splitAces)
    : numDecks(decks),
      dealerHitsSoft17(h17),
      blackjackPayout(bjPayout),
      canDoubleAfterSplit(das),
      canSurrender(surrender),
      canSplitAces(splitAces) {}

GameState BlackjackGame::getGameState(const Hand& playerHand,
                                      const Card& dealerUpcard,
                                      const Deck& currentDeck,
                                      const bool dealerCheckedForBJ) const {
  GameState state;
  state.playerHand = playerHand;
  state.dealerUpcard = dealerUpcard;
  state.dealerHand = Hand();
  state.dealerHand.addCard(dealerUpcard);
  state.remainingCardCounts = currentDeck.getRemainingCardCounts();
  state.totalCardsRemaining = currentDeck.getRemainingCardsCount();
  state.dealerChecked = dealerCheckedForBJ;
  return state;
}

double BlackjackGame::calculateEVForHit(const GameState& state) const {}

double BlackjackGame::calculateEVForStand(const GameState& state) {
  std::map<DealerMemoKey, DealerOutcomeProbabilities> dealerMemoTable;
  DealerOutcomeProbabilities outcomeProbs =
      calcDealerOutcomeProbs(state, dealerMemoTable);

  if (state.playerHand.getValue() > 21) {
    return -1;
  }

  if (state.playerHand.isBlackjack()) {
    double probPush = outcomeProbs.prob_blackjack;
    return probPush * 0 + (1 - probPush) * 1.5;
  }

  else if (state.playerHand.getValue() == 21) {
    return outcomeProbs.prob_blackjack * -1 + outcomeProbs.prob_21 * 0 +
           (1 - outcomeProbs.prob_21 - outcomeProbs.prob_blackjack) * 1;
  }

  else if (state.playerHand.getValue() == 20) {
    double probLose = outcomeProbs.prob_blackjack + outcomeProbs.prob_21;
    double probPush = outcomeProbs.prob_20;
    return probLose * -1 + probPush * 0 + (1 - probLose - probPush) * 1;
  }

  else if (state.playerHand.getValue() == 19) {
    double probLose = outcomeProbs.prob_blackjack + outcomeProbs.prob_21 +
                      outcomeProbs.prob_20;
    double probPush = outcomeProbs.prob_19;
    return probLose * -1 + probPush * 0 + (1 - probLose - probPush) * 1;
  }

  else if (state.playerHand.getValue() == 18) {
    double probLose = outcomeProbs.prob_blackjack + outcomeProbs.prob_21 +
                      outcomeProbs.prob_20 + outcomeProbs.prob_19;
    double probPush = outcomeProbs.prob_18;
    return probLose * -1 + probPush * 0 + (1 - probLose - probPush) * 1;
  }

  else if (state.playerHand.getValue() == 17) {
    double probLose = outcomeProbs.prob_blackjack + outcomeProbs.prob_21 +
                      outcomeProbs.prob_20 + outcomeProbs.prob_19 +
                      outcomeProbs.prob_18;
    double probPush = outcomeProbs.prob_17;
    return probLose * -1 + probPush * 0 + (1 - probLose - probPush) * 1;
  }

  else {
    return outcomeProbs.prob_bust + (1 - outcomeProbs.prob_bust) * -1;
  }
}

double BlackjackGame::calculateEVForSplit(const GameState& state) const {
  if (!state.playerHand.canSplit()) {
    return std::nan("");
  }
}

double BlackjackGame::calculateEVForDouble(const GameState& state) const {
  if (state.playerHand.isBlackjack()) {
    return std::nan("");
  }
}

double BlackjackGame::calculateEVForSurrender(const GameState& state) const {
  return canSurrender ? -0.5 : std::nan("");
}

// NOTE: Add insurance EV calc

// NOTE: Add optimal strategy EV calc

// Calculates the probability of each dealer outcome
DealerOutcomeProbabilities BlackjackGame::calcDealerOutcomeProbs(
    const GameState& state,
    std::map<DealerMemoKey, DealerOutcomeProbabilities>& memo) {
  // Key used for memo
  DealerMemoKey key(state.dealerHand.getValue(), state.dealerHand.isSoft(),
                    convertMapToDeckCount(state.remainingCardCounts));

  if (memo.count(key)) {
    return memo[key];
  }

  DealerOutcomeProbabilities outcomes;

  // If dealer busted
  if (state.dealerHand.getValue() > 21) {
    outcomes.prob_bust = 1.0;
    memo[key] = outcomes;
    return outcomes;
  }

  // If dealer has hard 17 or soft 17 with dealer standing soft 17s
  if (state.dealerHand.getValue() == 17 &&
      (!state.dealerHand.isSoft() ||
       (state.dealerHand.isSoft() && !dealerHitsSoft17))) {
    outcomes.prob_17 = 1.0;
    memo[key] = outcomes;
    return outcomes;
  }
  // If dealer has 18-21
  else if (state.dealerHand.getValue() >= 18) {
    if (state.dealerHand.getValue() == 18) {
      outcomes.prob_18 = 1.0;
    } else if (state.dealerHand.getValue() == 19) {
      outcomes.prob_19 = 1.0;
    } else if (state.dealerHand.getValue() == 20) {
      outcomes.prob_20 = 1.0;
    } else if (state.dealerHand.getValue() == 21) {
      if (state.dealerHand.isBlackjack()) {
        outcomes.prob_blackjack = 1.0;
      } else {
        outcomes.prob_21 = 1.0;
      }
    }
    memo[key] = outcomes;
    return outcomes;
  }

  // Iterate through all ranks for the next possible card
  for (const auto& pair : state.remainingCardCounts) {
    if (pair.second > 0) {
      double probDrawCard = getCardDrawProbability(state, pair.first);

      if (probDrawCard == 0.0) {
        continue;
      }

      // Get new GameState for after card is dealt
      GameState newState = getGameStateMinusCardToDealer(state, pair.first);

      // Recursively call this method with the new GameState
      DealerOutcomeProbabilities subOutcomes =
          calcDealerOutcomeProbs(newState, memo);

      outcomes.prob_17 += probDrawCard * subOutcomes.prob_17;
      outcomes.prob_18 += probDrawCard * subOutcomes.prob_18;
      outcomes.prob_19 += probDrawCard * subOutcomes.prob_19;
      outcomes.prob_20 += probDrawCard * subOutcomes.prob_20;
      outcomes.prob_21 += probDrawCard * subOutcomes.prob_21;
      outcomes.prob_bust += probDrawCard * subOutcomes.prob_bust;
      outcomes.prob_blackjack += probDrawCard * subOutcomes.prob_blackjack;
    }
  }
  // Add situation to memo and return outcomes
  memo[key] = outcomes;
  return outcomes;
}

std::string BlackjackGame::getRuleDescription() const {
  return "";  // TODO
}

std::string BlackjackGame::getDealerOutcomesAsString(const GameState& state) {
  std::map<DealerMemoKey, DealerOutcomeProbabilities> dealerMemoTable;
  DealerOutcomeProbabilities outcomeProbs =
      calcDealerOutcomeProbs(state, dealerMemoTable);
  std::string str;
  str += "17: " + std::to_string(outcomeProbs.prob_17) + "\n";
  str += "18: " + std::to_string(outcomeProbs.prob_18) + "\n";
  str += "19: " + std::to_string(outcomeProbs.prob_19) + "\n";
  str += "20: " + std::to_string(outcomeProbs.prob_20) + "\n";
  str += "21: " + std::to_string(outcomeProbs.prob_21) + "\n";
  str += "Blackjack: " + std::to_string(outcomeProbs.prob_blackjack) + "\n";
  str += "Bust: " + std::to_string(outcomeProbs.prob_bust) + "\n";
  return str;
}