// BlackjackGame.cpp
#include "BlackjackGame.h"

#include <algorithm>
#include <array>
#include <iostream>
#include <map>
#include <numeric>
#include <stdexcept>
#include <string>

#include "BlackjackUtils.h"
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

BlackjackGame::DeckCounts BlackjackGame::convertMapToDeckCount(
    const std::map<Card::Rank, int>& remainingCardCounts) const {
  DeckCounts arrayCounts = {};
  arrayCounts[0] = remainingCardCounts.at(Card::Rank::Two);
  arrayCounts[1] = remainingCardCounts.at(Card::Rank::Three);
  arrayCounts[2] = remainingCardCounts.at(Card::Rank::Four);
  arrayCounts[3] = remainingCardCounts.at(Card::Rank::Five);
  arrayCounts[4] = remainingCardCounts.at(Card::Rank::Six);
  arrayCounts[5] = remainingCardCounts.at(Card::Rank::Seven);
  arrayCounts[6] = remainingCardCounts.at(Card::Rank::Eight);
  arrayCounts[7] = remainingCardCounts.at(Card::Rank::Nine);
  arrayCounts[8] = remainingCardCounts.at(Card::Rank::Ten) +
                   remainingCardCounts.at(Card::Rank::Jack) +
                   remainingCardCounts.at(Card::Rank::Queen) +
                   remainingCardCounts.at(Card::Rank::King);
  arrayCounts[9] = remainingCardCounts.at(Card::Rank::Ace);
  return arrayCounts;
}

BlackjackGame::GameState BlackjackGame::getGameStateMinusCardToDealer(
    const GameState& oldState, Card::Rank rankToDealer) const {
  GameState newState = oldState;
  // Only for calculation purposes so suit doesn't matter, defaulted to hearts
  newState.dealerHand.addCard(Card(rankToDealer, Card::Suit::Hearts));
  newState.totalCardsRemaining = oldState.totalCardsRemaining - 1;
  newState.remainingCardCounts[rankToDealer]--;
  // The dealer is taking a card, so they have not checked for BJ on this new
  // state.
  newState.dealerChecked = false;
  return newState;
}

BlackjackGame::GameState BlackjackGame::getGameStateMinusCardToPlayer(
    const GameState& oldState, Card::Rank rankToPlayer) const {
  GameState newState = oldState;
  // Only for calculation purposes so suit doesn't matter, defaulted to hearts
  newState.playerHand.addCard(Card(rankToPlayer, Card::Suit::Hearts));
  newState.totalCardsRemaining = oldState.totalCardsRemaining - 1;
  newState.remainingCardCounts[rankToPlayer]--;
  return newState;
}

BlackjackGame::GameState BlackjackGame::getGameStateAfterSplit(
    const GameState& oldState, Card cardToKeep) const {
  GameState newState = oldState;
  newState.playerHand = Hand();
  newState.playerHand.addCard(cardToKeep);
  newState.wasSplit = true;
  newState.numPlayerHands = oldState.numPlayerHands + 1;
  return newState;
}

double BlackjackGame::getCardDrawProbability(const GameState& state,
                                             Card::Rank cardRank,
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
      if (cardRank == Card::Rank::Ace) {
        return 0.0;
      }
      totalCards -= state.remainingCardCounts.at(Card::Rank::Ace);
    }
    // If dealer upcard is an Ace, the hole card cannot be a 10-value card.
    else if (state.dealerUpcard.getRank() == Card::Rank::Ace) {
      if (Card(cardRank, Card::Suit::Hearts).getValue() == 10) {
        return 0.0;
      }
      totalCards -= (state.remainingCardCounts.at(Card::Rank::Ten) +
                     state.remainingCardCounts.at(Card::Rank::Jack) +
                     state.remainingCardCounts.at(Card::Rank::Queen) +
                     state.remainingCardCounts.at(Card::Rank::King));
    }
  }

  if (totalCards <= 0) return 0.0;

  return countOfRank / totalCards;
}

BlackjackGame::BlackjackGame(const GameRules& rules)
    : numDecks(rules.numDecks),
      dealerHitsSoft17(rules.dealerHitsSoft17),
      blackjackPayout(rules.blackjackPayout),
      insurancePayout(rules.insurancePayout),
      canDoubleAfterSplit(rules.canDoubleAfterSplit),
      surrenderType(rules.surrenderType),
      canSplitAces(rules.canSplitAces),
      maxSplits(rules.maxSplits) {}

void BlackjackGame::clearMemos() const {
  DealerMemo_.clear();
  PlayerMemo_.clear();
}

BlackjackGame::GameState BlackjackGame::getGameStateForCalculation(
    const std::vector<Card::Rank>& player_ranks, const Card::Rank& dealer_rank,
    const int num_decks, const bool dealerCheckedForBJ) {
  Hand playerHand;
  Hand dealerHand;

  Deck deck(num_decks);

  for (const auto& rank : player_ranks) {
    bool cardExists = false;
    for (int i = 0; i < 4; i++) {
      Card::Suit suit = static_cast<Card::Suit>(i);
      Card cardToDeal(rank, suit);
      try {
        deck.dealCard(cardToDeal);
        playerHand.addCard(cardToDeal);
        cardExists = true;
        break;
      } catch (const std::runtime_error& e) {
        // Card can't be added with this suit
      }
    }
    if (!cardExists) {
      throw std::runtime_error("Too many cards of rank " +
                               BlackjackUtils::rankToString(rank) +
                               " requested.");
    }
  }

  bool cardExists = false;
  for (int i = 0; i < 4; i++) {
    Card::Suit suit = static_cast<Card::Suit>(i);
    Card cardToDeal(dealer_rank, suit);
    try {
      deck.dealCard(cardToDeal);
      dealerHand.addCard(cardToDeal);
      cardExists = true;
      break;
    } catch (const std::runtime_error& e) {
      // Card can't be added with this suit
    }
  }
  if (!cardExists) {
    throw std::runtime_error("Too many cards of rank " +
                             BlackjackUtils::rankToString(dealer_rank) +
                             " requested.");
  }

  GameState state{
      playerHand,
      dealerHand.getCards().at(0),
      dealerHand,
      deck.getRemainingCardCounts(),
      deck.getRemainingCardsCount(),
      num_decks,
      dealerCheckedForBJ,
      false,  // wasSplit
      1       // numPlayerHands
  };

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

  // If player hand is a soft 12 (Ace + Ace) and splitting aces is not allowed
  if (state.playerHand.getValue() == 12 && state.playerHand.isSoft() &&
      !canSplitAces) {
    return std::nan("");
  }

  GameState singleHandState =
      getGameStateAfterSplit(state, state.playerHand.getCards().at(0));
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
  // Since splitting creates two hands, we multiply the single hand EV by 2.
  // This is not perfectly accurate, but it gives a very close approximation and
  // runs in a reasonable time frame (calculating exact EV would be extremely
  // computationally expensive).
  return 2 * singleHandEV;
}

double BlackjackGame::calculateEVForDouble(const GameState& state) const {
  if (state.playerHand.getCards().size() != 2 ||
      state.playerHand.getValue() == 21) {
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
  if (state.playerHand.getCards().size() != 2) {
    return std::nan("");
  }

  if (surrenderType == SurrenderType::None) {
    return std::nan("");
  }

  bool dealerCanHaveBlackjack =
      (state.dealerUpcard.getValue() == 10 ||
       state.dealerUpcard.getRank() == Card::Rank::Ace);

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
  if (state.dealerUpcard.getRank() != Card::Rank::Ace || state.dealerChecked) {
    return std::nan("");
  }
  double nextCardTenProb = getCardDrawProbability(state, Card::Rank::Ten) +
                           getCardDrawProbability(state, Card::Rank::Jack) +
                           getCardDrawProbability(state, Card::Rank::Queen) +
                           getCardDrawProbability(state, Card::Rank::King);
  return nextCardTenProb * insurancePayout + (1 - nextCardTenProb) * -1.0;
}

BlackjackGame::EVResult BlackjackGame::calculateEVForOptimalStrategy(
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

  // Record the optimal action and its EV in the result
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

BlackjackGame::DealerOutcomeProbabilities BlackjackGame::calcDealerOutcomeProbs(
    const GameState& state) const {
  // Key used for memo
  DealerMemoKey key(state.dealerHand.getValue(), state.dealerHand.isSoft(),
                    convertMapToDeckCount(state.remainingCardCounts));

  // Check if cache contains result
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