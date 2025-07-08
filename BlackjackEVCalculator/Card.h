// Card.h
#pragma once

#include <string>
#include <vector>

enum class Rank
{
	Two, Three, Four, Five, Six, Seven, Eight, Nine, Ten,
	Jack, Queen, King, Ace
};

enum class Suit
{
	Hearts, Diamonds, Clubs, Spades
};

class Card {
public: 
	Rank rank;
	Suit suit;

	Card(Rank r, Suit s);

	int getValue() const;

	std::string toString() const;

	bool operator==(const Card& other) const;

	bool operator!=(const Card& other) const;
};