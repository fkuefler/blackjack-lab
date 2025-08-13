// Card.h
#pragma once

#include <string>
#include <vector>

class Card {
 public:
  enum class Rank {
    Ace = 1,
    Two,
    Three,
    Four,
    Five,
    Six,
    Seven,
    Eight,
    Nine,
    Ten,
    Jack,
    Queen,
    King
  };

  enum class Suit { Hearts, Diamonds, Clubs, Spades };

  // Default constructor for Card, initializes to Ace of Hearts
  Card() : rank(Rank::Ace), suit(Suit::Hearts) {}
  // Constructor for new Card object
  Card(Rank r, Suit s) : rank(r), suit(s) {}

  // Returns the rank of the card
  Rank getRank() const { return rank; }
  // Returns the suit of the card
  Suit getSuit() const { return suit; }
  // Returns the blackjack value of the card
  int getValue() const;

  // Returns a string representation of the card
  std::string toString() const;

  // Operator Overload Definition: checks two cards for equality
  bool operator==(const Card& other) const;

  // Operator Overload Definition: checks two cards for inequality
  bool operator!=(const Card& other) const;

 private:
  Rank rank;
  Suit suit;
};