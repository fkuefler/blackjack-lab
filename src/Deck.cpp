// Deck.cpp
#include "Deck.h"

#include <algorithm>
#include <ctime>
#include <stdexcept>

void Deck::buildDeck() {
  cards.clear();
  originalDeckContents.clear();
  remainingCardCounts.clear();

  for (int d = 0; d < numberOfDecks; ++d) {
    for (int s = static_cast<int>(Suit::Hearts);
         s <= static_cast<int>(Suit::Spades); ++s) {
      for (int r = static_cast<int>(Rank::Ace);
           r <= static_cast<int>(Rank::King); ++r) {
        originalDeckContents.emplace_back(static_cast<Rank>(r),
                                          static_cast<Suit>(s));
      }
    }
  }
  cards = originalDeckContents;
  // Populate the counts map from the newly built deck
  for (const auto& card : cards) {
    remainingCardCounts[card.rank]++;
  }
}

// Constructor
Deck::Deck(int numDecks) : numberOfDecks(numDecks) {
  std::random_device rd;
  rng.seed(rd());

  buildDeck();
  shuffle();
}

// Shuffle cards currently in cards vector
void Deck::shuffle() { std::shuffle(cards.begin(), cards.end(), rng); }

// Deal a card from top of the deck (back of the vector)
Card Deck::dealCard() {
  if (isEmpty()) {
    throw std::runtime_error("Attempted to deal from empty deck");
  }
  Card dealtCard = cards.back();
  remainingCardCounts[dealtCard.rank]--;
  cards.pop_back();
  return dealtCard;
}

Card Deck::dealCard(Card card) {
  auto it = std::find(cards.begin(), cards.end(), card);

  if (it != cards.end()) {
    Card dealtCard = *it;
    remainingCardCounts[dealtCard.rank]--;
    cards.erase(it);
    return dealtCard;
  }

  throw std::runtime_error(
      "Attempted to deal a specific card that is not in the deck.");
}

// Check if deck is empty
bool Deck::isEmpty() const { return cards.empty(); }

// Return number of cards remaining in deck
int Deck::getRemainingCardsCount() const { return cards.size(); }

// Returns a summary of the number of cards of each Rank remaining in the deck
std::map<Rank, int> Deck::getRemainingCardCounts() const {
  return remainingCardCounts;
}

// Resets deck to original state and shuffles
void Deck::reset() {
  cards = originalDeckContents;
  // Recalculate the counts from the full original deck
  remainingCardCounts.clear();
  for (const auto& card : cards) {
    remainingCardCounts[card.rank]++;
  }
  shuffle();
}