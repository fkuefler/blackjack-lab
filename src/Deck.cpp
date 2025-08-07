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
    for (int s = static_cast<int>(Card::Suit::Hearts);
         s <= static_cast<int>(Card::Suit::Spades); ++s) {
      for (int r = static_cast<int>(Card::Rank::Ace);
           r <= static_cast<int>(Card::Rank::King); ++r) {
        originalDeckContents.emplace_back(static_cast<Card::Rank>(r),
                                          static_cast<Card::Suit>(s));
      }
    }
  }
  cards = originalDeckContents;
  // Populate the counts map from the newly built deck
  for (const auto& card : cards) {
    remainingCardCounts[card.getRank()]++;
  }
}

Deck::Deck(int numDecks) : numberOfDecks(numDecks) {
  std::random_device rd;
  rng.seed(rd());

  buildDeck();
  shuffle();
}

void Deck::shuffle() { std::shuffle(cards.begin(), cards.end(), rng); }

Card Deck::dealCard() {
  if (isEmpty()) {
    throw std::runtime_error("Attempted to deal from empty deck");
  }
  Card dealtCard = cards.back();
  remainingCardCounts[dealtCard.getRank()]--;
  cards.pop_back();
  return dealtCard;
}

Card Deck::dealCard(Card card) {
  auto it = std::find(cards.begin(), cards.end(), card);

  if (it != cards.end()) {
    Card dealtCard = *it;
    remainingCardCounts[dealtCard.getRank()]--;
    cards.erase(it);
    return dealtCard;
  }

  throw std::runtime_error(
      "Attempted to deal a specific card that is not in the deck.");
}

bool Deck::isEmpty() const { return cards.empty(); }

int Deck::getRemainingCardsCount() const { return cards.size(); }

std::map<Card::Rank, int> Deck::getRemainingCardCounts() const {
  return remainingCardCounts;
}

void Deck::reset() {
  cards = originalDeckContents;
  // Recalculate the counts from the full original deck
  remainingCardCounts.clear();
  for (const auto& card : cards) {
    remainingCardCounts[card.getRank()]++;
  }
  shuffle();
}