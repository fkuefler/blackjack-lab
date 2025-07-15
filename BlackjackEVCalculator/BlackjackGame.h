// BlackjackGame.h
#pragma once

#include <vector>
#include <map>
#include <string>
#include <random>
#include <numeric>

#include "Card.h"
#include "Hand.h"
#include "Deck.h"

struct GameState {
	Hand playerHand;
	Card dealerUpcard;
	Hand dealerHand;

	std::map<Rank, int> remainingCardCounts;
	int totalCardsRemaining;
	int originalNumDecks;
	bool dealerChecked;
};

enum class PlayerAction {
	Hit, Stand, Split, Double, Surrender
};

struct DealerOutcomeProbabilities {
	double prob_17 = 0.0;
	double prob_18 = 0.0;
	double prob_19 = 0.0;
	double prob_20 = 0.0;
	double prob_21 = 0.0;
	double prob_blackjack = 0.0;
	double prob_bust = 0.0;
};

// remainingCardsCounts as array
using DeckCounts = std::array<int, 10>;

// Dealer hand score, isSoft, remaining card counts as array
using DealerMemoKey = std::tuple<int, bool, DeckCounts>;

struct EVResult {
	double hitEV = 0.0;
	double standEV = 0.0;
	double splitEV = 0.0;
	double doubleEV = 0.0;
	double surrenderEV = 0.0;
	PlayerAction optimalAction;
	double optimalEV;
};

class BlackjackGame {
private:
	int numDecks;
	bool dealerHitsSoft17;
	double blackjackPayout;
	bool canDoubleAfterSplit;
	bool canSurrender;
	bool canSplitAces;

	double calculatePayout(int playerHandScore, int dealerHandScore,
		bool isPlayerBlackjack, bool isDealerBlackjack,
		bool isDoubledDown = false) const;

	DeckCounts convertMapToDeckCount(const std::map<Rank, int>& remainingCardCounts);

	GameState getGameStateMinusCardToDealer(const GameState& oldState, Rank rankToDealer) const;

public:
	BlackjackGame(int decks = 6, bool h17 = true, double bjPayout = 1.5,
		bool das = true, bool surrender = true, bool splitAces = false);

	GameState getGameState(const Hand& playerHand, const Card& dealerUpcard, const Deck& currentDeck, const bool dealerCheckedForBJ) const;

	double calculateEVForHit(const GameState& state) const;
	double calculateEVForStand(const GameState& state) ;
	double calculateEVForSplit(const GameState& state) const;
	double calculateEVForDouble(const GameState& state) const;
	double calculateEVForSurrender(const GameState& state) const;

	DealerOutcomeProbabilities calcDealerOutcomeProbs(const GameState& state, std::map<DealerMemoKey, DealerOutcomeProbabilities>& memo);

	std::string getRuleDescription() const;
	};