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
};

enum class PlayerAction {
	Hit, Stand, Split, Double, Surrender
};

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

	GameState getGameStateMinusCardToDealer(const GameState& oldState, Rank rankToDealer) const;

	double calcProbOfDealerX(const GameState& state, int x, double totalProb) const;

public:
	BlackjackGame(int decks = 6, bool h17 = true, double bjPayout = 1.5,
		bool das = true, bool surrender = true, bool splitAces = false);

	GameState getGameState(const Hand& playerHand, const Card& dealerUpcard, const Deck& currentDeck) const;

	double calculateEVForHit(const GameState& state) const;
	double calculateEVForStand(const GameState& state) const;
	double calculateEVForSplit(const GameState& state) const;
	double calculateEVForDouble(const GameState& state) const;
	double calculateEVForSurrender(const GameState& state) const;

	std::string getRuleDescription() const;
	};