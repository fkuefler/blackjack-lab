// Hand.cpp

#include "Hand.h"
#include <numeric>
#include <algorithm>
#include <sstream>
#include <stdexcept>

// Constructor
Hand::Hand() {}

// Add a card to the hand
void Hand::addCard(const Card& card) {
	cards.push_back(card);
}

// Calculate the total value of the hand
int Hand::getValue() const {
	int score = 0;
	int numAces = 0;

	// First pass: Sum card values using Ace as default 11
	for (const auto& card : cards) {
		if (card.rank == Rank::Ace) {
			numAces++; // Count no. of aces in hand
		}
		score += card.getValue();
	}

	// Second pass: Adjust aces from 11 to 1 if needed
	while (score > 21 && numAces > 0) {
		score -= 10;
		numAces--;
	}

	return score;
}

// Calculate the total value of the hand, but always treat soft 17s as 7
// Used for calculations where dealer needs to hit on soft 17
int Hand::getValueSoft17As7() const {
	int score = 0;
	int numAces = 0;

	// First pass: Sum card values using Ace as default 11
	for (const auto& card : cards) {
		if (card.rank == Rank::Ace) {
			numAces++; // Count no. of aces in hand
		}
		score += card.getValue();
	}

	// Second pass: Adjust aces from 11 to 1 if needed
	while (score > 21 && numAces > 0) {
		score -= 10;
		numAces--;
	}

	// If soft 17, change to 7
	if (numAces == 1 && score == 17) {
		return 7;
	}

	return score;
}

// Checks if hand has an ace that can be counted as 11 without busting
bool Hand::isSoft() const {
	bool hasAce = false;
	int numAces = 0;
	int potentialSoftScore = 0; // Score if ace counted as 11

	for (const auto& card : cards) {
		if (card.rank == Rank::Ace) {
			hasAce = true;
			numAces++;
		}
		potentialSoftScore += card.getValue();
	}

	// If hand has at least one ace, update potentialSoftScore by changing all but one ace from 11 to 1
	// (a soft hand will always have exactly one ace counted as 11)
	if (hasAce) {
		potentialSoftScore -= (numAces - 1) * 10;
	}

	return hasAce && potentialSoftScore <= 21;
}

// Checks if hand is busted
bool Hand::isBust() const {
	return getValue() > 21;
}

// Checks if hand is a blackjack
bool Hand::isBlackjack() const {
	return cards.size() == 2 && getValue() == 21;
}

// Checks if hand can split (hand contains exactly two cards of same rank)
bool Hand::canSplit() const {
	return cards.size() == 2 && cards[0].rank == cards[1].rank;
}

// Clears the hand
void Hand::clear() {
	cards.clear();
}

// Returns a string representation of hand (e.g. "As Kd 4c")
std::string Hand::toString() const {
	std::stringstream ss;
	for (size_t i = 0; i < cards.size(); ++i) {
		ss << cards[i].toString();
		if (i < cards.size() - 1) {
			ss << " ";
		}
	}
	return ss.str();
}