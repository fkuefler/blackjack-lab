#include "BlackjackGame.h"

#include <iostream>
#include <stdexcept>
#include <numeric>
#include <algorithm>

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

BlackjackGame::BlackjackGame(int decks, bool h17, double bjPayout, bool das, bool surrender, bool splitAces)
	: numDecks(decks),
	dealerHitsSoft17(h17),
	blackjackPayout(bjPayout),
	canDoubleAfterSplit(das),
	canSurrender(surrender),
	canSplitAces(splitAces) {
}

GameState BlackjackGame::getGameState(const Hand& playerHand, const Card& dealerUpcard, const Deck& currentDeck) const {
	GameState state;
	state.playerHand = playerHand;
	state.dealerUpcard = dealerUpcard;
	state.remainingCardCounts = currentDeck.getRemainingCardCounts();
	state.totalCardsRemaining = currentDeck.getRemainingCardsCount();
}

double BlackjackGame::calculateEVForHit(const GameState& state) const {
	return 0.0; // TODO
}

double BlackjackGame::calculateEVForStand(const GameState& state) const {
	return 0.0; // TODO
}

double BlackjackGame::calculateEVForSplit(const GameState& state) const {
	return 0.0; // TODO
}

double BlackjackGame::calculateEVForDouble(const GameState& state) const {
	return 0.0; // TODO
}

double BlackjackGame::calculateEVForSurrender(const GameState& state) const {
	return -0.5;
}

std::string BlackjackGame::getRuleDescription() const {
	return ""; // TODO
}
