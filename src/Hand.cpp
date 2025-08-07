// Hand.cpp
#include "Hand.h"

#include <algorithm>
#include <numeric>
#include <sstream>
#include <stdexcept>

void Hand::addCard(const Card& card) { cards.push_back(card); }

int Hand::getValue() const {
  int score = 0;
  int numAces = 0;

  // First pass: Sum card values using Ace as default 11
  for (const auto& card : cards) {
    if (card.getRank() == Card::Rank::Ace) {
      numAces++;  // Count no. of aces in hand
    }
    score += card.getValue();
  }

  // Second pass: Adjust aces from 11 to 1 if needed
  while (score > 21 && numAces > 0) {
    score -= 10;
    numAces--;
  }

  return score;
}

bool Hand::isSoft() const {
  bool hasAce = false;
  int numAces = 0;
  int potentialSoftScore = 0;  // Score if ace counted as 11

  for (const auto& card : cards) {
    if (card.getRank() == Card::Rank::Ace) {
      hasAce = true;
      numAces++;
    }
    potentialSoftScore += card.getValue();
  }

  // If hand has at least one ace, update potentialSoftScore by changing all but
  // one ace from 11 to 1 (a soft hand will always have exactly one ace counted
  // as 11)
  if (hasAce) {
    potentialSoftScore -= (numAces - 1) * 10;
  }

  return hasAce && potentialSoftScore <= 21;
}

bool Hand::isBust() const { return getValue() > 21; }

bool Hand::isBlackjack() const { return cards.size() == 2 && getValue() == 21; }

bool Hand::canSplit() const {
  return cards.size() == 2 && cards[0].getRank() == cards[1].getRank();
}

void Hand::clear() { cards.clear(); }

std::string Hand::toString() const {
  std::stringstream ss;
  for (size_t i = 0; i < cards.size(); ++i) {
    ss << cards[i].toString();
    if (i < cards.size() - 1) {
      ss << " ";
    }
  }
  return ss.str();
}