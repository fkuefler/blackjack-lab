// Card.cpp
#include "Card.h"
#include <string>
#include <stdexcept>

// Constructor for new Card object
Card::Card(Rank r, Suit s) : rank(r), suit(s) {}

// Returns the blackjack value of the card
int Card::getValue() const {
	switch (rank) {
	case Rank::Two: return 2;
	case Rank::Three: return 3;
	case Rank::Four: return 4;
	case Rank::Five: return 5;
	case Rank::Six: return 6;
	case Rank::Seven: return 7;
	case Rank::Eight: return 8;
	case Rank::Nine: return 9;
	case Rank::Ten:
	case Rank::Jack:
	case Rank::Queen:
	case Rank::King: return 10;
	case Rank::Ace: return 11; // Ace can also be 1, but we'll use 11 for now
	default: throw std::invalid_argument("Invalid rank");
	}
}

// Returns a string representation of the card
std::string Card::toString() const {
	std::string rankStr;
	switch (rank) {
	case Rank::Two: rankStr = "2"; break;
	case Rank::Three: rankStr = "3"; break;
	case Rank::Four: rankStr = "4"; break;
	case Rank::Five: rankStr = "5"; break;
	case Rank::Six: rankStr = "6"; break;
	case Rank::Seven: rankStr = "7"; break;
	case Rank::Eight: rankStr = "8"; break;
	case Rank::Nine: rankStr = "9"; break;
	case Rank::Ten: rankStr = "T"; break;
	case Rank::Jack: rankStr = "J"; break;
	case Rank::Queen: rankStr = "Q"; break;
	case Rank::King: rankStr = "K"; break;
	case Rank::Ace: rankStr = "A"; break;
	default: rankStr = "?"; break;
	}

	std::string suitStr;
	switch (suit) {
	case Suit::Clubs: suitStr = "C"; break;
	case Suit::Diamonds: suitStr = "D"; break;
	case Suit::Hearts: suitStr = "H"; break;
	case Suit::Spades: suitStr = "S"; break;
	default: suitStr = "?"; break;
	}

	return rankStr + suitStr;
}

// Operator Overload Definition: operator==
bool Card::operator==(const Card& other) const {
	return (rank == other.rank && suit == other.suit);
}

// Operator Overload Definition: operator!=
bool Card::operator!=(const Card& other) const {
	return !(*this == other);
}