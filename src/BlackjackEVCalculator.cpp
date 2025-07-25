// BlackjackEVCalculator.cpp : This file contains the 'main' function. Program
// execution begins and ends there.
//

#include <iostream>

#include "../include/BlackjackGame.h"

// Helper function to create a card from a string like "A", "K", "7"
Card cardFromStr(const std::string& s) {
  Rank r;
  if (s == "A")
    r = Rank::Ace;
  else if (s == "2")
    r = Rank::Two;
  else if (s == "3")
    r = Rank::Three;
  else if (s == "4")
    r = Rank::Four;
  else if (s == "5")
    r = Rank::Five;
  else if (s == "6")
    r = Rank::Six;
  else if (s == "7")
    r = Rank::Seven;
  else if (s == "8")
    r = Rank::Eight;
  else if (s == "9")
    r = Rank::Nine;
  else if (s == "T" || s == "J" || s == "Q" || s == "K")
    r = Rank::Ten;  // Group 10-value cards
  else
    throw std::invalid_argument("Invalid card rank input");
  return Card(r, Suit::Hearts);  // Suit doesn't matter for calculations
}

// Helper function to convert a PlayerAction enum to a string
std::string actionToStr(PlayerAction action) {
  switch (action) {
    case PlayerAction::Hit:
      return "Hit";
    case PlayerAction::Stand:
      return "Stand";
    case PlayerAction::Double:
      return "Double";
    case PlayerAction::Split:
      return "Split";
    case PlayerAction::Surrender:
      return "Surrender";
    case PlayerAction::None:
      return "None (Bust)";
    default:
      return "Unknown";
  }
}

int main() {
  std::string p1_str, p2_str, d1_str;
  std::cout << "Enter player's first card (A, 2-9, T, J, Q, K): ";
  std::cin >> p1_str;
  std::cout << "Enter player's second card: ";
  std::cin >> p2_str;
  std::cout << "Enter dealer's up card: ";
  std::cin >> d1_str;

  Card p1 = cardFromStr(p1_str);
  Card p2 = cardFromStr(p2_str);
  Card d1 = cardFromStr(d1_str);

  Deck testDeck = Deck(6);
  testDeck.dealCard(p1);
  testDeck.dealCard(p2);
  testDeck.dealCard(d1);

  Hand playerHand = Hand();
  playerHand.addCard(p1);
  playerHand.addCard(p2);

  BlackjackGame game = BlackjackGame();
  GameState state = game.getGameState(playerHand, d1, testDeck, true);

  // Before starting a new, independent analysis, always clear the memoization
  // caches to ensure results from a previous run aren't used.
  game.clearMemos();

  // Calculate all EVs by calling the main strategy function once.
  // This is the most efficient way, as it populates and uses the cache
  // internally to solve the entire problem.
  EVResult result = game.calculateEVForOptimalStrategy(state);

  std::cout << "\n--- Results ---" << std::endl;
  std::cout << "Player has " << playerHand.getValue() << " vs Dealer "
            << d1.getValue() << std::endl;

  std::cout << "EV for standing: " << result.standEV << std::endl;
  std::cout << "EV for doubling: " << result.doubleEV << std::endl;
  std::cout << "EV for splitting: " << result.splitEV << std::endl;
  std::cout << "EV for hitting: " << result.hitEV << std::endl;
  std::cout << "EV for surrender: " << result.surrenderEV << std::endl;
  // Insurance is not part of the optimal strategy EV calculation, so call it
  // separately if needed.
  std::cout << "EV for insurance: " << game.calculateEVForInsurance(state)
            << std::endl;
  std::cout << "\nOptimal action: " << actionToStr(result.optimalAction)
            << std::endl;
  std::cout << "Optimal EV: " << result.optimalEV << std::endl;

  return 0;
}