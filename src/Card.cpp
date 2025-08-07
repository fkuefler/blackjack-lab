// Card.cpp
#include "Card.h"

#include <stdexcept>
#include <string>

int Card::getValue() const {
  switch (rank) {
    case Rank::Two:
      return 2;
    case Rank::Three:
      return 3;
    case Rank::Four:
      return 4;
    case Rank::Five:
      return 5;
    case Rank::Six:
      return 6;
    case Rank::Seven:
      return 7;
    case Rank::Eight:
      return 8;
    case Rank::Nine:
      return 9;
    case Rank::Ten:
    case Rank::Jack:
    case Rank::Queen:
    case Rank::King:
      return 10;
    case Rank::Ace:
      return 11;  // Ace can also be 1, but we'll use 11 for now
    default:
      throw std::invalid_argument("Invalid rank");
  }
}

std::string Card::toString() const {
  return Card::rankToString(rank) + Card::suitToString(suit);
}

std::string Card::rankToString(Card::Rank rank) {
  switch (rank) {
    case Rank::Ace:
      return "Ace";
    case Rank::Two:
      return "Two";
    case Rank::Three:
      return "Three";
    case Rank::Four:
      return "Four";
    case Rank::Five:
      return "Five";
    case Rank::Six:
      return "Six";
    case Rank::Seven:
      return "Seven";
    case Rank::Eight:
      return "Eight";
    case Rank::Nine:
      return "Nine";
    case Rank::Ten:
      return "Ten";
    case Rank::Jack:
      return "Jack";
    case Rank::Queen:
      return "Queen";
    case Rank::King:
      return "King";
    default:
      throw std::invalid_argument("?");
  }
}

std::string Card::suitToString(Card::Suit suit) {
  switch (suit) {
    case Suit::Hearts:
      return "Hearts";
    case Suit::Diamonds:
      return "Diamonds";
    case Suit::Clubs:
      return "Clubs";
    case Suit::Spades:
      return "Spades";
    default:
      throw std::invalid_argument("?");
  }
}

bool Card::operator==(const Card& other) const {
  return (rank == other.rank && suit == other.suit);
}

bool Card::operator!=(const Card& other) const { return !(*this == other); }