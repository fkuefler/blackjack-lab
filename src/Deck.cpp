// Deck.cpp
#include "Deck.h"
#include <ctime>
#include <stdexcept>

void Deck::buildDeck() {
	cards.clear();
	originalDeckContents.clear();

	for (int d = 0; d < numberOfDecks; ++d) {
		for (int s = static_cast<int>(Suit::Clubs); s <= static_cast<int>(Suit::Spades); ++s) {
			for (int r = static_cast<int>(Rank::Ace); r <= static_cast<int>(Rank::King); ++r) {
				originalDeckContents.emplace_back(static_cast<Rank>(r), static_cast<Suit>(s));
			}
		}
	}
	cards = originalDeckContents;
}

// Constructor
Deck::Deck(int numDecks) : numberOfDecks(numDecks) {
	std::random_device rd;
	rng.seed(rd());

	buildDeck();
	shuffle();
}

// Shuffle cards currently in cards vector
void Deck::shuffle() {
	std::shuffle(cards.begin(), cards.end(), rng);
}

// Deal a card from top of the deck (back of the vector)
Card Deck::dealCard() {
	if (isEmpty()) {
		throw std::runtime_error("Attempted to deal from empty deck");
	}
	Card dealtCard = cards.back();
	cards.pop_back();
	return dealtCard;
}

// Check if deck is empty
bool Deck::isEmpty() const {
	return cards.empty();
}

// Return number of cards remaining in deck
int Deck::getRemainingCardsCount() const {
	return cards.size();
}

// Returns a summary of the number of cards of each Rank remaining in the deck
std::map<Rank, int> Deck::getRemainingCardCounts() const {
	std::map<Rank, int> counts;
	for (int r = static_cast<int>(Rank::Ace); r <= static_cast<int>(Rank::King); ++r) {
		counts[static_cast<Rank>(r)] = 0;
	}

	for (const auto& card : cards) {
		counts[card.rank]++;
	}

	return counts;
}

// Resets deck to original state and shuffles
void Deck::reset() {
	cards = originalDeckContents;
	shuffle();
}