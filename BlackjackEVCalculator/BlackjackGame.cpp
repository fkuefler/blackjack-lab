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
	return newState;
}

/*
* double BlackjackGame::calcProbOfDealerX(const GameState& state, int x, bool nextCardCanBeAce) const {
	double totalProb = 0.0;
	// If dealer hits soft 17s, different getValue method is used to treat soft 17s as 7
	if (dealerHitsSoft17) {
		if (state.dealerHand.getValueSoft17As7() == x) {
			return 1.0;
		}
		else if (state.dealerHand.getValueSoft17As7() > x) {
			return 0.0;
		}
		for (const auto& pair : state.remainingCardCounts) {
			if (!nextCardCanBeAce && pair.first == Rank::Ace) {
				continue;
			}
			double rankProb = static_cast<double>(pair.second) / state.totalCardsRemaining;
			totalProb += (rankProb * calcProbOfDealerX(getGameStateMinusCardToDealer(state, pair.first), x, true));
		}
	}
	// If dealer stands soft 17s, standard getValue method is used
	else {
		if (state.dealerHand.getValue() == x) {
			return 1.0;
		}
		else if (state.dealerHand.getValue() > x) {
			return 0.0;
		}
		for (const auto& pair : state.remainingCardCounts) {
			if (!nextCardCanBeAce && pair.first == Rank::Ace) {
				continue;
			}
			double rankProb = static_cast<double>(pair.second) / state.totalCardsRemaining;
			totalProb += (rankProb * calcProbOfDealerX(getGameStateMinusCardToDealer(state, pair.first), x, true));
		}
	}
	return totalProb;
}
*/


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
	state.dealerCheckedForBJ = dealerCheckedForBJ;
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

// Add insurance EV calc

DealerOutcomeProbabilities BlackjackGame::calcDealerOutcomeProbs(
	const GameState& state, std::map<DealerMemoKey, DealerOutcomeProbabilities>& memo) {
	
	DealerMemoKey key(state.dealerHand.getValue(), state.dealerHand.isSoft(), convertMapToDeckCount(state.remainingCardCounts));

	if (memo.count(key)) {
		return memo[key];
	}

	DealerOutcomeProbabilities outcomes;

	if (state.dealerHand.getValue() > 21) {
		outcomes.prob_bust = 1.0;
		memo[key] = outcomes;
		return outcomes;
	}

	if (state.dealerHand.getValue() >= 17) {
		// If hard 17 or soft 17 but dealer stands soft 17s
		if (state.dealerHand.getValue() == 17 && (!state.dealerHand.isSoft() || (state.dealerHand.isSoft() && !dealerHitsSoft17))) {
			outcomes.prob_17 = 1.0;
		}
		else if (state.dealerHand.getValue() == 18) {
			outcomes.prob_18 = 1.0;
		}
		else if (state.dealerHand.getValue() == 19) {
			outcomes.prob_19 = 1.0;
		}
		else if (state.dealerHand.getValue() == 20) {
			outcomes.prob_20 = 1.0;
		}
		else if (state.dealerHand.getValue() == 21) {
			outcomes.prob_21 = 1.0;
		}
		memo[key] = outcomes;
		return outcomes;
	}


	/*
	*     // Iterate through all possible card ranks the dealer could draw
    FOR EACH rank, count IN currentState.REMAINING_CARD_COUNTS DO
        IF count > 0 THEN
            probDrawCard = (DOUBLE)count / totalRemainingCards

            // Create the next GameState after drawing this card
            nextState = getGameStateMinusCardToDealer(currentState, rank) // Assumes this function exists

            // Recursive call for the next state
            subOutcomes = calcProbOfDealerOutcomes(nextState, rules, memo)

            // Combine results: weighted average
            outcomes.PROB_17 += probDrawCard * subOutcomes.PROB_17
            outcomes.PROB_18 += probDrawCard * subOutcomes.PROB_18
            outcomes.PROB_19 += probDrawCard * subOutcomes.PROB_19
            outcomes.PROB_20 += probDrawCard * subOutcomes.PROB_20
            outcomes.PROB_21 += probDrawCard * subOutcomes.PROB_21
            outcomes.PROB_BUST += probDrawCard * subOutcomes.PROB_BUST
        END IF
    END FOR

    // 5. Store and Return
    memo[currentMemoKey] = outcomes
    RETURN outcomes
	*/

}

std::string BlackjackGame::getRuleDescription() const {
	return ""; // TODO
}