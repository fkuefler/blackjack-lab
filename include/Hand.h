// Hand.h
#pragma once

#include <string>
#include <vector>

#include "Card.h"

class Hand {
 public:
  // Constructor for a new Hand object
  Hand();

  // Get the list of cards in the hand
  std::vector<Card> getCards() const { return cards; }

  // Add a card to the hand
  void addCard(const Card& card);

  // Calculate the total value of the hand
  int getValue() const;

  // Checks if hand contains an Ace currently counted as 11
  bool isSoft() const;

  // Checks if the hand is a bust (total value exceeds 21)
  bool isBust() const;

  // Checks if the hand is a blackjack (total value is exactly 21 with two
  // cards)
  bool isBlackjack() const;

  // Checks if the hand can be split into two hands (two cards with the same
  // rank)
  bool canSplit() const;

  // Clears the hand, removing all cards
  void clear();

  // Returns a string representation of the hand
  std::string toString() const;

 private:
  // Represents a hand of cards in a card game
  std::vector<Card> cards;
};