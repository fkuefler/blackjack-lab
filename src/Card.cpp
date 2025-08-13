// Card.cpp
#include "Card.h"

#include <stdexcept>
#include <string>

#include "BlackjackUtils.h"

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
  return BlackjackUtils::rankToString(rank) + " of " +
         BlackjackUtils::suitToString(suit);
}

bool Card::operator==(const Card& other) const {
  return (rank == other.rank && suit == other.suit);
}

bool Card::operator!=(const Card& other) const { return !(*this == other); }