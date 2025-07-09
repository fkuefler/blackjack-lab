// Deck.h
#pragma once

#include <vector>
#include <random>
#include <algorithm>
#include <map>
#include "Card.h"

class Deck {
private: 
	std::vector<Card> cards; // Cards currently in shoe
	std::vector<Card> originalDeckContents; // Initial set of cards
	std::mt19937 rng; // Mersenne Twister engine for random numbers
	int numberOfDecks; // Initial number of decks used to build shoe

	// Private helper method to build initial set of cards
	void buildDeck();

public: 
	// Constructor: Takes number of decks to use. Default: 6.
	Deck(int NumDecks = 6);

	// Shuffles cards currently in cards vector
	void shuffle();

	// Deals card from top of deck
	Card dealCard();

	// Checks if deck is empty
	bool isEmpty() const;

	// Returns number of undealt cards remaining in deck
	int getRemainingCardsCount() const;

	// Returns a summary of the number of cards of each Rank remaining in the deck
	std::map<Rank, int> getRemainingCardCounts() const;

	// Resets deck to initial state and stuffles
	void reset();
};