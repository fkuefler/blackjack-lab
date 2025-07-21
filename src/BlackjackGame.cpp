// BlackjackGame.cpp
#include "BlackjackGame.h"

#include <iostream>
#include <stdexcept>
#include <numeric>
#include <algorithm>
#include <array>
#include "Hand.h"
#include "Card.h"
#include "Deck.h"

double BlackjackGame::calculatePayout(int playerHandScore, int dealerHandScore,
	bool isPlayerBlackjack, bool isDealerBlackjack,
	bool isDoubledDown) const {
	double basePayout = 0.0; // Default

	if (playerHandScore > 21) { // Player bust
		basePayout = -1.0;
	}

	else if (dealerHandScore > 21) { // Dealer bust
		basePayout = 1.0;
	}

	else if (isPlayerBlackjack && isDealerBlackjack) { // Both have blackjack
		basePayout = 0.0;
	}

	else if (isPlayerBlackjack) { // Player blackjack
		basePayout = blackjackPayout;
	}

	else if (isDealerBlackjack) { // Dealer blackjack
		basePayout = -1.0;
	}

	else if (playerHandScore < dealerHandScore) { // Player loses
		basePayout = -1.0;
	}

	else if (playerHandScore > dealerHandScore) { // Player wins
		basePayout = 1.0;
	}

	// Push scenario not needed since basePayout initialized to zero

	if (isDoubledDown) { // If hand was doubled down
		return basePayout * 2.0;
	}
	return basePayout;
}

// Converts the remainingCardCounts map to an array
DeckCounts BlackjackGame::convertMapToDeckCount(const std::map<Rank, int>& remainingCardCounts) {
	DeckCounts arrayCounts = {};

	arrayCounts[0] = remainingCardCounts.at(Rank::Two);
	arrayCounts[1] = remainingCardCounts.at(Rank::Three);
	arrayCounts[2] = remainingCardCounts.at(Rank::Four);
	arrayCounts[3] = remainingCardCounts.at(Rank::Five);
	arrayCounts[4] = remainingCardCounts.at(Rank::Six);
	arrayCounts[5] = remainingCardCounts.at(Rank::Seven);
	arrayCounts[6] = remainingCardCounts.at(Rank::Eight);
	arrayCounts[7] = remainingCardCounts.at(Rank::Nine);
	arrayCounts[8] = remainingCardCounts.at(Rank::Ten);
	arrayCounts[9] = remainingCardCounts.at(Rank::Ace);
}

// Helper method to make a new GameState that has added a card to the dealer hand and removed it from the shoe
// Since this is for calculation purposes, only the rank of the card matters and the suit is defaulted to hearts
GameState BlackjackGame::getGameStateMinusCardToDealer(const GameState& oldState, Rank rankToDealer) const {
	GameState newState;
	newState.playerHand = oldState.playerHand;
	Card cardToAdd = Card(rankToDealer, Suit::Hearts); // Suit doesn't matter; defaulted to hearts
	newState.dealerHand = oldState.dealerHand;
	newState.dealerHand.addCard(cardToAdd);
	newState.totalCardsRemaining = oldState.totalCardsRemaining - 1;
	newState.remainingCardCounts = oldState.remainingCardCounts;
	newState.remainingCardCounts[rankToDealer]--; // Decrement the count for the rank of the card we just removed from the shoe
	newState.dealerChecked = false; // This method only runs once the dealer has atleast two cards--they will never check their next card
	return newState;
}

BlackjackGame::BlackjackGame(int decks, bool h17, double bjPayout, bool das, bool surrender, bool splitAces)
	: numDecks(decks),
	dealerHitsSoft17(h17),
	blackjackPayout(bjPayout),
	canDoubleAfterSplit(das),
	canSurrender(surrender),
	canSplitAces(splitAces) {
}

GameState BlackjackGame::getGameState(const Hand& playerHand, const Card& dealerUpcard, const Deck& currentDeck,
	const bool dealerCheckedForBJ) const {
	GameState state;
	state.playerHand = playerHand;
	state.dealerUpcard = dealerUpcard;
	state.dealerHand = Hand();
	state.dealerHand.addCard(dealerUpcard);
	state.remainingCardCounts = currentDeck.getRemainingCardCounts();
	state.totalCardsRemaining = currentDeck.getRemainingCardsCount();
	state.dealerChecked = dealerCheckedForBJ;
}

double BlackjackGame::calculateEVForHit(const GameState& state) const {
	return 0.0; // TODO
}

double BlackjackGame::calculateEVForStand(const GameState& state) {
	std::map<DealerMemoKey, DealerOutcomeProbabilities> dealerMemoTable;
	DealerOutcomeProbabilities outcomeProbs = calcDealerOutcomeProbs(state, dealerMemoTable);
	return 0.0; // TODO
}

double BlackjackGame::calculateEVForSplit(const GameState& state) const {
	return 0.0; // TODO
}

double BlackjackGame::calculateEVForDouble(const GameState& state) const {
	return 0.0; // TODO
}

double BlackjackGame::calculateEVForSurrender(const GameState& state) const {
	return canSurrender ? -0.5 : std::nan("");
}

// NOTE: Add insurance EV calc

// NOTE: Add optimal strategy EV calc

// Calculates the probability of each dealer outcome
DealerOutcomeProbabilities BlackjackGame::calcDealerOutcomeProbs(
	const GameState& state, std::map<DealerMemoKey, DealerOutcomeProbabilities>& memo) {
	// Key used for memo
	DealerMemoKey key(state.dealerHand.getValue(), state.dealerHand.isSoft(), convertMapToDeckCount(state.remainingCardCounts));

	if (memo.count(key)) {
		return memo[key];
	}

	DealerOutcomeProbabilities outcomes;

	// If dealer busted
	if (state.dealerHand.getValue() > 21) {
		outcomes.prob_bust = 1.0;
		memo[key] = outcomes;
		return outcomes;
	}

	// If dealer has hard 17 or soft 17 with dealer standing soft 17s
	if (state.dealerHand.getValue() == 17 && (!state.dealerHand.isSoft() || (state.dealerHand.isSoft() && !dealerHitsSoft17))) {
		outcomes.prob_17 = 1.0;
		memo[key] = outcomes;
		return outcomes;
	}
	// If dealer has 18-21
	else if (state.dealerHand.getValue() >= 18) {
		if (state.dealerHand.getValue() == 18) {
			outcomes.prob_18 = 1.0;
		}
		else if (state.dealerHand.getValue() == 19) {
			outcomes.prob_19 = 1.0;
		}
		else if (state.dealerHand.getValue() == 20) {
			outcomes.prob_20 = 1.0;
		}
		else if (state.dealerHand.getValue() == 21) {
			if (state.dealerHand.isBlackjack()) {
				outcomes.prob_blackjack = 1.0;
			}
			else {
				outcomes.prob_21 = 1.0;
			}
		}
		memo[key] = outcomes;
		return outcomes;
	}

	// Iterate through all ranks for the next possible card
	for (const auto& pair : state.remainingCardCounts) {
		if (pair.second > 0) {

			double probDrawCard;
			// If we know dealer checked for blackjack and doesn't have it, this gives additional information about the next card
			if (state.dealerChecked) {
				if (state.dealerUpcard.rank == Rank::Ten) {
					// If upcard is ten and no blackjack, next card can't be an ace, so skip it.
					if (pair.first == Rank::Ace) {
						continue;
					}
					// We know next card can't be an ace, so subtract the number of aces from the number of available cards
					probDrawCard = static_cast<double>(pair.second) / state.totalCardsRemaining - state.remainingCardCounts.at(Rank::Ace);
				}
				else if (state.dealerUpcard.rank == Rank::Ace) {
					// If upcard is ace and no blackjack, next card can't be an ten, so skip it.
					if (pair.first == Rank::Ten) {
						continue;
					}
					// We know next card can't be an ten, so subtract the number of tens from the number of available cards
					probDrawCard = static_cast<double>(pair.second) / state.totalCardsRemaining - state.remainingCardCounts.at(Rank::Ten);
				}
				else {
					probDrawCard = static_cast<double>(pair.second) / state.totalCardsRemaining;
				}
			}
			else {
				// Dealer hasn't checked
				probDrawCard = static_cast<double>(pair.second) / state.totalCardsRemaining;
			}

			// Get new GameState for after card is dealt
			GameState newState = getGameStateMinusCardToDealer(state, pair.first);

			// Recursively call this method with the new GameState
			DealerOutcomeProbabilities subOutcomes = calcDealerOutcomeProbs(newState, memo);

			outcomes.prob_17 += probDrawCard * subOutcomes.prob_17;
			outcomes.prob_18 += probDrawCard * subOutcomes.prob_18;
			outcomes.prob_19 += probDrawCard * subOutcomes.prob_19;
			outcomes.prob_20 += probDrawCard * subOutcomes.prob_20;
			outcomes.prob_21 += probDrawCard * subOutcomes.prob_21;
			outcomes.prob_bust += probDrawCard * subOutcomes.prob_bust;
			outcomes.prob_blackjack += probDrawCard * subOutcomes.prob_blackjack;
		}
	}
	// Add situation to memo and return outcomes
	memo[key] = outcomes;
	return outcomes;
}

std::string BlackjackGame::getRuleDescription() const {
	return ""; // TODO
}