// BlackjackGame.cpp
#include "BlackjackGame.h"

#include <algorithm>
#include <array>
#include <iostream>
#include <map>
#include <numeric>
#include <stdexcept>
#include <string>

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
  } else if (dealerHandScore > 21) {  // Dealer bust
    basePayout = 1.0;
  } else if (isPlayerBlackjack && isDealerBlackjack) {  // Both have blackjack
    basePayout = 0.0;
  } else if (isPlayerBlackjack) {  // Player blackjack
    basePayout = blackjackPayout;
  } else if (isDealerBlackjack) {  // Dealer blackjack
    basePayout = -1.0;
  } else if (playerHandScore < dealerHandScore) {  // Player loses
    basePayout = -1.0;
  } else if (playerHandScore > dealerHandScore) {  // Player wins
    basePayout = 1.0;
  }
  // Push scenario not needed since basePayout initialized to zero
  if (isDoubledDown) {  // If hand was doubled down
    basePayout *= 2.0;
  }

  return basePayout;
}

// Converts the remainingCardCounts map to an array
DeckCounts BlackjackGame::convertMapToDeckCount(
    const std::map<Rank, int>& remainingCardCounts) const {
  DeckCounts arrayCounts = {};

  arrayCounts[0] = remainingCardCounts.at(Rank::Two);
  arrayCounts[1] = remainingCardCounts.at(Rank::Three);
  arrayCounts[2] = remainingCardCounts.at(Rank::Four);
  arrayCounts[3] = remainingCardCounts.at(Rank::Five);
  arrayCounts[4] = remainingCardCounts.at(Rank::Six);
  arrayCounts[5] = remainingCardCounts.at(Rank::Seven);
  arrayCounts[6] = remainingCardCounts.at(Rank::Eight);
  arrayCounts[7] = remainingCardCounts.at(Rank::Nine);
  arrayCounts[8] =
      remainingCardCounts.at(Rank::Ten) + remainingCardCounts.at(Rank::Jack) +
      remainingCardCounts.at(Rank::Queen) + remainingCardCounts.at(Rank::King);
  arrayCounts[9] = remainingCardCounts.at(Rank::Ace);
  return arrayCounts;
}

// Helper method to make a new GameState that has added a card to the dealer
// hand and removed it from the shoe.
GameState BlackjackGame::getGameStateMinusCardToDealer(
    const GameState& oldState, Rank rankToDealer) const {
  GameState newState = oldState;
  // Only for calculation purposes so suit doesn't matter, defaulted to hearts
  newState.dealerHand.addCard(Card(rankToDealer, Suit::Hearts));
  newState.totalCardsRemaining = oldState.totalCardsRemaining - 1;
  newState.remainingCardCounts[rankToDealer]--;
  // The dealer is taking a card, so they have not checked for BJ on this new
  // state.
  newState.dealerChecked = false;
  return newState;
}

// Helper method to make a new GameState that has added a card to the player
// hand and removed it from the shoe.
GameState BlackjackGame::getGameStateMinusCardToPlayer(
    const GameState& oldState, Rank rankToPlayer) const {
  GameState newState = oldState;
  // Only for calculation purposes so suit doesn't matter, defaulted to hearts
  newState.playerHand.addCard(Card(rankToPlayer, Suit::Hearts));
  newState.totalCardsRemaining = oldState.totalCardsRemaining - 1;
  newState.remainingCardCounts[rankToPlayer]--;
  return newState;
}

// Helper method to make a new GameState using one card kept from a split.
GameState BlackjackGame::getGameStateAfterSplit(const GameState& oldState,
                                                Card cardToKeep) const {
  GameState newState = oldState;
  newState.playerHand = Hand();
  newState.playerHand.addCard(cardToKeep);
  newState.wasSplit = true;
  newState.numPlayerHands = oldState.numPlayerHands + 1;
  return newState;
}

double BlackjackGame::getCardDrawProbability(const GameState& state,
                                             Rank cardRank,
                                             bool cardForDealer) const {
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
  if (state.dealerChecked && cardForDealer) {
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
                             SurrenderType surrender, bool splitAces,
                             int maxSplits, double insurancePayout)
    : numDecks(decks),
      dealerHitsSoft17(h17),
      blackjackPayout(bjPayout),
      insurancePayout(insurancePayout),
      canDoubleAfterSplit(das),
      surrenderType(surrender),
      canSplitAces(splitAces),
      maxSplits(maxSplits) {}

void BlackjackGame::clearMemos() const {
  DealerMemo_.clear();
  PlayerMemo_.clear();
}

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
  state.originalNumDecks = numDecks;
  state.dealerChecked = dealerCheckedForBJ;
  return state;
}

double BlackjackGame::calculateEVForHit(const GameState& state) const {
  // If player hand is already 21+, hitting is an invalid action
  if (state.playerHand.getValue() >= 21) {
    return std::nan("");
  }

  double hitEV = 0.0;
  // Iterate through all ranks for the next possible card
  for (const auto& pair : state.remainingCardCounts) {
    if (pair.second > 0) {
      double probDrawCard = getCardDrawProbability(state, pair.first);

      if (probDrawCard == 0.0) {
        continue;
      }

      // Get new GameState for after card is dealt
      GameState newState = getGameStateMinusCardToPlayer(state, pair.first);

      // Add P(drawing this card) * EV of optimal play from this point
      hitEV += probDrawCard * calculateEVForOptimalStrategy(newState).optimalEV;
    }
  }
  return hitEV;
}

double BlackjackGame::calculateEVForStand(const GameState& state) const {
  DealerOutcomeProbabilities outcomeProbs = calcDealerOutcomeProbs(state);

  if (state.playerHand.isBust()) {
    return -1.0;
  }

  if (state.playerHand.isBlackjack()) {
    // Win with blackjack payout unless dealer also has blackjack (push).
    return outcomeProbs.prob_blackjack * 0.0 +
           (1 - outcomeProbs.prob_blackjack) * blackjackPayout;
  }

  double standEV = 0.0;
  int playerScore = state.playerHand.getValue();

  // Sum the EV by weighting the payout of each possible dealer outcome by its
  // probability. The calculatePayout function handles win/loss/push logic.
  standEV +=
      outcomeProbs.prob_17 * calculatePayout(playerScore, 17, false, false);
  standEV +=
      outcomeProbs.prob_18 * calculatePayout(playerScore, 18, false, false);
  standEV +=
      outcomeProbs.prob_19 * calculatePayout(playerScore, 19, false, false);
  standEV +=
      outcomeProbs.prob_20 * calculatePayout(playerScore, 20, false, false);
  standEV +=
      outcomeProbs.prob_21 * calculatePayout(playerScore, 21, false, false);
  standEV += outcomeProbs.prob_blackjack *
             calculatePayout(playerScore, 21, false, true);
  standEV +=
      outcomeProbs.prob_bust * calculatePayout(playerScore, 22, false, false);

  return standEV;
}

double BlackjackGame::calculateEVForSplit(const GameState& state) const {
  if (!state.playerHand.canSplit() || state.numPlayerHands >= maxSplits + 1) {
    return std::nan("");
  }

  if (state.playerHand.getValue() == 12 && state.playerHand.isSoft() &&
      !canSplitAces) {
    return std::nan("");
  }

  GameState singleHandState =
      getGameStateAfterSplit(state, state.playerHand.cards[0]);
  double singleHandEV = 0.0;

  for (const auto& pair : singleHandState.remainingCardCounts) {
    if (pair.second > 0) {
      double probDrawCard = getCardDrawProbability(singleHandState, pair.first);

      if (probDrawCard == 0.0) {
        continue;
      }

      GameState newState =
          getGameStateMinusCardToPlayer(singleHandState, pair.first);

      singleHandEV +=
          probDrawCard * calculateEVForOptimalStrategy(newState).optimalEV;
    }
  }
  return 2 * singleHandEV;
}

double BlackjackGame::calculateEVForDouble(const GameState& state) const {
  if (state.playerHand.cards.size() != 2 || state.playerHand.getValue() == 21) {
    return std::nan("");
  }

  if (state.wasSplit && !canDoubleAfterSplit) {
    return std::nan("");
  }

  double doubleEV = 0.0;
  // Iterate through all ranks for the next possible card
  for (const auto& pair : state.remainingCardCounts) {
    if (pair.second > 0) {
      double probDrawCard = getCardDrawProbability(state, pair.first);

      if (probDrawCard == 0.0) {
        continue;
      }

      // Get new GameState for after card is dealt
      GameState newState = getGameStateMinusCardToPlayer(state, pair.first);

      doubleEV += 2 * probDrawCard * calculateEVForStand(newState);
    }
  }
  return doubleEV;
}

double BlackjackGame::calculateEVForSurrender(const GameState& state) const {
  // Surrender is only allowed on the initial two cards.
  if (state.playerHand.cards.size() != 2) {
    return std::nan("");
  }

  if (surrenderType == SurrenderType::None) {
    return std::nan("");
  }

  bool dealerCanHaveBlackjack = (state.dealerUpcard.getValue() == 10 ||
                                 state.dealerUpcard.rank == Rank::Ace);

  if (dealerCanHaveBlackjack) {
    // For Late Surrender, we must wait for the dealer to check for BJ.
    if (surrenderType == SurrenderType::Late && !state.dealerChecked) {
      return std::nan("");  // Not allowed to surrender yet.
    }
    // For Early Surrender, we must act before the dealer checks for BJ.
    if (surrenderType == SurrenderType::Early && state.dealerChecked) {
      return std::nan("");  // Missed the window to surrender.
    }
  }

  // If we reach here, surrender is a legal move because either:
  // 1. The dealer could not have a blackjack
  // 2. The dealer could have a blackjack, and we are in the correct window of
  //    time to act based on the surrender rule.
  return -0.5;
}

double BlackjackGame::calculateEVForInsurance(const GameState& state) const {
  if (state.dealerUpcard.rank != Rank::Ace || state.dealerChecked) {
    return std::nan("");
  }
  double nextCardTenProb = getCardDrawProbability(state, Rank::Ten) +
                           getCardDrawProbability(state, Rank::Jack) +
                           getCardDrawProbability(state, Rank::Queen) +
                           getCardDrawProbability(state, Rank::King);
  return nextCardTenProb * insurancePayout + (1 - nextCardTenProb) * -1.0;
}

EVResult BlackjackGame::calculateEVForOptimalStrategy(
    const GameState& state) const {
  // If player hand is busted, EV is always -1
  if (state.playerHand.isBust()) {
    return EVResult{-1.0, -1.0, -1.0, -1.0, -1.0, PlayerAction::None, -1.0};
  }
  // Create a unique player key for the player's hand
  PlayerMemoKey playerKey(
      state.playerHand.getValue(), state.playerHand.isSoft(),
      state.playerHand.canSplit(), state.dealerUpcard.getValue(),
      state.wasSplit, state.dealerChecked, state.numPlayerHands,
      convertMapToDeckCount(state.remainingCardCounts));

  // Check if cache contains result
  if (PlayerMemo_.count(playerKey)) {
    return PlayerMemo_[playerKey];
  }

  EVResult result;
  result.standEV = calculateEVForStand(state);
  result.hitEV = calculateEVForHit(state);
  result.doubleEV = calculateEVForDouble(state);
  result.surrenderEV = calculateEVForSurrender(state);
  result.splitEV = calculateEVForSplit(state);

  result.optimalEV = result.standEV;
  result.optimalAction = PlayerAction::Stand;

  if (!std::isnan(result.hitEV) && result.hitEV > result.optimalEV) {
    result.optimalEV = result.hitEV;
    result.optimalAction = PlayerAction::Hit;
  }
  if (!std::isnan(result.doubleEV) && result.doubleEV > result.optimalEV) {
    result.optimalEV = result.doubleEV;
    result.optimalAction = PlayerAction::Double;
  }
  if (!std::isnan(result.splitEV) && result.splitEV > result.optimalEV) {
    result.optimalEV = result.splitEV;
    result.optimalAction = PlayerAction::Split;
  }
  if (!std::isnan(result.surrenderEV) &&
      result.surrenderEV > result.optimalEV) {
    result.optimalEV = result.surrenderEV;
    result.optimalAction = PlayerAction::Surrender;
  }
  PlayerMemo_[playerKey] = result;

  return result;
}

// Calculates the probability of each dealer outcome
DealerOutcomeProbabilities BlackjackGame::calcDealerOutcomeProbs(
    const GameState& state) const {
  // Key used for memo
  DealerMemoKey key(state.dealerHand.getValue(), state.dealerHand.isSoft(),
                    convertMapToDeckCount(state.remainingCardCounts));

  if (DealerMemo_.count(key)) {
    return DealerMemo_[key];
  }

  DealerOutcomeProbabilities outcomes;

  // If dealer busted
  if (state.dealerHand.getValue() > 21) {
    outcomes.prob_bust = 1.0;
    DealerMemo_[key] = outcomes;
    return outcomes;
  }

  // If dealer has hard 17 or soft 17 with dealer standing soft 17s
  if (state.dealerHand.getValue() == 17 &&
      (!state.dealerHand.isSoft() ||
       (state.dealerHand.isSoft() && !dealerHitsSoft17))) {
    outcomes.prob_17 = 1.0;
    DealerMemo_[key] = outcomes;
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
    DealerMemo_[key] = outcomes;
    return outcomes;
  }

  // Iterate through all ranks for the next possible card
  for (const auto& pair : state.remainingCardCounts) {
    if (pair.second > 0) {
      double probDrawCard = getCardDrawProbability(state, pair.first, true);

      if (probDrawCard == 0.0) {
        continue;
      }

      // Get new GameState for after card is dealt
      GameState newState = getGameStateMinusCardToDealer(state, pair.first);

      // Recursively call this method with the new GameState
      DealerOutcomeProbabilities subOutcomes = calcDealerOutcomeProbs(newState);

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
  DealerMemo_[key] = outcomes;
  return outcomes;
}

std::string BlackjackGame::getRuleDescription() const {
  return "";  // TODO
}

std::string BlackjackGame::getDealerOutcomesAsString(const GameState& state) {
  DealerOutcomeProbabilities outcomeProbs = calcDealerOutcomeProbs(state);
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